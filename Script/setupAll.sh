#!/bin/bash

control_c() {
    kill -TERM $$
    # exit
}

trap control_c SIGINT

sudo ~/jetson_clocks.sh
echo "================= FULL POWER ENABLED ================="
sleep 2

sudo ~/RM2019SummerCamp/Script/setupCAN.sh
echo "================= CAN INITIALIZED ================="
sleep 2


# TODO get children pid and set nice priority

roslaunch ~/RM2019SummerCamp/Script/setupVision.launch 2> /dev/null &
sleep 3
pstree $! -p| awk -F"[()]" '{print $2}'| xargs sudo renice -20 -p"

echo "================= VISION ENABLED ================="


roslaunch ~/RM2019SummerCamp/Script/setupController.launch &
sleep 3
pstree $! -p| awk -F"[()]" '{print $2}'| xargs sudo renice -15 -p"

echo "================= CONTROLLER ENABLED ================="

# echo "############################"
# echo "############################"
# echo "####                    ####"
# echo "############################"
# echo "############################"

while true ; do 
   sleep 1
done


# sleep 365d
# pstree pid -p| awk -F"[()]" '{print $2}'| xargs kill -9