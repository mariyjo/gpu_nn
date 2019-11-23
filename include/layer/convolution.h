#pragma once
#ifndef convolution_h
#define convolution_h
//#include "cublas_v2.h"
#include <cudnn.h>
#include "../common.h"
#include "layer.h"
class Convolution : public Layer {
   public:
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

    SharedStorage Column; //contains the converted image
    void im2col(const SharedStorage&);
   private:
    FilterShape _filter_shape;
    Pad _pad;
    Stride _stride;
    Filters _filters;
    ImageShape _inp;
    ImageShape _out;
    Channels _channels;
    void initialize_weight();
    void initialize_grad();
    cudnnHandle_t cudnn;
    cudnnTensorDescriptor_t input_descriptor;
    cudnnTensorDescriptor_t output_descriptor;
    cudnnFilterDescriptor_t kernel_descriptor;
    cudnnConvolutionDescriptor_t convolution_descriptor;
    cudnnConvolutionFwdAlgo_t convolution_algorithm;
    size_t workspace_bytes;
    void* d_workspace;
    int batch_size;
    SharedStorage col;

    void initialize_cudnn_handles();
    void resize(int);
    void resize_gpu(int);
    void resize_cpu(int);
    void allocate_memory();
    void initialize_algorithm();
    void initialize_kernel();
    void calculate_output_size();

    // im2col function
    //
    // int _input_dimension;
    // int _output_dimension;
};
#endif
