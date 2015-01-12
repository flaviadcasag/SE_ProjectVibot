#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "NeighborMesh.h"

using namespace Eigen;

class Camera
{
public:
    Camera();

    Vector3d getTarget();
    Vector3d getPosition();

    //Init camera and return the distance from the camera to the object
    double initCamera(NeighborMesh& mesh);

    double getZnear ();
    double getZfar ();

private:
    Vector3d position;
    Vector3d target;
    double znear;
    double zfar;
};

#endif // CAMERA_H
