/*
	File name			:	quaternion.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 05
	Date last modified	:	2018 , ??? ??
	Purpose				:	For make function peration tf::Quaternion

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	production

	Namespace			:	zeabus_library::tf_handle
*/

#include	<tf/transform_broadcaster.h>

#include	<stdio.h>

#include	<iostream>

#ifndef PI
	#define	PI 3.14159265
#endif

#ifndef _ZEABUS_LIBRARY_TF_HANDLE_TF_QUATERNION_H__
#define _ZEABUS_LIBRARY_TF_HANDLE_TF_QUATERNION_H__

namespace zeabus_library{

namespace tf_handle{

	class TF_Quaternion : public tf::Quaternion{

		public:
			TF_Quaternion();
			TF_Quaternion( const tfScalar& x , const tfScalar&y 
							, const tfScalar& z , const tfScalar& w );
			TF_Quaternion( const tf::Vector3 &axis , const tfScalar& angle );
			TF_Quaternion( const tfScalar& yaw , const tfScalar& pitch , const tfScalar& roll);

			void get_RPY( double& roll , double& pitch , double& yaw );

			double* rotation( double* problem );
			
			void rotation( double& x , double& y , double& z );

		private:
			double temp_double[3];

	};


}

}

#endif