#include "GLWidget.h"

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

void GLWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    Keyboard(keyEvent);
    updateGL();
}


#include<iostream>
using namespace std;

void GLWidget::timeOutSlot()
{
    updateGL();
}

void GLWidget::initializeGL()
{
    rotate=0;
    Axis=0;

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            Current_Matrix[i][j]=0;

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

    setWindowTitle(QString(name.c_str()));
    glClearColor(1,1,1,1);
    //fileName = ("d:/Users/Luis/Documents/uB/Software Engineering/SEProject/SE_ProjectVibot/VRML/dolphin.wrl");
    //initMesh();

}


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
    //example on How To Use the timer
    timer.Reset();
    timer.Start();

    //load a mesh from a file
    //string file_name ("d:/Users/Luis/Documents/uB/Software Engineering/SEProject/SE_ProjectVibot/VRML/mannequin.wrl");
    if (!globalMesh.ReadFile(fileName)) exit(0);

    timer.Stop();
    cout<<"Loading time :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct P2P : point to point connectivity
    timer.Reset();
    timer.Start();
    globalMesh.Build_P2P_Neigh();
    timer.Stop();
    cout<<"P2P construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct P2F : point to face connectivity
    timer.Reset();
    timer.Start();
    globalMesh.Build_P2F_Neigh();
    timer.Stop();
    cout<<"P2F construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct F2F : face to face connectivity
    timer.Reset();
    timer.Start();
    globalMesh.Build_F2F_Neigh();
    timer.Stop();
    cout<<"F2F construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct Edges
    timer.Reset();
    timer.Start();
    globalMesh.Build_Edges();
    timer.Stop();
    cout<<"Egdes construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //Init camera parameters based on the globalMesh
    double distance = cam.initCamera(globalMesh);

    //adjust displacements consequently
    objectMove[0] = distance/20;
    objectMove[1] = distance/20;
    objectMove[2] = distance/20;

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
    cout<<"Distance label construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct colors from labels
    timer.Reset();
    timer.Start();
    globalMesh.SetColorsFromLabels();
    timer.Stop();
    cout<<"Constructing colors from distance :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //ok now here render
    id_globalmesh=glGenLists(1);
    glNewList(id_globalmesh,GL_COMPILE_AND_EXECUTE);
    //globalMesh.Draw(VERTEX_NORMAL_RGB);
    globalMesh.Draw(FACE_NORMAL_RGB);


    glEndList();
    resizeGL(width(),height());
    updateGL();
}

void GLWidget::calculateSpectrum(int frequency)
{
    cout << "Laplacian starts now..." << endl;
    //Laplacian Calculation
    globalMesh.laplacian();

    timer.Reset();
    timer.Start();
    globalMesh.SpectralDecomposition();
    timer.Stop();
    cout<<"Finish calculating spectral decomposition :"<< timer.GetTotal()/1000.0<<" s"<<endl;

//    timer.Reset();
//    timer.Start();
    globalMesh.BuildSpectralLabels(frequency);
//    timer.Stop();
//    cout<<"Building colors from distance :"<< timer.GetTotal()/1000.0<<" s"<<endl;

//    timer.Reset();
//    timer.Start();
    globalMesh.SetColorsFromLabels();
//    timer.Stop();
//    cout<<"Setting colors from distance :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    id_globalmesh=glGenLists(1);
    glNewList(id_globalmesh,GL_COMPILE_AND_EXECUTE);
    globalMesh.Draw(FACE_NORMAL_RGB);
    glEndList();


    updateGL();
}

void GLWidget::colorSpectrum(int value)
{
    timer.Reset();
    timer.Start();
    globalMesh.BuildSpectralLabels(value);
    timer.Stop();
    cout<<"Building colors from distance :"<< timer.GetTotal()/1000.0<<" s"<<endl;
    timer.Reset();
    timer.Start();
    globalMesh.SetColorsFromLabels();
    timer.Stop();
    cout<<"Setting colors from distance :"<< timer.GetTotal()/1000.0<<" s"<<endl;
    id_globalmesh=glGenLists(1);
    glNewList(id_globalmesh,GL_COMPILE_AND_EXECUTE);
    globalMesh.Draw(FACE_NORMAL_RGB);
    glEndList();
    updateGL();
}

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

void GLWidget :: paintGL()
{
    display();
}

void GLWidget::setName(string name)
{
    this->name = name;
}

void GLWidget::setFramesPerSecond(float fps)
{
    framesPerSecond = fps;
}

void GLWidget::createLighting()
{
    //function to play around with open GL lights
    glEnable(GL_COLOR_MATERIAL);

    //roughly speaking : color of reflected light
    GLfloat specular[]={1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT,GL_SPECULAR,specular);

    //intensity of the shining...
    GLfloat shine[]={100};
    glMaterialfv(GL_FRONT,GL_SHININESS,shine);

    //ambient and diffuse light
    GLfloat ambient[]={0.0, 0.0, 0.0, 1.0};
    GLfloat diffuse[]={0.4, 0.4, 0.4, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    //set spot light cone, direction, angle, etc
    GLfloat position[]={cam.getPosition()[0],cam.getPosition()[1],cam.getPosition()[2] + 10 ,1};
    Vector3d Dir = -(cam.getPosition() -cam.getTarget()).normalized();
    GLfloat spot_direction[] = {Dir[0],Dir[1],Dir[2]};
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 360.0);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);

    //activate the first light
    glEnable(GL_LIGHT0);

    //-- if you want to add more lights, up to 8 lights are possible
    //-- insert similar code here
}

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

void GLWidget::Motion( int x ,int y)
{
    static Vector3d current_position;

    if(rotate)	{

        current_position = TrackballMapping( x, y );  // Map the mouse position to a logical

        //
        // Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
        //

        rotation_axis = previous_trackball_position .cross ( current_position );
        rotation_angle = 90.0 *( current_position - previous_trackball_position ).norm() * 1.5;
        previous_trackball_position = current_position;
    }

    if(translate){

        if( (y - oldy) > 0)
            translations[2] += objectMove[2];
        else
            translations[2] -= objectMove[2];

        oldx=x;
        oldy=y;
    }

}

void GLWidget::Mouse (QMouseEvent* event)
{
    rotate=0;
    if (event->buttons() && Qt::LeftButton)
    {
        rotate=1;
        previous_trackball_position = TrackballMapping( event->x(), event->y() );
    }
    else if (event->buttons() && Qt::RightButton)
    {
        translate=1;
        oldx = event->x();
        oldy = event->y();
    }
}





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


void GLWidget::mouseMoveEvent(QMouseEvent * mouse)
{
    Motion(mouse->x(),mouse->y());
    updateGL();
}


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

void GLWidget::mouseReleaseEvent(QMouseEvent* mouse)
{
    MouseRelease(mouse);
    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    Mouse(event);
    updateGL();
}

void GLWidget::loadMesh(string filename)
{
    fileName = filename;
    initMesh();
}

void GLWidget::smoothMesh()
{
    globalMesh.smoothing();
    id_globalmesh=glGenLists(1);
    glNewList(id_globalmesh,GL_COMPILE_AND_EXECUTE);
    globalMesh.Draw(FACE_NORMAL_RGB);
    glEndList();


    updateGL();
}


