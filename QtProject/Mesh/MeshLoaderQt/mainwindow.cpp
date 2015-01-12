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
    ui->SpecButton->setEnabled(false);
    ui->RenderingWidget->setLog(ui->logText);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_SpecButton_clicked()
{
    ui->RenderingWidget->calculateSpectrum(ui->horizontalSlider->value());
    isSpectralComputed=true;
    ui->SpecButton_2->setEnabled(true);
    ui->SpecButton_3->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if(isLapCalculated)
    {
        ui->RenderingWidget->colorSpectrum(value);
    }
    QString s = QString::number(value);
    ui->freqValue->setText(s);
}

void MainWindow::on_smoothButton_clicked()
{
}

void MainWindow::on_browseButton_clicked()
{
    isLapCalculated = false;
    isSpectralComputed = false;

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
    ui->horizontalSlider_4->setValue(25);
    ui->SpecButton->setEnabled(false);
    ui->SpecButton_2->setEnabled(false);
    ui->SpecButton_3->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->horizontalSlider_2->setEnabled(false);
    ui->horizontalSlider_3->setEnabled(false);
    ui->horizontalSlider_4->setEnabled(false);
    ui->ComputeLaplacianButton->setEnabled(true);
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    if (isSpectralComputed)
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
    if (isSpectralComputed)
    {
        ui->RenderingWidget->frequencyRemoval(value);
    }
    QString s = QString::number(value);
    ui->freqValue_3->setText(s);
}

void MainWindow::on_horizontalSlider_2_actionTriggered(int action)
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
        ui->RenderingWidget->calculateSpectrum(ui->horizontalSlider->value());
        isLapCalculated = true;
    }
    ui->horizontalSlider_2->setEnabled(true);
}

void MainWindow::on_SpecButton_3_clicked()
{
    if (!isLapCalculated)
    {
        int mode = 0;
        if (ui->graphLap->isChecked()) mode = 1;
        else mode = 2;
        ui->RenderingWidget->computeLaplacian(mode);
        isLapCalculated = true;
    }
    ui->horizontalSlider_3->setEnabled(true);
}


void MainWindow::on_ComputeLaplacianButton_clicked()
{
    int mode = 0;
    if (ui->graphLap->isChecked()) mode = 1;
    else mode = 2;
    ui->RenderingWidget->computeLaplacian(mode);
    ui->SpecButton->setEnabled(true);
    isLapCalculated=true;

    ui->SpecButton->setEnabled(true);
    ui->horizontalSlider_4->setEnabled(true);
}

void MainWindow::on_horizontalSlider_4_valueChanged(int value)
{
    int mode;
    if (ui->meshx->isChecked()) mode = 0;
    else mode =1;
    double maximum = 4;
    double precision = 0.1;
    double size = ((maximum - precision)*value+maximum)/ui->horizontalSlider_4->maximum();
    ui->sizeMeshLabel->setText(QString::number(size));
    if (isLapCalculated)
    {
        ui->RenderingWidget->meshEdit(size,mode);
    }
}

void MainWindow::on_meshx_clicked()
{
    ui->meshy->setChecked(false);
}

void MainWindow::on_meshy_clicked()
{
    ui->meshx->setChecked(false);
}
