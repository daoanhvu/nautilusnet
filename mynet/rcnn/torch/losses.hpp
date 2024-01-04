#ifndef __LOSS_HPP__
#define __LOSS_HPP__

#include <torch/torch.h>

namespace rcnn {
    class Loss: public torch::nn::Module {
        public:
            virtual torch::Tensor forward(torch::Tensor pred, torch::Tensor target, double avg_factor) {
                throw std::runtime_error("Need to implement a Loss");
            }
    };

    std::shared_ptr<Loss> buildLoss() {
        return nullptr;
    }
}

#endif