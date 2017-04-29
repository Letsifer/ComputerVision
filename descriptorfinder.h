#ifndef DESCRIPTORFINDER_H
#define DESCRIPTORFINDER_H
#include <cmath>
#include <assert.h>
#include <iostream>
#include <QDir>
#include <QTextStream>

#include "MyImage.h"
#include "pyramid.h"
#include "interestpointsfinder.h"
using namespace std;

class Descriptor
{
    InterestingPoint point;

    constexpr static int regionsX = 4, regionsY = 4;
    constexpr static int sizeOfGrid = 16;

    constexpr static int binsInHistogram = 8, manyBinsNumber = 36;

//    constexpr static double harrisThreshold = 0.0005; for ransac
    constexpr static double harrisThreshold = 1e-4; //for hough
    constexpr static int scalesInOctave = 3, octaves = 5;
    constexpr static double BORDER_OF_CHOOSING_SECOND_PICK = 0.8;

    constexpr static double NORMALIZE_THRESHOLD = 0.2;

    int sizeOfDescriptor;
    unique_ptr<double[]> elements;

    void normalize();
    double findAngleCoefficient(double angle, double centerOfAnotherBin, double basicValue) const;
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
    double calculateAngle(int index, int manyBinsNumber) const;
public:
    static vector<Descriptor> buildDescriptors(
            const MyImage& image
            );
    Descriptor(const MyImage& sobelX, const MyImage& sobelY,
               int pointX, int pointY,
               int regionsX, int regionsY,
               int binsInHistogram, double angleShift,
               bool normalize,
               double sigma, double basicSigma, double globalSigma, int octave
               );
    Descriptor(const Descriptor& sample);
    Descriptor& operator=(const Descriptor& sample);

    InterestingPoint getPoint() const {
        return point;
    }

    double getElement(int i) const{
        return elements[i];
    }
    double getDistance(const Descriptor& descriptor) const;    
};

#endif // DESCRIPTORFINDER_H
