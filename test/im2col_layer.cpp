#include <eigen-git-mirror/Eigen/Core>
#define CATCH_CONFIG_MAIN
#include <sys/time.h>
#include <iomanip>
#include <ios>
#include <iostream>
#include <istream>
#include "../include/common.h"
#include "../include/neural_network.h"
#include "../third_party/catch/catch.hpp"

double cpuSecond() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1e-6);
}
TEST_CASE("NeuralNetwork im2col_layer cpu", "[cpu]") {
    srand((unsigned int)time(0));
    FilterShape kernel(2, 2);
    Stride stride(1);
    Pad pad(1);
    Filters filters(1);
    ImageShape image(2, 2);
    Channels channels(2);
    int batches(2);
    int out_height =
        (image.first() + 2 * pad.get() - kernel.first()) / stride.get() + 1;
    int out_width =
        (image.second() + 2 * pad.get() - kernel.second()) / stride.get() + 1;
    Layer* inp1 =
        new Im2ColLayer(kernel, pad, stride, filters, image, channels);
    Matrix _input = Matrix::Random(
        image.first() * image.second() * channels.get(), batches);
    std::shared_ptr<Storage> input = std::make_shared<Storage>(_input);
    Matrix _out =
        Matrix::Zero(out_height * out_width, channels.get() * kernel.first() *
                                                 kernel.second() * batches);
    std::shared_ptr<Storage> output_cpu = std::make_shared<Storage>(_out);
    inp1->forward_cpu(input, output_cpu, "train");
}

TEST_CASE("NeuralNetwork im2col_layer gpu", "[gpu]") {
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    srand((unsigned int)time(0));
    FilterShape kernel(2, 2);
    Stride stride(1);
    Pad pad(1);
    Filters filters(1);
    ImageShape image(2, 2);
    Channels channels(2);
    int batches(2);
    int out_height =
        (image.first() + 2 * pad.get() - kernel.first()) / stride.get() + 1;
    int out_width =
        (image.second() + 2 * pad.get() - kernel.second()) / stride.get() + 1;
    Layer* inp1 =
        new Im2ColLayer(kernel, pad, stride, filters, image, channels);
    Matrix _input = Matrix::Random(
        image.first() * image.second() * channels.get(), batches);
    std::shared_ptr<Storage> input = std::make_shared<Storage>(_input);
    Matrix _out =
        Matrix::Zero(out_height * out_width, channels.get() * kernel.first() *
                                                 kernel.second() * batches);
    std::shared_ptr<Storage> output_cpu = std::make_shared<Storage>(_out);
    inp1->forward_gpu(input, output_cpu, "train");
    //std::cout << "input\n" <<input->return_data_const() << std::endl;
    //std::cout << output_cpu->return_data_const() << std::endl;
}

TEST_CASE("NeuralNetwork im2col_layer equivlance", "[equivlance]") {
    std::cout << std::fixed;
    std::cout << std::setprecision(4);
    srand((unsigned int)time(0));
    FilterShape kernel(2, 2);
    Stride stride(1);
    Pad pad(1);
    Filters filters(1);
    ImageShape image(220, 220);
    Channels channels(21);
    int batches(32);
    int out_height =
        (image.first() + 2 * pad.get() - kernel.first()) / stride.get() + 1;
    int out_width =
        (image.second() + 2 * pad.get() - kernel.second()) / stride.get() + 1;
    Layer* inp1 =
        new Im2ColLayer(kernel, pad, stride, filters, image, channels);
    Matrix _input = Matrix::Random(
        image.first() * image.second() * channels.get(), batches);
    std::shared_ptr<Storage> input = std::make_shared<Storage>(_input);
    Matrix _out =
        Matrix::Zero(out_height * out_width, channels.get() * kernel.first() *
                                                 kernel.second() * batches);
    std::shared_ptr<Storage> output_cpu = std::make_shared<Storage>(_out);
    std::shared_ptr<Storage> output_gpu = std::make_shared<Storage>(_out);
    double cpuStart = cpuSecond();
    inp1->forward_cpu(input, output_cpu, "train");
    double cpuEnd = cpuSecond() - cpuStart;
    double gpuStart = cpuSecond();
    inp1->forward_gpu(input, output_gpu, "train");
    double gpuEnd = cpuSecond() - gpuStart;
    Matrix diff = output_cpu->return_data_const() -
        output_gpu->return_data_const();
    dtype maxDiff = diff.cwiseAbs().maxCoeff();
    std::cout << "the difference is: " << maxDiff << std::endl;
    std::cout << "The CPU took " << cpuEnd << " and hte GPU took " << gpuEnd
              << std::endl;
    REQUIRE(maxDiff < 1e-5);
    REQUIRE(gpuEnd < cpuEnd);
}

