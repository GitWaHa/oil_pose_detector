#!/usr/bin/env python
#coding=utf-8

import os
import sys
import copy
import math
import threading

import rospy
import moveit_commander
import moveit_msgs.msg
import geometry_msgs.msg

import tf
import tf.transformations as tf_trans

import tf2_ros as tf2
import tf2_geometry_msgs as tf2_gm

from oil_pose_server import OilPoseServer
from oil_app import OilApp

if __name__ == "__main__":
    moveit_commander.roscpp_initialize(sys.argv)
    rospy.init_node('oil_app', anonymous=True)

    robot = moveit_commander.RobotCommander()

    scene = moveit_commander.PlanningSceneInterface()

    group_name = "arm"
    move_group = moveit_commander.MoveGroupCommander(group_name)
    move_group.set_max_velocity_scaling_factor(0.4)  # 设置最大关节速度
    # move_group.set_goal_joint_tolerance(0.1)

    tag_server = OilPoseServer("base_link", "aruco_marker_frame")
    oil = OilApp(tag_server, "aruco")

    # # 回到home
    # move_group.set_named_target("home")
    # if (not move_group.go()):
    #     rospy.logerr("home is go filed")
    #     sys.exit()
    # move_group.clear_pose_targets()
    # rospy.sleep(2)

    # # 设置为寻找二维码时的关节
    # move_group.set_named_target("oil_look_pcd")
    # if (not move_group.go()):
    #     rospy.logerr("oil_look is go filed")
    #     sys.exit()
    # move_group.clear_pose_targets()
    # rospy.sleep(2)

    # 识别二维码,　并转换到加油口
    is_find, pose_oil = oil.findOilHole()
    if (not is_find):
        print(" \033[1;31m ArTag is not find \033[0m")
        sys.exit()
    print("[findOilHole] pose_oil", pose_oil)

    # sys.exit()

    # 沿着加油口坐标系 后退一定距离作为加油起始点
    pose_out = oil.translationPoseFromEnd(pose_oil, OilApp.Z, -0.3)
    print("[transformPoseFromEnd] pose_out", pose_out)
    move_group.set_pose_target(pose_out)
    if (not move_group.go()):
        print(" \033[1;31m pose_out move_group.go() is failed \033[0m")
        sys.exit()

    rospy.sleep(2)

    move_group.set_goal_joint_tolerance(0.1)
    max_plan = 10
    ## 笛卡尔路径规划，加油动作
    for i in range(max_plan):
        (plan, fraction) = oil.oilCartesianPath(move_group,
                                                direction=OilApp.Z,
                                                distance_in=0.08)
        print("compute_cartesian_path fraction:", fraction)
        if (fraction >= 0.9):
            # print("compute_cartesian_path fraction:", fraction)
            plan = oil.scale_trajectory_speed(plan, 0.3)
            move_group.execute(plan)
            break
        else:
            rospy.logerr("compute_cartesian_path is failed")

    rospy.sleep(2)

    ## 笛卡尔路径规划，退出加油动作
    for i in range(max_plan):
        (plan, fraction) = oil.oilCartesianPath(move_group,
                                                direction=OilApp.Z,
                                                distance_in=-0.08)
        print("compute_cartesian_path fraction:", fraction)
        if (fraction >= 0.9):
            plan = oil.scale_trajectory_speed(plan, 0.3)
            move_group.execute(plan)
            break
        else:
            rospy.logerr("compute_cartesian_path is failed")
