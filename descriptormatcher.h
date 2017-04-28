#include <vector>
#include <algorithm>
#include "descriptorfinder.h"
#ifndef DESCRIPTORMATCHER_H
#define DESCRIPTORMATCHER_H

struct PointMatch {
    int firstX, firstY;
    double firstScale, firstOrientation;
    int secondX, secondY;
    double secondScale, secondOrientation;
    PointMatch() {
        firstX = 0; firstY = 0;
        secondX = 0; secondY = 0;
        firstScale = 0; firstOrientation = 0;
        secondScale = 0; secondOrientation = 0;
    }

    PointMatch(int firstX, int firstY,
               double firstOrientation, double firstScale,
               int secondX, int secondY,
               double secondOrientation, double secondScale) :
        firstX(firstX), firstY(firstY), secondX(secondX), secondY(secondY),
        firstScale(firstScale), firstOrientation(firstOrientation),
        secondScale(secondScale), secondOrientation(secondOrientation){}
    void swapPoints() {
        std::swap(firstX, secondX);
        std::swap(firstY, secondY);
        std::swap(firstScale, secondScale);
        std::swap(firstOrientation, secondOrientation);
    }
};


class DescriptorMatcher
{
    constexpr static double T = 0.8;
public:
    DescriptorMatcher();
    static vector<PointMatch> findMatchersBetweenDescriptors(
                const vector<Descriptor>& firstVector,
                const vector<Descriptor>& secondVector
            );

};

#endif // DESCRIPTORMATCHER_H
