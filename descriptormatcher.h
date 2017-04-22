#include <vector>
#include "descriptorfinder.h"
#ifndef DESCRIPTORMATCHER_H
#define DESCRIPTORMATCHER_H

struct PointMatch {
    int firstX, firstY;
    int secondX, secondY;
    PointMatch(int firstX, int firstY, int secondX, int secondY) :
        firstX(firstX), firstY(firstY), secondX(secondX), secondY(secondY){}
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
