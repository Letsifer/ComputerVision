#ifndef RANSACALGORITHM_H
#define RANSACALGORITHM_H

#include "descriptormatcher.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <random>
#include <assert.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
using namespace std;

class RansacAlgorithm
{
    constexpr static int K = 4; //число сопоставления для выборки
    constexpr static double EPS = 5; //погрешность в расстоянии между получившимися точками с матрицей гомографии и исходными точками
    constexpr static int MAX_ITERATIONS = 2000; //максимальное количество итераций алгоритма

    constexpr static int GSL_MATRIX_COLUMNS = 9;

    static vector<double> findCurrentHomography(const vector<PointMatch>& matches, vector<int>& choices, int points);
    static double countPartOfCoordinate(const vector<double>& homography, int startIndex, const PointMatch& match);
public:
    RansacAlgorithm();
    static vector<double> findHomography(const vector<PointMatch>& matches, vector<PointMatch>& result);
};

#endif // RANSACALGORITHM_H
