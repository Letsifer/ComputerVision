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

    constexpr static int regionsX = 4, regionsY = 4;
    constexpr static int sizeOfGrid = 16;
    constexpr static int binsInHistogram = 8, manyBinsNumber = 36;

    int sizeOfDescriptor;
    unique_ptr<double[]> elements;

    void normalize();
    double findAngleCoefficient(double angle, double center1, double center2) const;
    double findDistanceCoefficient(int x, int y, double sigma) const;
    double getAngle(double dx, double dy, double angleShift) const {
        double angle = atan2(dy, dx) - angleShift + M_PI;
        if (angle < 0) {
            angle += 2 * M_PI;
        }
        if (angle >= 2 * M_PI) {
            angle -= 2 * M_PI;
        }
        return angle;
    }

    double getDistanceToCenterOfBin(double center, double angle) const;
    pair<int, int> getNeighborsToPoint(double angle, int binsNumber, const unique_ptr<double[]>& centers) const;
    double calculateAngle(int index, int manyBinsNumber) const;
public:
    static vector<Descriptor> createOrientedDescriptors(
            const MyImage& sobelX, const MyImage& sobelY,
            int pointX, int pointY,
            double sigma
            );
    Descriptor(const MyImage& sobelX, const MyImage& sobelY,
               int pointX, int pointY,
               int regionsX, int regionsY,
               int binsInHistogram, double angleShift,
               bool normalize,
               double sigma
               );
    Descriptor(const Descriptor& sample);
    Descriptor& operator=(const Descriptor& sample);

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
