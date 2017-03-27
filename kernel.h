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
    int getHeight(){return height;}
    int getWIdth(){return width;}
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

    static Kernel createXSobelKernel();
    static Kernel createYSobelKernel();

    static Kernel createXGaussKernel(const double sigma);
    static Kernel createYGaussKernel(const double sigma);
    static Kernel createGaussKernel(const double sigma);
};

#endif // KERNEL_H
