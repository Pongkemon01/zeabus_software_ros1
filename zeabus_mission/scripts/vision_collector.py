#!/usr/bin/python2
#################################################################################################
####
####	FILE		: vision_collector.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , Nov 14
####	Purpose		: For manage about analysis vision data
####
#################################################################################################
from __future__ import print_function
#===============>

import rospy
import math

#service file of vision must send 2 data to use 
from zeabus_vision.srv import *
from zeabus_vision.msg import *

from std_msgs.msg import String

class VisionCollector:

	def __init__( self , mission_vision ):

		# this require
		if( mission_vision == 'gate'):
			self.gate_set_up()
		elif( mission_vision == 'flare' ):
			self.flare_set_up()	
		elif( mission_vision == 'drum' ):
			self.drum_set_up()
		elif( mission_vision == 'qualification'):
			self.qualification_set_up()
		elif( mission_vision == "golf" ):
			self.golf_set_up()
		else:
			print( "<=== VisionCollecotr ===> ERROR PLEASE Look condition on line 32")
			exit( 0 )
		
		# set variable for make to collect and use in data vision
		self.result			= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

		self.collect		= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

		self.data			= {	'n_obj'		: 0		, 'area'		: 0
								, 'cx'		: 0		, 'cy'			: 0
								, 'cx_1'	: 0		, 'cx_2'		: 0
								, 'cy_1'	: 0		, 'cy_2'		: 0
								, 'left'	: False	, 'right'		: False
								, 'forward'	: False	, 'backward'	: False
								, 'pos'		: 0 }

	def analysis_all( self , task , request , amont ):
		count_found = 0
		count_unfound = 0 
		self.reset_collect()
		while( count_found < amont and count_unfound < amont ):
			self.individual_data( task , request )
			if( self.data['n_obj'] > 0 ):
				if( self.data[ 'n_obj' ] == 2 ):
					count_found = amont
				count_found += 1
				for run in self.can_sum:
					self.collect[ run ] += self.data[ run ]
			else:
				count_unfound += 1
		if( count_found == amont ):
			self.result['n_obj'] = self.data['n_obj']
			for run in self.can_sum:
				self.result[run] = self.collect[run] / amont
			for run in self.not_sum:
				self.result[run] = self.data[run]
		else:
			self.result['n_obj'] = 0	
		
	def reset_collect( self ):		
		for run in self.collect.keys():
			self.collect[ run ] = 0

	def have_object( self ):
		if( self.result['n_obj'] > 0 ):
			return 	True
		else:
			return False

	def distance_x( self ):
		return abs( self.result['cx_1'] - self.result['cx_2'] )
	
	def distance_y( self ):
		return abs( self.result['cy_1'] - self.result['cy_2'] )

	def area( self ):
		return self.result['area']

	def echo_data( self ):
		if( not self.have_object() ):
			return str ( "<=== VISION ===> DON\'T HAVE OBJECT")
		return str ( "<=== VISION ===> Object : " + str( self.result[ 'n_obj'] )
			+ " center_x : center_y " + str( self.center_x() ) + " : " + str( self.center_y() )
			+ " and area is " + str( self.result['area'])
		)

	def echo_specific( self ):
		if( not self.have_object() ):
			return str ( "<=== VISION ===> DON\'T HAVE OBJECT")
		return str ( "<=== VISION ===> cx_1 : cx_2 , cy_1 : cy_2 , d_x : d_y " 
			+ str( self.result["cx_1"] ) + " : " + str( self.result['cx_2'] ) + " ===,=== "
			+ str( self.result["cy_1"] ) + " : " + str( self.result['cy_2'] ) + " ===,=== "
			+ str( self.distance_x() ) + " : " + str( self.distance_y() )
		)

	def num_object( self ):
		return self.result['n_obj']	
			
#################################################################################################
####
####	THIS SECTION IS INDIVIDUAL FOR EACH SERVICE OF ZEABUS TEAM VISION PART
####		AVAILABLE FOR USE
####
####	Note :	We use variable to declare function which want to use that make me have 
####			pattern to calculate vision data
#################################################################################################
	
