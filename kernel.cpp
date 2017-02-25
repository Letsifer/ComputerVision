#include "kernel.h"
#include <iostream>
#include <memory>
#include <cmath>
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

unique_ptr<Kernel> Kernel::createXSobelKernel() {
    unique_ptr<Kernel> kernel = make_unique<Kernel>(3, 3);
    copy(begin(VERTICAL_KERNEL), end(VERTICAL_KERNEL), kernel->kernel.get());
    return kernel;
}

unique_ptr<Kernel> Kernel::createYSobelKernel() {
    unique_ptr<Kernel> kernel = make_unique<Kernel>(3, 3);
    copy(begin(HORIZONTAL_KERNEL), end(HORIZONTAL_KERNEL), kernel->kernel.get());
    return kernel;
}

unique_ptr<Kernel> Kernel::createYGaussKernel(const double sigma) {
    const double denominator = sqrt(2 * M_PI * sigma), sigmaSqr = 2 * sigma * sigma;
    const int k = (int) round(3 * sigma), size = k * 2 + 1;
    unique_ptr<Kernel> kernel = make_unique<Kernel>(size, 1);
    for (int i = 0; i < size; i++) {
        int x = i - k;
        double value = exp(- x * x / sigmaSqr) / denominator;
        kernel->setElement(i, 0, value);
    }
    return kernel;
}

unique_ptr<Kernel> Kernel::createXGaussKernel(const double sigma) {
    const double denominator = sqrt(2 * M_PI * sigma), sigmaSqr = 2 * sigma * sigma;
    const int k = (int) round(3 * sigma), size = k * 2 + 1;
    unique_ptr<Kernel> kernel = make_unique<Kernel>(1, size);
    for (int i = 0; i < size; i++) {
        int x = i - k;
        double value = exp(- x * x / sigmaSqr) / denominator;
        kernel->setElement(0, i, value);
    }
    return kernel;
}
