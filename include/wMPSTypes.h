/**
* This file is part of ORB-SLAM3
*
* Copyright (C) 2021 Ze Yang
*/


#ifndef WMPSTYPES_H
#define WMPSTYPES_H

#include<vector>
#include<utility>
#include<opencv2/core/core.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <mutex>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>


namespace wMPS
{

class Point
{
public:
    Point(const float &p_x, const float &p_y, const float &p_z,
             const double &timestamp): p(p_x,p_y,p_z), t(timestamp){}
    Point(const cv::Point3f position, const double &timestamp):
                                       p(position),    t(timestamp){}
public:
    cv::Point3f p;
    double t;
};

struct Bearing
{
    int channel;
    int transmitter;
    double time;
    Eigen::Vector3d Bearing;
    Eigen::Vector2d Angle;
};

}

#endif // WMPSTYPES_H
