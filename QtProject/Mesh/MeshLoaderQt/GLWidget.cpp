#include<iostream>

#include "GLWidget.h"

using namespace std;

//GLWidget constructor
GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    // The QGLWidget needs focus to be able to handle key press events
    setFocusPolicy(Qt::StrongFocus);

    if(framesPerSecond == 0)
        t_Timer = NULL;
    else
    {
        int seconde = 1000; // 1 seconde = 1000 ms
        int timerInterval = seconde / framesPerSecond;
        t_Timer = new QTimer(this);
        connect(t_Timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
        t_Timer->start( timerInterval );
    }
}

//GL widget key press event
void GLWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    Keyboard(keyEvent);
    updateGL();
}

//Time out slot
void GLWidget::timeOutSlot()
{
    updateGL();
}

//Initialize opengl window
void GLWidget::initializeGL()
{
    rotate=0;
    Axis=0;

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            Current_Matrix[i][j]=0;

    name = "Laplacian Mesh Processing";

    Current_Matrix[0][0]=Current_Matrix[1][1]=Current_Matrix[2][2]=Current_Matrix[3][3]=1; // current matrix transform for trackball

    // useless I guess now
    Vector3d COL_BLUE(0,0,1);
    Vector3d COL_CYAN(0,1,1);
    Vector3d COL_GREEN(0,1,0);
    Vector3d COL_YELLOW(1,1,0);
    Vector3d COL_RED(1,0,0);

    Color_Steps.push_back(COL_BLUE);
    Color_Steps.push_back(COL_CYAN);
    Color_Steps.push_back(COL_GREEN);
    Color_Steps.push_back(COL_YELLOW);
    Color_Steps.push_back(COL_RED);

    //set name of the window
    setWindowTitle(QString(name.c_str()));

    //initializing the opengl widget with white color
    glClearColor(1,1,1,1);
}

//Init the mesh parameters
void GLWidget::initMesh()
{
    // set the drawing mode to full rendering
    glPolygonMode(GL_FRONT,GL_FILL);

    //activate Z buffer (hide elements in the back)
    glEnable(GL_DEPTH_TEST);

    //useful if you want to superpose the rendering in full mode and in wireless mode
    glEnable(GL_POLYGON_OFFSET_FILL);

    //convenient settings for polygon offset, do not change this
    glPolygonOffset(1.0,1.0);

    // unit normals, in case you would forget to compute them
    glEnable(GL_NORMALIZE);

    // now you can associate a color to a point...
    glEnable(GL_COLOR_MATERIAL);

    //background color is white (better for screenshot when writing a paper)
    glClearColor(1.0f,1.0f,1.0f,0.0f);

    //you can activate blending for better rendering...
    glEnable( GL_BLEND );

    // careful with those parameters, results depend on your graphic card
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //activate / disable those and compare ;)
    glEnable( GL_POINT_SMOOTH );
    glHint( GL_POINT_SMOOTH, GL_NICEST );

    timer.Reset();
    timer.Start();  
    //load a mesh from a file
    if (!globalMesh.ReadFile(fileName)) exit(0);

    timer.Stop();
    log->append("Loading time :"+ QString::number(timer.GetTotal()/1000.0)+" s");

    //store the original vertices  for further computations
    globalMesh.setOriginalVertices(vector<Vector3d>(globalMesh.Vertices()));

    //construct P2P : point to point connectivity
    timer.Reset();
    timer.Start();
    globalMesh.Build_P2P_Neigh();
    timer.Stop();
    log->append("P2P construction :"+ QString::number(timer.GetTotal()/1000.0)+" s");

    //construct P2F : point to face connectivity
    timer.Reset();
    timer.Start();
    globalMesh.Build_P2F_Neigh();
    timer.Stop();
    log->append("P2F construction :"+ QString::number(timer.GetTotal()/1000.0)+" s");

    //construct F2F : face to face connectivity
    timer.Reset();
    timer.Start();
    globalMesh.Build_F2F_Neigh();
    timer.Stop();
    log->append("F2F construction :"+ QString::number(timer.GetTotal()/1000.0)+" s");

    //construct Edges
    timer.Reset();
    timer.Start();
    globalMesh.Build_Edges();
    timer.Stop();
    log->append("Egdes construction :"+ QString::number(timer.GetTotal()/1000.0)+" s");

    //update camera parameters
    updateCamera();

    //creates lights accordingly to the position and size of the object
    createLighting();

    //compute normals
    globalMesh.ComputeFaceNormals(); //normal to faces
    globalMesh.ComputeVertexNormals(); //normals to vertex

    //Example of distance computing:
    int startpointindex = 0;

    timer.Reset();
    timer.Start();
    // will compute approximate geodesic distance from this point to all other points
    // consider the mesh as a graph and run shortest path algo
    // then stores distances in the label array
    globalMesh.BuildDistanceLabels(startpointindex);
    timer.Stop();
    log->append("Distance label construction :"+ QString::number(timer.GetTotal()/1000.0)+" s");

    //construct colors from labels
    timer.Reset();
    timer.Start();
    globalMesh.SetColorsFromLabels();
    timer.Stop();
    log->append("Constructing colors from distance :"+ QString::number(timer.GetTotal()/1000.0)+" s");
    log->append("MESH LOADED!");

    //ok now here render
    updateMesh();

    //update camera
    resizeGL(width(),height());

    //update window
    updateGL();
}

