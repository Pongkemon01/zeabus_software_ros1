/*
	File name			:	rotation_handle.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 23
	Date last modified	:	2018 , DEC 27
	Purpose				:	This is file for apply matrix and quaternionhandle

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/rotation/rotation_handle.h>

//#define _DEBUG_ROTATION_VALUE_

//#define _DEBUG_DECLARE_CLASS_

namespace zeabus_library{

namespace rotation{

	RotationHandle::RotationHandle() : QuaternionHandle(){
		temporary_matrix.resize( 3 , 3 );
		#ifdef _DEBUG_DECLARE_CLASS_
			printf("After RotationHandle init function\n");
		#endif
	}

	size_t RotationHandle::start_rotation( boost::numeric::ublas::matrix< double >& value 
							, boost::numeric::ublas::matrix< double >& result ){
		if( ! ( value.size1() == 3 && value.size2() == 1 ) ){
			zeabus_library::print_error( "zeabus_library::zeabus_rotation::RotationHandle::start_rotation value wrong size" );
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
		else if( ! ( result.size1() == 3 && result.size2() == 1 ) ){
			zeabus_library::print_error( "zeabus_library::zeabus_rotation::RotationHandle::start_rotation value wrong size" );
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
		this->update_rotation();
		this->matrix_rotation( this->temporary_matrix , true );

		zeabus_library::matrix::product( temporary_matrix , value , result );

		#ifdef _DEBUG_ROTATION_VALUE_
			printf("Infomation of start rotation to target\n");
			this->get_RPY( this->diff_euler[0] , this->diff_euler[1] , this->diff_euler[2] );
			zeabus_library::vector::print("value matrix " , value );
			zeabus_library::matrix::print("Matrix rotation" , this->temporary_matrix );
			zeabus_library::vector::print("result matrix " , result );
			printf("Diff roll : pitch : yaw <====> %8.4lf : %8.4lf : %8.4lf\n"
					, this->diff_euler[0] , this->diff_euler[1] , this->diff_euler[2] );
		#endif

		return zeabus_library::NO_ERROR;
		
	}

	size_t RotationHandle::target_rotation( boost::numeric::ublas::matrix< double >& value 
							, boost::numeric::ublas::matrix< double >& result ){
		if( ! ( value.size1() == 3 && value.size2() == 1 ) ){
			zeabus_library::print_error( "zeabus_library::zeabus_rotation::RotationHandle::start_rotation value wrong size" );
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
		else if( ! ( result.size1() == 3 && result.size2() == 1 ) ){
			zeabus_library::print_error( "zeabus_library::zeabus_rotation::RotationHandle::start_rotation value wrong size" );
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
		this->update_rotation();
		this->matrix_rotation( this->temporary_matrix , false );

		zeabus_library::matrix::product( temporary_matrix , value , result );

		#ifdef _DEBUG_ROTATION_VALUE_
			printf("Infomation of target rotation to start\n");
			this->get_RPY( this->diff_euler[0] , this->diff_euler[1] , this->diff_euler[2] );
			zeabus_library::vector::print("value matrix " , value );
			zeabus_library::matrix::print("Matrix rotation" , this->temporary_matrix );
			zeabus_library::vector::print("result matrix " , result );
			printf("Diff roll : pitch : yaw <====> %8.4lf : %8.4lf : %8.4lf\n"
					, this->diff_euler[0] , this->diff_euler[1] , this->diff_euler[2] );
		#endif

		return zeabus_library::NO_ERROR;
	}	

}

}
