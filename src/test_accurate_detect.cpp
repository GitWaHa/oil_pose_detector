#include <ros/ros.h>
#include <pcl/segmentation/sac_segmentation.h>
// #include <pcl/sample_consensus/method_types.h>
// #include <pcl/sample_consensus/model_types.h>
#include <pcl/features/normal_3d.h>
#include <pcl/filters/extract_indices.h>

#include <pcl/io/ply_io.h>
#include <pcl/common/common.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/visualization/pcl_visualizer.h>

#include <tf/transform_datatypes.h>

#include "oil_detect/oil_accurate_detect.h"

void plotCoordinate(pcl::visualization::PCLVisualizer::Ptr &visualizer, const float *posi, const float *quat, int flag = 0)
{
    pcl::PointXYZ p; //存储球的圆心位置
    p.x = posi[0];
    p.y = posi[1];
    p.z = posi[2];

    tf::Quaternion quat_;
    auto q = quat;
    quat_.setX(q[0]);
    quat_.setY(q[1]);
    quat_.setZ(q[2]);
    quat_.setW(q[3]);

    tf::Matrix3x3 martrix(quat_);

    // 坐标系
    std::vector<double> r = {1, 0, 0};
    std::vector<double> g = {0, 1, 0};
    std::vector<double> b = {0, 0, 1};
    for (int i = 0; i < 3; i++)
    {
        pcl::PointXYZ tmp;
        if (flag != 0 && i == 2)
        {
            tmp.x = p.x - martrix.getColumn(i).getX() * 0.1;
            tmp.y = p.y - martrix.getColumn(i).getY() * 0.1;
            tmp.z = p.z - martrix.getColumn(i).getZ() * 0.1;
        }
        else
        {
            tmp.x = p.x + martrix.getColumn(i).getX() * 0.1;
            tmp.y = p.y + martrix.getColumn(i).getY() * 0.1;
            tmp.z = p.z + martrix.getColumn(i).getZ() * 0.1;
        }

        visualizer->addArrow(tmp, p, r[i], g[i], b[i], false, "arrow" + std::to_string(i));
    }
}

int main(int argn, char **argv)
{
    ros::init(argn, argv, "split_plane");

    OilAccurateDetect detector;

    // 读取点云
    pcl::PointCloud<pcl::PointXYZ>::Ptr raw_cloud(new pcl::PointCloud<pcl::PointXYZ>);
    // pcl::io::loadPLYFile("/home/waha/Desktop/test_data/tsdf_cloud.ply", *raw_cloud);
    pcl::io::loadPCDFile("/home/waha/Desktop/test_data/cloud.pcd", *raw_cloud);

    detector.detect_once(raw_cloud);

    pcl::visualization::PCLVisualizer::Ptr visualizer0(new pcl::visualization::PCLVisualizer("getOriginalCloud"));
    visualizer0->setBackgroundColor(1, 1, 1);
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud0(detector.getOriginalCloud(), 0, 0, 0);
    visualizer0->addPointCloud(detector.getOriginalCloud(), cloud0, "getOriginalCloud");

    // 显示
    pcl::visualization::PCLVisualizer::Ptr visualizer1(new pcl::visualization::PCLVisualizer("getVoxelCloud"));
    visualizer1->setBackgroundColor(1, 1, 1);
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud1(detector.getVoxelCloud(), 0, 0, 0);
    visualizer1->addPointCloud(detector.getVoxelCloud(), cloud1, "getVoxelCloud");

    // plotCoordinate(visualizer1, detector.getPosition(), detector.getQuaternion());

    // 显示
    pcl::visualization::PCLVisualizer::Ptr visualizer2(new pcl::visualization::PCLVisualizer("getPlaneCloud"));
    visualizer2->setBackgroundColor(1, 1, 1);
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud2(detector.getPlaneCloud(), 0, 0, 0);
    visualizer2->addPointCloud(detector.getPlaneCloud(), cloud2, "getPlaneCloud");

    // 显示
    pcl::visualization::PCLVisualizer::Ptr visualizer3(new pcl::visualization::PCLVisualizer("getNotPlaneCloud"));
    visualizer3->setBackgroundColor(1, 1, 1);
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud3(detector.getNotPlaneCloud(), 0, 0, 0);
    visualizer3->addPointCloud(detector.getNotPlaneCloud(), cloud3, "getNotPlaneCloud");

    // 显示
    pcl::visualization::PCLVisualizer::Ptr visualizer4(new pcl::visualization::PCLVisualizer("getOilCloud"));
    visualizer4->setBackgroundColor(1, 1, 1);
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud4(detector.getOilCloud(), 0, 0, 0);
    visualizer4->addPointCloud(detector.getOilCloud(), cloud4, "getOilCloud");
    plotCoordinate(visualizer4, detector.getPosition(), detector.getQuaternion());

    // 显示
    pcl::visualization::PCLVisualizer::Ptr visualizer5(new pcl::visualization::PCLVisualizer("getOilCloud2"));
    visualizer5->setBackgroundColor(1, 1, 1);
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud5(detector.getOilCloud(), 0, 0, 0);
    visualizer5->addPointCloud(detector.getOilCloud(), cloud5, "getOilCloud2");
    // plotCoordinate(visualizer4, detector.getPosition(), detector.getQuaternion());

    // 显示
    pcl::visualization::PCLVisualizer::Ptr visualizer6(new pcl::visualization::PCLVisualizer("getOilCloud3"));
    visualizer6->setBackgroundColor(1, 1, 1);
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud6(detector.getOilCloud(), 0, 0, 0);
    visualizer6->addPointCloud(detector.getOilCloud(), cloud6, "getOilCloud3");
    plotCoordinate(visualizer6, detector.getPosition(), detector.getQuaternion(), 1);

    while (ros::ok())
    {
        visualizer1->spinOnce(10);
        visualizer2->spinOnce(10);
        visualizer3->spinOnce(10);
        visualizer4->spinOnce(10);
    }

    return 0;
}