/*
	File name			:	file.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 16
	Date last modified	:	2018 , ??? ??
	Purpose				:	This source code file will manage file commerge ros system
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	maintainance

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/file.h>

#define _DEBUG_CODE_

namespace zeabus_library{

	File::File( std::string package , std::string directory , std::string name , bool time ){
		this->status_file = false;
		this->update_time();
		this->set_package_file( package );
		this->set_directory_file( directory );
		this->set_name_file( name , time );
	}

	File::~File(){
		if( status_file ){
			this->close();
		}
	}

	size_t File::set_package_file( std::string package ){
		if( status_file ){
			print_warning( "zeabus_library::File::set_package_file now file open" );
			return ERROR_STATUS;		
		}
		this->location_file.package = ros::package::getPath( package );
		#ifdef _DEBUG_CODE_
			printf("Finish set directory package\n");
		#endif
		return NO_ERROR;
	}

	size_t File::set_directory_file( std::string directory ){
		if( status_file ){
			print_warning( "zeabus_library::File::set_directory_file now file open" );
			return ERROR_STATUS;		
		}
		this->location_file.subdirectory = directory;
		#ifdef _DEBUG_CODE_
			printf("Finish set directory support\n");
		#endif
		return NO_ERROR;
	}

	size_t File::set_name_file( std::string name , bool time ){
		if( status_file ){
			print_warning( "zeabus_library::File::set_directory_file now file open" );
			return ERROR_STATUS;		
		}
		if( time ){
			this->location_file.name_file = name + "_" +  
				convert::edit_string( this->time , "_") + ".txt";
		}	
		else{
			this->location_file.name_file = name + ".txt";
		}
		#ifdef _DEBUG_CODE_
			printf("Finish set name file\n");
		#endif
		return NO_ERROR;
	}

	void File::update_time(){
		this->time_ptime = boost::posix_time::second_clock::local_time();
		this->time = zeabus_library::convert::time_to_string( this->time_ptime );
	}

	size_t File::write_time(){
		if( ! status_file ){
			print_error( "zeabus_library::File::write_time please open file before use this");
			return ERROR_STATUS;
		}
		this->time_ptime = boost::posix_time::second_clock::local_time();
		this->time = zeabus_library::convert::time_to_string( this->time_ptime );
		std::string temp = "=============== " + this->time + " ===============\n";
		this->write( temp );
		return NO_ERROR;
	}

	void File::update_directory(){
		this->location_file.result = this->location_file.package + "/"
									+this->location_file.subdirectory + "/"
									+this->location_file.name_file;
	}

	size_t File::open(){
		if( status_file ){
			print_warning( "zeabus_library::File::open please close file before open again");
			return ERROR_STATUS;
		}
		this->update_directory();
		this->file = fopen( this->location_file.result.c_str() , "w");
		status_file = true;
		return NO_ERROR;
	}

	size_t File::close(){
		fclose( this->file );
		status_file = false;
		return NO_ERROR;
	}

	size_t File::write( std::string& message ){
		if( ! status_file ){
			print_error( "zeabus_library::File::write please open file before write file");
			return ERROR_STATUS;
		}
		fprintf( this->file , "%s" , message.c_str() );
		return NO_ERROR;
	}

	size_t File::writeline( std::string message ){
		if( ! status_file ){
			print_error( "zeabus_library::File::writeline please open file before write file");
			return ERROR_STATUS;
		}
		std::string data = message + "\n";
		return this->write( data );
	}
}
