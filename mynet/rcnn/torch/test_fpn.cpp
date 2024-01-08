#include <iostream>
#include <torch/torch.h>
#include "fpn.hpp"
#include "rpn.hpp"

namespace rcnn {
    struct SimpleNetImpl: torch::nn::Module {

        torch::nn::Sequential layer1;
        torch::nn::Sequential layer2;
        torch::nn::Sequential layer3;
        torch::nn::Sequential layer4;
        torch::nn::Sequential layer5;

        SimpleNetImpl() {
            layer1 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 64, 3).stride(1).padding(1)),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 64, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(64),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
            );

            layer2 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3).stride(1).padding(1)),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 128, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(128),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
            );

            layer3 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 256, 3).stride(1).padding(1)),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 256, 3).stride(1).padding(1)),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 256, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(256),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
            );

            layer4 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 512, 3).stride(1).padding(1)),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
            );

            layer5 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
            );

            register_module("layer1", layer1);
            register_module("layer2", layer2);
            register_module("layer3", layer3);
            register_module("layer4", layer4);
            register_module("layer5", layer5);
        }

        /**
         * In order to apply FPN, we need to output not only the last result of the forwarding pass
         * but the feature map at each layer in the model
        */
        std::vector<torch::Tensor> forward(torch::Tensor x) {
            std::vector<torch::Tensor> outputs;
            torch::Tensor out1 = layer1->forward(x);
            outputs.push_back(out1);
            torch::Tensor out2 = layer2->forward(out1);
            outputs.push_back(out2);
            torch::Tensor out3 = layer3->forward(out2);
            outputs.push_back(out3);
            torch::Tensor out4 = layer4->forward(out3);
            outputs.push_back(out4);
            torch::Tensor out5 = layer5->forward(out4);
            outputs.push_back(out5);
            return outputs;
        }
    };

    TORCH_MODULE(SimpleNet);
}

int test(int argc, char** args) {
    // Step 1: Defining a backbone network
    auto backbone = std::make_shared<rcnn::SimpleNetImpl>();
    // Assume that we have a input image
    torch::Tensor x = torch::rand({1, 3, 244, 244});
    // Step 2: Input the image into backbone network to get feature map at multiple scale level
    std::vector<torch::Tensor> outputs = backbone->forward(x);

    std::vector<int> featChannels;
    for(int i=0; i<outputs.size(); ++i) {
        featChannels.push_back(outputs[i].size(1));
        std::cout << "Feature map (" << i << ")" << outputs[i].size(1) << std::endl;    
    }

    // Step 3: Defining a FPN network to process the feature map at multiple scale level
    
    int fpnOutChannels = 256;
    auto fpn = std::make_shared<rcnn::FPNImpl>(fpnOutChannels, featChannels, featChannels.size());
    std::cout << "Finished defining FPN" << std::endl;
    
    // Step 4: put feature map to FPN
    auto outputFpn = fpn->forward(outputs);
    std::cout << "Number of FPN outputs: " << outputFpn.size() << std::endl;
    for(int i=0; i<outputFpn.size(); ++i) {
        std::cout << "P(" << i << ") shape: " << outputFpn[i].sizes() << std::endl;
    }

    // Step 5: Now, feed these feature maps to RPN
    rcnn::RPNHead rpn = rcnn::RPNHead(fpnOutChannels, 512, 9);
    auto rpnFeatOuts = rpn->forward(outputFpn);
    std::cout << "RPN size: " << rpnFeatOuts.size() << std::endl;
    for(int i=0; i<rpnFeatOuts.size(); ++i) {
        std::cout << "RPN Output(" << i << ") : class pro" << rpnFeatOuts[i].clsOutput << ", bbox: " << rpnFeatOuts[i].bboxOutput << std::endl;
    }

    std::cout << "Done testing" << std::endl;
    return 0;
}

int main(int argc, char** args) {
    torch::Tensor x = torch::rand({1, 64, 244, 244});
    torch::Tensor x1 = x.view({-1, 4});
    std::cout << x1.sizes() << std::endl;
    return 0;
}