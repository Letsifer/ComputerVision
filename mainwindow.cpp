#include "mainwindow.h"
#include "pyramid.h"
#include <QDir>
#include <QPainter>
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
    QString name("ialta.jpg");
    QPixmap pix(dir.absoluteFilePath(name));
    QImage image = pix.toImage();
    auto mine = MyImage::createMyImageFromQImage(image);
    const int scalesInOctave = 5, octaves = 3;
    const double basicSigma = 0.5;
    Pyramid pyramid = Pyramid(mine, basicSigma, octaves, scalesInOctave);
    pyramid.savePyramid(name.append(QString::fromStdString("lab2-")));
    QDir dir2 ("../images");
    ui->label->setText(dir2.absolutePath().append(" - all images are there"));
}

void MainWindow::lab3() {
    QDir dir ("../ComputerVision/images");
    QDir().mkdir("../images/lab3");
    QDir dir2 ("../images/lab3");
    ui->label->setText(dir2.absolutePath().append(" - all images are there"));
    QString name("lena.jpg");
    QPixmap pix(dir.absoluteFilePath(name));
    workWithImageInThirdLab(name, pix.toImage());
//    QString imageName1("image1.jpg");
//    QPixmap imagePix1(dir.absoluteFilePath(imageName1));
//    workWithImageInThirdLab(imageName1, imagePix1.toImage());
//    QString imageName2("image2.jpg");
//    QPixmap imagePix2(dir.absoluteFilePath(imageName2));
//    workWithImageInThirdLab(imageName2, imagePix2.toImage());
}
void MainWindow::workWithImageInThirdLab(const QString filename, const QImage& image) {
    auto mine = MyImage::createMyImageFromQImage(image);
    const int sizeOfWindow = 5, windowsShift = 1;
    const double contrastMoravecBorder = 0.001;
    auto points = InterestPointsFinder::moravecAlgorithm(
                mine,
                windowsShift,
                sizeOfWindow,
                contrastMoravecBorder,
                BorderType::MirrorBorder
                );
    printForThirdLab(image, points, filename + QString::fromStdString("-moravecBeforeSuppresion.jpg"));
    const int necessaryPoints = 150;
    InterestPointsFinder::adaptiveNonMaximumSuppression(
                points, necessaryPoints
                );
    string moravecAfterSuppression = "-moravecAfterSuppresion" + to_string(necessaryPoints) + "_points.jpg";
    printForThirdLab(image, points, filename + QString::fromStdString(moravecAfterSuppression));

    const double contrastHarrisBorder = 4;
    points = InterestPointsFinder::harrisAlgorithm(mine, 7, contrastHarrisBorder, BorderType::MirrorBorder);
    printForThirdLab(image, points, filename + QString::fromStdString("-harrisBeforeSuppresion.jpg"));
    InterestPointsFinder::adaptiveNonMaximumSuppression(
                points, necessaryPoints
                );
    string harrisAfterSuppression = "-harrisAfterSuppresion" + to_string(necessaryPoints) + "_points.jpg";
    printForThirdLab(image, points, filename + QString::fromStdString(harrisAfterSuppression));
}
void MainWindow::printForThirdLab(QImage image, const vector<InterestingPoint> points, const QString filename) {
    QPainter painter(&image);
    painter.setPen(QColor(255, 0, 0));
    const int radius = 6;
    for (auto &point : points) {
        painter.drawEllipse(point.x - radius / 2, point.y - radius / 2, radius, radius);
    }
    QDir dir ("../images/lab3");
    image.save(dir.absoluteFilePath(filename), "jpg");
}

