#include "mainwindow.h"
#include "MyImage.h"
#include <QDir>
#include "ui_mainwindow.h"
#include <iostream>
#include <memory>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDir dir ("../ComputerVision/images");
    QDir().mkdir("../images");
    QPixmap pix(dir.absoluteFilePath("wall.jpg"));
    ui->imageLabel->setPixmap(pix);


    QImage image = pix.toImage();
    auto mine = MyImage::createMyImageFromQImage(image);

    auto convoltY = mine->convoluton(Kernel::createYSobelKernel().get(), BorderType::CopyBorder)->normalize(0, 1);
    auto convoltX = mine->convoluton(Kernel::createXSobelKernel().get(), BorderType::CopyBorder)->normalize(0, 1);
    convoltX->save("lab1SobelX.jpg");
    convoltY->save("lab1SobelY.jpg");

    auto resultSobelLab1 = convoltX->countHypotenuse(convoltY.get());
    resultSobelLab1->save("lab1SobelResult.jpg");

    ui->imageLabel->setPixmap(QPixmap::fromImage(resultSobelLab1->createQImageFromImage()));

    double sigma = 5;
    auto convoltGaussY = mine->convoluton(Kernel::createYGaussKernel(sigma).get(), BorderType::CopyBorder)->normalize(0, 1);
    auto convoltGaussX = mine->convoluton(Kernel::createXGaussKernel(sigma).get(), BorderType::CopyBorder)->normalize(0, 1);
    convoltGaussX->save("lab1GaussX.jpg");
    convoltGaussY->save("lab1GaussY.jpg");

    auto resultGaussLab1 = convoltGaussX->countHypotenuse(convoltGaussY.get());
    resultGaussLab1->save("lab1GaussResult.jpg");

    QDir dir2 ("../images");
    ui->label->setText(dir2.absolutePath().append(" - all images are there"));
    ui->labelSigma->setText(QString::fromStdString("sigma is " + to_string(sigma)));
    ui->imageResult->setPixmap(QPixmap::fromImage(resultGaussLab1->createQImageFromImage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
