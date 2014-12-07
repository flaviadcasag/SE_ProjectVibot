#-------------------------------------------------
#
# Project created by QtCreator 2013-11-08T15:38:49
#
#-------------------------------------------------

#add opengl to QT config
QT       += core opengl

#We will see later how to have a gui interfaced opengl viewer
QT       -= gui

#usual things
TARGET = MeshLoaderQt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#add local sources and headers

SOURCES += \
    Camera.cpp \
    main.cpp \
    mainwindow.cpp \
    Mesh.cpp \
    myGLWidget.cpp \
    NeighborMesh.cpp \
    Scene.cpp \
    useful.cpp

HEADERS += \
    Camera.h \
    Constante.h \
    mainwindow.h \
    Mesh.h \
    myGLWidget.h \
    NeighborMesh.h \
    scene.h \
    Stopwatch.h \
    useful.h


#to include the GL and Eigen directories which are 3 directories above the current project directory
#I put the libs, Eigen, the opengl Headers in the same directory so the following instruction applies for all of them:
INCLUDEPATH += ..\..\..

#to link the dlls
LIBS += -lglut32 -lglu32 -lopengl32

FORMS += \
    mainwindow.ui





