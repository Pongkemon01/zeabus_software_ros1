/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: world_time.cpp
//
//	Last Update		: Sep 1 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: manage about time
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_TIME
	#include	<zeabus_extension/time.h>
	#define ZEABUS_EXTENSION_TIME
#endif

zeabus_extension::world_time::time::time( bool print_test){
	this->print_test = print_test;
	this->reset_time();
}

std::string zeabus_extension::world_time::time::local_time(){
	if( print_test ){
		std::cout	<< " At world_time::time::local_time " 
					<< convert::to_string( local_data )
					<< "\n";
	}
	return zeabus_extension::convert::to_string( local_data );	
}

std::string zeabus_extension::world_time::time::universal_time(){
	if( print_test ){
		std::cout	<< " At world_time::time::universal_time " 
					<< convert::to_string( universal_data )
					<< "\n";
	}
	return zeabus_extension::convert::to_string( universal_data );
}

void zeabus_extension::world_time::time::reset_time(){
	local_data = boost::posix_time::second_clock::local_time();
	universal_data = boost::posix_time::second_clock::universal_time();
}
