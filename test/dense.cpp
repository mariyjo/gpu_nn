#define CATCH_CONFIG_MAIN
#include "../include/layer/dense.h"
#include <eigen-git-mirror/Eigen/Core>
#include <memory>
#include "../include/common.h"
#include "../include/layer/layer.h"
#include "../third_party/catch/catch.hpp"
//#include "../include/math.h"
#include <sys/time.h>
#include "../include/storage.h"
#include "../include/neural_network.h"

using std::make_shared;
using std::shared_ptr;
using std::vector;
typedef std::shared_ptr<Storage> SharedStorage;

double cpuSecond() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1e-6);
}
TEST_CASE("Dense forward_gpu", "[gpu]") {
    srand((unsigned int)time(0));
    Init* init = new Glorot();
    Dense s1(Features(6), Features(5), init);
    Matrix in = Matrix::Random(5, 3);
    Matrix out = Matrix::Zero(6, 3);
    dtype begin = out(0, 0);
    std::shared_ptr<Storage> storage_in = std::make_shared<Storage>(in);
    std::shared_ptr<Storage> storage_out = std::make_shared<Storage>(out);
    s1.forward_gpu(storage_in, storage_out, "train");
    dtype end = storage_out->return_data_const()(0, 0);
    REQUIRE(begin != end);
}

TEST_CASE("Dense backward_gpu", "[gpu]") {
    srand((unsigned int)time(0));
    Init* init = new Glorot();
    Dense s1(Features(6), Features(5), init);
    //Dense s1(6, 5);
    //inp1 = &s1;
    Matrix gradient_in = Matrix::Random(6, 3);
    Matrix gradient_out = Matrix::Zero(5, 3);
    Matrix values = Matrix::Random(5, 3);
    SharedStorage shared_gradient_in = make_shared<Storage>(gradient_in);
    SharedStorage shared_gradient_out = make_shared<Storage>(gradient_out);
    SharedStorage shared_values = make_shared<Storage>(values);
    s1.backward_gpu(shared_values, shared_gradient_in, shared_gradient_out);
    std::cout << s1.return_gradients()[1]->return_data_const() <<
        std::endl;
    std::cout << gradient_in << std::endl;
    REQUIRE(s1.return_gradients()[1]->return_data_const() ==
            gradient_in.rowwise().sum());
}

TEST_CASE("Dense forward_cpu", "[cpu]") {
    srand((unsigned int)time(0));
    Init* init = new Glorot();
    Dense s1(Features(6), Features(5), init);
    Matrix in = Matrix::Random(5, 3);
    Matrix out = Matrix::Zero(6, 3);
    dtype begin = out(0, 0);
    std::shared_ptr<Storage> storage_in = std::make_shared<Storage>(in);
    std::shared_ptr<Storage> storage_out = std::make_shared<Storage>(out);
    s1.forward_cpu(storage_in, storage_out, "train");
    dtype end = storage_out->return_data_const()(0, 0);
    REQUIRE(begin != end);
}

TEST_CASE("Dense backward_cpu", "[cpu]") {
    srand((unsigned int)time(0));
    Init* init = new Glorot();
    Dense s1(Features(6), Features(5), init);
    Matrix gradient_in = Matrix::Random(6, 3);
    Matrix gradient_out = Matrix::Zero(5, 3);
    Matrix values = Matrix::Random(5, 3);
    SharedStorage shared_gradient_in = make_shared<Storage>(gradient_in);
    SharedStorage shared_gradient_out = make_shared<Storage>(gradient_out);
    SharedStorage shared_values = make_shared<Storage>(values);
    s1.backward_cpu(shared_values, shared_gradient_in, shared_gradient_out);
    std::cout << s1.return_gradients()[1]->return_data_const() <<
        std::endl;
    std::cout << gradient_in << std::endl;
    REQUIRE(s1.return_gradients()[1]->return_data_const() ==
            gradient_in.rowwise().sum());
}

TEST_CASE("Dense backard equivalence", "[backward equivalence]") {
    srand((unsigned int)time(0));
    Init* init = new Glorot();
    Dense s1(Features(1024), Features(1000), init);
    Dense s1_gpu(Features(1024), Features(1000), init);
    //Dense s1(1024, 1000);
    //inp1 = &s1;
    Matrix gradient_in = Matrix::Random(1024, 32);
    Matrix gradient_out_cpu = Matrix::Zero(1000, 32);
    Matrix gradient_out_gpu = Matrix::Zero(1000, 32);
    Matrix values = Matrix::Random(1000, 32);
    SharedStorage shared_gradient_in = make_shared<Storage>(gradient_in);
    SharedStorage shared_gradient_out_cpu =
        make_shared<Storage>(gradient_out_cpu);
    SharedStorage shared_gradient_out_gpu =
        make_shared<Storage>(gradient_out_gpu);
    SharedStorage shared_values = make_shared<Storage>(values);
    double cpuStart = cpuSecond();
    s1.backward_cpu(shared_values, shared_gradient_in,
                       shared_gradient_out_cpu);
    double cpuEnd = cpuSecond() - cpuStart;
    double gpuStart = cpuSecond();
    s1_gpu.backward_gpu(shared_values, shared_gradient_in,
                       shared_gradient_out_gpu);
    double gpuEnd = cpuSecond() - gpuStart;
    Matrix diff = shared_gradient_out_cpu->return_data_const() -
                  shared_gradient_out_gpu->return_data_const();
    dtype out = diff.array().abs().maxCoeff();
    Matrix bias_diff = s1.return_gradients()[1]->return_data_const() -
        s1.return_gradients()[1]->return_data_const();
    dtype out_bias = bias_diff.array().abs().maxCoeff();
    std::cout << "the bias diff is: " << out_bias << std::endl;
    dtype allowed = 1e-5;
    std::cout << "The CPU took " << cpuEnd << " and hte GPU took " << gpuEnd
              << std::endl;
    std::cout << "maximum difference: " << out << std::endl;
    REQUIRE(out < allowed);
    REQUIRE(gpuEnd < cpuEnd);
}

TEST_CASE("Dense forward equivalence", "[forward equivalence]") {
    srand((unsigned int)time(0));
    Init* init = new Glorot();
    Dense s1(Features(1024), Features(1000), init);
    //Dense s1(1024, 1000);
    //inp1 = &s1;
    Matrix in = Matrix::Random(1000, 32);
    Matrix out_cpu = Matrix::Zero(1024, 32);
    Matrix out_gpu = Matrix::Zero(1024, 32);
    std::shared_ptr<Storage> storage_in = std::make_shared<Storage>(in);
    std::shared_ptr<Storage> storage_out_cpu =
        std::make_shared<Storage>(out_cpu);
    std::shared_ptr<Storage> storage_out_gpu =
        std::make_shared<Storage>(out_gpu);
    double cpuStart = cpuSecond();
    s1.forward_cpu(storage_in, storage_out_cpu, "train");
    double cpuEnd = cpuSecond() - cpuStart;
    double gpuStart = cpuSecond();
    s1.forward_gpu(storage_in, storage_out_gpu, "train");
    double gpuEnd = cpuSecond() - gpuStart;
    Matrix diff = storage_out_cpu->return_data_const() -
                  storage_out_gpu->return_data_const();
    dtype out = diff.array().abs().maxCoeff();
    dtype allowed = 1e-5;
    std::cout << "The CPU took " << cpuEnd << " and hte GPU took " << gpuEnd
              << std::endl;
    std::cout << "maximum difference: " << out << std::endl;
    REQUIRE(out < allowed);
}