//calculate spectrum decomposition
//according to the given frequency to build the labels
void GLWidget::calculateSpectrum(int frequency)
{
    log->append("Laplacian calculation starts now");
    timer.Reset();
    timer.Start();
    globalMesh.spectralDecomposition();
    timer.Stop();
    log->append("Finish calculating spectral decomposition :"+ QString::number(timer.GetTotal()/1000.0)+" s");

    globalMesh.BuildSpectralLabels(frequency);
    globalMesh.SetColorsFromLabels();
    updateMesh();
    updateGL();
}

//build the color spectrum according to the given frequency
void GLWidget::colorSpectrum(int value)
{
    log->append("Selecting different frequencies for spectral decomposition");
    globalMesh.BuildSpectralLabels(value);
    globalMesh.SetColorsFromLabels();
    updateMesh();
    updateGL();
}

//update window if it is resized
//also update the camera
void GLWidget::resizeGL(int width, int height)
{
    glPushMatrix();
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
                    45,		    // 45 deg is ok
                    (float)width/height, // to preserve window ratio when rescaling the window
                    cam.getZnear(),      // computed in main function
                    cam.getZfar()        // same
                    );

    // wonderful gluLookAt function
    gluLookAt(
    cam.getPosition()[0], cam.getPosition()[1], cam.getPosition()[2], // from main.cpp
    cam.getTarget()[0], cam.getTarget()[1], cam.getTarget()[2],       // same
    0,1,0);		//Up Vector, do not change this

    glMatrixMode(GL_MODELVIEW);
}

//display
void GLWidget :: paintGL()
{
    display();
}

//set window name
void GLWidget::setName(string name)
{
    this->name = name;
}

//set frames per second
void GLWidget::setFramesPerSecond(float fps)
{
    framesPerSecond = fps;
}

