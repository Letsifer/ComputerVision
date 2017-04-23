#include "kernel.h"
#include <iostream>
#include <memory>
#include <math.h>
using namespace std;

constexpr static double VERTICAL_KERNEL[9] = {
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
};
constexpr static double HORIZONTAL_KERNEL[9] = {
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1
};

Kernel::Kernel()
{

}

Kernel::Kernel(int height, int width)
{
    this->height = height;
    this->width = width;
    kernel = make_unique<double[]>((size_t)(height * width));
}

double Kernel::getElement(int i, int j) const{
    return kernel[i * width + j];
}

void Kernel::setElement(int i, int j, double value){
    kernel[i * width + j] = value;
}

Kernel Kernel::createXSobelKernel() {
    Kernel kernel = Kernel(3, 3);
    copy(begin(VERTICAL_KERNEL), end(VERTICAL_KERNEL), kernel.kernel.get());
    return kernel;
}

Kernel Kernel::createYSobelKernel() {
    Kernel kernel = Kernel(3, 3);
    copy(begin(HORIZONTAL_KERNEL), end(HORIZONTAL_KERNEL), kernel.kernel.get());
    return kernel;
}

Kernel Kernel::createYGaussKernel(const double sigma) {
    const double denominator = sqrt(2 * M_PI) * sigma, sigmaSqr = 2 * sigma * sigma;
    const int k = (int) round(3 * sigma), size = k * 2 + 1;
    Kernel kernel = Kernel(1, size);
    double sum = 0;
    for (int i = 0; i < size; i++) {
        int x = i - k;
        double value = exp(- x * x / sigmaSqr) / denominator;
        sum += value;
        kernel.setElement(0, i, value);
    }
    for (int i = 0; i < size ; i++) {
        kernel.setElement(0, i, kernel.getElement(0, i) / sum);
    }
    return kernel;
}

Kernel Kernel::createXGaussKernel(const double sigma) {
    const double denominator = sqrt(2 * M_PI) * sigma, sigmaSqr = 2 * sigma * sigma;
    const int k = (int) round(3 * sigma), size = k * 2 + 1;
    Kernel kernel = Kernel(size, 1);
    double sum = 0;
    for (int i = 0; i < size; i++) {
        int x = i - k;
        double value = exp(- x * x / sigmaSqr) / denominator;
        sum += value;
        kernel.setElement(i, 0, value);
    }
    for (int i = 0; i < size ; i++) {
        kernel.setElement(i, 0, kernel.getElement(i, 0) / sum);
    }
    return kernel;
}

Kernel Kernel::createGaussKernel(const double sigma) {
    const double denominator = 2 * M_PI * sigma * sigma, sigmaSqr = 2 * sigma * sigma;
    const int k = (int) round(3 * sigma), size = k * 2 + 1;
    Kernel kernel = Kernel(size, size);
    for (int i = 0; i < size; i++) {
        int y = i - k;
        y *= y;
        for (int j = 0; j < size; j++) {
            int x = j - k;
            x *= x;
            double value = exp(- (x + y) / sigmaSqr) / denominator;
            kernel.setElement(i, j, value);
        }
    }
    return kernel;
}
