/*
	File name			:	ServiceOneVector3Stamped.h		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 10
	Date last modified	:	2019 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	zeabus_library/control
*/
//===============>

#include	<stdio.h>

#include	<iostream>

#include	<math.h>

#include	<nav_msgs/Odometry.h>

#include	<geometry_msgs/Vector3.h>

#include	<zeabus_library/linear_equation.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/OneVector3Stamped.h>

#ifndef EPSILON
	#define 5.0e-3
#endif

#ifndef _ZEABUS_LIBRARY_CONTROL_SERVICE_ONE_VECTOR3_STAMPED_H__
#define _ZEABUS_LIBRARY_CONTROL_SERVICE_ONE_VECTOR3_STAMPED_H__

namespace zeabus_library{

namespace control{

	class ServiceOneVector3Stamped{

		public:
			ServiceOneVector3Stamped();

			void register_all_state( nav_msgs::Odometry* data_current 
					, nav_msgs::Odometry* data_target , nav_msgs::Odometry* data_linear 
					, nav_msgs::Odometry* data_save);

			void register_all_quaternion( zeabus_library::tf_handle::TFQuaternion* data_current
					, zeabus_library::tf_handle::TFQuaternion* data_target 
					, zeabus_library::tf_handle::TFQuaternion* data_diff );

			void register_current( nav_msgs::Odometry* data);

			void register_target( nav_msgs::Odometry* data);

			void register_linear( nav_msgs::Odometry* data);

			void register_save( nav_msgs::Odometry* data);

			void register_equation( zeabus_library::LinearEquation* data);

			void register_velocity( bool* data_fix , double* data_velocity );

			bool callback_relative_xy( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

			bool callback_relative_z( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

			bool callback_relative_yaw( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

			bool callback_fix_yaw( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

			bool callback_fix_z( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

			bool callback_velocity_xy( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

			bool callback_velocity_z( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

			bool callback_velocity_yaw( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

		private:
			nav_msgs::Odometry* current_state;
			nav_msgs::Odometry* target_state;
			nav_msgs::Odometry* linear_state;
			nav_msgs::Odometry* save_state;

			geometry_msgs::Vector3 temp_vector3;
			double temp_double[3];

			zeabus_library::LinearEquation* equation;

			zeabus_library::tf_handle::TFQuaternion* current_quaternion;
			zeabus_library::tf_handle::TFQuaternion* target_quaternion;
			zeabus_library::tf_handle::TFQuaternion* diff_quaternion;

			bool* fix_velocity;
			double* value_velocity;


	};

}

}

#endif
