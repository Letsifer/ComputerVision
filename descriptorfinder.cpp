#include "descriptorfinder.h"


Descriptor::Descriptor(const MyImage& sobelX, const MyImage& sobelY,
                       int pointX, int pointY,
                       int regionsX, int regionsY,
                       int sizeOfGridX, int sizeOfGridY,
                       int binsInHistogram
                       ) : pointX(pointX), pointY(pointY) {
    const double NORMALIZE_THRESHOLD = 0.2;
    sizeOfDescriptor = regionsX * regionsY * binsInHistogram;
    elements = make_unique<double[]>((size_t)(sizeOfDescriptor));

    this->binsNumber = binsInHistogram;
    centersOfBins = make_unique<double[]>((size_t)(binsNumber));
    double basicValue = M_PI / binsNumber;
    for (int i = 0; i < binsNumber; i++) {
        centersOfBins[i] = (2 * i + 1) * basicValue;
    }

    for (int i = 0; i < sizeOfGridY; i++) {
        for (int j = 0; j < sizeOfGridX; j++) {
            int x = pointX + j - sizeOfGridX / 2 + 1,
                y = pointY + i - sizeOfGridY / 2 + 1;
            int regionsIndex = i / regionsY * regionsX + j / regionsX;
            double dx = sobelX.getBorderPixel(y, x, BorderType::MirrorBorder),
                   dy = sobelY.getBorderPixel(y, x, BorderType::MirrorBorder);
            double angle = getAngle(dx, dy);
            auto pair = getNeighborsToPoint(angle);
            int first = pair.first, second = pair.second;
            double pixel = sqrt(dx*dx+dy*dy);
            elements[regionsIndex * binsNumber + first] +=
                    pixel
                    * findAngleCoefficient(angle, centersOfBins[first], centersOfBins[second])
                    * findDistanceCoefficient(x, y);

            elements[regionsIndex * binsNumber + second] +=
                    pixel
                    * findAngleCoefficient(angle, centersOfBins[second], centersOfBins[first])
                    * findDistanceCoefficient(x, y);
        }
    }

    normalize();
    for (int i = 0; i < sizeOfDescriptor; i++) {
        elements[i] = min(elements[i], NORMALIZE_THRESHOLD);
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

double Descriptor::findAngleCoefficient(double angle, double center1, double center2) const{
    return getDistanceToCenterOfBin(center2, angle) / getDistanceToCenterOfBin(center1, center2);
}

double Descriptor::findDistanceCoefficient(int x, int y) const{
    return  1 - 0.03 * (abs(x - pointX) + abs(y - pointY));
}

double Descriptor::getDistance(const Descriptor& descriptor) const {
    double result = 0;
    for (int i = 0; i < sizeOfDescriptor; i++) {
        double dif = elements[i] - descriptor.elements[i];
        result += dif * dif;
    }
    return sqrt(result);
}

double Descriptor::getDistanceToCenterOfBin(double center, double angle) const{
    double absolutDistance = abs(angle - center);
    return min(absolutDistance, 2 * M_PI - absolutDistance);
}

pair<int, int> Descriptor::getNeighborsToPoint(double angle) const {
    int indexOfMin = -1, indexOfSecondMin = -1;
    double minValue = numeric_limits<double>::max(), secondMinValue = minValue;
    for (int i = 0; i < binsNumber; i++) {
        double distanceToCenter = getDistanceToCenterOfBin(centersOfBins[i], angle);
        if (minValue > distanceToCenter) {
            indexOfSecondMin = indexOfMin;
            secondMinValue = minValue;
            indexOfMin = i;
            minValue = distanceToCenter;
            continue;
        }
        if (secondMinValue > distanceToCenter) {
            indexOfSecondMin = i;
            secondMinValue = distanceToCenter;
        }
    }
    return pair<int, int>(indexOfMin, indexOfSecondMin);
}
