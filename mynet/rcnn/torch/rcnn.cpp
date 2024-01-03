#include <iostream>
#include <map>
#include <tuple>
#include <string>
#include <torch/torch.h>
#include "backbone.hpp"
#include "fpn.hpp"
#include "rpn.hpp"
#include "roi_extractor.hpp"


namespace rcnn {

    class RCNNHeadImpl: torch::nn::Module {
        private:
            RoIExtractor mRoIExtractor{nullptr};
    };

    TORCH_MODULE(RCNNHead);

    class FasterRCNNImpl: torch::nn::Module {
        private:
            std::shared_ptr<VGG16Impl> mBackbone{nullptr};
            FPN mNeck{nullptr};
            RPNHead mRpnHead{nullptr};
            RCNNHead mRcnnHead{nullptr};

        public:
            FasterRCNNImpl() {

            }

            std::map<std::string, torch::Tensor> forwardTrain(torch::Tensor x) {

            }

            std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> forwardTest(torch::Tensor x) {

            }

    };

    TORCH_MODULE(FasterRCNN);
}


int main(int argc, char** args) {
    auto vgg16 = std::make_shared<rcnn::VGG16Impl>();
    torch::Tensor x = torch::rand({1, 3, 244, 244});
    auto output = vgg16->forward(x);
    std::cout << "Hello ";
    std::cout << x.sizes() <<" " << output.sizes() << std::endl;
    return 0;
}