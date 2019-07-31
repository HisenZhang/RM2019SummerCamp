#include "chassis.h"
#include "hardware.h"

int main(int argc, char **argv)
{
	ros::init(argc, argv, "chassis");

	ros::NodeHandle nh;

	//Create Nodes
	Chassis chassis;

	ros::Rate loop_rate(200);

	//Process Jobs
	while (ros::ok())
	{
		//Update Subnodes
		chassis.update();

		//Update Hardware
		Hardware()->update();

		//Spin
		ros::spinOnce();
		loop_rate.sleep();
	}

	//Release Nodes

	//Release Hardware
	ReleaseHardware();
	return 0;
}