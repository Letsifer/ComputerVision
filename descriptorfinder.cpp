#include "descriptorfinder.h"

vector<Descriptor> Descriptor::createOrientedDescriptors(
        const MyImage &sobelX, const MyImage &sobelY,
        int pointX, int pointY,
        int regionsX, int regionsY,
        int sizeOfGridX, int sizeOfGridY,
        int binsInHistogram, int manyBinsNumber) {
    const double BORDER_OF_CHOOSING_SECOND_PICK = 0.8;
    Descriptor notOriented = Descriptor(
                sobelX, sobelY,
                pointX, pointY,
                1, 1,
                sizeOfGridX, sizeOfGridY,
                manyBinsNumber, 0, false
                );
    int indexOfMax = -1, indexOfSecondMax = -1;
    for (int i = 0; i < notOriented.sizeOfDescriptor; i++) {
        if (indexOfMax == -1 || notOriented.getElement(i) > notOriented.getElement(indexOfMax)) {
            indexOfSecondMax = indexOfMax;
            indexOfMax = i;
            continue;
        }
        if (indexOfSecondMax == -1 || notOriented.getElement(i) > notOriented.getElement(indexOfSecondMax)) {
            indexOfSecondMax = i;
        }
    }
    double angle = notOriented.centersOfBins[indexOfMax];
    vector<Descriptor> descs;
    descs.emplace_back(sobelX, sobelY,
                       pointX, pointY,
                       regionsX, regionsY,
                       sizeOfGridX, sizeOfGridY,
                       binsInHistogram, angle, true
                       );
    double maxValue = notOriented.getElement(indexOfMax),
           secondMaxValue = notOriented.getElement(indexOfSecondMax);
    if (maxValue * BORDER_OF_CHOOSING_SECOND_PICK <= secondMaxValue) {
        angle = notOriented.centersOfBins[indexOfSecondMax];
        descs.emplace_back(sobelX, sobelY,
                           pointX, pointY,
                           regionsX, regionsY,
                           sizeOfGridX, sizeOfGridY,
                           binsInHistogram, angle, true
                           );
    }
    return descs;
}

Descriptor::Descriptor(const MyImage& sobelX, const MyImage& sobelY,
                       int pointX, int pointY,
                       int regionsX, int regionsY,
                       int sizeOfGridX, int sizeOfGridY,
                       int binsInHistogram, double angleShift,
                       bool makeNormalize
                       ) : pointX(pointX), pointY(pointY), rotatedAngle(angleShift){
    const double NORMALIZE_THRESHOLD = 0.2;
    sizeOfDescriptor = regionsX * regionsY * binsInHistogram;
    elements = make_unique<double[]>((size_t)(sizeOfDescriptor));
    for (int i = 0; i < sizeOfDescriptor; i++) {
        elements[i] = 0;
    }

    this->binsNumber = binsInHistogram;
    centersOfBins = make_unique<double[]>((size_t)(binsNumber));
    double basicValue = M_PI / binsNumber;
    for (int i = 0; i < binsNumber; i++) {
        centersOfBins[i] = (2 * i + 1) * basicValue;
    }

    const int sizeOfRegionX = sizeOfGridX / regionsX,
              sizeOfRegionY = sizeOfGridY / regionsY;

    const double cosOfAngle = cos(angleShift),
                 sinOfAngle = sin(angleShift);
    int leftX = - sizeOfGridX / 2 + 1, rightX = sizeOfGridX / 2;
    int topY = - sizeOfGridY / 2 + 1, botY = sizeOfGridY / 2;
    for (int i = 0; i < sizeOfGridY; i++) {
        for (int j = 0; j < sizeOfGridX; j++) {
            int nonRotatedX = j - sizeOfGridX / 2 + 1,
                nonRotatedY = i - sizeOfGridY / 2 + 1;
            int rotatedX = nonRotatedX * cosOfAngle + nonRotatedY * sinOfAngle,
                rotatedY = - nonRotatedX * sinOfAngle + nonRotatedY * cosOfAngle;
            if (rotatedX < leftX || rotatedX > rightX || rotatedY < topY || rotatedY > botY) {
                continue;
            }
            int x = pointX + j - sizeOfGridX / 2 + 1;
            int y = pointY + i - sizeOfGridY / 2 + 1;
            rotatedX += sizeOfGridX / 2 - 1;
            rotatedY += sizeOfGridY / 2 - 1;
            int regionsIndex = rotatedY / sizeOfRegionY * regionsX + rotatedX / sizeOfRegionX;
            double dx = sobelX.getBorderPixel(y, x, BorderType::MirrorBorder),
                   dy = sobelY.getBorderPixel(y, x, BorderType::MirrorBorder);
            double angle = getAngle(dx, dy, angleShift);
            auto pair = getNeighborsToPoint(angle);
            int first = pair.first, second = pair.second;
            double pixel = sqrt(dx*dx+dy*dy) * findDistanceCoefficient(x, y, sizeOfGridX);
            elements[regionsIndex * binsNumber + first] += pixel
                    * findAngleCoefficient(angle, centersOfBins[first], centersOfBins[second]);

            elements[regionsIndex * binsNumber + second] += pixel
                    * findAngleCoefficient(angle, centersOfBins[second], centersOfBins[first]);
        }
    }
    if (makeNormalize) {
        normalize();
        for (int i = 0; i < sizeOfDescriptor; i++) {
            elements[i] = min(elements[i], NORMALIZE_THRESHOLD);
        }
        normalize();
    }

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

double Descriptor::findDistanceCoefficient(int x, int y, int gridSize) const {
    return max(1 - 0.03 * (abs(x - pointX) + abs(y - pointY)), 0.0);
//    return Kernel::canculateGaussInOnePoint(gridSize / 6, x - pointX, y - pointY);
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
