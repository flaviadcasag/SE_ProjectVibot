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

    void on_horizontalSlider_valueChanged(int value);

    void on_smoothButton_clicked();

    void on_browseButton_clicked();

    void on_horizontalSlider_2_actionTriggered(int action);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_freqRemButton_clicked();

    void on_horizontalSlider_3_valueChanged(int value);

    void on_graphLap_clicked();

    void on_geoLap_clicked();

    void on_SpecButton_2_clicked();

    void on_SpecButton_3_clicked();

    void on_ComputeLaplacianButton_clicked();

    void on_horizontalSlider_4_valueChanged(int value);

    void on_meshx_clicked();

    void on_meshy_clicked();

private:
    Ui::MainWindow *ui;
    bool isLapCalculated;
    bool isSpectralComputed;

};

#endif // MAINWINDOW_H
