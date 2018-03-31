#!usr/bin/env python

import numpy as np
import rospy
from geometry_msgs.msg import Vector3
from sensor_msgs.msg import Joy

#pygame.display.init()
#pygame.joystick.init()
#pygame.joystick.Joystick(0).init()

jakobian = np.zeros((3,3))
inverseJakobian = np.zeros((3,3))
w = [0,0,0]
v = [0, 0, 0]
maxSpeed_x = 0.3
maxSpeed_y = 0.3
maxSpeed_z = 0.3 # This value needs to be calibrated
l1 = 0.331
l2 = 0.242

msg = Vector3(1,1,1)

pub = rospy.Publisher('omegas', Vector3, queue_size = 100)

def joyCallback(joy):
	v[0] = joy.axes[0]*maxSpeed_x
	v[1] = joy.axes[1]*maxSpeed_y
	v[2] = joy.axes[3]*maxSpeed_z

rospy.Subscriber("joy", Joy, joyCallback)

def callback_bysub(data):
	t1 = data.x*0.0175
	t2 = data.y*0.0175
	t3 = data.z*0.0175
	t = [t1, t2, t3]
	
	jakobian [0, 0] = l2 * np.sin(t2)  * np.cos(t1) * np.cos(t3) - (l1 - l2 * np.cos(t2)) * np.sin(t1)
	jakobian [0, 1] = (l2 * np.sin(t2) * np.cos(t1)) + (l2 * np.cos(t2) *np.sin(t1) * np.cos(t3))
	jakobian [0, 2] = -l2 * np.sin(t2)  * np.sin(t1)  * np.sin(t3)

	jakobian [2, 0] = (l1 - l2 * np.cos(t2)) * np.cos(t1) + l2 * np.sin(t2)  * np.cos(t3) * np.sin(t1)
	jakobian [2, 1] = l2 * np.sin(t2)  * np.sin(t1) - l2 * np.cos(t2) * np.cos(t3) * np.cos(t1)
	jakobian [2, 2] = l2 * np.sin(t2)  * np.sin(t3)  * np.cos(t1)
	
	jakobian [1, 0] = 0
	jakobian [1, 1] = l2 * np.cos(t2) * np.sin(t3)
	jakobian [1, 2] = l2 * np.sin(t2)  * np.cos(t3)
	
	if(np.linalg.det(jakobian) == 0):
		global w
		w[0] = 0
		w[1] = 0
		w[2] = 0
	else:
		inverseJakobian = np.linalg.inv(jakobian)
		global w
		w = np.matmul(inverseJakobian, v)
	global msg
	msg = Vector3(float(w[0]), float(w[1]), float(w[2]))
	pub.publish(msg)

def Start():
	rospy.init_node('arm', anonymous = True)
	rospy.Subscriber("feedback", Vector3, callback_bysub)
	rospy.spin()

if __name__ == '__main__':
	try:
		Start()
	except  rospy.ROSInterruptException:
		pass
