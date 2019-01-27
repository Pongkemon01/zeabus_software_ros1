#!/usr/bin/env python
"""
    File name:gate.py
    Author: skconan, AyumiizZ
    Date created: 2018/10/13
    Date edit: 2018/1/23 
    Python Version: 2.7
"""
# from statistics import Statistics
import rospy
import numpy as np
import cv2 as cv
from sensor_msgs.msg import CompressedImage
from zeabus_vision.msg import vision_gate
from zeabus_vision.srv import vision_srv_gate
import color_text as ct
import vision_lib as lib
# from fourier_transform import FourierTransform
from operator import itemgetter

IMAGE = None
PROCESS_DATA = {}
PUBLIC_TOPIC = '/vision/mission/gate/'
SUB_SAMPLING = 0.3
DEBUG = {
    'time': False,
    'console': False,
    'rqt-grid': False,
    'detail': False
}


def mission_callback(msg):
    task = str(msg.task.data)
    req =  str(msg.req.data)
    if DEBUG['console']:
        lib.print_mission(task,req)
    if task == 'gate':
        return find_gate()


def image_callback(msg):
    global IMAGE
    arr = np.fromstring(msg.data, np.uint8)
    bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
                    fx=SUB_SAMPLING, fy=SUB_SAMPLING)
    IMAGE = bgr.copy()


def message(state=0, pos=0, cx1=0.0, cy1=0.0, cx2=0.0, cy2=0.0, area=0.0):
    msg = vision_gate()
    msg.state = state
    msg.pos = pos
    msg.cx1 = cx1
    msg.cy1 = cy1
    msg.cx2 = cx2
    msg.cy2 = cy2
    msg.area = area
    if DEBUG['console'] or DEBUG['detail']:
        print msg
    return msg


def what_align(cnt):
    x, y, w, h = cv.boundingRect(cnt)
    if w > h:
        return 'h'
    return 'v'


def is_pipe(cnt, percent):
    """
        Information
        Pipe    width = 150
                height = 40
    """
    (x, y), (w, h), angle = cv.minAreaRect(cnt)

    area_ratio_expected = 0.50
    wh_ratio_expected = 9  # (150 / 40.) * 2
    w, h = max(w, h), min(w, h)
    area_cnt = cv.contourArea(cnt)
    area_box = w * h
    if (area_box == 0) or area_cnt <= 500:
        return False

    area_ratio = area_cnt / area_box
    wh_ratio = 1.0 * w / h
    if (not (area_ratio > area_ratio_expected
             and wh_ratio > wh_ratio_expected * percent
             and (angle >= -20 or angle <= -70))):
        return False
    return True


