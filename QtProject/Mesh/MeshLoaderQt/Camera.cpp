#include "Camera.h"

Camera::Camera()
{
}


Vector3d Camera::getTarget()
{
    return target;
}

Vector3d Camera::getPosition()
{
    return position;
}

double Camera::initCamera(const NeighborMesh& mesh)
{
    //roughly adjust view frustrum to object and camera position
    Vector3d Pmin(mesh.vertices[0]), Pmax(mesh.vertices[0]);
    Vector3d Center(0,0,0);

    for( int i=0; i< mesh.vertices.size(); i++)
    {
        Vector3d P(mesh.vertices[i]);
        for( int j=0; j<2; j++)
        {
            Pmin[j] = min(Pmin[j],P[j]);
            Pmax[j] = max(Pmax[j],P[j]);
        }
        Center += P;
    }

    Center/=mesh.vertices.size();
    target = Center;

    //length of the diagonal of the bouding box
    double distance = (Pmax-Pmin).norm();

    //set arbitraty position to camera and adjusts max and min view planes
    position = target + Vector3d(0,0,distance*3);
    znear = distance*0.1;
    zfar = distance*5;

    return distance;
}

double Camera::getZnear ()
{
    return znear;
}

double Camera::getZfar ()
{
    return zfar;
}
