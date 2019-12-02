#include <stdlib.h>
//#include <type_traits>
#include "../include/neural_network.h"
#include "../third_party/cifar10/include/cifar/get_data.h"

void n_missclassified(const Matrix& y_pred, const Matrix& y_true) {
    int missclassified(0);
    for (int i = 0; i < y_pred.rows(); ++i) {
        int arg_pred =0;
        int arg_true = 0;
        dtype max_pred = y_pred(i, 0);
        dtype max_true = y_true(i, 0);
        for (int j = 0; j < 10; ++j) {
            if (max_pred < y_pred(i,j)) {
                arg_pred = j;
                max_pred = y_pred(i, j);
            }
            if (max_true < y_true(i,j)) {
                arg_true = j;
                max_true = y_true(i, j);
            }
        }
        if (arg_true != arg_pred)
            missclassified++;
    }
    std::cout << "fraction miassclassified : " <<
        float(missclassified) / y_pred.rows() << " and " <<
       "number missclassified " << missclassified <<  std::endl;
}
void print_Matrix_to_stdout2(const Matrix& val, std::string loc) {
    int rows(val.rows()), cols(val.cols());
    std::ofstream myfile(loc);
    myfile << "dimensions: rows, cols: " << rows << ", " << cols << std::endl;
    myfile << std::fixed;
    myfile << std::setprecision(2);
    for (int row = 0; row < rows; ++row) {
        myfile << val(row, 0);
        for (int col = 1; col < cols; ++col) {
            myfile << ", " << val(row, col);
        }
        myfile << std::endl;
    }
}
int main(int argc, char** argv) {
    // if ((argc != 2) and (argc != 5))
    // throw std::invalid_argument("Must have one or four arguemnts");
    Cifar10 data = Cifar10();
    Matrix test = data.get_x_train();
    srand((unsigned int)time(0));
    Layer* l1 = new Input(data.get_x_train().cols());
    //Layer* l2 = new Dense(200, 3 * 32 * 32);
    Layer* l2 = new Im2ColLayer(FilterShape(3, 3), Pad(1), Stride(1),
                                ImageShape(32, 32), Channels(3));
    Layer* l3 = new Convolution(FilterShape(3, 3), Pad(1), Stride(1),
                                Filters(32), ImageShape(32, 32), Channels(3));
    Layer* l4 = new Relu;
    Layer* l5 = new Im2ColLayer(FilterShape(3, 3), Pad(1), Stride(1),
                                ImageShape(32, 32), Channels(32));
    Layer* l6 = new Convolution(FilterShape(3, 3), Pad(1), Stride(1),
                                Filters(32), ImageShape(32, 32), Channels(32));
    Layer* l7 = new Relu;
    Layer *p1 = new Pooling(Window(2), Stride(2), ImageShape(32, 32), 
                            Channels(32));
    Layer* l8 = new Dense(10, 16 * 16 * 32);
    Layer* l9 = new Softmax;
    std::shared_ptr<Loss> loss =
        std::make_shared<CrossEntropy>(CrossEntropy("GPU"));
    NeuralNetwork n1({l1, l2, l3, l4, l5, l6, l7, p1, l8, l9}, loss, "GPU");
    std::shared_ptr<GradientDescent> sgd =
        std::make_shared<StochasticGradientDescent>(0.001);
    n1.train(data.get_x_train(), data.get_y_train(), sgd, Epochs(5),
             Patience(10), BatchSize(32));
    print_Matrix_to_stdout2(data.get_x_test(), 
            "/home/fabian/Documents/work/gpu_nn/debug/x_test.txt");
    Matrix predictions = n1.predict(data.get_x_test());
    print_Matrix_to_stdout2(predictions, 
            "/home/fabian/Documents/work/gpu_nn/debug/predictions.txt");
    n_missclassified(predictions, data.get_y_test());
    //delete l1;
    //delete l2;
    //delete l3;
    //delete l4;
    //delete l5;
    //delete l6;
}
