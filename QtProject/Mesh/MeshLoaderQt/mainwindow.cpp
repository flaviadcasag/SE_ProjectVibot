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
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_SpecButton_clicked()
{
    ui->RenderingWidget->calculateSpectrum(ui->horizontalSlider->value());
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
     ui->RenderingWidget->colorSpectrum(value);
     QString s = QString::number(value);
     ui->freqValue->setText(s);
}

void MainWindow::on_smoothButton_clicked()
{
    ui->RenderingWidget->smoothMesh(ui->horizontalSlider_2->value());
    QString s = QString::number(ui->horizontalSlider_2->value());
    ui->freqValue_2->setText(s);
}

void MainWindow::on_browseButton_clicked()
{
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
    QString s = QString::number(ui->horizontalSlider_2->value());
    ui->freqValue_2->setText(s);
}

void MainWindow::on_freqRemButton_clicked()
{
    ui->RenderingWidget->frequencyRemoval(ui->horizontalSlider_3->value());
    QString s = QString::number(ui->horizontalSlider_3->value());
    ui->freqValue_3->setText(s);
}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    QString s = QString::number(ui->horizontalSlider_3->value());
    ui->freqValue_3->setText(s);
}

void MainWindow::on_horizontalSlider_2_actionTriggered(int action)
{

}