##=================================> SERVICE VISION DRUM <=======================================
	def drum_set_up( self ):
		self.individual_data	= self.drum_data
		self.center_x			= self.drum_center_x
		self.center_y			= self.drum_center_y
		self.can_sum = [ 'cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' ]
		self.not_sum = [ 'left' , 'right' , 'forward' , 'backward']
		self.request_data = rospy.ServiceProxy( "/vision/drum" , vision_srv_drum )
		self.echo_special = self.drum_echo

	def drum_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data['n_obj']		= temporary.state
		self.data['cx_1']		= temporary.cx1
		self.data['cx_2']		= temporary.cx2
		self.data['cy_1']		= temporary.cy1
		self.data['cy_2']		= temporary.cy2
		self.data['area']		= temporary.area
		self.data['left']		= temporary.left
		self.data['right']		= temporary.right
		self.data['forward']	= temporary.forward
		self.data['backward']	= temporary.backward

	def drum_echo( self ):
		return str( "<=== VISION ===> point1 : point2 , left : right : forward : backward [ " 
				+ str( self.result["cx_1"] ) + " , " + str( self.result["cy_1"] ) + " ] : [ " 
				+ str( self.result["cx_2"] ) + " , " + str( self.result["cy_2"] ) + " ] : " 
				+ str( self.result["left"] ) + " : " + str( self.result["right"] ) + " : " 
				+ str( self.result["forward"] ) + " : " + str( self.result["backward"] ) 
		)

	def drum_center_x( self ):
		return (self.result['cx_1'] + self.result['cx_2'])/2

	def drum_center_y( self ):
		return (self.result['cy_1'] + self.result['cy_2'])/2

##=================================> SERVICE VISION GATE <=======================================
	def gate_set_up( self ):
		self.individual_data	= self.gate_data
		self.center_x			= self.gate_center_x
		self.center_y			= self.gate_center_y
		self.can_sum = [ 'cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area' , 'pos' ]
		self.not_sum = []
		self.request_data = rospy.ServiceProxy( "/vision/gate" , vision_srv_gate )
		self.echo_special = self.echo_specific

	def gate_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data['n_obj']		= temporary.state
		self.data['cx_1']		= temporary.cx1
		self.data['cy_1']		= temporary.cy1
		self.data['cx_2']		= temporary.cx2
		self.data['cy_2']		= temporary.cy2
		self.data['area']		= temporary.area
		self.data['pos']		= temporary.pos

	def gate_center_x( self ):
		return ( self.result['cx_2'] + self.result['cx_1'] ) / 2

	def gate_center_y( self ):
		return ( self.result['cy_2'] + self.result['cy_1'] ) / 2

##============================> SERVICE VISION QUALIFICATION <==================================
	def qualification_set_up( self ):
		self.individual_data	= self.qualification_data
		self.center_x			= self.qualification_center_x
		self.center_y			= self.qualification_center_y
		self.can_sum = [ 'cx_1' , 'cx_2' , 'cy_1' , 'cy_2' , 'area'  ]
		self.not_sum = []
		self.request_data = rospy.ServiceProxy( "/vision/qualification" , vision_srv_gate )
		self.echo_special = self.echo_specific
	
	def qualification_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data['n_obj']		= temporary.state
		self.data['cx_1']		= temporary.cx1
		self.data['cy_1']		= temporary.cy1
		self.data['cx_2']		= temporary.cx2
		self.data['cy_2']		= temporary.cy2
		self.data['area']		= temporary.area

	def qualification_center_x( self ):
		return ( self.result['cx_2'] + self.result['cx_1'] ) / 2

	def qualification_center_y( self ):
		return ( self.result['cy_2'] + self.result['cy_1'] ) / 2

##================================> SERVICE VISION FLARE <=======================================
	def flare_set_up( self ):
		self.individual_data	= self.flare_data
		self.center_x			= self.flare_center_x
		self.center_y			= self.flare_center_y
		self.can_sum = [ 'cx' , 'cy' , 'area' ]
		self.not_sum = []
		self.request_data = rospy.ServiceProxy( "/vision/flare" , vision_srv_flare )
		
	def flare_data( self , task , request ):
		temporary = self.request_data( String( task ) , String( request ) ).data
		self.data['n_obj']		= temporary.state
		self.data['cx']			= temporary.cx
		self.data['cy']			= temporary.cy
		self.data['area']		= temporary.area

	def flare_center_x( self ):
		return self.result['cx']

	def flare_center_y( self ):
		return self.result['cy']

