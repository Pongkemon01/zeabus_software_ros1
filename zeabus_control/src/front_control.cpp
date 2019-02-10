/*
	File name			:	front_control.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 06
	Date last modified	:	2019 , FEB 09
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/
//====================>

#define		_LOOK_TRANSFORM_
#define		_PRINT_DATA_

#include	<stdio.h>
#include	<iostream>

#include	<ros/ros.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/linear_equation.h>

#include	<zeabus_library/subscriber/SubOdometry.h>
#include	<zeabus_library/subscriber/SubTwistStamped.h>

#include	<zeabus_library/tf_handle/TFQuaternion.h>

#ifdef	_LOOK_TRANSFORM_
	#include	<tf/transform_broadcaster.h>
#endif

int main( int argv , char** argc ){

	ros::init( argv , argc , "/control/front");

	ros::NodeHandle ph("~");
	ros::NodeHandle nh("");

//====================> PARAMETER PART
	std::string state_topic;
	std::string twist_topic;
	std::string target_topic;
	std::string target_id;
	std::string my_id;
	std::string linear_id;
	std::string world_id;
	int frequency;
	int constant_tll;
	int aborted_control;

	ph.param< std::string >("state_topic" , state_topic , "/localize/state" );
	ph.param< std::string >("twist_topic" , twist_topic , "/mission/twist" );
	ph.param< std::string >("target_topic" , target_topic , "/control/twist" );
	ph.param< std::string >("target_id" , target_id , "robot_target");
	ph.param< std::string >("world_id" , world_id , "world");
	ph.param< std::string >("linear_id" , linear_id , "robot_linear");
	ph.param< std::string >("my_id" , my_id , "control_front");
	ph.param< int >("frequency" , frequency , 40 );
	ph.param< int >("constant_ttl" , constant_ttl , 10 );

	aborted_control = copysign( frequency , -1 );

//====================> SETUP VARIABLE
	bool fix_velocity[6] = { 0, 0, 0, 0, 0, 0}; // for manage by service  
	double value_fix_velocity[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // use with fix_velocity

	nav_msgs::Odometry current_state;
	nav_msgs::Odometry target_state; target_state.header.frame_id = target_id;

	nav_msgs::Odometry linear_state; linear_state.header.frame_id = linear_id;
	zeabus_library::LinearEquation linear_handle; // Available only 2-D plan

	geometry_msgs::TwistStamped control_twist; // output send to back_control
	geometry_msgs::TwistStamped received_twist; // receive twist from mission/twist
	int received_target_twist[6] = { 0, 0, 0, 0, 0, 0};

//====================> SETUP TF 
#ifdef _LOOK_TRANSFORM_
	static tf::TransformBroadcaster broadcaster;

	zeabus_library::tf_handle::TFQuaternion tf_quaternion;
	tf::transform target_transform;
	tf::transform linear_transform;
#endif
		
//====================> SETUP ROS SYSTEM
	ros::Rate rate( frequency );

	zeabus_library::subscriber::SubOdometry listener_state( &current_state );
	bool start_up = true; int receive_state = 0; 
	listener_state.register_ttl( &receive_state , 1 );
	ros::Subscriber sub_state = nh.subscribe( state_topic , 1 
			,&zeabus_library::subscribe::SubOdometry::callback_ttl , &listener_state );

	zeabus_library::subscribe::SubTwistStamped listerner_twist( &received_twist );
	listerner_twist.register_ttl( received_target_twist , 15 );
	ros::Subscriber sub_twist = nh.subscribe( twist_topic , 1 );

	ros::Publisher tell_target = nh.advertise< geometry_msgs::TwistStamped>( target_topic , 1);

//====================> LOOP ROS OPERATOR
	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		if( start_up ){

		}
		else{

		}	
	}

}