def find_pipe(binary, align):
    _, contours, _ = cv.findContours(
        binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    number_of_object = 2 if align == 'v' else 1
    percent_pipe = 0.4 if align == 'h' else 0.8
    result = []
    pipe = cv.cvtColor(binary, cv.COLOR_GRAY2BGR)

    for cnt in contours:
        (x, y), (w, h), angle = rect = cv.minAreaRect(cnt)
        w, h = max(w, h), min(w, h)
        if not is_pipe(cnt, percent_pipe) or not what_align(cnt) == align:
            continue

        box = cv.boxPoints(rect)
        box = np.int64(box)
        if align == 'v':
            cv.drawContours(pipe, [box], 0, (0, 255, 255), 2)
            result.append([int(x), int(y), int(h), int(w), angle])
        else:
            cv.drawContours(pipe, [box], 0, (255, 0, 255), 2)
            result.append([int(x), int(y), int(w), int(h), angle])
    if align == 'v':
        lib.publish_result(pipe, 'bgr', PUBLIC_TOPIC + 'mask/vpipe')
    if align == 'h':
        lib.publish_result(pipe, 'bgr', PUBLIC_TOPIC+'mask/hpipe')
    if align == 'v':
        result = sorted(result, key=itemgetter(3), reverse=True)
    else:
        result = sorted(result, key=itemgetter(2), reverse=True)

    if len(result) < number_of_object:
        return result, len(result)
    else:
        return result[:number_of_object], number_of_object


def find_gate():
    global IMAGE
    if IMAGE is None:
        lib.img_is_none()
        return message(state=-1)

    display = IMAGE.copy()
    gray = cv.cvtColor(IMAGE.copy(), cv.COLOR_BGR2GRAY)
    obj = lib.bg_subtraction(gray)

    kernel_box = lib.get_kernel(ksize=(7, 7))

    kernel_vertical = lib.get_kernel(ksize=(1, 21))
    vertical = cv.erode(obj.copy(), kernel_vertical)
    vertical = cv.dilate(vertical.copy(), kernel_box)
    kernel_erode = lib.get_kernel(ksize=(3, 11))
    vertical = cv.erode(vertical.copy(), kernel_erode)

    kernel_horizontal = lib.get_kernel(ksize=(21, 1))
    horizontal = cv.erode(obj.copy(), kernel_horizontal)
    horizontal = cv.dilate(horizontal.copy(), kernel_box)
    kernel_erode = lib.get_kernel(ksize=(11, 3))
    horizontal = cv.erode(horizontal.copy(), kernel_erode)

    vertical_pipe, no_pipe_v = find_pipe(vertical, 'v')
    horizontal_pipe, no_pipe_h = find_pipe(horizontal, 'h')

    horizontal_cx = []
    horizontal_cy = []
    vertical_cx = []
    vertical_cy1 = []
    vertical_cy2 = []

    for res in horizontal_pipe:
        x, y, w, h, angle = res
        cv.rectangle(display, (int(x - w / 2.), int(y - h / 2.)),
                     (int(x + w / 2.), int(y + h / 2.)), (0, 255, 0), 2)
        horizontal_cx.append([(x - w / 2.), (x + w / 2.)])
        horizontal_cy.append(y)
    for res in vertical_pipe:
        x, y, w, h, angle = res
        cv.rectangle(display, (int(x - w / 2.), int(y - h / 2.)),
                     (int(x + w / 2.), int(y + h / 2.)), (108, 105, 255), 2)
        vertical_cx.append(x)
        vertical_cy1.append((y - h / 2.))
        vertical_cy2.append((y + h / 2.))
    himg, wimg = obj.shape[:2]
    mode = 0
    if no_pipe_h == 1:
        if no_pipe_v == 2:
            mode = 1
        elif no_pipe_v == 1:
            mode = 2
        elif no_pipe_v == 0:
            mode = 3
    elif no_pipe_h == 0 and no_pipe_v == 2:
        mode = 4
    else:
        mode = 0
    if mode == 0:
        lib.print_result("NOT FOUND", ct.RED)
        lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'image_result')
        lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
        lib.publish_result(horizontal, 'gray',
                           PUBLIC_TOPIC + 'mask/horizontal')
        lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
        return message()
    elif mode == 1:
        lib.print_result("FOUND GATE", ct.GREEN)
        cx1 = (horizontal_cx[0][0] + min(vertical_cx)) / 2.
        cx2 = (horizontal_cx[0][1] + max(vertical_cx)) / 2.
        cy1 = horizontal_cy[0]
    elif mode == 2:
        lib.print_result("FOUND ONE V AND ONE H", ct.YELLOW)
        cx1 = horizontal_cx[0][0]
        cx2 = horizontal_cx[0][1]
        cy1 = horizontal_cy[0]
    elif mode == 3:
        lib.print_result("FOUND ONE H", ct.YELLOW)
        cx1 = horizontal_cx[0][0]
        cx2 = horizontal_cx[0][1]
        cy1 = horizontal_cy[0]
    elif mode == 4:
        lib.print_result("FOUND TWO V", ct.YELLOW)
        cx1 = min(vertical_cx)
        cx2 = max(vertical_cx)
        cy1 = (sum(vertical_cy1)+min(vertical_cy1))/len(vertical_cy1)
    cy2 = (sum(vertical_cy2)+max(vertical_cy2)) / \
        (len(vertical_cy2)+1) if no_pipe_v != 0 else himg
    right_excess = (cx2 > 0.95*wimg)
    left_excess = (cx1 < (0.05*wimg))
    if (right_excess and not left_excess):
        pos = 1
    elif (not right_excess and left_excess):
        pos = -1
    else:
        pos = 0
    # cv.rectangle(display, (int(cx1), int(cy1)),
        #  (int(cx2), int(cy2)), (0, 255, 0), 3)
    cv.circle(display, (int((cx1+cx2)/2), int((cy1+cy2)/2)),
              3, (0, 255, 255), -1)
    area = 1.0*abs(cx2-cx1)*abs(cy1-cy2)/(himg*wimg)
    lib.publish_result(display, 'bgr', PUBLIC_TOPIC + 'image_result')
    lib.publish_result(vertical, 'gray', PUBLIC_TOPIC + 'mask/vertical')
    lib.publish_result(horizontal, 'gray', PUBLIC_TOPIC + 'mask/horizontal')
    lib.publish_result(obj, 'gray', PUBLIC_TOPIC + 'mask')
    cx1 = lib.Aconvert(cx1, wimg)
    cx2 = lib.Aconvert(cx2, wimg)
    cy1 = -1.0*lib.Aconvert(cy1, himg)
    cy2 = -1.0*lib.Aconvert(cy2, himg)
    return message(state=mode, cx1=cx1, cx2=cx2, cy1=cy1, cy2=cy2, pos=pos, area=area)


if __name__ == '__main__':
    rospy.init_node('vision_gate', anonymous=False)
    rospy.Subscriber(lib.get_topic("front"), CompressedImage, image_callback)
    rospy.Service('vision/gate', vision_srv_gate(),
                  mission_callback)
    lib.print_result("INIT NODE GATE", ct.GREEN)
    rospy.spin()
    lib.print_result("END PROGRAM", ct.YELLOW_HL+ct.RED)