void MainWindow::showDescriptorInfo(const Descriptor &desc, QPainter& painter, const int xShift) {
    int length = 25;
    QPen pen = QPen(QColor(0, 255, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    int x1 = desc.getPointX() + xShift, y1 = desc.getPointY();
    painter.drawEllipse(x1, y1, 5, 5);

    double angle = desc.getRotatedAngle();
    int x3 = x1 + length * cos(angle),
        y3 = y1 + length * sin(angle);
    painter.drawLine(x1, y1, x3, y3);

    pen = QPen(QColor(0, 0, 255));
    pen.setWidth(3);
    painter.setPen(pen);
    if (y3 < 0) {
        y3 = -y3;
    }
    painter.drawText(x3, y3, QString::number(angle));
}

void MainWindow::lab4() {
    QDir inputImagesDir ("../ComputerVision/images");
    QDir().mkdir("../images/lab4");
    QDir outputDir ("../images/lab4");
    ui->label->setText(outputDir.absolutePath().append(" - all images are there"));
    const int points = 100;

    QString imageName1("lena-min-for45-0");
    QImage qImage1 = QPixmap(inputImagesDir.absoluteFilePath(imageName1 + ".jpg")).toImage();
    MyImage image1 = MyImage::createMyImageFromQImage(qImage1);
    auto firstVector = findPoints(image1, points);

    QString imageName2("lena-min-for45-45");
    QImage qImage2 = QPixmap(inputImagesDir.absoluteFilePath(imageName2 + ".jpg")).toImage();
    MyImage image2 = MyImage::createMyImageFromQImage(qImage2);
    auto secondVector = findPoints(image2, points);

    QImage result = QImage(image1.getWidth() + image2.getWidth(),
                           max(image1.getHeight(), image2.getHeight()),
                           QImage::Format_RGB32
                           );
    for (int i = 0; i < qImage1.height(); i++) {
        for (int j = 0; j < qImage1.width(); j++) {
            result.setPixel(j, i, qImage1.pixel(j, i));
        }
    }
    for (int i = 0, shift = qImage1.width(); i < qImage2.height(); i++) {
        for (int j = 0; j < qImage2.width(); j++) {
            result.setPixel(j + shift, i, qImage2.pixel(j, i));
        }
    }

    QPainter painter(&result);
    QPen pen = QPen();
    pen.setWidth(2);
    painter.setPen(pen);
    int color = 0;
    const double T = 0.8;
    QColor red = QColor(255, 0, 0),
           green = QColor(0, 255, 0),
           blue = QColor(0, 0, 255),
           black = QColor(0, 0, 0);
    for (unsigned int i = 0; i < firstVector.size(); i++) {
        double minDistance = numeric_limits<double>::max(), secondMinDistance = minDistance;
        int indexMin = -1;
        for (unsigned int j = 0; j < secondVector.size(); j++) {
            double distance = firstVector[i].getDistance(secondVector[j]);
            if (minDistance > distance) {
                secondMinDistance = minDistance;
                minDistance = distance;
                indexMin = j;
                continue;
            }
            if (secondMinDistance > distance) {
                secondMinDistance = distance;
            }
        }
        double rate = minDistance / secondMinDistance;
        if (rate <= T) {
            switch (color) {
            case 0:
                pen.setColor(red);
                painter.setPen(pen);
                color++;
                break;
            case 1:
                pen.setColor(blue);
                painter.setPen(pen);
                color++;
                break;
            case 2:
                pen.setColor(green);
                painter.setPen(pen);
                color++;
                break;
            case 3:
                pen.setColor(black);
                painter.setPen(pen);
                color = 0;
                break;
            }

            int x1 = firstVector[i].getPointX(), y1 = firstVector[i].getPointY(),
                x2 = secondVector[indexMin].getPointX() + qImage1.width(), y2 = secondVector[indexMin].getPointY();
            painter.drawLine(x1, y1, x2, y2);
        }
    }
    result.save(outputDir.absoluteFilePath(imageName1 + imageName2 + ".jpg"), "jpg");
}

vector<Descriptor> MainWindow::findPoints(const MyImage& image, int points) {
    const double contrastHarrisBorder = 0.01;
    const int regionsX = 4, regionsY = 4, sizeOfGrid = 16, binsInHistogram = 8, windowSize = 7;
    const int manyBins = 36;
    auto pointVector = InterestPointsFinder::harrisAlgorithm(image, windowSize, contrastHarrisBorder, BorderType::MirrorBorder);
    InterestPointsFinder::adaptiveNonMaximumSuppression(
                pointVector, points
                );
    MyImage sobelX = image.convoluton(Kernel::createXSobelKernel(), BorderType::MirrorBorder),
            sobelY = image.convoluton(Kernel::createYSobelKernel(), BorderType::MirrorBorder);
    vector<Descriptor> descriptors;
    for (InterestingPoint point : pointVector) {
        auto added = Descriptor::createOrientedDescriptors(
                    sobelX, sobelY,
                    point.x, point.y,
                    regionsX, regionsY,
                    sizeOfGrid, sizeOfGrid,
                    binsInHistogram, manyBins);
        descriptors.push_back(added[0]);
        if (added.size() == 2) {
            descriptors.push_back(added[1]);
        }
    }
    return descriptors;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lab4();
}

MainWindow::~MainWindow()
{
    delete ui;
}
