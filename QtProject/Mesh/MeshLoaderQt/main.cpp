#include "mainwindow.h"
#include <QApplication>

//Main function
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Initialize main window
    MainWindow w;

    //Show window
    w.show();

    //return
    return a.exec();
}
