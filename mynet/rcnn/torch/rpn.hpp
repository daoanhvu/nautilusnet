#ifndef __RPN_HPP__
#define __RPN_HPP__

#include <tuple>
#include <vector>
#include <torch/torch.h>
#include "losses.hpp"

namespace rcnn {
    class RPNHeadImpl: torch::nn::Module {
        private:
            int _class_channels{1};
    
            torch::nn::Conv2d mConv{nullptr};
            torch::nn::Conv2d mClassifier{nullptr};
            torch::nn::Conv2d mRegressor{nullptr};
            std::shared_ptr<Loss> mLossCls{nullptr};
            std::shared_ptr<Loss> mLossBbox{nullptr};


        public:
            RPNHeadImpl() {

            }

            torch::Tensor forward(std::vector<torch::Tensor> feats) {

            }

            std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> forwardTrain(std::vector<torch::Tensor> feats) {

            }

            torch::Tensor getProposals(std::vector<torch::Tensor> anchor_list, std::vector<torch::Tensor> cls_out_list, std::vector<torch::Tensor> bbox_out_list,
                const std::vector<int64_t> &img_shape) {

            }
    };

    TORCH_MODULE(RPNHead);
}

#endif