#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalSlider_2->setValue(ui->horizontalSlider_2->maximum());
    QString s = QString::number(ui->horizontalSlider_2->value());
    ui->freqValue_2->setText(s);
    isLapCalculated = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

<<<<<<< HEAD
void MainWindow::on_pushButton_clicked()
=======

void MainWindow::on_SpecButton_clicked()
{
    //flag = 0 , geolap
    bool flag;
    if (ui->graphLap->isChecked()) flag = true;
    else flag = false;
    ui->RenderingWidget->calculateSpectrum(ui->horizontalSlider->value(),flag);
    isLapCalculated = true;
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
     ui->RenderingWidget->colorSpectrum(value);
     QString s = QString::number(value);
     ui->freqValue->setText(s);
}

void MainWindow::on_smoothButton_clicked()
{
}

void MainWindow::on_browseButton_clicked()
{
    isLapCalculated = false;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",tr("VRML File (*.wrl)"));
    ui->RenderingWidget->loadMesh(fileName.toStdString());
    ui->horizontalSlider->setMaximum(ui->RenderingWidget->getGlobalMesh().VertexNumber()-1);
    ui->horizontalSlider_2->setMaximum(ui->RenderingWidget->getGlobalMesh().VertexNumber()-1);
    ui->horizontalSlider_2->setValue(ui->RenderingWidget->getGlobalMesh().VertexNumber()-1);
    QString s = QString::number(ui->horizontalSlider_2->value());
    ui->freqValue_2->setText(s);

    ui->horizontalSlider_3->setMaximum(ui->RenderingWidget->getGlobalMesh().VertexNumber()-1);
    s = QString::number(ui->horizontalSlider_3->value());
    ui->freqValue_3->setText(s);
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    if (isLapCalculated)
    {
        ui->RenderingWidget->smoothMesh(value);
    }
    QString s = QString::number(value);
    ui->freqValue_2->setText(s);
}

void MainWindow::on_freqRemButton_clicked()
{
}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    if (isLapCalculated)
    {
        ui->RenderingWidget->frequencyRemoval(value);
    }
    QString s = QString::number(value);
    ui->freqValue_3->setText(s);
}

void MainWindow::on_horizontalSlider_2_actionTriggered(int action)
>>>>>>> 4487723a2949a68a1c202d52a957a2cdad047d5a
{
}

void MainWindow::on_graphLap_clicked()
{
    ui->graphLap->setChecked(false);
}

void MainWindow::on_geoLap_clicked()
{
    ui->graphLap->setChecked(false);
}

void MainWindow::on_SpecButton_2_clicked()
{
    if (!isLapCalculated)
    {
        bool flag;
        if (ui->graphLap->isChecked()) flag = true;
        else flag = false;
        ui->RenderingWidget->calculateSpectrum(ui->horizontalSlider->value(),flag);
        isLapCalculated = true;
    }
}

void MainWindow::on_SpecButton_3_clicked()
{
    if (!isLapCalculated)
    {
        bool flag;
        if (ui->graphLap->isChecked()) flag = true;
        else flag = false;
        ui->RenderingWidget->calculateSpectrum(ui->horizontalSlider->value(),flag);
        isLapCalculated = true;
    }
}

