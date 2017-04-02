#ifndef DESCRIPTORFINDER_H
#define DESCRIPTORFINDER_H
#include<cmath>
#include"MyImage.h"
#include <iostream>
using namespace std;

class Descriptor
{
    int pointX, pointY;

    int sizeOfDescriptor;
    unique_ptr<double[]> elements;

    int binsNumber;
    unique_ptr<double[]> centersOfBins;

    void normalize();
    double findAngleCoefficient(double angle, double center1, double center2) const;
    double findDistanceCoefficient(int x, int y) const;
    double getAngle(double dx, double dy) const {
        double angle = atan2(dy, dx);
        if (angle < 0) {
            angle += 2 * M_PI;
        }
        return angle;
    }

    double getDistanceToCenterOfBin(double center, double angle) const;
    pair<int, int> getNeighborsToPoint(double angle) const;
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
