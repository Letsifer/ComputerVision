#ifndef DESCRIPTORFINDER_H
#define DESCRIPTORFINDER_H
#include<cmath>
#include"MyImage.h"
#include <iostream>
using namespace std;

struct Bin {
    double value;
    double centerOfBin;
    Bin() : value(0), centerOfBin(0){}
    Bin(double center, double value) : value(value), centerOfBin(center){}
    double getDistanceToCenter(double angle) const{
        double absolutDistance = abs(angle - centerOfBin);
        return min(absolutDistance, 2 * M_PI - absolutDistance);
    }
};

struct Histogram {
    unique_ptr<Bin[]> bins;
    int binsNumber;
    Histogram(int binsNumber) : binsNumber(binsNumber) {
        bins = make_unique<Bin[]>((size_t)(binsNumber));
        double basicValue = M_PI / binsNumber;
        for (int i = 0; i < binsNumber; i++) {
            bins[i].centerOfBin = (2 * i + 1) * basicValue;
        }
    }

    double getBinValue(int i) const{
        return bins[i].value;
    }

    pair<int, int> getNeighborsToPoint(double angle) const{
        int indexOfMin = -1, indexOfSecondMin = -1;
        double minValue = numeric_limits<double>::max(), secondMinValue = minValue;
        for (int i = 0; i < binsNumber; i++) {
            double distanceToCenter = bins[i].getDistanceToCenter(angle);
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
};

class Descriptor
{
    int pointX, pointY;
    unique_ptr<double[]> elements;
    int sizeOfDescriptor;
    void normalize();
    double findAngleCoefficient(double angle, const Bin& bin1, const Bin& bin2) const;
    double findDistanceCoefficient(int x, int y) const;
public:
    Descriptor(const MyImage& sobelX, const MyImage& sobelY,
               int pointX, int pointY,
               int regionsX, int regionsY,
               int sizeOfNetworkX, int sizeOfNetworkY,
               int basketsInHystogramm
               );
    int getPointX() const {return pointX;}
    int getPointY() const {return pointY;}
    double getElement(int i) const{
        return elements[i];
    }
    int getSize() const {
        return sizeOfDescriptor;
    }
    double getDistance(const Descriptor& descriptor) const;
};

#endif // DESCRIPTORFINDER_H
