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

    class GIoULoss: public Loss {
        private:
            float lossWeight;

            torch::Tensor giou(torch::Tensor a, torch::Tensor b) {

                // TODO: To be implemented.
                throw std::runtime_error("Need to implement giou");
            }

        public:
            GIoULoss(float lw): lossWeight(lw) { }

            torch::Tensor forward(torch::Tensor pred, torch::Tensor target, double avgFactor) {
                return (1 - giou(pred, target)).sum() / avgFactor * lossWeight;
            }


    };

    class L1Loss: public Loss {
        private:
            float lossWeight;

        public:
            L1Loss(float lw): lossWeight(lw) { }

            torch::Tensor forward(torch::Tensor pred, torch::Tensor target, double avgFactor) {
                return (pred - target).abs().sum() / avgFactor * lossWeight;
            }


    };

    std::shared_ptr<Loss> buildLoss(std::string type, float lossWeight) {
        if(type == "l1_loss") {
            return std::make_shared<L1Loss>(lossWeight);
        }

        return std::make_shared<GIoULoss>(lossWeight);
    }
}

#endif