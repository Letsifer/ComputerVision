#include "MyImage.h"
#include <QDir>
#include <cmath>
#include <iostream>
#include <memory>
using namespace std;

MyImage::MyImage()
{
    height = 0; width = 0;
    pixels = nullptr;
}

unique_ptr<MyImage> MyImage::createMyImageFromQImage(const QImage qImage) {
    const int height = qImage.height(), width = qImage.width();
    unique_ptr<MyImage> result = make_unique<MyImage>(height, width);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            const QRgb rgb = qImage.pixel(j, i);
            double red = qRed(rgb) * 0.299,
                   green = qGreen(rgb) * 0.587,
                    blue = qBlue(rgb) * 0.114;
            result->setPixel(i, j, red + green + blue);
        }
    }
    return result;
}

MyImage::MyImage(int height, int width)
{
    this->height = height;
    this->width = width;
    pixels = make_unique<double[]>((size_t) (width * height));
}

void MyImage::setPixel(int i, int j, double value) {
    pixels[i * width + j] = value;
}

double MyImage::getPixel(int i, int j) const{
    return pixels[i * width + j];
}

unique_ptr<MyImage> MyImage::convoluton(const Kernel* kernel, const BorderType borderType) {
    unique_ptr<MyImage> result = make_unique<MyImage>(height, width);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double value = pixelConvolution(i, j, kernel, borderType);
            result->setPixel(i, j, value);
        }
    }
    return result;
}

double MyImage::pixelConvolution(int y, int x, const Kernel* kernel, const BorderType borderType) const{
    double result = 0;
    int rangeX = kernel->getKernelHalfWidth();
    int rangeY = kernel->getKernelHalfHeight();
    for (int i = -rangeY; i <= rangeY; i++) {
        int currentY = y + i;
        for (int j = -rangeX; j <= rangeX; j++) {
            int currentX = x + j;
            bool areNotInRangeValue = areNotInRange(currentX, currentY);
            double pixelResult = areNotInRangeValue
                    ? getBorderPixel(currentY, currentX, borderType)
                    : getPixel(currentY, currentX);
            result += pixelResult * kernel->getElementInRelationToCenter(j, i);
        }
    }
    return result;
}

double MyImage::getBorderPixel(int i, int j, const BorderType borderType) const{
    switch (borderType) {
    case BorderType::BlackBorder: return BLACK_BORDER;
    case BorderType::WhiteBorder: return WHITE_BORDER;
    case BorderType::MirrorBorder:
        return getPixel(
                    i < 0 ? abs(i) : (i >= height ? 2 * height - i - 1 : i),
                    j < 0 ? abs(j) : (j >= width  ? 2 * width - j - 1  : j)
                );
    case BorderType::CopyBorder:
        return getPixel(
                    i < 0 ? 0 : (i >= height ? height - 1 : i),
                    j < 0 ? 0 : (j >= width ? width - 1 : j)
                );
    }
    return -1.0;
}

shared_ptr<MyImage> MyImage::normalize(double newMin, double newMax) {
    const double newRange = newMax - newMin;
    shared_ptr<MyImage> result = make_shared<MyImage>(height, width);

    auto minmax = minmax_element(&pixels[0], &pixels[width * height]);
    const double minBorder = *minmax.first, maxBorder = *minmax.second;
    double range = maxBorder - minBorder;
    if (range == 0) {
        range = 1;
    }
    const double coefficient = newRange / range;
    transform(
                &pixels[0], &pixels[width * height],
                &result->pixels[0],
                [=](double value){return convertToAnotherRange(value, minBorder, newMin, coefficient);}
            );
    return result;
}

double MyImage::convertToAnotherRange(double value, double oldMin, double newMin, double coefficient) {
    return newMin + coefficient * (value - oldMin);
}

unique_ptr<MyImage> MyImage::countHypotenuse(const MyImage* other) {
   unique_ptr<MyImage> result = make_unique<MyImage>(height, width);
   transform(
               &pixels[0], &pixels[width * height],
               &other->pixels[0], &result->pixels[0],
               [=](double value1, double value2){return sqrt(value1 * value1 + value2 * value2);}
           );
   return result;
}

bool MyImage::save(const QString filename) {
    QImage qImage = QImage(width, height, QImage::Format_RGB32);
    auto normalized = this->normalize(0, 255);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int color = (int)(normalized -> getPixel(i, j) * 255.);
            qImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
    QDir dir ("../images");
    return qImage.save(dir.absoluteFilePath(filename), "jpg");
}

QImage MyImage::createQImageFromImage(){
    QImage image = QImage(width, height, QImage::Format_RGB32);
    auto normalized = this->normalize(0, 255);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int color = (int)(normalized -> getPixel(i, j));
            image.setPixel(j, i, qRgb(color, color, color));
        }
    }
    return image;
}
