#-------------------------------------------------
#
# Project created by QtCreator 2013-11-08T15:38:49
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MeshLoaderQt
TEMPLATE = app

#usual things
#TARGET = MeshLoaderQt
#CONFIG   += console
#CONFIG   -= app_bundle
#CONFIG -= console

#TEMPLATE = app

#add local sources and headers

SOURCES += \
    Camera.cpp \
    main.cpp \
    mainwindow.cpp \
    Mesh.cpp \
    myGLWidget.cpp \
    NeighborMesh.cpp \
    useful.cpp

HEADERS += \
    Camera.h \
    Constante.h \
    mainwindow.h \
    Mesh.h \
    myGLWidget.h \
    NeighborMesh.h \
    Stopwatch.h \
    useful.h


#to include the GL and Eigen directories which are 3 directories above the current project directory
#I put the libs, Eigen, the opengl Headers in the same directory so the following instruction applies for all of them:
INCLUDEPATH += ..\..\..

#to link the dlls
LIBS += -lglut32 -lglu32 -lopengl32

FORMS += \
    mainwindow.ui





