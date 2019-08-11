#ifndef ACTION_EXECUTER
#define ACTION_EXECUTER

#include "global_planner.h"
#include "mechanical_definition.h"
#include "mechanical_executer.h"

#include <tf/tf.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>

//position threshold
const double POSITION_THRESHOLD = 0.05;
//angle threshold
const double ANGLE_THRESHOLD = 5.0 / 180.0 * M_PI;

double YawFromQuaternion(const tf::Quaternion &q_orient)
{
    tf::Matrix3x3 m_orient(q_orient);

    double roll, pitch, yaw;
    m_orient.getRPY(roll, pitch, yaw);

    return yaw;
}

double AngularMinus(double a, double b)
{
    a = fmod(a, 2.0 * M_PI);
    b = fmod(b, 2.0 * M_PI);

    double res1 = a - b;
    double res2 = (a < b) ? (a + 2 * M_PI - b) : (a - 2 * M_PI - b);

    return (std::abs(res1) < std::abs(res2)) ? res1 : res2;
}

namespace ActionExecuter
{
    //Communication
    ros::Publisher  setpoint_pub;

    geometry_msgs::PoseStamped setpoint_pose;

    //Actions
    GlobalPlanner::ActionList ActionList;
    GlobalPlanner::Action CurAction;
    ros::Time CurActionLastTime;

    void LoadActionList(const GlobalPlanner::ActionList &_ActionList) {
        ActionList = _ActionList;

        CurAction.type = GlobalPlanner::ACTION_NONE;
    }

    bool CurActionFinished()
    {
        if (CurAction.type == GlobalPlanner::ACTION_NONE)
            return true;

        //Mechanical Busy
        if(MechanicalExecuter::IsBusy())
            return false;

        //Read Pose
        static tf::TransformListener tf_listener;
        tf::StampedTransform tf_map_base;
        try {
            tf_listener.lookupTransform("map", "base", ros::Time(0), tf_map_base);
        }
        catch (tf::TransformException ex) {
            ROS_ERROR("%s",ex.what());
            return false;
        }

        double pose_x   = tf_map_base.getOrigin().getX();
        double pose_y   = tf_map_base.getOrigin().getY();
        double pose_yaw = YawFromQuaternion(tf_map_base.getRotation());

        //Angle settings
        if (CurAction.yaw_enabled) {
            if (fabs( AngularMinus(CurAction.world_yaw, pose_yaw ) ) > ANGLE_THRESHOLD)
                return false;
        }

        //Position settings
        if( 
            (fabs( pose_x - CurAction.world_x ) > POSITION_THRESHOLD) ||
            (fabs( pose_y - CurAction.world_y ) > POSITION_THRESHOLD)
        ) {
            return false;
        }

        //Countdown
        if(CurAction.countdown > 0) {
            double dt = (ros::Time::now() - CurActionLastTime).toSec();
            CurActionLastTime = ros::Time::now();

            CurAction.countdown -= dt;
        }

        //Timeout
        bool isTimeout = CurAction.countdown <= 0;
        if(isTimeout) return true;

        //Wait for feedback
        bool Feedback_Done = true;

        switch(CurAction.type) {
            case GlobalPlanner::ACTION_OCCUPY:
                Feedback_Done = GlobalBoard.board_state(CurAction.feedback.grid_x, CurAction.feedback.grid_y) == WhoAmI;
            break;

            case GlobalPlanner::ACTION_PLACEBALL:
            case GlobalPlanner::ACTION_PLACECUP:
                Feedback_Done = GlobalBoard.castle[CurAction.feedback.castle_id] != CurAction.feedback.castle_last_value;
            break;
        }

        if(!Feedback_Done) return false;

        //Else complete
        return true;
    }

    void update()
    {
        //broadcast pose
        if(CurAction.type != GlobalPlanner::ACTION_NONE) {
            setpoint_pose.pose.position.x = CurAction.world_x;
            setpoint_pose.pose.position.y = CurAction.world_y;
            setpoint_pose.pose.position.z = 0;

            if(CurAction.yaw_enabled) {
                setpoint_pose.pose.orientation = tf::createQuaternionMsgFromYaw(CurAction.world_yaw);
            }

            setpoint_pose.header.stamp = ros::Time::now();
            setpoint_pose.header.frame_id = "map";
            setpoint_pub.publish( setpoint_pose );
        }

        //check finished
        if (!CurActionFinished())
            return;

        //get next action
        if(ActionList.empty()) return;

        CurAction = *ActionList.begin();
        ActionList.erase(ActionList.begin());

        //init next action
        CurActionLastTime = ros::Time::now();

        //do mechanical ops
        if(CurAction.type == GlobalPlanner::ACTION_PLACEBALL) {
            MechanicalDefinition::PlaceBall();
        }
        else if(CurAction.type == GlobalPlanner::ACTION_PLACECUP) {
            MechanicalDefinition::PlaceCup();
        }
    }

    void init() {
        //init mechanical executer & definition
        MechanicalExecuter::init();
        MechanicalDefinition::init();

        //init action executer
        ros::NodeHandle nh;

        setpoint_pub = nh.advertise<geometry_msgs::PoseStamped>("move_base_simple/goal", 100);

        setpoint_pose.pose.orientation = tf::createQuaternionMsgFromYaw(0.0);
    }
} // namespace ActionExecuter

#endif
