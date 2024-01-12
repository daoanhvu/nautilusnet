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
    // Step 5.1: Prepair AnchorGeneration info
    rcnn::AnchorInputData anchorInput;
    anchorInput.ratios = {0.5f, 1.0f, 2.0f};
    rcnn::RPNHead rpn = rcnn::RPNHead(fpnOutChannels, 512, anchorInput);
    auto rpnFeatOuts = rpn->forward(outputFpn);
    std::cout << "RPN size: " << rpnFeatOuts.size() << std::endl;
    for(int i=0; i<rpnFeatOuts.size(); ++i) {
        std::cout << "RPN Output(" << i << ") : class pro" << rpnFeatOuts[i].clsOutput << ", bbox: " << rpnFeatOuts[i].bboxOutput << std::endl;
    }

    std::cout << "Done testing" << std::endl;
    return 0;
}


std::vector<torch::Tensor> get_anchors(torch::Tensor baseAnchors, const std::vector<rcnn::GridSize> grid_sizes) {
    std::vector<torch::Tensor> anchors;
    std::vector<int> anchor_strides = {4, 8, 16, 32, 64};

    for(int i=0; i<grid_sizes.size(); i++) {
      auto grid_size = grid_sizes[i];
      auto stride = anchor_strides[i];
      auto base_anchor = baseAnchors[i]; // [num_scales, num_ratios, 4]
      auto grid_x = grid_size.width;
      auto grid_y = grid_size.height;
      auto x_offset = torch::arange(grid_x, base_anchor.device()) * stride; // [grid_x]
      auto y_offset = torch::arange(grid_y, base_anchor.device()) * stride; // [grid_y]
      
      auto anchor = base_anchor.repeat({grid_y, grid_x, 1, 1, 1}); // [grid_y, grid_x, num_scales, num_ratios, 4]
      auto shape = anchor.sizes().vec();
      // from [grid_y, grid_x, num_scales, num_ratios, 4] 
      // to   [grid_y, grid_x, num_scales, num_ratios, 2, 2]
      // so that we can add offset to the last dim
      shape.pop_back(); shape.push_back(2); shape.push_back(2); // can't find an easier way
      anchor = anchor.view(shape);
      // move x by x_offset and y by y_offset
      anchor.index_put_({"...", 1}, y_offset.view({-1, 1, 1, 1, 1})+anchor.index({"...", 1}));
      anchor.index_put_({"...", 0}, x_offset.view({-1, 1, 1, 1})+anchor.index({"...", 0}));
      // from [grid_y, grid_x, num_scales, num_ratios, 2, 2] 
      // to   [grid_y, grid_x, num_scales, num_ratios, 4] 
      shape.pop_back(); shape.pop_back(); shape.push_back(4);
      anchor = anchor.view(shape);
      // the shape of anchor for current stride is [grid_y, grid_x, num_scales, num_ratios, 4]
      anchors.push_back(anchor);
    }
    return anchors;
  }

int main(int argc, char** args) {
    std::vector<float> anchor_scales = {0.5f, 1.0f, 2.0f};
    std::vector<float> anchor_ratios = {0.5f, 1.0f, 2.0f};
    std::vector<int> anchor_strides = {4, 8, 16, 32, 64};

    //torch::Tensor scales = torch::tensor({0.5f, 1.0f, 2.0f}, torch::kFloat32);
    torch::Tensor scales = torch::tensor({0.5f, 1.0f, 2.0f}, torch::kFloat32);
    torch::Tensor ratios = torch::tensor({0.5f, 1.0f, 2.0f}, torch::kFloat32);
    torch::Tensor strides = torch::tensor({4, 8, 16, 32, 64}, torch::kFloat32);

    torch::Tensor stridesV = strides.view({-1, 1});
    std::cout << "Stride view: " << stridesV.sizes() << std::endl;
    scales = stridesV * scales;
    std::cout << "Before unzqueeze: " << scales.sizes() << std::endl;
    std::cout << scales << std::endl;
    scales = scales.unsqueeze(-1);
    std::cout << "After unzqueeze: " << scales.sizes() << std::endl;
    std::cout << scales << std::endl;

    auto w = scales / ratios.sqrt();
    auto h = scales * ratios.sqrt();

    auto baseAnchors = torch::stack({-w/2, -h/2, w/2, h/2}, -1); 
    // move center by offset
    baseAnchors += strides.view({-1, 1, 1, 1}) * 0.0;

    std::vector<rcnn::GridSize> gridSizes;
    gridSizes.emplace_back(7, 7);
    gridSizes.emplace_back(15, 15);

    std::vector<torch::Tensor> anchors = get_anchors(baseAnchors, gridSizes);

    std::cout << "Number of anchors: " << anchors.size() << std::endl;
    // for(torch::Tensor& a: anchors) {
    //     std::cout << "Anchor: " << std::endl << a << std::endl;
    // }

    std::cout << "Finish testing" << std::endl;

    return 0;
}