#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_SpecButton_clicked()
{
    ui->RenderingWidget->calculateSpectrum(ui->horizontalSlider->value());
    ui->horizontalSlider->setMaximum(ui->RenderingWidget->getGlobalMesh().Vertices().size()-1);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
     ui->RenderingWidget->colorSpectrum(value);
     QString s = QString::number(value);
     ui->freqValue->setText(s);
}

void MainWindow::on_smoothButton_clicked()
{
    ui->RenderingWidget->smoothMesh();
}

void MainWindow::on_browseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",tr("VRML File (*.wrl)"));
    ui->RenderingWidget->loadMesh(fileName.toStdString());
}
