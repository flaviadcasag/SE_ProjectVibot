#include "light.h"

//constructor
Light::Light()
{
    //setting the default parameters of the light
    specular[0] = 1.0;
    specular[1] = 1.0;
    specular[2] = 1.0;
    specular[3] = 1.0;

    shine = 100;

    //ambient and diffuse light
    ambient[0] = 0.0;
    ambient[1] = 0.0;
    ambient[2] = 0.0;
    ambient[3] = 1.0;

    diffuse[0] = 0.4;
    diffuse[1] = 0.4;
    diffuse[2] = 0.4;
    diffuse[3] = 1.0;
}

//set position of the light
void Light::setPosition(float posX, float posY, float posZ)
{
    position[0] = posX;
    position[1] = posY;
    position[2] = posZ;
}

//set position using the camera position
void Light::setPositionByCamera(float posX, float posY, float posZ)
{
    position[0] = posX;
    position[1] = posY;
    position[2] = posZ+10;
}

//Set camera direction
void Light::setDirection(float posX, float posY, float posZ)
{
    dir[0] = posX;
    dir[1] = posY;
    dir[2] = posZ;
}

