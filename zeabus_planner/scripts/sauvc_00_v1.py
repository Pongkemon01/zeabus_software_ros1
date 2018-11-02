#!/usr/bin/python2

import rospy
from sauvc_01_gate_v3 import play_gate
from sauvc_02_flare_v1 import play_flare
from sauvc_03_drum_v2 import play_drum
from sauve_04_gripper_v1 import play_gripper

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

if __name__=="__main__":
	rospy.init_node("Mission Planner")		
	auv = control_auv("Connection")
	gate = play_gate( 30 )
	gate.setup( 2 , 4 , 1 , 2)
	gate.play()

	print("Finish play gate")
	auv.relative_yaw( -1.57 )
	rate = rospy.Rate(30)
	count_ok = 0
	auv.collect_position()
	print("Connection Forward")
	while( not rospy.is_shutdown() and auv.calculate_distance() < 1.0):
		print( auv.calculate_distance() )
		auv.velocity( x = 0.3)
		rate.sleep()
	print("Connection wait yaw")
	while( not rospy.is_shutdown() and not auv.ok_position("yaw" , 0.1)):
		if( auv.ok_position("yaw" , 0.1 )):
			count_ok += 1
			if(count_ok == 4 ):
				break
			else:
				print( "Ok yaw is " + count_ok )
		else:
			if( count_ok > 0):
				print(" reset ok yaw ")
			count_ok = 0
		rate.sleep()
	print("Connection Left")
	auv.collect_position()
	while( not rospy.is_shutdown() and auv.calculate_distance() < 2.2):
		print( auv.calculate_distance() )
		auv.velocity( y = 0.5)
		rate.sleep()
	print("Connection wait yaw")
	while( not rospy.is_shutdown() and not auv.ok_position("yaw" , 0.1)):
		if( auv.ok_position("yaw" , 0.1 )):
			count_ok += 1
			if(count_ok == 4 ):
				break
			else:
				print( "Ok yaw is " + count_ok )
		else:
			if( count_ok > 0):
				print(" reset ok yaw ")
			count_ok = 0
		rate.sleep()
	print("Connection Left")
	print("Now play flare")

	flare = play_flare( 30 )
	flare.set_up()
	flare.play()

	auv.relative_yaw( 1.57 )
	print("Connection wait yaw")
	while( not rospy.is_shutdown() and not auv.ok_position("yaw" , 0.1)):
		if( auv.ok_position("yaw" , 0.1 )):
			count_ok += 1
			if(count_ok == 4 ):
				break
			else:
				print( "Ok yaw is " + count_ok )
		else:
			if( count_ok > 0):
				print(" reset ok yaw ")
			count_ok = 0
		rate.sleep()

	print( "now playdrum")	
	mission_drum = play_drum( 30 , 1 , 3 , 1 , 2)
	mission_drum.play()
