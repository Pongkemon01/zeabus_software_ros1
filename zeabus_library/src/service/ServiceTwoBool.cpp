/*
	File name			:	ServiceTwoBool.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 19
	Date last modified	:	2018 , FEB ??
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	production

	Namespace			:	zeabus_library::service
*/
//===============>

#include	<zeabus_library/service/ServiceTwoBool.h>

namespace zeabus_library{

namespace service{

	ServiceTwoBool::ServiceTwoBool( bool* data ){
		this->data = data;
	}

	void ServiceTwoBool::register_bool( bool* data ){
		this->data = data;
	}

	bool ServiceTwoBool::callback( zeabus_library::TwoBool::Request& request 
			, zeabus_library::TwoBool::Response& response ){
		response.result = *(this->data);
		*(this->data) = request.data;
		return true;
	}

}

}
