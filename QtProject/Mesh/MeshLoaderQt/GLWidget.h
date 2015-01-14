#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL>
#include <QGLWidget>
#include <GL/glu.h>
#include <iostream>

#include <string>

#include "LaplacianMesh.h"
#include "Stopwatch.h"
#include "Camera.h"
#include "Constante.h"
#include "Light.h"


using namespace std;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:

    //name of the file of the mesh loaded
    string fileName;

    //explicit constructor
    explicit GLWidget(QWidget *parent = 0);

    //Init the mesh (loads and build the P2P, F2P, F2F
    void initMesh();

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
    void calculateSpectrum(int frequency);

    //Color the spectrum
    void colorSpectrum(int);

    //get laplacianmesh
    LaplacianMesh getGlobalMesh() {return globalMesh;}

    //smooths the globalmesh
    void smoothMesh(int frequency);

    //remove one frequency from global mesh
    void frequencyRemoval(int frequency);

    //edit global mesh
    void meshEdit(double size, int axis);

    //compute laplacian according to the mode
    //mode 1 normal laplacian
    //mode 2 weighted laplacian
    void computeLaplacian(int mode);

    //set a output string to show on the program status window
    void setLog(QTextEdit* log) {this->log = log;}

    //update camera parameters using the global mesh measures
    void updateCamera();

    //update the mesh in order to draw it again
    void updateMesh();

    //Draw just the points of the mesh
    void DrawPoints ( set <int> s);

    //Draw the face of the mesh
    void DrawFaces  ( set <int> s);

    //draw the edge of the mesh
    void DrawEdge( map< pair<int,int>, set<int> > :: iterator it);

    //Draw the egge of the mesh
    void DrawEdge(int i);

    //draw boundary edges
    void  DrawBoudaryEdges();

    //Illustrate points neighbourhood computation
    void IllustratePointNeighbourhoodComputation(int p_index, int n);

    //Illustrate face neighbourhood computation
    void IllustrateFaceNeighbourhoodComputation(int f_index, int n);

    //Illustrate edges
    void  IllustrateEdges( int n);

    //Illustrate P2P Neigh
    void  IllustrateP2P_Neigh( int n);

    //Illustrate P2F Neigh
    void   IllustrateP2F_Neigh( int n);

    //Illustrate F2F Neigh
    void   IllustrateF2F_Neigh( int n);

    //Illustrate Shortest paths
    void   IllustrateShortestPaths (int ngeod, int startpointindex);

    //draw p2p neigh
    void DrawP2P_Neigh( int i );

    //draw p2f neigh
    void DrawP2F_Neigh( int i );

    //draw F2F neigh
    void DrawF2F_Neigh( int i );

    //draw default
    void Draw_Default(int i);

    //draw face normal
    void Draw_Face_Normal(int i);

    //draw vertex normal
    void Draw_Vertex_Normal(int i);

    //Draw the mesh calling the given mode
    void Draw(int DRAW_MODE);

    //draw face normal
    void Draw_Face_Normal_Rgb(int i);

    //draw vertex normal rgb
    void Draw_Vertex_Normal_Rgb(int i);

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
    LaplacianMesh globalMesh;

    //Timer
    Stopwatch timer;

    //Camera
    Camera cam;

    //Light of the Scene
    Light light;

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

    //function to treat the mouse events
    void Mouse (QMouseEvent* event);

    //function to treat the keyboard events
    void Keyboard(QKeyEvent* key);

    //function to treat the keyboard events
    void Special(QKeyEvent* key);

    //function to treat the mouse movements events
    void Motion( int x ,int y);

    //function to display
    void display(void);

    //function to treat the mouse release events
    void MouseRelease(QMouseEvent* event);

    //variables to deal with mouse interaction
    Vector3d translations = Vector3d(0,0,0);
    Vector3d rotations = Vector3d(0,0,0);
    Vector2i previous_mouse_position = Vector2i(0,0);
    Vector3d previous_trackball_position = Vector3d(0,0,0);
    Vector3d rotation_axis = Vector3d(0,0,0);
    double rotation_angle = 0;
    float trackball_transform[4][4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

    //draw color bar
    void Draw_Color_Bar(int size_x, int size_y,int x_init,int y_init);

    // color steps for the color bar
    vector<Vector3d> Color_Steps;

    //Text edit put log on the main window
    QTextEdit* log;

};


#endif // GLWIDGET_H


