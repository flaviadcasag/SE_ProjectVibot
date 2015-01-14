#include "mainwindow.h"
#include "ui_mainwindow.h"

//Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->SmoothingSlider->setValue(ui->SmoothingSlider->maximum());
    QString s = QString::number(ui->SmoothingSlider->value());
    ui->freqValue_2->setText(s);
    isLapCalculated = false;
    ui->SpecButton->setEnabled(false);
    ui->RenderingWidget->setLog(ui->logText);
}

//Destructor
MainWindow::~MainWindow()
{
    delete ui;
}

//Button to browse for the VRML file
void MainWindow::on_browseButton_clicked()
{
    isLapCalculated = false;
    isSpectralComputed = false;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",tr("VRML File (*.wrl)"));
    ui->RenderingWidget->loadMesh(fileName.toStdString());
    ui->ColoringSlider->setMaximum(ui->RenderingWidget->getGlobalMesh().VertexNumber()-1);
    ui->SmoothingSlider->setMaximum(ui->RenderingWidget->getGlobalMesh().VertexNumber()-1);
    ui->SmoothingSlider->setValue(ui->RenderingWidget->getGlobalMesh().VertexNumber()-1);
    QString s = QString::number(ui->SmoothingSlider->value());
    ui->freqValue_2->setText(s);

    ui->RemovalSlider->setMaximum(ui->RenderingWidget->getGlobalMesh().VertexNumber()-1);
    s = QString::number(ui->RemovalSlider->value());
    ui->freqValue_3->setText(s);
    ui->ScaleFactorSlider->setValue(50);
    ui->SpecButton->setEnabled(false);
    ui->smoothingButton->setEnabled(false);
    ui->RemovalButton->setEnabled(false);
    ui->ColoringSlider->setEnabled(false);
    ui->SmoothingSlider->setEnabled(false);
    ui->RemovalSlider->setEnabled(false);
    ui->ScaleFactorSlider->setEnabled(false);
    ui->ComputeLaplacianButton->setEnabled(true);
}

//Button to Compute the specified Laplacian
void MainWindow::on_ComputeLaplacianButton_clicked()
{
    int mode = 0;
    if (ui->graphLap->isChecked()) mode = 1;
    else mode = 2;
    ui->RenderingWidget->computeLaplacian(mode);
    ui->SpecButton->setEnabled(true);
    isLapCalculated=true;

    ui->SpecButton->setEnabled(true);
    ui->ScaleFactorSlider->setEnabled(true);
}


//Button to perform the Spectral Decomposition
void MainWindow::on_SpecButton_clicked()
{
    ui->RenderingWidget->calculateSpectrum(ui->ColoringSlider->value());
    isSpectralComputed=true;
    ui->smoothingButton->setEnabled(true);
    ui->RemovalButton->setEnabled(true);
    ui->ColoringSlider->setEnabled(true);
}

//Allows the Smoothing specified by the Smoothing Slider
void MainWindow::on_smoothingButton_clicked()
{
    if (!isLapCalculated)
    {
        ui->RenderingWidget->calculateSpectrum(ui->ColoringSlider->value());
        isLapCalculated = true;
    }
    ui->SmoothingSlider->setEnabled(true);
}

//Allows the Frequency Removal specified by Removal Slider
void MainWindow::on_RemovalButton_clicked()
{
    if (!isLapCalculated)
    {
        int mode = 0;
        if (ui->graphLap->isChecked()) mode = 1;
        else mode = 2;
        ui->RenderingWidget->computeLaplacian(mode);
        isLapCalculated = true;
    }
    ui->RemovalSlider->setEnabled(true);
}

//To choose the Frequency to be colored in the Mesh
void MainWindow::on_ColoringSlider_valueChanged(int value)
{
    if(isLapCalculated)
    {
        ui->RenderingWidget->colorSpectrum(value);
    }
    QString s = QString::number(value);
    ui->freqValue->setText(s);
}

//Specifies from which frequency are going to be removed
void MainWindow::on_SmoothingSlider_valueChanged(int value)
{
    if (isSpectralComputed)
    {
        ui->RenderingWidget->smoothMesh(value);
    }
    QString s = QString::number(value);
    ui->freqValue_2->setText(s);
}

//Chooses an specific Frequency to be removed
void MainWindow::on_RemovalSlider_valueChanged(int value)
{
    if (isSpectralComputed)
    {
        ui->RenderingWidget->frequencyRemoval(value);
    }
    QString s = QString::number(value);
    ui->freqValue_3->setText(s);
}

//Specifies a factor to make the mesh editing
void MainWindow::on_ScaleFactorSlider_valueChanged(int value)
{
    int mode;
    if (ui->meshx->isChecked()) mode = 0;
    else mode = 1;
    double maximum = 4;
    double precision = 0.1;
    double size = ((maximum - precision)*value+maximum)/ui->ScaleFactorSlider->maximum();

    size = 2*((double)value/100.0) - 1;

    ui->sizeMeshLabel->setText(QString::number(size));
    if (isLapCalculated)
    {
        ui->RenderingWidget->meshEdit(size,mode);
    }
}
