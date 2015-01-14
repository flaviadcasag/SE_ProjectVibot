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

private slots:

    void on_SpecButton_clicked();

    void on_browseButton_clicked();

    void on_ComputeLaplacianButton_clicked();

    void on_smoothingButton_clicked();

    void on_RemovalButton_clicked();

    void on_ColoringSlider_valueChanged(int value);

    void on_SmoothingSlider_valueChanged(int value);

    void on_RemovalSlider_valueChanged(int value);

    void on_ScaleFactorSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    bool isLapCalculated;
    bool isSpectralComputed;

};

#endif // MAINWINDOW_H
