#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "NeighborMesh.h"

using namespace Eigen;

//class that contains the camera parameters
class Camera
{
public:
    //Constructor
    Camera();

    //get camera target
    Vector3d getTarget();

    //get camera position
    Vector3d getPosition();

    //Init camera parameters
    //This function also sets the extreme points in the mesh
    void initCamera(NeighborMesh& mesh);

    //Get Z near
    double getZnear ();

    //Get Z far
    double getZfar ();

    //Get distance from the mesh
    double getDistance();

private:
    //Position of the camera
    Vector3d position;

    //Target of the camera
    Vector3d target;

    //z near plane
    double znear;

    //z far plane
    double zfar;

    //distance from the mesh
    double distance;
};

#endif // CAMERA_H
