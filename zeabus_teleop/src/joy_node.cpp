
/*
 * zeabus_joy.cpp
 *
 *  Created on: Feb 2, 2015
 *      Author: mahisorn
 */

/*
 * Copyright (c) 2010, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the Willow Garage, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <string>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"
#include "ros/console.h"
#include <zeabus_library/Twist.h>

class ZeabusTeleop
{
public:
  ZeabusTeleop();
private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);
  void publish();
  ros::NodeHandle ph_, nh_;

  int roll_axis_, pitch_axis_,yaw_axis_, depth_axis_;
  int deadman_button_, mode_button_;


  ros::Publisher vel_pub_;
  ros::Subscriber joy_sub_;
  zeabus_library::Twist last_published_;
  boost::mutex publish_mutex_;

  bool deadman_pressed_;
  bool zero_twist_published_;
  double vx_scale_;
  double vy_scale_;
  double vz_scale_;
  double ax_scale_;
  double ay_scale_;
  double az_scale_;
  std::string topic_joy;

  ros::Timer timer_;
};

ZeabusTeleop::ZeabusTeleop()
  : ph_("~")
{

  ph_.param("deadman_button", deadman_button_, 0);
  ph_.param("mode_button", mode_button_, 1);

  ph_.param("roll_axis", roll_axis_, 0);
  ph_.param("roll_pitch", pitch_axis_, 1);
  ph_.param("roll_yaw", yaw_axis_, 2);
  ph_.param("depth_axis", depth_axis_, 3);

  ph_.param("vx_scale", vx_scale_, 1.0);
  ph_.param("vy_scale", vy_scale_, 1.0);
  ph_.param("vz_scale", vz_scale_, 0.0);
  ph_.param("ax_scale", ax_scale_, 0.0);
  ph_.param("ay_scale", ay_scale_, 0.0);
  ph_.param("az_scale", az_scale_, 0.2);
  //ph_.param<std::string>("topic_joy", topic_joy, "/zeabus/cmd_vel");
  ph_.param<std::string>("topic_joy", topic_joy, "/zeabus/cmd_vel");



  deadman_pressed_ = false;
  zero_twist_published_ = false;
  vel_pub_ = ph_.advertise<geometry_msgs::Twist>(topic_joy, 1, true);
  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("/joy", 10, &ZeabusTeleop::joyCallback, this);
  timer_ = nh_.createTimer(ros::Duration(0.1), boost::bind(&ZeabusTeleop::publish, this));
}
void ZeabusTeleop::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  double r, p, y, depth;

  zeabus_library::Twist vel;

  if(!joy->buttons[mode_button_])
  {
    vel.linear.x = joy->axes[pitch_axis_] * vx_scale_;
    vel.linear.y = -1.0*joy->axes[roll_axis_] * vy_scale_;
  }
  else
  {
    vel.angular.x = joy->axes[roll_axis_] * ax_scale_;
    vel.angular.y = joy->axes[pitch_axis_] * ay_scale_;
  }

  vel.linear.z = -1*joy->axes[depth_axis_] * vz_scale_;
  if (joy->buttons[3]) vel.linear.z=-1;
  else if (joy->buttons[0]) vel.linear.z=1;
  else vel.linear.z=0;

  vel.angular.z = joy->axes[yaw_axis_] * az_scale_;

  last_published_ = vel;
  deadman_pressed_ = joy->buttons[deadman_button_];
}
void ZeabusTeleop::publish()
{
  boost::mutex::scoped_lock lock(publish_mutex_);
  if (deadman_pressed_)
  {
    vel_pub_.publish(last_published_);
    zero_twist_published_ = false;
  }
  else if (!deadman_pressed_ && !zero_twist_published_)
  {
    vel_pub_.publish(geometry_msgs::Twist());
    zero_twist_published_ = true;
  }
}
int main(int argc, char** argv)
{
  ros::init(argc, argv, "zeabus_teleop");
  ZeabusTeleop Zeabus_teleop;
  ros::spin();
}


