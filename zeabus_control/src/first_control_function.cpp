/*
	File name			:	first_control_function.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	For helping function
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	-
*/

#include	<stdio.h>

#include	<iostream>

#include	<math.h>

#include	<zeabus_library/general.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/Point3.h>

#include	<zeabus_library/Point4.h>

void clear_point4( zeabus_library::Point4& variable ){
	variable.w = 0;
	variable.x = 0;
	variable.y = 0;
	variable.z = 0;
}

void clear_point3( zeabus_library::Point3& variable ){
	variable.x = 0;
	variable.y = 0;
	variable.z = 0;
}

void next_point_xy( double& yaw , double& x0 , double& y0 , double& x1 , double& x2 
						, double distance_x , double distance_y ){
	x1 = x0 
		+ distance_x*zeabus_library::euler::cos( yaw ) 
		+ distance_y*zeabus_library::euler::cos( yaw + PI );
	
	y1 = y0
		+ distance_x*zeabus_library::euler::sin( yaw )
		+ distance_y*zeabus_library::euler::sin( yaw + PI );  
}

double assign_velocity_xy( double error ){
	double magnitude = zeabus_library::abs( error );
	if( magnitude < 0.05 ) return 0;
	else if( magnitude < 0.25 ) return copysign( 0.1 , error );
	else if( magnitude < 1 ) return copysign( 0.5 , error );
	else return copysign( 1 , error );
}

double assign_veloity_z( double error ){
	double magnitude = zeabus_library::abs( error );
	if( magnitude < 0.05 ) return 0;
	else if( magnitude < 0.5 ) return copysign( 0.1 , error );
	else return copysign( 0.5 , error );
}
