#include <memory>
using namespace std;
#ifndef KERNEL_H
#define KERNEL_H


class Kernel
{
    int width, height;
    unique_ptr<double[]> kernel;
    void setElement(int i, int j, double value);
public:
    Kernel();
    Kernel(int height, int width);
    double getElement(int i, int j) const;
    int getKernelHalfWidth() const {// get k from 2*k-1
        return width / 2;
    }

    int getKernelHalfHeight() const {// get k from 2*k-1
        return height / 2;
    }
    double getElementInRelationToCenter(int pushX, int pushY) const{
        return getElement(getKernelHalfHeight() + pushY, getKernelHalfWidth() + pushX);
    }

    static unique_ptr<Kernel> createXSobelKernel();
    static unique_ptr<Kernel> createYSobelKernel();

    static unique_ptr<Kernel> createXGaussKernel(const double sigma);
    static unique_ptr<Kernel> createYGaussKernel(const double sigma);
};

#endif // KERNEL_H
