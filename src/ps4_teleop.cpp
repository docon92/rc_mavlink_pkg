#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sensor_msgs/Joy.h>
#include <mavlink_pkg/SetPositionTargetLocalNED.h>
#include "mavlink/common/mavlink.h"
#include <sstream>

mavlink_pkg::SetPositionTargetLocalNED VelCmd_;
std::string ControllerType_;

void JoyCallback(const sensor_msgs::Joy::ConstPtr& joy);
float SaturateCommand(float cmd,float max);


void JoyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
   float left_stick_up_, left_stick_side_,right_stick_up_,right_stick_side_;
    //xbox360 wired
    // left_stick_up = joy->axes[1];
	// left_stick_side = joy->axes[0];
	// right_stick_up = joy->axes[4];
	// right_stick_side = joy->axes[3];


/*    //ps4 bluetooth
    left_stick_up_ = joy->axes[1];
	left_stick_side_ = joy->axes[0];
	right_stick_up_ = joy->axes[5];
	right_stick_side_ = joy->axes[2];*/

	//ps4 wired
    left_stick_up_ = joy->axes[1];
	left_stick_side_ = joy->axes[0];
	right_stick_up_ = joy->axes[4];
	right_stick_side_ = joy->axes[3];

	VelCmd_.vx=right_stick_up_;
	VelCmd_.vy=right_stick_side_;
	VelCmd_.vz=-left_stick_up_;
	VelCmd_.yaw_rate=left_stick_side_;

    //CustomControl::SetCmd();

    //ROS_INFO("zd: %f,yaw command: %f,ax0: %f,ax1: %f",zd,yaw_cmd,joy->axes[0],joy->axes[1]);

}


/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char *argv[])
{
	/**
	 * The ros::init() function needs to see argc and argv so that it can perform
	 * any ROS arguments and name remapping that were provided at the command line.
	 * For programmatic remappings you can use a different version of init() which takes
	 * remappings directly, but for most command-line programs, passing argc and argv is
	 * the easiest way to do it.  The third argument to init() is the name of the node.
	 *
	 * You must call one of the versions of ros::init() before using any other
	 * part of the ROS system.
	 */
	ros::init(argc, argv, "ps4_teleop");

	/**
	 * NodeHandle is the main access point to communications with the ROS system.
	 * The first NodeHandle constructed will fully initialize this node, and the last
	 * NodeHandle destructed will close down the node.
	 */
	ros::NodeHandle nh;
	 nh.param("ControllerType", ControllerType_,std::string("PS4_WIRED"));
	 ROS_INFO("Starting Remote Control Using %s",ControllerType_.c_str());
	/**
	 * The advertise() function is how you tell ROS that you want to
	 * publish on a given topic name. This invokes a call to the ROS
	 * master node, which keeps a registry of who is publishing and who
	 * is subscribing. After this advertise() call is made, the master
	 * node will notify anyone who is trying to subscribe to this topic name,
	 * and they will in turn negotiate a peer-to-peer connection with this
	 * node.  advertise() returns a Publisher object which allows you to
	 * publish messages on that topic through a call to publish().  Once
	 * all copies of the returned Publisher object are destroyed, the topic
	 * will be automatically unadvertised.
	 *
	 * The second parameter to advertise() is the size of the message queue
	 * used for publishing messages.  If messages are published more quickly
	 * than we can send them, the number here specifies how many messages to
	 * buffer up before throwing some away.
	 */
	ros::Subscriber Joy_Sub_ = nh.subscribe<sensor_msgs::Joy>("joy",10,JoyCallback);
	ros::Publisher SetVelPub_ =  nh.advertise<mavlink_pkg::SetPositionTargetLocalNED>("mavlink/set/ned_position",10);

	VelCmd_.type_mask = static_cast<uint16_t>(0b0000001111000111);
	VelCmd_.coordinate_frame = MAV_FRAME_BODY_NED;
	VelCmd_.vx = 		0.0f;
	VelCmd_.vy = 		0.0f;
	VelCmd_.vz=			0.0f;
	VelCmd_.yaw_rate=	0.0f;
	

	ros::Rate loop_rate(10);
	
	while (ros::ok())
	{

		SetVelPub_.publish(VelCmd_);

		ros::spinOnce();

		loop_rate.sleep();

	}

	return 0;
}