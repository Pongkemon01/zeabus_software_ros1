#!/usr/bin/python2
#################################################################################################
####
####	FILE		: standard_mission.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 20
####	Purpose		: Pattern of mission planner
####
#################################################################################################
from __future__				import print_function
#===============>

import rospy
import math
import time

from control_connection		import ControlConnection

from zeabus_library.srv		import TwoBool

from std_msgs.msg			import String , Bool

#===============> Standard Mission will manage about echo&service <ros system> sleep

class StandardMission( ControlConnection ):

	def __init__( self , name , name_service  , callback_service ):

		ControlConnection.__init__( self , name )

		self.data_pub = rospy.Publisher("/mission/echo" , String , queue_size = 1 )
		self.rate = rospy.Rate( 30 )

		self.mission = rospy.Service( name_service , TwoBool , callback_service )

	def sleep( self , second = 0.1 ):
		self.rate.sleep()
		time.sleep( second )

	def echo_vision( self , message ):
		self.data_pub.publish( String( message ) )
		print( str(message) , end = "\n\n")

	def echo( self , name , message ):
		self.data_pub.publish( String("<==" + str(name) + "==> " + str(message) ) )
		print( "<==" + str(name) + "==> " + str(message) , end = "\n\n")

def callback( request ):
	print("Callback in file standard_mission.py")
	return True

if __name__=="__main__":
	rospy.init_node("Testing_class_inherited")
	standart_mission = StandardMission("Sevinar" , "/mission/testing" , callback )
	print("spinnnnn")
	rospy.spin()