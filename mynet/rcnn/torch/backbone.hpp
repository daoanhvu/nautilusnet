#ifndef __BACKBONE_HPP__
#define __BACKBONE_HPP__

#include <torch/torch.h>
#include <vector>

#define VAL_7_7_512 25088

namespace rcnn {

    struct VGG16Impl: torch::nn::Module {
        // VGG-16 Layers
        torch::nn::Sequential input;
        torch::nn::Sequential features;
        torch::nn::Sequential fc1;
        torch::nn::Sequential fc2;
        torch::nn::Sequential dropout;
        torch::nn::Sequential output;

        VGG16Impl(float dropPC = 0.3f) {
            const int val4096 = 4096;
            input = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 64, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(64), // do we need this?
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
            );

            features = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 64, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(64),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(128),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),

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
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2)),

                torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2)),
                
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(1)),
                torch::nn::BatchNorm2d(512),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)),
                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions(2).stride(2))
            );

            dropout = torch::nn::Sequential(torch::nn::Dropout(dropPC));

            fc1 = torch::nn::Sequential(
                torch::nn::Linear(VAL_7_7_512, val4096),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
            );

            fc2 = torch::nn::Sequential(
                torch::nn::Linear(val4096, val4096),
                torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))
            );

            output = torch::nn::Sequential(
                torch::nn::Linear(val4096, 1000)
            );

            register_module("input", input);
            register_module("features", features);
            register_module("output", output);
        }

        torch::Tensor forward(torch::Tensor x) {
            torch::Tensor out = input->forward(x);
            out = features->forward(out);
            out = out.reshape({out.size(0), -1});
            out = fc1->forward(out);
            out = fc2->forward(out);
            return output->forward(out);
        }

        torch::Tensor trainForward(torch::Tensor x) {
            torch::Tensor out = input->forward(x);
            out = features->forward(out);
            out = out.reshape({out.size(0), -1});
            out = dropout->forward(out);
            out = fc1->forward(out);
            out = dropout->forward(out);
            out = fc2->forward(out);
            return output->forward(out);
        }
    };

    TORCH_MODULE(VGG16);
}

#endif