#include <QCoreApplication>

/***************************************************************************
                                 main.cpp
                             -------------------
    update               : 2009-10-09
    copyright            : (C) 2008-2009 by Yohan Fougerolle
    email                : yohan.fougerolle@u-bourgogne.fr
 ***************************************************************************/

// this program is just a bunch of functions I often need, and students too

#include <iostream>
#include <fstream>
#include "scene.h"
#include "Constante.h"
#include "useful.h"
#include "Stopwatch.h"
#include <ctime>
#include <string>
#include "NeighborMesh.h"
//
#include <windows.h>


using namespace std;

// horrible global variables next
// opengl rendering of the scene implemented "quick and (very) dirty" way

Stopwatch timer;   // in case you want to have a class to measure time

scene My_Scene;    // class to handle lights, position, etc

NeighborMesh globalmesh;   // yes another infamous global variable

int id_globalmesh; // identifier for display list. See function display() in scene.cpp

//global variables... could be handled as attributes in the scene class
Vector3d Camera_Target;
Vector3d Camera_Position;
double znear, zfar;

int main(int argc,char * argv[]){

    QCoreApplication a(argc, argv);
    //openGL initialization
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA |GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(200,200);
    glutCreateWindow("YF_Viewer 1.1.2");
    glutReshapeFunc(reshape); // function call when reshape the window
    glutMotionFunc(Motion);   // function called when mouse move
    glutMouseFunc(Mouse);     // function called when mouse click
    glutKeyboardFunc(Keyboard); // if key pressed
    glutSpecialFunc(Special);   // if special key pressed
    glutDisplayFunc(display);   // display function

    //my own inits here
    Init();
    My_Scene.Axis_Id_List=My_Scene.Draw_Axis(30);

    //example on How To Use the timer
    timer.Reset();
    timer.Start();

    //load a mesh from a file
    string file_name ("C:\\david.wrl");
    cout << file_name;

    if (!globalmesh.ReadFile(file_name)) exit(0);

    timer.Stop();
    cout<<"Loading time :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct P2P : point to point connectivity
    timer.Reset();
    timer.Start();
    globalmesh.Build_P2P_Neigh();
    timer.Stop();
    cout<<"P2P construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct P2F : point to face connectivity
    timer.Reset();
    timer.Start();
    globalmesh.Build_P2F_Neigh();
    timer.Stop();
    cout<<"P2F construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct F2F : face to face connectivity
    timer.Reset();
    timer.Start();
    globalmesh.Build_F2F_Neigh();
    timer.Stop();
    cout<<"F2F construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct Edges
    timer.Reset();
    timer.Start();
    globalmesh.Build_Edges();
    timer.Stop();
    cout<<"Egdes construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //roughly adjust view frustrum to object and camera position
    Vector3d Pmin(globalmesh.vertices[0]), Pmax(globalmesh.vertices[0]);
    Vector3d Center(0,0,0);

    for( int i=0; i<globalmesh.vertices.size(); i++)
    {
        Vector3d P(globalmesh.vertices[i]);
        for( int j=0; j<2; j++)
        {
            Pmin[j] = min(Pmin[j],P[j]);
            Pmax[j] = max(Pmax[j],P[j]);
        }
        Center += P;
    }

    Center/=globalmesh.vertices.size();
    Camera_Target = Center;

    //length of the diagonal of the bouding box
    double distance = (Pmax-Pmin).norm();

    //set arbitraty position to camera and adjusts max and min view planes
    Camera_Position = Camera_Target + Vector3d(0,0,distance*3);
    znear = distance*0.1;
    zfar = distance*5;

    //adjust displacements consequently

    My_Scene.Object_Move[0]=My_Scene.Object_Move[1]=My_Scene.Object_Move[2] = distance/20;

    //creates lights accordingly to the position and size of the object

    My_Scene.Create_Lighting(Pmax,Pmin, Camera_Position, Camera_Target);

    //compute normals
    globalmesh.ComputeFaceNormals(); //normal to faces
    globalmesh.ComputeVertexNormals(); //normals to vertex

    //Example of distance computing:

    int startpointindex = 0;

    timer.Reset();
    timer.Start();
    // will compute approximate geodesic distance from this point to all other points
    // consider the mesh as a graph and run shortest path algo
    // then stores distances in the label array
    globalmesh.BuildDistanceLabels(startpointindex);
    timer.Stop();
    cout<<"Distance label construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct colors from labels
    timer.Reset();
    timer.Start();
    globalmesh.SetColorsFromLabels();
    timer.Stop();
    cout<<"Constructing colors from distance :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //ok now here render
    id_globalmesh=glGenLists(1);
    glNewList(id_globalmesh,GL_COMPILE_AND_EXECUTE);


    //couple of extended neighbourhood computations
    glDisable(GL_LIGHTING);
    globalmesh.IllustratePointNeighbourhoodComputation(150,10);
    globalmesh.IllustrateFaceNeighbourhoodComputation(1000,10);
    glEnable(GL_LIGHTING);
    glLineWidth(1);
    glDisable(GL_LIGHTING);

    //render a couple of edges, with adjacent faces
    globalmesh.IllustrateEdges(100);

    //render a couple of point to point connectivity
    globalmesh.IllustrateP2P_Neigh(100);

    //render a couple of point to face connectivity
    globalmesh.IllustrateP2F_Neigh(300);

    //render a couple of face to face connectivity
    globalmesh.IllustrateF2F_Neigh(100);

    //renders obtuse triangles?
    /*
    for (int i=0; i<globalmesh.faces.size(); i++)
    {
        int dum = globalmesh.IsObtuse(i);
        if ( dum != -1 )
        {
            //cout <<"OBTUSE AT:"<<dum<<endl;


            Vector3d P(globalmesh.vertices[dum]);
            glColor3f(double(i)/globalmesh.faces.size(),1-double(i)/globalmesh.faces.size(),0);
            glPointSize(10);
            glBegin(GL_POINTS);
            glVertex3f(P[0], P[1], P[2]);
            glEnd();

            glColor3f(double(i)/globalmesh.faces.size(),1-double(i)/globalmesh.faces.size(),0);
            glBegin(GL_TRIANGLES);
            globalmesh.Draw_Face_Normal(i);
            glEnd();
        }
    }
    */
    glEnable(GL_LIGHTING);
    globalmesh.Draw(VERTEX_NORMAL_RGB);

    int ngeod = 20;
    timer.Reset();
    timer.Start();

    globalmesh.IllustrateShortestPaths (ngeod, startpointindex);
    timer.Stop();
    cout<<"Geodesic construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    globalmesh.DrawBoudaryEdges();
    glEndList();

    //now render
    glutMainLoop();

    return a.exec();
}
