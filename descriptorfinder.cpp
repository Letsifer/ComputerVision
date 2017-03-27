#include "descriptorfinder.h"


Descriptor::Descriptor(const MyImage& image,
                       int pointX, int pointY,
                       int regionsX, int regionsY,
                       int sizeOfRegionX, int sizeOfRegionY,
                       int basketsInHystogramm
                       ) : pointX(pointX), pointY(pointY) {
    const double NORMALIZE_THRESHOLD = 0.2;
    sizeOfDescriptor = regionsX * regionsY * basketsInHystogramm;
    elements = make_unique<double[]>((size_t)(sizeOfDescriptor));
    int leftX = pointX + 1 - regionsX / 2 * sizeOfRegionX,
         topY = pointY + 1 - regionsY / 2 * sizeOfRegionY;
    for (int i = 0, cnt = 0; i < regionsX; i++) {
        for (int j = 0; j < regionsY; j++) {
            Hystogramm hystogramm = findHystogrammForRegion(image, leftX + sizeOfRegionX * j, topY + sizeOfRegionY * i, sizeOfRegionX, sizeOfRegionY, basketsInHystogramm);
            for (int k = 0; k < basketsInHystogramm; k++) {
                elements[cnt++] = hystogramm.getBasketValue(k);
            }
        }
    }
    normalize();
    for (int i = 0; i < sizeOfDescriptor; i++) {
        elements[i] = max(elements[i], NORMALIZE_THRESHOLD);
    }
    normalize();
}

void Descriptor::normalize() {
    double sumOfElements = 0;
    for (int i = 0; i < sizeOfDescriptor; i++) {
        sumOfElements += elements[i] * elements[i];
    }
    sumOfElements = sqrt(sumOfElements);
    for (int i = 0; i < sizeOfDescriptor; i++) {
        elements[i] /= sumOfElements;
    }
}

Hystogramm Descriptor::findHystogrammForRegion(const MyImage& image,
                                               int leftX, int topY,
                                               int width, int height,
                                               const int basketsInHystogramm
                                   ) {
    Hystogramm hystogramm = Hystogramm(basketsInHystogramm);
    int xCenterOfRegion = leftX + width / 2, yCenterOfRegion = topY + height / 2;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int x = leftX + j, y = topY + i;
            double angle = atan2(y - yCenterOfRegion, x - xCenterOfRegion);
            if (angle < 0) {
                angle += 2 * M_PI;
            }
            auto pair = hystogramm.getNeighborsToPoint(angle);
            Basket first, second;
            if (pair.first.centerOfBasket > pair.second.centerOfBasket) {
                first = pair.first;
                second = pair.second;
            } else {
                second = pair.first;
                first = pair.second;
            }
            double value = countValueForBasket(image.getBorderPixel(y, x, BorderType::MirrorBorder), angle, first.centerOfBasket, second.centerOfBasket);
            first.value += value * findDistanceCoefficient(x, y);
            value = countValueForBasket(image.getBorderPixel(y, x, BorderType::MirrorBorder), angle, second.centerOfBasket, first.centerOfBasket);
            second.value += value * findDistanceCoefficient(x, y);
        }
    }
    return hystogramm;
}

double Descriptor::countValueForBasket(double value, double angle, double center1, double center2) {
    return value * abs(angle - center1) / abs(center2 - center1);
}

double Descriptor::findDistanceCoefficient(int x, int y) {
    return 1 - 0.03 * (abs(x - pointX) + abs(y - pointY));
}

double Descriptor::getDistance(const Descriptor& descriptor) const {
    double result = 0;
    for (int i = 0; i < sizeOfDescriptor; i++) {
        double dif = elements[i] - descriptor.elements[i];
        result += dif * dif;
    }
    return sqrt(result);
}
