#!/usr/bin/env python

import rospy
#from modbus_ascii_ros.msg import Pwm
from hg_ros_pololu.msg import Pwm
from geometry_msgs.msg import Twist
from geometry_msgs.msg import TwistWithCovarianceStamped
from sensor_msgs.msg import Joy
from numpy  import *
import LookUpPWM_2018 as lup
"""
t = [t0, t1, t2, t3, t4, t5, t6, t7]
F = [Fx, Fy, Fz, Mx, My, Mz]
"""

pwm_publisher = 0
d = 0 #d = direction of each thruster
r = 0 #r = distance from origin to each thruster
T = 0 #T = matrix map thruster forces to robot force
M = 0 #M = matrix map robot force to thruster forces

class thrust_mapper:
	def __init__(self):
		rospy.init_node('thrust_mapper')
		self.pwm_publisher = rospy.Publisher('/pwm', Pwm, queue_size=10)
		rospy.Subscriber("/cmd_vel", Twist, self.joy_callback)
		self.print_data = False;
		if(self.print_data):
			print '============d============='
	#green robot
		self.d = array([[0, 0, 1],
			[0, 0, 1],
			[0, 0, 1],
			[0, 0, 1],
			[math.cos(math.radians(45)), -math.sin(math.radians(45)), 0],
			[math.cos(math.radians(45)), math.sin(math.radians(45)), 0],
			[-math.cos(math.radians(45)), -math.sin(math.radians(45)), 0],
			[-math.cos(math.radians(45)), math.sin(math.radians(45)), 0]])
#			[0, 0, 0]])

		if(self.print_data):
			print self.d

			print '\n============r============='

		self.r = array([[ 0.332,  0.2202, -0.023],   #thruster 1
	               	[ 0.332, -0.2202, -0.023],   #thruster 2
					[-0.332,  0.2202, -0.023],   #thruster 3  
                   [-0.332, -0.2202, -0.023],   #thruster 4
					[ 0.3536,  0.3536, -0.023],  #thruster 5
		   			[ 0.3536, -0.3536, -0.023],  #thruster 6
					[-0.3536,  0.3536, -0.023],  #thruster 7
					[-0.3536, -0.3536, -0.023]]) #thruster 8
#					[0, 0, 0]]) #thruster 8

		self.min_force = array([	[ -0.048, 0.045]
								,	[ -0.048, 0.045]
								,	[ -0.048, 0.045]
								,	[ -0.048, 0.045]
								,	[ -0.01	, 0.01]
								,	[ -0.01	, 0.01]
								,	[ -0.01	, 0.01]
								,	[ -0.01	, 0.01]])

		if(self.print_data):
			print self.r

			print '\n============T============='
		self.T_tmp = array([cross(self.r[0].T, self.d[0].T),
		   cross(self.r[1].T, self.d[1].T),
		   cross(self.r[2].T, self.d[2].T),
		   cross(self.r[3].T, self.d[3].T),
		   cross(self.r[4].T, self.d[4].T),
		   cross(self.r[5].T, self.d[5].T),
		   cross(self.r[6].T, self.d[6].T),
		   cross(self.r[7].T, self.d[7].T)])
		self.T = concatenate((self.d.T, self.T_tmp.T))
		if(self.print_data):
			print self.T
	#M = matrix map robot force to thruster forces
	#M = [8 x 6]
			print '\n============M============='
		self.M = linalg.pinv(self.T)
		if(self.print_data):
			print self.M.shape
			print self.M

	def bound(self,cmd):
		x = []
		limit_bound = 450
		for i in cmd:
			if i > limit_bound:
				x.append(limit_bound)
			elif i < -limit_bound:
				x.append(-limit_bound)
			else:
				x.append(i)
		return x

	def joy_callback(self , message):
		pwm_command = Pwm()
		pwm_command.pwm = [1500]*8

	
		F = array([message.linear.x, message.linear.y, message.linear.z,
		   	message.angular.x, message.angular.y, message.angular.z])
#		F = array([message.linear.x * 2, message.linear.y * 2, message.linear.z,
#		   	message.angular.x, message.angular.y, message.angular.z])
		t = dot(self.M, F.T)

		for run in range( 0 , 8):
			if( t[run] < 0 ):
				if( t[run] > self.min_force[run][0]/2 ):
					t[run] = 0
				elif( t[run] > self.min_force[run][0]):
					t[run] = self.min_force[run][0]
			else:
				if( t[run] < self.min_force[run][1]/2 ):
					t[run] = 0
				elif( t[run] < self.min_force[run][1]):
					t[run] = self.min_force[run][1]

		print( "=============== last force ==============" )
		print( t )
		cmd = []
		for i in range (0,4):
			cmd.append(lup.lookup_pwm_02(t[i]))
			if( t[i] == 0 ):
				cmd[i] = 1500
		for i in range(4,8):
			cmd.append(lup.lookup_pwm_01(t[i]))
			if( t[i] == 0 ):
				cmd[i] = 1500


	#green robot
		force = 1
		pwm_command.pwm[0] = cmd[0]*force #500*t[0]; #thrust 1		       
		pwm_command.pwm[1] = cmd[3]*force #500*t[1]; #thrust 2
		pwm_command.pwm[2] = cmd[1]*force #500*t[2]; #thrust 3			 
		pwm_command.pwm[3] = cmd[3]*force #500*t[3]; #thrust 4			
		pwm_command.pwm[4] = cmd[4]*force #500*t[4]; #thrust 5			
		pwm_command.pwm[5] = cmd[5]*force #500*t[5]; #thrust 6			  
		pwm_command.pwm[6] = cmd[6]*force #500*t[6]; #thrust 7 			
		pwm_command.pwm[7] = cmd[7]*force #500*t[7]; #thrust 8		

		pwm = pwm_command.pwm
		print '=========== PWM ==========='
		print( str( pwm[0] ) + "\t" +	str( pwm[1] ) + "\t" 
			+  str( pwm[2] ) + "\t" +	str( pwm[3] ) + "\n" 
			+  str( pwm[4] ) + "\t" +	str( pwm[5] ) + "\t" 
			+  str( pwm[6] ) + "\t" +	str( pwm[7] ) )

		self.pwm_publisher.publish(pwm_command)
	
if __name__ == '__main__':
    x = thrust_mapper()
    #x.publishOdom()
    rospy.spin()