//create light getting the parameters from the light object
void GLWidget::createLighting()
{
    //function to play around with open GL lights
    glEnable(GL_COLOR_MATERIAL);

    //roughly speaking : color of reflected light

    GLfloat specular[]={light.getSpecular()[0],
                        light.getSpecular()[1],
                        light.getSpecular()[2],
                        light.getSpecular()[3]};

    glMaterialfv(GL_FRONT,GL_SPECULAR,specular);

    //intensity of the shining...
    GLfloat shine[]={light.getShine()};
    glMaterialfv(GL_FRONT,GL_SHININESS,shine);

    //ambient and diffuse light
    GLfloat ambient[]={light.getAmbient()[0],
                       light.getAmbient()[1],
                       light.getAmbient()[2],
                       light.getAmbient()[3]};

    GLfloat diffuse[]={light.getDiffuse()[0],
                       light.getDiffuse()[1],
                       light.getDiffuse()[2],
                       light.getDiffuse()[3]};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    //set spot light cone, direction, angle, etc
    light.setPositionByCamera(cam.getPosition()[0],
                              cam.getPosition()[1],
                              cam.getPosition()[2]);

    Vector3d Dir = -(cam.getPosition() -cam.getTarget()).normalized();
    light.setDirection(Dir[0],Dir[1],Dir[2]);
    GLfloat spot_direction[] = {light.getDirection()[0],
                                light.getDirection()[1],
                                light.getDirection()[2]};

    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 360.0);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);

    //activate the first light
    glEnable(GL_LIGHT0);
}

//trackball mapping
Vector3d GLWidget::TrackballMapping( int x, int y )
{
    static const double PI_2 = 3.141592653589793238462643383/2.0;

    Vector3d v(0,0,0);
    static GLint params[4];

    glGetIntegerv(GL_VIEWPORT,params);

    v[0] = ( 2.0 * x - params[2] ) / params[2];
    v[1] = ( params[3] - 2.0 * y ) / params[3];

    double  d = v.norm();
    if( d > 1.0 ) d = 1.0;

    v[2] = cos((PI_2) * d);

    return v.normalized();
}

//mouse motion function
void GLWidget::Motion( int x ,int y)
{
    static Vector3d current_position;

    if(rotate)
    {
        current_position = TrackballMapping( x, y );  // Map the mouse position to a logical

        //
        // Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
        //

        rotation_axis = previous_trackball_position .cross ( current_position );
        rotation_angle = 90.0 *( current_position - previous_trackball_position ).norm() * 1.5;
        previous_trackball_position = current_position;
    }

    if(translate)
    {
//        if( (y - oldy) > 0)
//            translations[2] += objectMove[2];
//        else
//            translations[2] -= objectMove[2];

//        oldx=x;
//        oldy=y;
    }
}

//Mouse event function
void GLWidget::Mouse (QMouseEvent* event)
{
    rotate=0;
    if (event->buttons() == Qt::LeftButton)
    {
        rotate=1;
        previous_trackball_position = TrackballMapping( event->x(), event->y() );
    }
    else if (event->buttons() == Qt::RightButton)
    {
        translate=1;
        oldx = event->x();
        oldy = event->y();
    }
}

//display function
void GLWidget::display(void)
{
    GLboolean lights_on[1];
    glGetBooleanv(GL_LIGHTING,lights_on);

    GLint polygon_draw_mode[2];
    glGetIntegerv(GL_POLYGON_MODE,polygon_draw_mode);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    ///////
    //
    //	Windows informations display (color bar, referential,...)
    //
    //////

    glDisable(GL_LIGHTING);

    //render 3D ref at the bottom left corner of the Opengl window

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

        //Trackball Transformation
        glRotatef( rotation_angle, rotation_axis[0], rotation_axis[1], rotation_axis[2] );
        glMultMatrixf((GLfloat *)trackball_transform);
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)trackball_transform);

        //Color Bar Draw
        Draw_Color_Bar(30,300,30,170);

        //translation in windows coordinates
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
            glLoadIdentity();

            //local 3d projection to draw 3D referential(size=30 pixels)
            glOrtho(0,viewport[2],0,viewport[3],0,max(viewport[2],viewport[3]));
            //to avoid clip planes... quick and dirty-> this could be adjusted automatically
            glTranslatef(50,90,-50);

            glPushMatrix();

                glMatrixMode(GL_MODELVIEW);
                //disable lights before drawing 3d axis
                glDisable(GL_LIGHTING);
                //glCallList(Axis_Id_List);

                glMatrixMode(GL_PROJECTION);

            glPopMatrix();

        glPopMatrix();


    ///////
    //
    //	render mesh
    //
    //////


    glPolygonMode(GL_FRONT,polygon_draw_mode[0]|polygon_draw_mode[1] ); //set drawing mode

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector3d center(0,0,0);
    glTranslatef(translations[0],translations[1],translations[2]);

    glMultMatrixf((GLfloat *)trackball_transform);
    if(lights_on) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
    glCallList(id_globalmesh); // call the display list created in the main function

    //illustrate results for a point
    rotation_axis = Vector3d(0,0,0);
    rotation_angle = 0;
}

