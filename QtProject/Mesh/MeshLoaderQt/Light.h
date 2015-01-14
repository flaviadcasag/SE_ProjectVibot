#ifndef LIGHT_H
#define LIGHT_H

#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;

class Light
{
public:
    //Constructor
    Light();

    //Set position of the light
    void setPosition(float posX, float posY, float posZ);

    //Set position of the camera passing the camera position
    void setPositionByCamera(float posX, float posY, float posZ);

    //Set direction of the light
    void setDirection(float posX, float posY, float posZ);

    //Get specular parameters
    float* getSpecular() {return specular;}

    //get ambient light parameters
    float* getAmbient() {return ambient;}

    //get diffuse light parameters
    float* getDiffuse() {return diffuse;}

    //get shininess parameter
    float getShine() {return shine;}

    //get direction of the lights
    float* getDirection() {return dir;}

private:
    //specular parameters
    float specular[4];

    //shininess
    float shine;

    //ambient light
    float ambient[4];

    //diffuse light
    float diffuse[4];

    //light position
    float position[3];

    //light direction
    float dir[3];
};

#endif // LIGHT_H
