#ifndef INTERESTPOINTSFINDER_H
#define INTERESTPOINTSFINDER_H
#include "MyImage.h"
#include "cmath"
#include <vector>
using namespace std;

struct InterestingPoint {
    int x, y;
    double contrast;
    double rotationAngle;
    double localSigma, globalSigma;
    int octave;
    InterestingPoint() : x(-1), y(-1), contrast(-1){}
    InterestingPoint(int x, int y) : x(x), y(y), contrast(0){}
    InterestingPoint(int x, int y, double contrast) : x(x), y(y), contrast(contrast){}
    InterestingPoint(const InterestingPoint& point) {
        x = point.x;
        y = point.y;
        contrast = point.contrast;
        rotationAngle = point.rotationAngle;
        localSigma = point.localSigma;
        globalSigma = point.globalSigma;
        octave = point.octave;
    }

    double getDistance(const InterestingPoint point) const{
        return hypot(x - point.x, y - point.y);
    }
    int getXInFirstImageScale() const {
        return x * pow(2, octave);
    }
    int getYInFirstImageScale() const {
        return y * pow(2, octave);
    }
};



class InterestPointsFinder
{
    static MyImage findContrastsImageForMoravecAlgorithm(
            const MyImage& image, int windowsShift,
            int halfSizeOfWindow, const BorderType type
            );
    static vector<InterestingPoint> findInteresingPointsFromContrastImage(
            const MyImage& contrastImage, double contrastBorder,
            const BorderType type, int halfSizeOfWindow
            );
public:
    static vector<InterestingPoint> moravecAlgorithm(
            const MyImage& image, int windowsShift,
            int sizeOfWindow, double contrastBorder, const BorderType type
            );
    static vector<InterestingPoint> harrisAlgorithm(
            const MyImage& image, const int windowSize, double contrastBorder, const BorderType type
            );
    static void adaptiveNonMaximumSuppression(
            vector<InterestingPoint>& points,
            unsigned int necessaryPoints
            );
    static double computeHarrisInOnePoint(const MyImage& image,
                                          int i, int j, double sigma);

};

#endif // INTERESTPOINTSFINDER_H
