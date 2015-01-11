#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL>
#include <QGLWidget>
#include <GL/glu.h>
#include <iostream>

#include <string>

#include "NeighborMesh.h"
#include "Stopwatch.h"
#include "Camera.h"
#include "Constante.h"


using namespace std;

class GLWidget : public QGLWidget
{
    //QT macro... I hate macros but cannot avoid this one
    Q_OBJECT


public:

    string fileName;


    //explicit constructor
    explicit GLWidget(QWidget *parent = 0);


    void initMesh();

    /*observe the virtual ... =0 notation for some functions*/

    //initialization
    void initializeGL();

    //function called when the widget is resized
    void resizeGL(int width, int height);

    //function called when the widget should be drawn or updated
    void paintGL();

    //Mouse motion
    void mouseMotion(int x, int y);

    //Mouse
    void mousePressEvent(QMouseEvent *event);

    //function called when a key is pressed
    void keyPressEvent( QKeyEvent *keyEvent );

    //Catch mouse movement
    void mouseMoveEvent(QMouseEvent * mouse);

    //Catch mouse release
    void mouseReleaseEvent(QMouseEvent* mouse);

    //Set the window name
    void setName(string name);

    //Set frames per second
    void setFramesPerSecond(float fps);

    //Load mesh
    void loadMesh(string filename);

    //Calculate the spectrum
    void calculateSpectrum(int frequency, bool flag);

    //Color the spectrum
    void colorSpectrum(int);

    NeighborMesh getGlobalMesh() {return globalMesh;}

    void smoothMesh(int frequency);

    void frequencyRemoval(int frequency);


public slots:
    void timeOutSlot();

private:
    //timer
    QTimer *t_Timer;

    //Window name
    string name;

    //frames per second
    float framesPerSecond;

    //frame counter
    int n;

    //the global mesh
    NeighborMesh globalMesh;

    //Timer
    Stopwatch timer;

    //Camera
    Camera cam;

    //Object move speed;
    Vector3d objectMove;

    //Create Light in the scene
    void createLighting();

    //Id of global mesh (OPENGL SETTINGS)
    int id_globalmesh;

    //Trackball mapping
    static Vector3d TrackballMapping( int x, int y );

    //Variable that controls if lights are on or not
    int lights_on;

    //Variable to control old values of x and y of the mouse
    int oldx,oldy;
    float Current_Matrix[4][4];// = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

    // flag of rotation and translation
    int rotate,translate;
    int Axis;
    int Axis_Id_List;

    void Mouse (QMouseEvent* event);
    void Keyboard(QKeyEvent* key);
    void Special(QKeyEvent* key);
    void Motion( int x ,int y);
    void display(void);
    void MouseRelease(QMouseEvent* event);

    int window_width = 640;
    int window_height = 480;
    int window_number = 0;
    double view_angle = 45;
    Vector3d translations = Vector3d(0,0,0);
    Vector3d rotations = Vector3d(0,0,0);
    Vector2i previous_mouse_position = Vector2i(0,0);
    Vector3d previous_trackball_position = Vector3d(0,0,0);
    Vector3d rotation_axis = Vector3d(0,0,0);
    double rotation_angle = 0;
    float trackball_transform[4][4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

    void Draw_Color_Bar(int size_x, int size_y,int x_init,int y_init);
    vector<Vector3d> Color_Steps; // for the color bar

};


#endif // GLWIDGET_H


