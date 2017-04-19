#ifndef INTERESTPOINTSFINDER_H
#define INTERESTPOINTSFINDER_H
#include "MyImage.h"
#include "cmath"
#include <vector>
using namespace std;

struct InterestingPoint {
    int x, y;
    double contrast;
    double radiusToGreaterContrast;
    double sigma;
    InterestingPoint() : x(-1), y(-1), contrast(-1), radiusToGreaterContrast(numeric_limits<double>::max()){}
    InterestingPoint(int x, int y) : x(x), y(y), contrast(0), radiusToGreaterContrast(0){}
    InterestingPoint(int x, int y, double contrast) : x(x), y(y), contrast(contrast), radiusToGreaterContrast(numeric_limits<double>::max()){}
    double getDistance(const InterestingPoint point) const{
        return hypot(x - point.x, y - point.y);
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
