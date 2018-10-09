/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: imu_port.cpp
//
//	Create	: Oct 09 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : Detail of cord from imu_port
//				
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_SENSOR_IMU_PORT_H
	#include	<zeabus_sensor/imu_port.h>
	#define	ZEABUS_SENSOR_IMU_PORT_H
#endif

#define TEST_IMU_PORT

namespace zeabus_sensor{

	namespace MIP_COMMUNICATION{

		microstrain_imu_port::microstrain_imu_port( std::string name_port): 
										specific_port( name_port ){
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> Init call microstrain_imu_port by name port is "
							<< name_port << "\n";
			#endif
		} 

		microstrain_imu_port::~microstrain_imu_port(): ~specific_port(){
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> THE END OF microstrain_imu_port\n";
			#endif
		}		

		void microstrain_imu_port::set_idle(){
			this->adding_header( this->write_buffer );
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> After adding_head in call function area have size "
							<< data.size() << "\n"
			#endif
			this->write_buffer.push_back( BASE::DESCRIPTOR ); // add descriptor set
			this->write_buffer.push_back( 0x02 ); // add Payload Length
			this->write_buffer.push_back( 0x02 ); // add Field Length
			this->write_buffer.push_back( BASE::SET_TO_IDLE ); // add descriptor field
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> After adding Command field have size " 
							<< write_buffer.size() << "\n";
			#endif
			this->adding_checksum( this->write_buffer);
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> After adding Check some have size " 
							<< write_buffer.size() << "\n";
			#endif

		}

		template<typename type_vector>void microstrain_imu_port::adding_header(
												type_vector data){
			this->data.resize(0);
			data.push_back( this->sync_1);
			data.push_back( this->sync_2);
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> After adding_head have size " 
							<< data.size() << "\n";
			#endif
		}
		template<typename type_vector>void microstrain_imu_port::adding_checksum( 
												type_vector data){
			uint16_t check_sum_01 = 0;
			uint16_t check_sum_02 = 0;
			for( type_vector::iterator = data.begin() ; iterator != data.end() ; iterator++){
				check_sum_01 += *iterator;
				check_sum_02 += check_sum_01;
				#ifdef TEST_IMU_PORT
					printf("<---HOW---> %x : %x : %x" , *iterator , check_sum_01 , check_sum_02);
				#endif
			}
			
			uint32_t all_result = (check_sum_01 << 8) + check_sum_02;
			data.push_back( (uint8_t)((all_result>>8)&0xff));
			data.push_back( (uint8_t)(all_result&0xff));
			#ifdef TEST_IMU_PORT
				print("<--RESULT--> %x : %x\n", *(data.end()-2) , *(data.end()-1));
				std::cout	<< "<--TESTER--> After addinh_checksun have size is "
							<< data.size() << "\n"; 
			#endif
		}

}

}