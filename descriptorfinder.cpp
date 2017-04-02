#include "descriptorfinder.h"


Descriptor::Descriptor(const MyImage& sobelX, const MyImage& sobelY,
                       int pointX, int pointY,
                       int regionsX, int regionsY,
                       int sizeOfNetworkX, int sizeOfNetworkY,
                       int binsInHistogram
                       ) : pointX(pointX), pointY(pointY) {
    const double NORMALIZE_THRESHOLD = 0.2;
    sizeOfDescriptor = regionsX * regionsY * binsInHistogram;
    elements = make_unique<double[]>((size_t)(sizeOfDescriptor));
    vector<Histogram> histograms;
    for (int i = 0; i < regionsX * regionsY; i++) {
        histograms.emplace_back(binsInHistogram);
    }

    for (int i = 0; i < sizeOfNetworkY; i++) {
        for (int j = 0; j < sizeOfNetworkX; j++) {
            int x = pointX + j - sizeOfNetworkX / 2 + 1,
                y = pointY + i - sizeOfNetworkY / 2 + 1;
            int histogramIndexX = j / regionsX, histogramIndexY = i / regionsY;
            int histogramIndex = histogramIndexY * regionsX + histogramIndexX;
            double dx = sobelX.getBorderPixel(y, x, BorderType::MirrorBorder),
                    dy = sobelY.getBorderPixel(y, x, BorderType::MirrorBorder);
            double angle = atan2(dy, dx);
            if (angle < 0) {
                angle += 2 * M_PI;
            }
            auto pair = histograms[histogramIndex].getNeighborsToPoint(angle);
            int first = pair.first, second = pair.second;
            double pixel = sqrt(dx*dx+dy*dy);
            double value = pixel * findAngleCoefficient(angle,
                                               histograms[histogramIndex].bins[first],
                                               histograms[histogramIndex].bins[second]);
            value *= findDistanceCoefficient(x, y);
            histograms[histogramIndex].bins[first].value += value;

            value = pixel * findAngleCoefficient(angle,
                                        histograms[histogramIndex].bins[second],
                                        histograms[histogramIndex].bins[first]);
            value *= findDistanceCoefficient(x, y);
            histograms[histogramIndex].bins[second].value += value;
        }
    }

    for (int i = 0, cnt = 0; i < regionsX * regionsY; i++) {
        for (int j = 0; j < binsInHistogram; j++) {
            elements[cnt++] = histograms[i].getBinValue(j);
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

double Descriptor::findAngleCoefficient(double angle, const Bin& bin1,  const Bin& bin2) const{
    return abs(bin2.getDistanceToCenter(angle)) / bin1.getDistanceToCenter(bin2.centerOfBin);
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
