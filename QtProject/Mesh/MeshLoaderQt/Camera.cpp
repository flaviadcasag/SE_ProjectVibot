#include "Camera.h"

//Constructor
Camera::Camera()
{
}

//Get camera targer
Vector3d Camera::getTarget()
{
    return target;
}

//Get distance from the mesh
double Camera::getDistance()
{
    return distance;
}

//get camera position
Vector3d Camera::getPosition()
{
    return position;
}

//Init camera parameters
//This function also sets the extreme points in the mesh
void Camera::initCamera(NeighborMesh& mesh)
{
    //roughly adjust view frustrum to object and camera position
    Vector3d Pmin(mesh.getVertex(0));
    Vector3d Pmax(mesh.getVertex(0));
    Vector3d Center(0,0,0);
    Vector3d mins = Vector3d(0,0,0);
    Vector3d maxs = Vector3d(0,0,0);
    for( int i=0; i< mesh.VertexNumber(); i++)
    {
        Vector3d P(mesh.getVertex(i));
        for( int j=0; j<2; j++)
        {
            if (P[j] < Pmin[j])
            {
                Pmin[j] = P[j];
                mins[j] = i;
            }

            if (P[j] > Pmax[j])
            {
                Pmax[j] = P[j];
                maxs[j] = i;
            }
        }
        Center += P;
    }

    //set max and min
    mesh.setMaxs(maxs);
    mesh.setMins(mins);

    Center/=mesh.VertexNumber();
    target = Center;

    //length of the diagonal of the bouding box
    distance = (Pmax-Pmin).norm();

    //set arbitraty position to camera and adjusts max and min view planes
    position = target + Vector3d(0,0,distance*3);
    znear = distance*0.1;
    zfar = distance*5;
}

//get z near parameter from the camera
double Camera::getZnear ()
{
    return znear;
}

//get z far parameter from the camera
double Camera::getZfar ()
{
    return zfar;
}
