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
    Pyramid pyramid = Pyramid(mine, octaves, scalesInOctave);
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

void MainWindow::lab4() {
    QDir inputImagesDir ("../ComputerVision/images");
    QDir().mkdir("../images/lab6");
    QDir outputDir ("../images/lab6");
    ui->label->setText(outputDir.absolutePath().append(" - all images are there"));

    const QString imageName1("lena");
    const QImage qImage1 = QPixmap(inputImagesDir.absoluteFilePath(imageName1 + ".jpg")).toImage();
    const MyImage image1 = MyImage::createMyImageFromQImage(qImage1);
    const auto firstVector = findPoints(image1);

    const QString imageName2("lena-min-for30-30");
    const QImage qImage2 = QPixmap(inputImagesDir.absoluteFilePath(imageName2 + ".jpg")).toImage();
    const MyImage image2 = MyImage::createMyImageFromQImage(qImage2);
    auto secondVector = findPoints(image2);

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
        const double rate = minDistance / secondMinDistance;
        if (rate <= T) {
            switch (color) {
            case 0:
                pen.setColor(red);
                color++;
                break;
            case 1:
                pen.setColor(blue);
                color++;
                break;
            case 2:
                pen.setColor(green);
                color++;
                break;
            case 3:
                pen.setColor(black);
                color = 0;
                break;
            }
            painter.setPen(pen);
            const int x1 = firstVector[i].getPointX(),
                      y1 = firstVector[i].getPointY(),
                      x2 = secondVector[indexMin].getPointX() + qImage1.width(),
                      y2 = secondVector[indexMin].getPointY();
            painter.drawLine(x1, y1, x2, y2);
        }
    }
    result.save(outputDir.absoluteFilePath(imageName1 + imageName2 + ".jpg"), "jpg");
}
vector<Descriptor> MainWindow::findPoints(const MyImage& image) {
    const double harrisThreshold = 0.0008;
    const int scalesInOctave = 3, octaves = 5;
    const Pyramid pyramid = Pyramid(image, octaves, scalesInOctave);
    const auto centers = pyramid.createDogPyramid().findExtremums();
    const MyImage sobelX = image.convoluton(Kernel::createXSobelKernel(), BorderType::MirrorBorder),
                  sobelY = image.convoluton(Kernel::createYSobelKernel(), BorderType::MirrorBorder);
    vector<Descriptor> descriptors;
    for (const BlobsCenter& center : centers) {
        const double harrisValue = InterestPointsFinder::computeHarrisInOnePoint(
                    pyramid.getElement(center.octave, center.scale).image,
                    center.y, center.x, center.sigma * M_SQRT2
                    );
        if (harrisValue > harrisThreshold) {
            const double sizeFactor = pow(2.0, center.octave);
            const double sigma = center.sigma * M_SQRT2 * sizeFactor;
            auto added = Descriptor::createOrientedDescriptors(
                        sobelX, sobelY,
                        center.x * sizeFactor, center.y * sizeFactor,
                        sigma);
            descriptors.insert(descriptors.end(), added.begin(), added.end());
        }
    }
    return descriptors;
}

void MainWindow::findBlobs() {
    QDir inputImagesDir ("../ComputerVision/images");
    QDir().mkdir("../images/lab6");
    QDir outputDir ("../images/lab6");
    ui->label->setText(outputDir.absolutePath().append(" - all images are there"));

    const QString imageName1("rounds");
    QImage qImage = QPixmap(inputImagesDir.absoluteFilePath(imageName1 + ".jpg")).toImage();
    const MyImage image = MyImage::createMyImageFromQImage(qImage);
    const int scalesInOctave = 3, octaves = 5;
    const Pyramid pyramid = Pyramid(image, octaves, scalesInOctave);
    auto centers = pyramid.createDogPyramid().findExtremums();

    const double harrisThreshold = 0.001;
    QPainter painter(&qImage);
    QPen pen = QPen();
    pen.setWidth(1);
    pen.setColor(QColor(0, 255, 0));
    painter.setPen(pen);
    for (const BlobsCenter& center : centers) {
        const double radius = center.sigma * M_SQRT2;
        const double harrisValue = InterestPointsFinder::computeHarrisInOnePoint(
                    pyramid.getElement(center.octave, center.scale).image,
                    center.y, center.x, radius
                    );
        if (harrisValue > harrisThreshold) {
            const double sizeFactor = pow(2.0, center.octave);
            const double realRedius = radius * sizeFactor;
            painter.setPen(pen);
            painter.drawEllipse(QPointF(center.x * sizeFactor, center.y * sizeFactor),
                                realRedius, realRedius);
        }
    }
    qImage.save(outputDir.absoluteFilePath(imageName1 + ".jpg"), "jpg");
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    findBlobs();
    lab4();
}

MainWindow::~MainWindow()
{
    delete ui;
}
