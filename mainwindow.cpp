#include "mainwindow.h"
#include "MyImage.h"
#include "pyramid.h"
#include <QDir>
#include <QTextStream>
#include "ui_mainwindow.h"
#include <iostream>
#include <memory>
using namespace std;

void MainWindow::lab1() {
    //need 4 labels
    QDir dir ("../ComputerVision/images");
    QDir().mkdir("../images");
    QPixmap pix(dir.absoluteFilePath("wall.jpg"));
    ui->imageLabel->setPixmap(pix);


    QImage image = pix.toImage();
    auto mine = MyImage::createMyImageFromQImage(image);

    auto convoltY = mine.convoluton(Kernel::createYSobelKernel(), BorderType::CopyBorder);
    auto convoltX = mine.convoluton(Kernel::createXSobelKernel(), BorderType::CopyBorder);
    convoltX.save("lab1SobelX.jpg");
    convoltY.save("lab1SobelY.jpg");

    auto resultSobelLab1 = convoltX.countHypotenuse(convoltY);
    resultSobelLab1.save("lab1SobelResult.jpg");

    ui->imageLabel->setPixmap(QPixmap::fromImage(resultSobelLab1.createQImageFromImage()));

    double sigma = 5;
    auto convoltGaussY = mine.convoluton(Kernel::createYGaussKernel(sigma), BorderType::CopyBorder);
    auto convoltGaussX = mine.convoluton(Kernel::createXGaussKernel(sigma), BorderType::CopyBorder);
    convoltGaussX.save("lab1GaussX.jpg");
    convoltGaussY.save("lab1GaussY.jpg");

    auto resultGaussLab1 = convoltGaussX.countHypotenuse(convoltGaussY);
    resultGaussLab1.save("lab1GaussResult.jpg");

    QDir dir2 ("../images");
    ui->label->setText(dir2.absolutePath().append(" - all images are there"));
    ui->labelSigma->setText(QString::fromStdString("sigma is " + to_string(sigma)));
    ui->imageResult->setPixmap(QPixmap::fromImage(resultGaussLab1.createQImageFromImage()));
}

void MainWindow::lab2() {
    QDir dir ("../ComputerVision/images");
    QDir().mkdir("../images");
    QPixmap pix(dir.absoluteFilePath("ialta.jpg"));
    QImage image = pix.toImage();
    auto mine = MyImage::createMyImageFromQImage(image);
    const int scalesInOctave = 5, octaves = 3;
    const double basicSigma = 0.5;
    Pyramid pyramid = Pyramid(mine, basicSigma, octaves, scalesInOctave);
    pyramid.savePyramid(QString::fromStdString("lab2-"));
    QDir dir2 ("../images");
    ui->label->setText(dir2.absolutePath().append(" - all images are there"));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lab2();
}

MainWindow::~MainWindow()
{
    delete ui;
}
