#ifndef DESCRIPTORFINDER_H
#define DESCRIPTORFINDER_H
#include<cmath>
#include"MyImage.h"
#include <iostream>
#include <QDir>
#include <QTextStream>
using namespace std;

class Descriptor
{
    int pointX, pointY;

    double rotatedAngle;

    int sizeOfDescriptor;
    unique_ptr<double[]> elements;

    int binsNumber;
    unique_ptr<double[]> centersOfBins;

    void normalize();
    double findAngleCoefficient(double angle, double center1, double center2) const;
    double findDistanceCoefficient(int x, int y, int gridSize) const;
    double getAngle(double dx, double dy, double angleShift) const {
        double angle = atan2(dy, dx) - angleShift;
        if (angle < 0) {
            angle += 2 * M_PI;
        }
        if (angle >= 2 * M_PI) {
            angle -= 2 * M_PI;
        }
        return angle;
    }

    double getDistanceToCenterOfBin(double center, double angle) const;
    pair<int, int> getNeighborsToPoint(double angle) const;
public:
    double getRotatedAngle() const {
        return rotatedAngle;
    }

    static vector<Descriptor> createOrientedDescriptors(
            const MyImage& sobelX, const MyImage& sobelY,
            int pointX, int pointY,
            int regionsX, int regionsY,
            int sizeOfNetworkX, int sizeOfNetworkY,
            int binsInHistogram,
            int manyBinsNumber
            );
    Descriptor(const MyImage& sobelX, const MyImage& sobelY,
               int pointX, int pointY,
               int regionsX, int regionsY,
               int sizeOfNetworkX, int sizeOfNetworkY,
               int binsInHistogram, double angleShift,
               bool normalize
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

    Descriptor(const Descriptor& sample) {
        pointX = sample.pointX;
        pointY = sample.pointY;
        rotatedAngle = sample.rotatedAngle;

        sizeOfDescriptor = sample.sizeOfDescriptor;
        elements = make_unique<double[]>((size_t)(sizeOfDescriptor));
        for (int i = 0; i < sizeOfDescriptor; i++) {
            elements[i] = sample.elements[i];
        }

        binsNumber = sample.binsNumber;
        centersOfBins = make_unique<double[]>((size_t)(binsNumber));
        for (int i = 0; i < binsNumber; i++) {
            centersOfBins[i] = sample.centersOfBins[i];
        }
    }
};

#endif // DESCRIPTORFINDER_H
