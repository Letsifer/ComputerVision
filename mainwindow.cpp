#include "mainwindow.h"

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
    const auto firstVector = Descriptor::buildDescriptors(image1);

    const QString imageName2("lena-min-for45-45");
    const QImage qImage2 = QPixmap(inputImagesDir.absoluteFilePath(imageName2 + ".jpg")).toImage();
    const MyImage image2 = MyImage::createMyImageFromQImage(qImage2);
    auto secondVector = Descriptor::buildDescriptors(image2);
    auto matches = DescriptorMatcher::findMatchersBetweenDescriptors(firstVector, secondVector);
    auto result = drawMatches(qImage1, qImage2, matches);
    result.save(outputDir.absoluteFilePath(imageName1 + imageName2 + ".jpg"), "jpg");
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

QImage MainWindow::drawMatches(const QImage &image1, const QImage &image2, const vector<PointMatch> &matches) {
    QImage result = QImage(image1.width() + image2.width(),
                           max(image1.height(), image2.height()),
                           QImage::Format_RGB32
                           );
    for (int i = 0; i < image1.height(); i++) {
        for (int j = 0; j < image1.width(); j++) {
            result.setPixel(j, i, image1.pixel(j, i));
        }
    }
    for (int i = 0, shift = image1.width(); i < image2.height(); i++) {
        for (int j = 0; j < image2.width(); j++) {
            result.setPixel(j + shift, i, image2.pixel(j, i));
        }
    }

    QPainter painter(&result);
    QPen pen = QPen();
    pen.setWidth(2);
    painter.setPen(pen);
    int colorIndex = 0;
    vector<QColor> colors;
    colors.push_back(QColor(255, 0, 0)); //red
    colors.push_back(QColor(0, 255, 0)); //green
    colors.push_back(QColor(0, 0, 255)); //blue
    colors.push_back(QColor(0, 0, 0)); //black
    const int colorNumber = colors.size();
    for (const PointMatch& match : matches) {
        colorIndex = (colorIndex + 1) % colorNumber;
        pen.setColor(colors.at(colorIndex));
        painter.setPen(pen);
        const int x1 = match.firstX, y1 = match.firstY,
                  x2 = match.secondX + image1.width(),
                  y2 = match.secondY;
        painter.drawLine(x1, y1, x2, y2);
    }
    return result;
}

void MainWindow::ransac() {
    QDir inputImagesDir ("../ComputerVision/images");
    QDir().mkdir("../images/lab8");
    QDir outputDir ("../images/lab8");
    ui->label->setText(outputDir.absolutePath().append(" - all images are there"));

    const QString imageName1("image1");
    QString filepath = inputImagesDir.absoluteFilePath(imageName1 + ".jpg");
    QPixmap pix = QPixmap(filepath);
    const QImage qImage1 = pix.toImage();
    const MyImage image1 = MyImage::createMyImageFromQImage(qImage1);
    const auto firstVector = Descriptor::buildDescriptors(image1);

    const QString imageName2("image2");
    const QImage qImage2 = QPixmap(inputImagesDir.absoluteFilePath(imageName2 + ".jpg")).toImage();
    const MyImage image2 = MyImage::createMyImageFromQImage(qImage2);
    auto secondVector = Descriptor::buildDescriptors(image2);

    auto matches = DescriptorMatcher::findMatchersBetweenDescriptors(firstVector, secondVector);

    auto resultOfMatches = drawMatches(qImage1, qImage2, matches);
    QString matchString = QString("matches-");
    resultOfMatches.save(outputDir.absoluteFilePath(matchString + imageName1 + imageName2 + ".jpg"), "jpg");

    for (PointMatch match : matches) {
        match.swapPoints();
    }

    auto homography = RansacAlgorithm::findHomography(matches);

    QImage result((qImage1.width() + qImage2.width()) / 2 * 1.1,
                  (qImage1.height() + qImage2.height()) / 2 * 1.1,
                  QImage::Format_RGB32);
    QPainter painter(&result);
    painter.drawImage(0, 0, qImage2);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QTransform transform(homography.at(0), homography.at(3), homography.at(6),
                         homography.at(1), homography.at(4), homography.at(7),
                         homography.at(2), homography.at(5), homography.at(8)
                         );
    painter.setTransform(transform);
    painter.drawImage(0, 0, qImage1);
    result.save(outputDir.absoluteFilePath(imageName1 + imageName2 + ".jpg"), "jpg");
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ransac();
}

MainWindow::~MainWindow()
{
    delete ui;
}