//mouse movement function
void GLWidget::mouseMoveEvent(QMouseEvent * mouse)
{
    Motion(mouse->x(),mouse->y());
    updateGL();
}

//draw color bar
void GLWidget::Draw_Color_Bar(int size_x, int size_y,int x_init,int y_init)
{
    // store draw mode (light and polygon mode)
    GLint polygon_draw_mode[2];
    glGetIntegerv(GL_POLYGON_MODE,polygon_draw_mode);

    vector<Vector3d>::iterator it(Color_Steps.begin());
    Vector3d current,prev;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    glDisable ( GL_LIGHTING );
    glPolygonMode(GL_FRONT,GL_FILL);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, viewport[2], 0, viewport[3]);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glTranslatef(x_init, y_init,0);
        double coef_x(size_x),coef_y(size_y/double(Color_Steps.size()-1) );

        for(int i=0;i<Color_Steps.size()-1;i++){
            prev=(*it);
            it++;
            current=(*it);

            glBegin(GL_QUADS);

            glNormal3d(0,0,1.0);
            glColor3d(prev[0], prev[1], prev[2]);
            glVertex3d(size_x,(i)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(current[0],current[1],current[2]);
            glVertex3d(size_x,(i+1)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(current[0],current[1],current[2]);
            glVertex3d(0,(i+1)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(prev[0], prev[1], prev[2]);
            glVertex3d(0,(i)*coef_y,0);


            glEnd();


            double t0=(double)(i)/((double)Color_Steps.size()-1);
            double t1=(double)(i+1)/((double)Color_Steps.size()-1);

            glBegin(GL_QUADS);

            glNormal3d(0,0,1.0);
            glColor3d(t0,t0,t0);
            glVertex3d(2*size_x+10,i*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(t1,t1,t1);
            glVertex3d(2*size_x+10,(i+1)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(t1,t1,t1);
            glVertex3d(size_x+10,(i+1)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(t0,t0,t0);
            glVertex3d(size_x+10,i*coef_y,0);


            glEnd();
        }

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    //old polygon drawing mode restoration
    glPolygonMode(GL_FRONT,polygon_draw_mode[0]|polygon_draw_mode[1]);
}

//keyboard event function
void GLWidget::Keyboard(QKeyEvent *keyEvent)
{
    glMatrixMode(GL_PROJECTION);

    switch(keyEvent->key()){
    case Qt::Key_Up :
        translations[1] += objectMove[0];
        break;
    case Qt::Key_Left:
        translations[0] -= objectMove[0];
        break;
    case Qt::Key_Right:
        translations[0] += objectMove[1];
        break;
    case Qt::Key_Down:
        translations[1] -= objectMove[1];
        break;
    case Qt::Key_F :
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        break;
    case Qt::Key_L :
        glLineWidth(1.0);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        break;
    case Qt::Key_I :
        if(lights_on==0)lights_on=1;
        else lights_on=0;
        break;
    case Qt::Key_Plus :
        translations[2] += objectMove[2];
        break;
    case Qt::Key_Minus :
        translations[2] -= objectMove[2];
        break;
    }

    glMatrixMode(GL_MODELVIEW);
}

//mouse release function
void GLWidget::MouseRelease(QMouseEvent* event)
{
    if (event->buttons() && Qt::LeftButton)
    {
        rotate=0;
    }
    else if ( event->buttons() && Qt::RightButton)
    {
        translate=0;
        oldx = event->x();
        oldy = event->y();
    }
}

//mouse release event function
void GLWidget::mouseReleaseEvent(QMouseEvent* mouse)
{
    MouseRelease(mouse);
    updateGL();
}

//mouse press event function
void GLWidget::mousePressEvent(QMouseEvent* event)
{
    Mouse(event);
    updateGL();
}

//interface to the load mesh
void GLWidget::loadMesh(string filename)
{
    fileName = filename;
    globalMesh = LaplacianMesh();
    initMesh();
}

//interface to smooth the mesh
void GLWidget::smoothMesh(int frequency)
{
    globalMesh.smoothing(frequency);
    updateMesh();
    updateGL();
}

//interface to remove one frequency
void GLWidget::frequencyRemoval(int frequency)
{
    globalMesh.frequencyRemoval(frequency);
    updateMesh();
    updateGL();
}

//interface to edit the mesh
void GLWidget::meshEdit(double size, int axis)
{
    globalMesh.meshEditing(size, axis);
    updateMesh();
    updateCamera();
    updateGL();
}

//compute laplacian according to the mode
//mode 1 normal laplacian
//mode 2 weighted laplacian
void GLWidget::computeLaplacian(int mode)
{
    switch (mode)
    {
        case 1 : globalMesh.laplacian(); break;
        case 2 : globalMesh.weightedLaplacian(); break;
    }
}

//Update the camera parameters according to the global mesh measures
void GLWidget::updateCamera()
{
    //Init camera parameters based on the globalMesh
    cam.initCamera(globalMesh);

    //get the distance from the camera to the mesh
    double distance = cam.getDistance();

    //adjust displacements consequently
    objectMove[0] = distance/20;
    objectMove[1] = distance/20;
    objectMove[2] = distance/20;

    resizeGL(width(), height());
}

//Update the mesh in order to draw
void GLWidget::updateMesh()
{
    id_globalmesh=glGenLists(1);
    glNewList(id_globalmesh,GL_COMPILE_AND_EXECUTE);
    Draw(FACE_NORMAL_RGB);
    glEndList();
}

//draw points
void GLWidget ::DrawPoints ( set <int> s)
{
    glBegin(GL_POINTS);
    for( set<int>::iterator it(s.begin()); it != s.end(); it++)
    {
        Vector3d P(globalMesh.getVertex(*it));
        glVertex3f(P[0],P[1],P[2]);
    }
    glEnd();
}

//draw faces
void GLWidget :: DrawFaces  ( set <int> s)
{
    for( set<int>::iterator it(s.begin()); it != s.end(); it++)
    {
        Draw_Face_Normal(*it);
    }
}

//draw edge
void GLWidget :: DrawEdge( map< pair<int,int>, set<int> > :: iterator it)
{
    pair < pair<int,int>,set<int> > myedge = *it;

    Vector3d A(globalMesh.getVertex(myedge.first.first));
    Vector3d B(globalMesh.getVertex(myedge.first.second));

    glDisable(GL_LIGHTING);
    glPointSize(5);

    glBegin(GL_POINTS);
    glColor3f(1,0,0);
    glVertex3f(A[0],A[1],A[2]);
    glColor3f(0,1,0);
    glVertex3f(B[0],B[1],B[2]);
    glEnd();

    glLineWidth(3);
    glColor3f(0,0,1);
    glBegin(GL_LINES);
    glVertex3f(A[0],A[1],A[2]);
    glVertex3f(B[0],B[1],B[2]);
    glEnd();

    glEnable(GL_LIGHTING);
    glColor3f(0.8,0.8,0);
    glBegin(GL_TRIANGLES);
    for(set<int> :: iterator it = myedge.second.begin(); it != myedge.second.end(); it++)
    Draw_Face_Normal(*it);
    glEnd();

}

//draw edge
void GLWidget :: DrawEdge(int i)
{
    if(i > globalMesh.getEdges().size() ) return;

    map <pair<int,int>,set<int> > :: iterator it(globalMesh.getEdges().begin());
    for(int j=0;j<i;j++, it++);
        DrawEdge (it);

}

//draw boundary edges
void  GLWidget :: DrawBoudaryEdges()
{
    //Illustrative function
    //span edges which have a set a adjacent faces with 0 or 1 element

    map <pair<int,int>,set<int> > :: iterator it;

    glLineWidth(5);
    glColor3f(1,0,0);
    glDisable(GL_LIGHTING);
    for(it = globalMesh.getEdges().begin(); it != globalMesh.getEdges().end(); it++)
    {
    pair < pair<int,int>,set<int> > myedge = *it;

    if(myedge.second.size()<2)
        {
        Vector3d A(globalMesh.getVertex(myedge.first.first));
        Vector3d B(globalMesh.getVertex(myedge.first.second));
        glBegin(GL_LINES);
        glVertex3f(A[0],A[1],A[2]);
        glVertex3f(B[0],B[1],B[2]);
        glEnd();
        }

    if(myedge.second.size()>2)
        {
        Vector3d A(globalMesh.getVertex(myedge.first.first));
        Vector3d B(globalMesh.getVertex(myedge.first.second));
        glColor3f(0,1,0);
        glLineWidth(20);
        glBegin(GL_LINES);
        glVertex3f(A[0],A[1],A[2]);
        glVertex3f(B[0],B[1],B[2]);
        glEnd();
        }
    }
}

//illustrate point neighourhood computation
void GLWidget :: IllustratePointNeighbourhoodComputation(int p_index, int n)
{
    glPointSize(10);
    for(int i=0; i<n; i++)
    {
        set <int> tmp = globalMesh.GetP2P_Neigh(p_index, i);
        if (i%2 == 0)    glColor3f(1,1-i/double(n),0);
        else glColor3f(0,1-i/double(n),1);
        DrawPoints ( tmp );
    }
}

//illustrate face neighourhood computation
void GLWidget :: IllustrateFaceNeighbourhoodComputation(int f_index, int n)
{
    for(int i=0; i<n; i++)
    {
        set <int> tmp = globalMesh.GetF2F_Neigh(f_index, i);
        if (i%2 == 0)    glColor3f(1,1-i/double(n),0);
        else glColor3f(0,1-i/double(n),1);
        glBegin(GL_TRIANGLES);
        DrawFaces ( tmp );
        glEnd();
    }
}

//illustrate edges
void  GLWidget :: IllustrateEdges( int n)
{
    map< pair<int,int>, set<int> > :: iterator it (globalMesh.getEdges().begin());
    int i(0);
    while (i<n)
    {
        DrawEdge(it) ;

        for (int j=0; j<globalMesh.getEdges().size()/n; j++, it++){}
        i++;
    }
}

//illustrate P2P neighourhood computation
void  GLWidget :: IllustrateP2P_Neigh( int n)
{
    for(int i=0; i<n; i++)
    {
        DrawP2P_Neigh( i * int(globalMesh.getP2P_NeighSize()/n) );
    }

}

//illustrate P2F neighourhood computation
void GLWidget :: IllustrateP2F_Neigh( int n)
{
    for(int i=0; i<n; i++)
    {
        DrawP2F_Neigh( i * int(globalMesh.getP2F_Neigh().size()/n) );
    }
}

//illustrate F2F neighourhood computation
void   GLWidget :: IllustrateF2F_Neigh( int n)
{
    for(int i=0; i<n; i++)
    {
        DrawF2F_Neigh( i * int(globalMesh.getF2F_Neigh().size()/n) );
    }
}


//illustrate shortest paths
void   GLWidget :: IllustrateShortestPaths (int ngeod, int startpointindex)
{
    //Recompute everything in case labels have been corrupted

    globalMesh.BuildDistanceLabels(startpointindex);
    globalMesh.SetColorsFromLabels();

    //construct geod approx

     for(int i=1; i< ngeod+1; i++){
        cout<<"Geodesic "<<i<<"...";
        vector<int> mypath = globalMesh.ShortestPath(
            startpointindex,
            i*int(globalMesh.VertexNumber()/ngeod - 1)
         );

        if(!mypath.empty())
        {
        Vector3d Col=DoubleToColor((i-1.0)/ngeod);
        glColor3f(Col[0],Col[1], Col[2]);
        glLineWidth(5);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINE_STRIP);
        for(vector<int>::iterator it(mypath.begin()); it != mypath.end(); it++)
            {
           Vector3d P(globalMesh.getVertex(*it));
           glVertex3f(P[0],P[1],P[2]);
            }
        glEnd();
         }
        cout<<"done"<<endl;
    }
}

//Draw p2p neigh
void GLWidget :: DrawP2P_Neigh( int i )
{
    if ( globalMesh.getP2P_Neigh().empty() || i >= globalMesh.getP2P_Neigh().size() )
        return;

    glPointSize(5);

    glDisable(GL_LIGHTING);

    //render the considered point in red
    Vector3d P(globalMesh.getVertex(i));

    //Write the number of neighbours in the OGL window
    glColor3f(1,0,0);
    glPushMatrix();
    glTranslatef(P[0], P[1], P[2]);
    char s[255];
    sprintf(s, "%d", globalMesh.getP2P_NeighSet(i).size());
    string n(s);
    //Print3DMessage(0,0,n);
    glPopMatrix();

    glBegin(GL_POINTS);
    glColor3f(1,0,0);
    glVertex3f(P[0], P[1], P[2]);

    //render the neighbors in green
    glColor3f(0,1,0);
    for ( set <int> :: iterator it = globalMesh.getP2P_NeighSet(i).begin(); it != globalMesh.getP2P_NeighSet(i).end(); ++it)
    {
        P = globalMesh.getVertex(*it);
        glVertex3f(P[0], P[1], P[2]);
    }
    glEnd();
}

//Draw p2f neigh
void GLWidget :: DrawP2F_Neigh( int i )
{
    if ( globalMesh.getP2F_Neigh().empty() || i >= globalMesh.getP2F_Neigh().size() )  return;

    glPointSize(5);
    glColor3f(1,0,0);
    glDisable(GL_LIGHTING);

    //render the considered point in red
    Vector3d P(globalMesh.getVertex(i));
    glBegin(GL_POINTS);
    glVertex3f(P[0], P[1], P[2]);
    glEnd();

    //render the neighbor faces in dark green
    glColor3f(0,0.5,0);

    set <int> myset ( globalMesh.getP2F_NeighSet(i));

    glEnable(GL_LIGHTING);
    glBegin(GL_TRIANGLES);

    for ( set <int> :: iterator it = myset.begin(); it != myset.end(); it++)
        Draw_Face_Normal(*it);

    glEnd();

}

//Draw F2F neigh
void GLWidget :: DrawF2F_Neigh( int i )
{
    if(globalMesh.getF2F_Neigh().empty() || i>globalMesh.getF2F_Neigh().size() ) return;

    //draw the considered face in red

    glBegin(GL_TRIANGLES);
    glColor3f(1,0,0);
    Draw_Face_Normal(i);

    //draw neighbor faces in blue
    glColor3f(0,0,1);
    set <int> myset ( globalMesh.getF2F_NeighSet(i));
    for ( set <int> :: iterator it = myset.begin(); it != myset.end(); it++)
        Draw_Face_Normal(*it);
    glEnd();
}

//Draw default
void GLWidget::Draw_Default(int i)
{
    for(int j=0; j<3; j++)
        glVertex3d(globalMesh.getVertex(i)[0],globalMesh.getVertex(i)[1],globalMesh.getVertex(i)[2]);


}

//draw face normal
void GLWidget::Draw_Face_Normal(int i)
{
    for(int j=0; j<3; j++)	{
        glNormal3d(globalMesh.FaceNormal(i)[0],globalMesh.FaceNormal(i)[1],globalMesh.FaceNormal(i)[2]);
        Vector3d V=globalMesh.Vertex(i,j);
        glVertex3d(V[0],V[1],V[2]);
    }
}

//draw vertex normal
void GLWidget::Draw_Vertex_Normal(int i)
{
    for(int j=0; j<3; j++)	{

        int vertex_index = globalMesh.Face(i)[j];

        Vector3d N(globalMesh.VertexNormal(vertex_index));
        Vector3d V(globalMesh.Vertex(vertex_index));
        glNormal3d(N[0],N[1],N[2]);
        glVertex3d(V[0],V[1],V[2]);
    }
}



//draw according to the mode
void GLWidget::Draw(int DRAW_MODE)
{
    int nb_faces=globalMesh.FaceNumber();
    int i;


    switch(DRAW_MODE)
    {
        case ONLY_VERTEX	:
        {
            glDisable(GL_LIGHTING);
            cout<<"Vertices displayed="<<globalMesh.VertexNumber()<<endl;

            glPointSize(5.0);
            glBegin(GL_POINTS);
            bool color_on=false;
            if(globalMesh.ColorNumber() == globalMesh.VertexNumber()) color_on=true;
            for( i=0 ; i<globalMesh.VertexNumber(); i++ )
            {
                if(color_on) glColor3f(globalMesh.Color(i)[0],globalMesh.Color(i)[1],globalMesh.Color(i)[2]);
                glVertex3d(globalMesh.Vertex(i)[0],globalMesh.Vertex(i)[1],globalMesh.Vertex(i)[2]);
            }

            glEnd();

            glEnable(GL_LIGHTING);

        }break;

        case FACE_NORMAL:
        {
            glBegin(GL_TRIANGLES);
            for( i=0; i<nb_faces; i++)
                Draw_Face_Normal(i);
        }break;

        case FACE_NORMAL_RGB:
        {
            glBegin(GL_TRIANGLES);
            for( i=0; i<nb_faces; i++)
                Draw_Face_Normal_Rgb(i);
            glEnd();
        }break;

        case VERTEX_NORMAL		:
        {
            glBegin(GL_TRIANGLES);
            for( i=0; i<nb_faces; i++)
                Draw_Vertex_Normal(i);
            glEnd();
        }	break;

        case VERTEX_NORMAL_RGB	:
        {
//            vector<Vector3d> delta;

//            for (i = 0; i < vertex.size(); i++)
//            {
//                di =
//                delta.push_back(Vector3d(vertex[i]-1/));
//            }

//            for (i = 0; i < colors.size(); i++)
//            {
//                colors[i] = Vector3d(rand()/(double)RAND_MAX, rand()/(double)RAND_MAX, rand()/(double)RAND_MAX);
//            }

            glBegin(GL_TRIANGLES);
            for( i=0; i<globalMesh.FaceNumber(); i++)
                Draw_Vertex_Normal_Rgb(i);
            glEnd();
        }	break;

    }
}

//Draw face normal rgb
void GLWidget::Draw_Face_Normal_Rgb(int i)
{
    for(int j=0; j<3; j++)	{
        glNormal3d(globalMesh.FaceNormal(i)[0],globalMesh.FaceNormal(i)[1],globalMesh.FaceNormal(i)[2]);
        Vector3d V=globalMesh.Vertex(globalMesh.Face(i)[j]);
        Vector3d C=globalMesh.Color(globalMesh.Face(i)[j]);
        glColor3d(C[0],C[1],C[2]);
        glVertex3d(V[0],V[1],V[2]);
    }
}

//draw vertex normal rgb
void GLWidget::Draw_Vertex_Normal_Rgb(int i){

    for(int j=0; j<3; j++)
    {
        int vertex_index = globalMesh.Face(i)[j];

        Vector3d N(globalMesh.VertexNormal(vertex_index));
        Vector3d V(globalMesh.Vertex(vertex_index));
        Vector3d Col(globalMesh.Color(vertex_index));

        glNormal3d( N[0] , N[1] , N[2]);
        glColor3d( Col[0] , Col[1] , Col[2]);

        glVertex3d( V[0] , V[1] , V[2]);
    }
}

