#include "MyImage.h"
#include <cmath>
#include <iostream>
#include <memory>
using namespace std;

MyImage::MyImage()
{
    height = 0; width = 0;
    pixels = nullptr;
}

MyImage::MyImage(int height, int width, const QImage image)
{
    this->height = height;
    this->width = width;
    this->pixels = make_unique<double[]>((size_t) (width * height));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            QRgb rgb = image.pixel(j, i);
            double red = qRed(rgb) * 0.299,
                   green = qGreen(rgb) * 0.587,
                    blue = qBlue(rgb) * 0.114;
            setPixel(i, j, min(1.0, (red + green + blue) / 255.));
        }
    }
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

unique_ptr<MyImage> MyImage::convoluton(const Kernel& kernel, const BorderType borderType) {
    unique_ptr<MyImage> result = make_unique<MyImage>(height, width);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double value = pixelConvolution(i, j, kernel, borderType);
            result->setPixel(i, j, value);
        }
    }
    return result;
}

double MyImage::pixelConvolution(int y, int x, const Kernel& kernel, const BorderType borderType) {
    double result = 0;
    int rangeX = kernel.getKernelHalfWidth();
    int rangeY = kernel.getKernelHalfHeight();
    for (int i = -rangeY; i <= rangeY; i++) {
        int currentY = y + i;
        for (int j = -rangeX; j <= rangeX; j++) {
            int currentX = x + j;
            bool areNotInRangeValue = areNotInRange(currentX, currentY);
            double pixelResult = areNotInRangeValue
                    ? getBorderPixel(currentY, currentX, borderType)
                    : getPixel(currentY, currentX);
            result += pixelResult * kernel.getElementInRelationToCenter(j, i);
        }
    }
    return result;
}

double MyImage::getBorderPixel(int i, int j, const BorderType borderType) {
    switch (borderType) {
    case BorderType::BlackBorder: return BLACK_BORDER;
    case BorderType::WhiteBorder: return WHITE_BORDER;
    case BorderType::MirrorBorder:
        int mirrowedX = j < 0 ? abs(j) : (j >= width  ? 2 * width - j - 1  : j);
        int mirrowedY = i < 0 ? abs(i) : (i >= height ? 2 * height - i - 1 : i);
        return getPixel(mirrowedY, mirrowedX);
    }
    return -1.0;
}

unique_ptr<MyImage> MyImage::normalize() {
    unique_ptr<MyImage> result = make_unique<MyImage>(height, width);
    double minBorder = *min_element(&pixels[0], &pixels[width * height]),
           maxBorder = *max_element(&pixels[0], &pixels[width * height]);
    double range = maxBorder - minBorder;
    if (range == 0) {
        range = 1;
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            result->setPixel(i, j, (getPixel(i, j) - minBorder) / range);
        }
    }
    return result;
}

unique_ptr<MyImage> MyImage::countHypotenuse(const MyImage& other) {
   unique_ptr<MyImage> result = make_unique<MyImage>(height, width);
   for (int i = 0; i < height; i++) {
       for (int j = 0; j < width; j++) {
           double value1 = getPixel(i, j),
                  value2 = other.getPixel(i, j);
           result->setPixel(i, j, sqrt(value1 * value1 + value2 * value2));
       }
   }
   return result;
}

bool MyImage::save(const QString filename) const {
    QImage qImage = QImage(width, height, QImage::Format_RGB32);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int color = (int)(getPixel(i, j) * 255.);
            qImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
    return qImage.save(QGuiApplication::applicationDirPath() + "\images\ " + filename, "jpg");
}

MyImage& MyImage::operator=(MyImage &&other) {
    if (this != &other) {
        width = other.width;
        height = other.height;
        pixels = move(other.pixels);
    }
    return *this;
}

QImage MyImage::createQImageFromImage() const{
    QImage image = QImage(width, height, QImage::Format_RGB32);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int color = (int)(getPixel(i, j) * 255.);
            image.setPixel(j, i, qRgb(color, color, color));
        }
    }
    return image;
}
