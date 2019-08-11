#ifndef __ONEBOT_CONFIG__
#define __ONEBOT_CONFIG__

#include <cmath>

/* 
 * Preset Types
 */
enum MotorCloseloopType
{
    CLOSELOOP_VELOCITY,
    CLOSELOOP_POSITION
};

struct MotorPreset
{
    int PWMMaxValue;
    double RPMToRad;
    double TickToRad;
};

struct MotorParamter
{
    MotorCloseloopType CloseloopType;

    bool DoCalibration;
    double CalibrateCurrent;
};

struct MotionMotor
{
    int ID_Main;
    int ID_Sub;

    const MotorPreset *Preset;
    MotorParamter Paramter;
};

/* 
 * Robot Hardware Config
 */

#define ROBOT_SAMPLING_RATE 200

#define HW_MOTOR_COUNT 10
#define HW_CAN_MOTOR_ID_1 0x200
#define HW_CAN_MOTOR_ID_2 0x1FF
#define HW_CAN0_ID "can0"
#define HW_CAN1_ID "can1"

const double MOTOR_CALIBRATION_THRESHOLD = 0.628;
const double MOTOR_CALIBRATION_DURATION  = 1.0;

/*
 * Different Motor Presets
 */
const MotorPreset MOTOR_GM2006 = {
    10000,                                           //PWM Max Value
    (1.0 / 60.0) * 2.0 * M_PI / 36.0,                //RPM To rad/s
    (1.0 / 8192.0) * 2.0 * M_PI / 36.0               //TICK to rad
};

const MotorPreset MOTOR_GM3508 = {
    16384,
    (1.0 / 60.0) * 2.0 * M_PI / (3591.0 / 187.0),
    (1.0 / 8192.0) * 2.0 * M_PI / (3591.0 / 187.0)};

const MotorPreset MOTOR_GM3510 = {
    29000,
    (1.0 / 60.0) * 2.0 * M_PI,
    (1.0 / 8192.0) * 2.0 * M_PI};

/*
 * Default Motor Paramter
 */
const MotorParamter MOTOR_PARAMTER_DEFAULT = {
    CLOSELOOP_VELOCITY,
    false,
    0.0
};

/*
 * Chassis Paramters (SI Unit)
 */

#define MOTOR_CHASSIS MOTOR_GM3508
#define MOTOR_CHASSIS_PARAMTER MOTOR_PARAMTER_DEFAULT

const double CHASSIS_WATCHDOG_TIMEOUT = 1.0;

const double CHASSIS_WHEEL_R = 0.076;
const double CHASSIS_LENGTH_A = 0.55;
const double CHASSIS_LENGTH_B = 0.285;

/*
 * Motion Paramters
 */
#define MOTION_MOTOR_COUNT 6
const double MOTION_WATCHDOG_TIMEOUT = 1.0;

const MotionMotor MOTION_MOTOR_PRESET[MOTION_MOTOR_COUNT] = {
    //Main screw pole
    {
        4, -1,
        &MOTOR_GM3508,

        {
            CLOSELOOP_POSITION,
            true,
            0.04
        }
    },

    //Place ball 0
    {
        5, -1,
        &MOTOR_GM2006,

        {
            CLOSELOOP_POSITION,
            false,
            0.0
        }
    },

    //Place ball 1
    {
        6, -1,
        &MOTOR_GM2006,

        {
            CLOSELOOP_POSITION,
            false,
            0.0
        }
    },

    //Cup release screw pole
    {
        7, -1,
        &MOTOR_GM2006,

        {
            CLOSELOOP_POSITION,
            false,
            0.0
        }
    },

    //Cup move screw pole
    {
        8, 9,
        &MOTOR_GM2006,

        {
            CLOSELOOP_POSITION,
            true,
            0.05
        }
    },

    {
        9, -1,
        &MOTOR_GM2006,

        {
            CLOSELOOP_POSITION,
            true,
            0.05
        }
    },
};

#endif
