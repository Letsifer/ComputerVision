#include "mainwindow.h"
#include "MyImage.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <memory>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pix("C://QtProjects//ComputerVision//images//wall.jpg");
    ui->imageLabel->setPixmap(pix);


    QImage image = pix.toImage();
    unique_ptr<MyImage> mine = make_unique<MyImage>(image.height(), image.width(), image);

    unique_ptr<MyImage> convoltY = mine->convoluton(*Kernel::createYSobelKernel(), BorderType::MirrorBorder)->normalize();
    unique_ptr<MyImage> convoltX = mine->convoluton(*Kernel::createXSobelKernel(), BorderType::MirrorBorder)->normalize();
    convoltX->save("lab1SobelX.jpg");
    convoltY->save("lab1SobelY.jpg");

    unique_ptr<MyImage> resultSobelLab1 = convoltX->countHypotenuse(*convoltY)->normalize();
    resultSobelLab1->save("lab1SobelResult.jpg");

    ui->imageLabel->setPixmap(QPixmap::fromImage(resultSobelLab1->createQImageFromImage()));

    double sigma = 5;
    unique_ptr<MyImage> convoltGaussY = mine->convoluton(*Kernel::createYGaussKernel(sigma), BorderType::MirrorBorder)->normalize();
    unique_ptr<MyImage> convoltGaussX = mine->convoluton(*Kernel::createXGaussKernel(sigma), BorderType::MirrorBorder)->normalize();
    convoltGaussX->save("lab1GaussX.jpg");
    convoltGaussY->save("lab1GaussY.jpg");

    unique_ptr<MyImage> resultGaussLab1 = convoltGaussX->countHypotenuse(*convoltGaussY)->normalize();
    resultGaussLab1->save("lab1GaussResult.jpg");
    ui->label->setText(QGuiApplication::applicationDirPath() + "\images\ - all images are there");
    ui->labelSigma->setText(QString::fromStdString("sigma is " + to_string(sigma)));
    ui->imageResult->setPixmap(QPixmap::fromImage(resultGaussLab1->createQImageFromImage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
