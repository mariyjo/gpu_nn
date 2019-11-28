#pragma once
#ifndef convolution_h
#define convolution_h
//#include "cublas_v2.h"
#include "../common.h"
#include "layer.h"
class Convolution : public Layer {
   public:
    int output_dimension() override;
    int output_dimension() const override;
    Convolution(FilterShape, Pad, Stride, Filters, ImageShape, Channels);
    virtual ~Convolution();
    void forward_gpu(const SharedStorage&, SharedStorage&,
                     const std::string&) override;
    void forward_cpu(const SharedStorage&, SharedStorage&,
                     const std::string&) override;
    void backward_gpu(const SharedStorage&, const SharedStorage&,
                      SharedStorage&) override;
    void backward_cpu(const SharedStorage&, const SharedStorage&,
                      SharedStorage&) override;

    //SharedStorage Column;  // contains the converted image

   private:
    FilterShape _filter_shape;
    Pad _pad;
    Stride _stride;
    Filters _filters;
    ImageShape _inp, _out;
    Channels _channels;
    int batch_size;
    void initialize_weight();
    void initialize_grad();

    void initialize_kernel();
};
#endif
