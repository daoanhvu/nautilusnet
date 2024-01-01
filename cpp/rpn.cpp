#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>

struct VGG16 : torch::nn::Module {
    // VGG-16 Layers
    torch::nn::Sequential layer1;
    torch::nn::Sequential layer2;
    torch::nn::Sequential layer3;
    torch::nn::Sequential layer4;
    torch::nn::Sequential layer5;
    torch::nn::Sequential layer6;
    torch::nn::Sequential layer7;
    torch::nn::Sequential layer8;
    torch::nn::Sequential layer9;
    torch::nn::Sequential layer10;
    torch::nn::Sequential layer11;
    torch::nn::Sequential layer12;
    torch::nn::Sequential layer13;
    torch::nn::Sequential fcLayer;
    torch::nn::Sequential outputLayer;

    VGG16() {
        layer1 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 64, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(64), // do we need this?
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        layer2 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 64, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(64),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
            torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
        );

        layer3 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(128),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        layer4 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 128, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(128),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
            torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
        );

        layer5 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 256, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(256),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        layer6 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 256, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(256),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        layer7 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 256, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(256),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
            torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
        );

        layer8 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 512, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(512),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        layer9 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(512),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        layer10 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(512),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
            torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
        );

        layer11 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(512),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        layer12 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(512),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        layer13 = torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
            torch::nn::BatchNorm2d(512),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
            torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
        );

        fcLayer = torch::nn::Sequential(
            torch::nn::Dropout(0.5f),
            torch::nn::Linear(7*7*512, 4096),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
            torch::nn::Dropout(0.5f),
            torch::nn::Linear(4096, 4096),
            torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
        );

        outputLayer = torch::nn::Sequential(
            torch::nn::Linear(4096, 1000)
        );
    }

    torch::Tensor forward(torch::Tensor x) {
        torch::Tensor out = layer1->forward(x);
        out = layer2->forward(out);
        out = layer3->forward(out);
        out = layer4->forward(out);
        out = layer5->forward(out);
        out = layer6->forward(out);
        out = layer7->forward(out);
        out = layer8->forward(out);
        out = layer9->forward(out);
        out = layer10->forward(out);
        out = layer11->forward(out);
        out = layer12->forward(out);
        out = layer13->forward(out);
        out = out.reshape({out.size(0), -1});
        out = fcLayer->forward(out);
        out = outputLayer->forward(out);
        return out;
    }
};

struct RPN: torch::nn::Module {
    RPN() {

    } 

    // torch::Tensor forward(torch::Tensor x) {

    // }
};

torch::jit::script::Module loadModel(const char* filename) {
    torch::jit::script::Module model = torch::jit::load(filename);
    return model;
}

int main(int argc, char** args) {
    auto vgg16 = std::make_shared<VGG16>();
    torch::Tensor x = torch::rand({1, 3, 224, 224});
    torch::Tensor out = vgg16->forward(x);
    // std::cout << out << std::endl;
    return 0;
}