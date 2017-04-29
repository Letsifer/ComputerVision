#include <vector>
#include <algorithm>
#include "descriptorfinder.h"
#ifndef DESCRIPTORMATCHER_H
#define DESCRIPTORMATCHER_H

struct PointMatch {
    InterestingPoint first, second;
    PointMatch() {
    }

    PointMatch(InterestingPoint first, InterestingPoint second) :
        first(first), second(second){}
    void swapPoints() {
        std::swap(first, second);
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
