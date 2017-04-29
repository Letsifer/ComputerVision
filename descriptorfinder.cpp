#include "descriptorfinder.h"


vector<Descriptor> Descriptor::buildDescriptors(
            const MyImage& image
            ) {
    const Pyramid pyramid = Pyramid(image, octaves, scalesInOctave);
    const double basicSigma = pyramid.getElement(0, 0).currentSigma;
    const auto centers = pyramid.createDogPyramid().findExtremums();
    const Kernel kernelSobelX = Kernel::createXSobelKernel(),
                 kernelSObelY = Kernel::createYSobelKernel();
    vector<Descriptor> result;
    for (const BlobsCenter& center : centers) {
        const MyImage currentImage = pyramid.getElement(center.octave, center.scale).image;
        const double harrisValue = InterestPointsFinder::computeHarrisInOnePoint(
                    currentImage,
                    center.y, center.x, center.sigma * M_SQRT2
                    );
        if (harrisValue > harrisThreshold) {
            const MyImage sobelX = currentImage.convoluton(kernelSobelX, BorderType::MirrorBorder),
                          sobelY = currentImage.convoluton(kernelSObelY, BorderType::MirrorBorder);
            const double sigma = center.sigma;
            const int pointX = center.x, pointY = center.y;
            Descriptor notOriented = Descriptor(
                        sobelX, sobelY,
                        pointX, pointY,
                        1, 1,
                        manyBinsNumber, 0,
                        false,
                        sigma, basicSigma, 0, center.octave
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
            const double firstAngle = notOriented.calculateAngle(indexOfMax, manyBinsNumber);
            result.push_back(Descriptor(
                                sobelX, sobelY,
                                pointX, pointY,
                                regionsX, regionsY,
                                binsInHistogram, firstAngle,
                                true,
                                sigma, basicSigma, center.globalSigma, center.octave
                                ));
            const double maxValue = notOriented.getElement(indexOfMax),
                         secondMaxValue = notOriented.getElement(indexOfSecondMax);
            if (maxValue * BORDER_OF_CHOOSING_SECOND_PICK <= secondMaxValue) {
                const double secondAngle = notOriented.calculateAngle(indexOfSecondMax, manyBinsNumber);
                result.push_back(Descriptor(sobelX, sobelY,
                                    pointX, pointY,
                                    regionsX, regionsY,
                                    binsInHistogram, secondAngle,
                                    true,
                                    sigma, basicSigma, center.globalSigma, center.octave
                                    ));
            }
        }
    }
    return result;
}

double Descriptor::calculateAngle(const int index, const int binsNumber) const {
    const double left = elements[(index - 1 + binsNumber) % binsNumber];
    const double center = elements[index];
    const double right = elements[(index + 1) % binsNumber];
    const double x = -0.5 * (right - left) / (left - 2.0 * center + right);
    const double angle = 2.0 * M_PI * (x + 0.5 + index) / binsNumber;
    return angle;
}

Descriptor::Descriptor(const MyImage& sobelX, const MyImage& sobelY,
                       int pointX, int pointY,
                       int regionsX, int regionsY,
                       int binsInHistogram, double angleShift,
                       bool makeNormalize, double sigma, double basicSigma,
                       double globalSigma, int octave
                       ) {
    point = InterestingPoint(pointX, pointY);
    point.octave = octave;
    point.localSigma = sigma;
    point.globalSigma = globalSigma;
    point.rotationAngle = angleShift;

    sizeOfDescriptor = regionsX * regionsY * binsInHistogram;
    elements = make_unique<double[]>((size_t)(sizeOfDescriptor));
    for (int i = 0; i < sizeOfDescriptor; i++) {
        elements[i] = 0;
    }

    unique_ptr<double[]> centersOfBins = make_unique<double[]>((size_t)(binsInHistogram));
    const double centerBasicValue = M_PI / binsInHistogram;
    for (int i = 0; i < binsInHistogram; i++) {
        centersOfBins[i] = (2 * i + 1) * centerBasicValue;
    }

    const double basicValue = 2 * centerBasicValue;

    const double sizeCoef = sigma / basicSigma;
    const int scaledSizeOfGrid = sizeCoef * sizeOfGrid;

    const double sizeOfRegionX = (double)scaledSizeOfGrid / regionsX,
                 sizeOfRegionY = (double)scaledSizeOfGrid / regionsY;

    const double cosOfAngle = cos(angleShift),
                 sinOfAngle = sin(angleShift);
    const int halfSize = scaledSizeOfGrid / 2;
    for (int i = 0; i < scaledSizeOfGrid; i++) {
        for (int j = 0; j < scaledSizeOfGrid; j++) {
            const int nonRotatedX = j - halfSize + 1,
                      nonRotatedY = i - halfSize + 1;
            const int rotatedX = nonRotatedX * cosOfAngle + nonRotatedY * sinOfAngle + halfSize - 1,
                      rotatedY = - nonRotatedX * sinOfAngle + nonRotatedY * cosOfAngle + halfSize - 1;
            if (rotatedX < 0 || rotatedX > scaledSizeOfGrid - 1 ||
                rotatedY < 0 || rotatedY > scaledSizeOfGrid - 1) {
                continue;
            }
            const int x =  j - halfSize + 1;
            const int y =  i - halfSize + 1;
            const double dx = sobelX.getBorderPixel(y + pointY, x + pointX, BorderType::MirrorBorder),
                         dy = sobelY.getBorderPixel(y + pointY, x + pointX, BorderType::MirrorBorder);
            const double pixel = sqrt(dx*dx+dy*dy) * findDistanceCoefficient(x, y, sigma);

            const int regionsIndexX = rotatedX / sizeOfRegionX / sizeCoef,
                      regionsIndexY = rotatedY / sizeOfRegionY / sizeCoef;
            const int regionsIndex = regionsIndexY * regionsX + regionsIndexX;

            double rotatedAngle = getAngle(dx, dy, angleShift);
            int rightBin = floor(rotatedAngle / basicValue + 0.5);
            int leftBin = rightBin - 1;
            if (rightBin == binsInHistogram) {
                rightBin = 0;
            }
            if (leftBin < 0) {
                leftBin = binsInHistogram - 1;
            }

            elements[regionsIndex * binsInHistogram + leftBin] += pixel
                    * findAngleCoefficient(rotatedAngle, centersOfBins[rightBin], basicValue);
            elements[regionsIndex * binsInHistogram + rightBin] += pixel
                    * findAngleCoefficient(rotatedAngle, centersOfBins[leftBin], basicValue);
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

double Descriptor::findAngleCoefficient(double angle, double centerOfAnotherBin, double basicValue) const{
    return getDistanceToCenterOfBin(centerOfAnotherBin, angle) / basicValue;
}

double Descriptor::findDistanceCoefficient(int x, int y, double sigma) const {
    return Kernel::canculateGaussInOnePoint(sigma, x, y);
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

Descriptor::Descriptor(const Descriptor& sample) {
    point = InterestingPoint(sample.point);

    sizeOfDescriptor = sample.sizeOfDescriptor;
    elements = make_unique<double[]>((size_t)(sizeOfDescriptor));
    for (int i = 0; i < sizeOfDescriptor; i++) {
        elements[i] = sample.elements[i];
    }
}

Descriptor& Descriptor::operator=(const Descriptor& sample) {
    if (this == &sample) {
        return *this;
    }
    point = InterestingPoint(sample.point);

    sizeOfDescriptor = sample.sizeOfDescriptor;
    elements = make_unique<double[]>((size_t)(sizeOfDescriptor));
    for (int i = 0; i < sizeOfDescriptor; i++) {
        elements[i] = sample.elements[i];
    }
    return *this;
}
