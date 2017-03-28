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

void MainWindow::lab4() {
    QDir dir ("../ComputerVision/images");
    QDir().mkdir("../images/lab4");
    QDir dir2 ("../images/lab4");
    ui->label->setText(dir2.absolutePath().append(" - all images are there"));
    const int points = 150;
    QFile file("../images/lab4/answer.txt");
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream( &file );


    QString imageName1("image1.jpg");
    QImage qImage1 = QPixmap(dir.absoluteFilePath(imageName1)).toImage();
    MyImage image1 = MyImage::createMyImageFromQImage(qImage1);
    auto firstVector = findPoints(image1, points, stream);

    QString imageName2("image2.jpg");
    QImage qImage2 = QPixmap(dir.absoluteFilePath(imageName2)).toImage();
    MyImage image2 = MyImage::createMyImageFromQImage(qImage2);
    auto secondVector = findPoints(image2, points, stream);

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
    painter.setPen(QColor(255, 0, 0));



        for (int i = 0; i < firstVector[0].getSize(); i++) {
            double d = firstVector[0].getElement(i);
            stream << QString::number(d) << ' ';
        }
        stream << "\n";
        for (int i = 0; i < secondVector[0].getSize(); i++) {
            double d = secondVector[0].getElement(i);
            stream << QString::number(d) << " ";
        }


    const double T = 0.8;
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
            int x1 = firstVector[i].getPointX(), y1 = firstVector[i].getPointY(),
                x2 = secondVector[indexMin].getPointX(), y2 = secondVector[indexMin].getPointY();
            painter.drawLine(x1, y1, x2, y2);
        }
    }
    result.save(dir2.absoluteFilePath("lab4-result.jpg"), "jpg");
    } //end fileopen
}

vector<Descriptor> MainWindow::findPoints(const MyImage& image, int points, QTextStream& stream) {
    const double contrastHarrisBorder = 3;
    const int regionsX = 2, regionsY = 2, sizeOfRegion = 4, baskets = 8;
    auto pointVector = InterestPointsFinder::harrisAlgorithm(image, 7, contrastHarrisBorder, BorderType::MirrorBorder);
    InterestPointsFinder::adaptiveNonMaximumSuppression(
                pointVector, points
                );
    vector<Descriptor> descriptors;
    for (InterestingPoint point : pointVector) {
        descriptors.emplace_back(image, point.x, point.y, regionsX, regionsY, sizeOfRegion, sizeOfRegion, baskets);
    }
    for (int i = 0; i < descriptors[0].getSize(); i++) {
        double d = descriptors[0].getElement(i);
        stream << QString::number(d) << ' ';
    }
    stream << "\n";
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
