#include "positionctl.h"

double YawFromQuaternion(const geometry_msgs::Quaternion &quat) {
    tf::Quaternion q_orient(quat.x, quat.y, quat.z, quat.w);
	tf::Matrix3x3  m_orient(q_orient);

    double roll, pitch, yaw;
    m_orient.getRPY(roll, pitch, yaw);

    return yaw;
}

double AngularMinus(double a, double b) {
    double res1 = a - b;
    double res2 = (a < b) ? (a + 2 * M_PI - b) : (a - 2 * M_PI - b);

    return ( std::abs(res1) < std::abs(res2) ) ? res1 : res2;
}

PositionCtl::PositionCtl() {
	ros::NodeHandle node_priv;

    //Setup PID Paramters
    Kp_X = A_X = 0;
    Kp_Y = A_Y = 0;
    Kp_W = A_W = 0;

    //Setup Position Paramters
    X = Y = W = 0;
    Set_X = Set_Y = Set_W = 0;

	//Setup Reconfigurable Paramters
	static ros::NodeHandle DynamicParamNodeHandle("~/position");
	static dynamic_reconfigure::Server<rcbigctl::ControllerConfig> DynamicParamServer(DynamicParamNodeHandle);
	DynamicParamServer.setCallback(boost::bind(&PositionCtl::CallbackDynamicParam, this, _1, _2));

	//Setup Comm
	pos_sub         = node_priv.subscribe<nav_msgs::Odometry>     ("odom",   100, &PositionCtl::CallbackPosition, this);
	setpoint_sub    = node_priv.subscribe<geometry_msgs::Pose>    ("setpos", 100, &PositionCtl::CallbackSetpoint, this);
	twist_pub	    = node_priv.advertise<geometry_msgs::Twist>   ("velocity", 100);
}

PositionCtl::~PositionCtl() {}

void PositionCtl::update() {

}

void PositionCtl::CallbackDynamicParam(rcbigctl::ControllerConfig &config, uint32_t level) {
    ROS_INFO("Position Controller Reconfigure: [Kp_X = %lf, A_X = %lf, Kp_Y = %lf, A_Y = %lf, Kp_W = %lf, A_W = %lf]",
        config.Kp_X, config.A_X,
        config.Kp_Y, config.A_Y,
        config.Kp_W, config.A_W
    );

    Kp_X = config.Kp_X;
    A_X  = config.A_X;

    Kp_Y = config.Kp_Y;
    A_Y  = config.A_Y;

    Kp_W = config.Kp_W;
    A_W  = config.A_W;
}

void PositionCtl::UpdateCloseloop() {
    geometry_msgs::Twist twist;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;

    tf::Stamped<tf::Vector3> vWorld;
    vWorld.x = Kp_X * (Set_X - X);
    vWorld.y = Kp_Y * (Set_Y - Y);
    vWorld.z = 0;

    //Transform World Velocity To Robot Velocity
    tf::Stamped<tf::Vector3> vRobot;
    try {
        tf_pos.transformVector("/base_link", ros::Time(0), vWorld, "/world", vRobot);
    }
    catch (tf::TransformException ex) {
        ROS_ERROR("%s",ex.what());
    }

    twist.linear.x  = FoutX.filter( vRobot.x ,  A_X );
    twist.linear.y  = FoutY.filter( vRobot.y ,  A_Y );
    twist.angular.z = FoutW.filter( Kp_W * AngularMinus( W, Set_W ) , A_W );

    twist_pub.publish( twist );
}

void PositionCtl::CallbackPosition(const nav_msgs::Odometry::ConstPtr& odom) {
    X = odom->pose.pose.position.x;
    Y = odom->pose.pose.position.y;
    W = fmod( YawFromQuaternion(odom->pose.pose.orientation), 2 * M_PI );

    UpdateCloseloop();
}

void PositionCtl::CallbackSetpoint(const geometry_msgs::Pose::ConstPtr& pose) {
    Set_X = pose->position.x;
    Set_Y = pose->position.y;
    Set_W = fmod( YawFromQuaternion(pose->orientation), 2 * M_PI );
}