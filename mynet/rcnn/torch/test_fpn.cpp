#include <iostream>
#include <torch/torch.h>
#include "fpn.hpp"


namespace rcnn {
    struct SimpleNetImpl: torch::nn::Module {

        torch::nn::Sequential layer1;
        torch::nn::Sequential layer2;
        torch::nn::Sequential layer3;
        torch::nn::Sequential layer4;
        torch::nn::Sequential output;

        SimpleNetImpl() {
            layer1 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 64, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(64),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
            );

            layer2 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 64, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(64),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(128),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
            );

            layer3 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 128, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(128),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 256, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(256),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                
                torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 256, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(256),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 256, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(256),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
            );

            layer4 = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
            );

            output = torch::nn::Sequential(
                torch::nn::Linear(15*15*512, 2048),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Dropout(0.45f),
                torch::nn::Linear(2048, 2048),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Linear(2048, 1000)
            );

            register_module("layer1", layer1);
            register_module("layer2", layer2);
            register_module("layer3", layer3);
            register_module("layer4", layer4);
            register_module("output", output);
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
            torch::Tensor lastOutput = output->forward(out4.reshape({out4.size(0), -1}));
            outputs.push_back(lastOutput);
            return outputs;
        }
    };

    TORCH_MODULE(SimpleNet);
}

int main(int argc, char** args) {

    auto model = std::make_shared<rcnn::SimpleNetImpl>();

    std::vector<int> featChannels = {64, 128, 256, 512};
    torch::Tensor x = torch::rand({1, 3, 244, 244});
    std::cout << "Input shape: " << x.sizes() << std::endl;
    std::vector<torch::Tensor> outputs = model->forward(x);

    torch::nn::ModuleList lateralConvs = torch::nn::ModuleList();
    torch::nn::ModuleList fpnConvs = torch::nn::ModuleList();
    int mOutChannels = 512;
    for(auto i=0; i<featChannels.size(); ++i) {
        int featChannel = featChannels[i];
        torch::nn::Conv2d lateralConv = torch::nn::Conv2d(torch::nn::Conv2dOptions(featChannel, mOutChannels, 1));
        torch::nn::init::xavier_uniform_(lateralConv->weight);
        torch::nn::init::constant_(lateralConv->bias, 0);
        lateralConvs->push_back(lateralConv);
        fpnConvs->push_back(torch::nn::Conv2d(torch::nn::Conv2dOptions(mOutChannels, mOutChannels, 3).padding(1)));
    }
    std::cout << "Finished defining FPN" << std::endl;

    //forward
    std::vector<torch::Tensor> lateralOuts;
    for(int i=0; i<featChannels.size(); ++i) {
        torch::Tensor lateralOut = lateralConvs[i]->as<torch::nn::Conv2d>()->forward(outputs[i]);
        std::cout << "Lateral out " << i << ": " << lateralOut.sizes() << std::endl;
        lateralOuts.push_back(lateralOut);
    }

    std::vector<torch::Tensor> upsampleOuts({lateralOuts.back()});
    std::cout << outputs.size() << std::endl;
    for(int i = outputs.size()-2; i>=0; i--) {
        auto upSizes = lateralOuts[i].sizes();
        std::cout << "Up size: (" << upSizes[2] << ", " << upSizes[3] << ")" << std::endl;
        auto interOpts = torch::nn::functional::InterpolateFuncOptions().mode(torch::kNearest);
        interOpts.size(std::vector<int64_t>({upSizes[2], upSizes[3]}));
        upsampleOuts.push_back(torch::nn::functional::interpolate(upsampleOuts.back(), interOpts) + lateralOuts[i]);
    }

    std::reverse(upsampleOuts.begin(), upsampleOuts.end());
    std::vector<torch::Tensor> outs;
    for(int i=0; i<outputs.size()-1; i++) {
        outs.push_back(fpnConvs[i]->as<torch::nn::Conv2d>()->forward(upsampleOuts[i]));
    }

    // // Define FPN
    // auto fpn = std::make_shared<rcnn::FPNImpl>(512, featChannels, featChannels.size());
    // std::cout << "Finished defining FPN" << std::endl;
    // // put feature map to FPN
    // auto outputFpn = fpn->forward(outputs);
    // std::cout << "Finished" << std::endl;

    std::cout << "Done testing" << std::endl;
    return 0;
}