TEST_CASE("NeuralNetwork im2col_layer backward cpu", "[cpu]") {
    srand((unsigned int)time(0));
    FilterShape kernel(3, 3);
    Stride stride(1);
    Pad pad(1);
    Filters filters(1);
    ImageShape image(3, 3);
    Channels channels(2);
    int batches(2);
    int out_height =
        (image.first() + 2 * pad.get() - kernel.first()) / stride.get() + 1;
    int out_width =
        (image.second() + 2 * pad.get() - kernel.second()) / stride.get() + 1;
    Layer* inp1 =
        new Im2ColLayer(kernel, pad, stride, filters, image, channels);
    Matrix _input = Matrix::Random(
        image.first() * image.second() * channels.get(), batches);
    Matrix _grad_out = Matrix::Zero(
        image.first() * image.second() * channels.get(), batches);
    std::shared_ptr<Storage> input = std::make_shared<Storage>(_input);
    std::shared_ptr<Storage> grad_out = std::make_shared<Storage>(_grad_out);
    Matrix _out =
        Matrix::Zero(out_height * out_width, channels.get() * kernel.first() *
                                                 kernel.second() * batches);
    std::shared_ptr<Storage> output_cpu = std::make_shared<Storage>(_out);
    inp1->forward_cpu(input, output_cpu, "train");
    inp1->backward_cpu(input, output_cpu, grad_out);
}

TEST_CASE("NeuralNetwork im2col_layer backward gpu", "[gpu]") {
    srand((unsigned int)time(0));
    FilterShape kernel(3, 3);
    Stride stride(1);
    Pad pad(1);
    Filters filters(1);
    ImageShape image(3, 3);
    Channels channels(2);
    int batches(2);
    int out_height =
        (image.first() + 2 * pad.get() - kernel.first()) / stride.get() + 1;
    int out_width =
        (image.second() + 2 * pad.get() - kernel.second()) / stride.get() + 1;
    Layer* inp1 =
        new Im2ColLayer(kernel, pad, stride, filters, image, channels);
    Matrix _input = Matrix::Random(
        image.first() * image.second() * channels.get(), batches);
    Matrix _grad_out = Matrix::Zero(
        image.first() * image.second() * channels.get(), batches);
    std::shared_ptr<Storage> input = std::make_shared<Storage>(_input);
    std::shared_ptr<Storage> grad_out = std::make_shared<Storage>(_grad_out);
    Matrix _out =
        Matrix::Zero(out_height * out_width, channels.get() * kernel.first() *
                                                 kernel.second() * batches);
    std::shared_ptr<Storage> output_gpu = std::make_shared<Storage>(_out);
    inp1->forward_gpu(input, output_gpu, "train");
    inp1->backward_gpu(input, output_gpu, grad_out);
}

TEST_CASE("im2col_layer equivlance backwards", "[equivlance back]") {
    std::cout << std::fixed;
    std::cout << std::setprecision(4);
    srand((unsigned int)time(0));
    FilterShape kernel(2, 2);
    Stride stride(1);
    Pad pad(1);
    Filters filters(1);
    ImageShape image(220, 220);
    Channels channels(21);
    int batches(32);
    int out_height =
        (image.first() + 2 * pad.get() - kernel.first()) / stride.get() + 1;
    int out_width =
        (image.second() + 2 * pad.get() - kernel.second()) / stride.get() + 1;
    Layer* inp1 =
        new Im2ColLayer(kernel, pad, stride, filters, image, channels);
    Matrix _input = Matrix::Random(
        image.first() * image.second() * channels.get(), batches);
    Matrix _grad_cpu = Matrix::Zero(
        image.first() * image.second() * channels.get(), batches);
    Matrix _grad_gpu = Matrix::Zero(
        image.first() * image.second() * channels.get(), batches);
    std::shared_ptr<Storage> input = std::make_shared<Storage>(_input);
    std::shared_ptr<Storage> grad_cpu = std::make_shared<Storage>(_grad_cpu);
    std::shared_ptr<Storage> grad_gpu = std::make_shared<Storage>(_grad_gpu);
    Matrix _out =
        Matrix::Zero(out_height * out_width, channels.get() * kernel.first() *
                                                 kernel.second() * batches);
    std::shared_ptr<Storage> output_cpu = std::make_shared<Storage>(_out);
    std::shared_ptr<Storage> output_gpu = std::make_shared<Storage>(_out);
    inp1->forward_cpu(input, output_cpu, "train");
    double cpuStart = cpuSecond();
    inp1->backward_cpu(input, output_cpu, grad_cpu);
    double cpuEnd = cpuSecond() - cpuStart;
    inp1->forward_gpu(input, output_gpu, "train");
    double gpuStart = cpuSecond();
    inp1->backward_gpu(input, output_gpu, grad_gpu);
    double gpuEnd = cpuSecond() - gpuStart;
    Matrix diff = grad_cpu->return_data_const() - grad_gpu->return_data_const();
    dtype maxDiff = diff.cwiseAbs().maxCoeff();
    std::cout << "the difference is: " << maxDiff << std::endl;
    std::cout << "The CPU took " << cpuEnd << " and hte GPU took " << gpuEnd
              << std::endl;
    REQUIRE(maxDiff < 1e-5);
    REQUIRE(gpuEnd < cpuEnd);
}