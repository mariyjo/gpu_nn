#include <eigen-git-mirror/Eigen/Core>
#define CATCH_CONFIG_MAIN
#include <eigen-git-mirror/Eigen/Core>
#include "../third_party/catch/catch.hpp"
#include "../include/common.h"
#include "../include/layer/dense.h"
#include "../include/layer/softmax.h"
#include "../include/layer/layer.h"
#include "../include/math.h"
#include "../include/storage.h"
#include <sys/time.h>

double cpuSecond() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1e-6);
}
TEST_CASE("DataFrame gpu", "[gpu]") {
    srand((unsigned int) time(0));
    cublasHandle_t handle;
    cublasStatus_t stat = cublasCreate(&handle);
    CHECK_CUBLAS(stat);
    Layer* inp1;
    Softmax s1(handle);
    inp1 = &s1;
    Eigen::MatrixXd in = Eigen::MatrixXd::Random(6, 5);
    Eigen::MatrixXd out = Eigen::MatrixXd::Zero(6, 5);
    std::shared_ptr<Storage> storage_in = std::make_shared<Storage>(in);
    std::shared_ptr<Storage> storage_out = std::make_shared<Storage>(out);
    inp1->forward_gpu(storage_in, storage_out);
    Eigen::VectorXd sum = storage_out->return_data_const().colwise().sum();
    REQUIRE(sum(0) == 1);
    REQUIRE(sum.sum() == 5);
}

TEST_CASE("DataFrame cpu", "[cpu]") {
    srand((unsigned int) time(0));
    cublasHandle_t handle;
    cublasStatus_t stat = cublasCreate(&handle);
    CHECK_CUBLAS(stat);
    Layer* inp1;
    Softmax s1(handle);
    inp1 = &s1;
    Eigen::MatrixXd in = Eigen::MatrixXd::Random(6, 5);
    Eigen::MatrixXd out = Eigen::MatrixXd::Zero(6, 5);
    std::shared_ptr<Storage> storage_in = std::make_shared<Storage>(in);
    std::shared_ptr<Storage> storage_out = std::make_shared<Storage>(out);
    inp1->forward_cpu(storage_in, storage_out);
    Eigen::VectorXd sum = storage_out->return_data_const().colwise().sum();
    REQUIRE(sum(0) == 1.0);
    REQUIRE(sum.sum() == 5.0);
}

TEST_CASE("DataFrame equivalence", "[equivalence]") {
    srand((unsigned int) time(0));
    cublasHandle_t handle;
    cublasStatus_t stat = cublasCreate(&handle);
    CHECK_CUBLAS(stat);
    Layer* inp1;
    Softmax s1(handle);
    inp1 = &s1;
    Eigen::MatrixXd in = Eigen::MatrixXd::Random(1024, 100);
    Eigen::MatrixXd out = Eigen::MatrixXd::Zero(1024, 100);
    std::shared_ptr<Storage> storage_in = std::make_shared<Storage>(in);
    std::shared_ptr<Storage> storage_gpu = std::make_shared<Storage>(out);
    std::shared_ptr<Storage> storage_cpu = std::make_shared<Storage>(out);
    double cpuStart = cpuSecond();
    inp1->forward_cpu(storage_in, storage_cpu);
    double cpuEnd = cpuSecond() - cpuStart;
    double gpuStart = cpuSecond();
    inp1->forward_gpu(storage_in, storage_gpu);
    double gpuEnd = cpuSecond() - gpuStart;
    std::cout << "The CPU took " << cpuEnd << " and hte GPU took " <<
        gpuEnd << std::endl;
    Eigen::MatrixXd diff = storage_cpu->return_data_const() -
        storage_gpu->return_data_const();
    double maxDiff = diff.array().abs().maxCoeff();
    REQUIRE(cpuEnd > gpuEnd);
    REQUIRE(maxDiff < 1e-6);
}