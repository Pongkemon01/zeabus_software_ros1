/*
	File name			:	listen_IMUData.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 07
	Date last modified	:	2018 , DEC 26
	Purpose				:	This is source of file for listen zeabus_library/IMUData.h

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product
							
	Namespace			:	zeabus_library/sensor
*/

#include	<zeabus_library/sensor/listen_IMUData.h>

#define _DEBUG_RECIEVE_DATA_
#define _DEBUG_CALCULATE_ACCELERATION_
#define _DEBUG_CODE_
#define _DEBUG_TIME_

namespace zeabus_library{

namespace sensor{

	ListenIMUData::ListenIMUData( double roll , double pitch , double yaw , double gravity ){
		this->matrix_imu_to_robot.resize( 3 , 3 ); // set size of matrix
		this->matrix_imu_to_world.resize( 3 , 3 ); // set size of matrix
		this->matrix_robot_to_imu.resize( 3 , 3 ); // set size of matrix
		this->matrix_world_to_imu.resize( 3 , 3 ); // set size of matrix
		// find matrix to convert imu to robot so this is will have only one value	
		this->matrix_handle.all_rotation(	roll	, pitch		, yaw 
											, matrix_imu_to_robot );

		// set size matrix for result when want to get value
		this->result_euler.resize( 1 , 3 );
		this->result_gyro.resize( 1 , 3 );
		this->result_acceleration.resize( 1 , 3 );	

		// set size matrix for receive when want to get value
		this->receive_euler.resize( 1 , 3 );
		this->receive_gyro.resize( 1 , 3 );
		this->receive_acceleration.resize( 1 , 3 );	

		// set size matrix for collecting specific value
		this->offset_euler.resize( 1 , 3 );
		this->offset_gravity.resize( 1 , 3 );
		this->temporary_matrix.resize( 1 , 3 );
		this->set_orientation( roll , pitch , yaw );
		this->set_gravity( gravity );
	}
			
	void ListenIMUData::callback( const zeabus_library::IMUData& message ){
		#ifdef _DEBUG_TIME_
			this->timer.start();
		#endif
		this->receive_msg = message;
		zeabus_library::convert::Point3_to_matrix( this->receive_msg.euler , this->receive_euler );
		zeabus_library::convert::Point3_to_matrix( this->receive_msg.angular_velocity , this->receive_gyro );
		zeabus_library::convert::Point3_to_matrix( this->receive_msg.linear_acceleration 
										, this->receive_acceleration );
		#ifdef _DEBUG_RECIEVE_DATA_
			zeabus_library::matrix::print( "Receive_euler" , this->receive_euler );
			zeabus_library::matrix::print( "Receive_gyro" , this->receive_gyro );
			zeabus_library::matrix::print( "Receive_acceleration" 
															, this->receive_acceleration );
		#endif
		// step for delete gravity acceleration
		// find gravity in robot frame
		this->matrix_handle.all_rotation( this->receive_euler( 0 , 0 ) 
										, this->receive_euler( 0 , 1 )
										, this->receive_euler( 0 , 2 )
										, this->matrix_world_to_imu );
		this->temporary_matrix = boost::numeric::ublas::prod( this->offset_gravity 
															, this->matrix_world_to_imu );
		this->result_acceleration( 0 , 0 ) = this->receive_acceleration( 0 , 0 ) - 
												this->temporary_matrix( 0 , 0 ); 
		this->result_acceleration( 0 , 1 ) = this->receive_acceleration( 0 , 1 ) - 
												this->temporary_matrix( 0 , 1 ); 
		this->result_acceleration( 0 , 2 ) = this->receive_acceleration( 0 , 2 ) + 
												this->temporary_matrix( 0 , 2 ); 
		
		#ifdef _DEBUG_CALCULATE_ACCELERATION_
			zeabus_library::matrix::print( "Matrix_world_to_imu"
															, this->matrix_world_to_imu );
			zeabus_library::matrix::print( "Product offset and convert"
															, this->temporary_matrix );
			zeabus_library::matrix::print( "Result_acceleration"
															, this->result_acceleration );
		#endif
		#ifdef _DEBUG_TIME_
			printf("Last line of callback listen_IMUData : %15.8f\n" , this->timer.capture() );
		#endif
	}

	void ListenIMUData::set_orientation( double roll , double pitch , double yaw ){
		this->offset_euler( 0 , 0 ) = roll ; 
		this->offset_euler( 0 , 1 ) = pitch;
		this->offset_euler( 0 , 2 ) = yaw;
	}

	void ListenIMUData::set_gravity( double gravity ){
		this->offset_gravity( 0 , 0 ) = 0 ;
		this->offset_gravity( 0 , 1 ) = 0 ;
		this->offset_gravity( 0 , 2 ) = gravity ;
	}

	void ListenIMUData::get_result( zeabus_library::IMUData& message ){
		zeabus_library::convert::split_matrix_to_IMUData( this->result_euler , this->result_gyro
								,this->result_acceleration , message );		
	}

}
}
