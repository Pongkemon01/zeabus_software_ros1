////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_two_point.cpp 
//	Purpose		: for main_service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<zeabus_library/TwoPoint.h>
#include	<zeabus_library/zeabus_control/service_control.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_SERVICE_TWO_POINT_H__
#define _ZEABUS_LIBRARY_CONTROL_SERVICE_TWO_POINT_H__

namespace zeabus_control{

	class two_point_service: public main_service{
		public:
			two_point_service ( double* current_state , double* target_state 
						, double* robot_error , double * ok_error ) :
							main_service( current_state , target_state 
											, robot_error , ok_error ){}
			bool call_relative_xy( zeabus_library::TwoPoint::Request &req ,
								 zeabus_library::TwoPoint::Response &res );
			bool call_absolute_xy( zeabus_library::TwoPoint::Request &req ,
								 zeabus_library::TwoPoint::Response &res );
	
	};

}
#endif