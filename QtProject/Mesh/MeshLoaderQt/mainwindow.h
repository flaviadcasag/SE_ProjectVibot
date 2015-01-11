#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QString>

#include "GLWidget.h"
#include "GLWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

<<<<<<< HEAD
private slots:
    void on_pushButton_clicked();
=======

private slots:

    void on_SpecButton_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_smoothButton_clicked();

    void on_browseButton_clicked();

    void on_horizontalSlider_2_actionTriggered(int action);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_freqRemButton_clicked();

    void on_horizontalSlider_3_valueChanged(int value);
>>>>>>> 4487723a2949a68a1c202d52a957a2cdad047d5a

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
