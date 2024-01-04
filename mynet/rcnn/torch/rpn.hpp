#ifndef __RPN_HPP__
#define __RPN_HPP__

#include <tuple>
#include <vector>
#include <torch/torch.h>
#include "anchor.hpp"
#include "losses.hpp"

namespace rcnn {
    struct ClassBBoxOutputPair {
        torch::Tensor clsOutput;
        torch::Tensor bboxOutput;
    };

    class RPNHeadImpl: torch::nn::Module {
        private:
            int mClassChannels{1};
            int mInChannels;
            int mFeatureChannels;

            AnchorGenerator mAnchorGenerator{nullptr};
    
            torch::nn::Conv2d mConv{nullptr};
            torch::nn::Conv2d mClassifier{nullptr};
            torch::nn::Conv2d mRegressor{nullptr};
            std::shared_ptr<Loss> mLossCls{nullptr};
            std::shared_ptr<Loss> mLossBbox{nullptr};

        public:
            RPNHeadImpl(AnchorGenerator &_anchorGenerator, int inChannels, int featChannels)
            : mAnchorGenerator(_anchorGenerator), mInChannels(inChannels), mFeatureChannels(featChannels)  {
                mConv = torch::nn::Conv2d(torch::nn::Conv2dOptions(mInChannels, mFeatureChannels, 3).stride(1).padding(1));
                mClassifier = torch::nn::Conv2d(torch::nn::Conv2dOptions(mFeatureChannels, mAnchorGenerator->numAnchors() * mClassChannels, 1));
                // mRegressor = torch::nn::Conv2d(torch::nn::Conv2dOptions(mFeatureChannels, mAnchorGenerator->numAnchors() * 4, 1));
                // _loss_cls  = build_loss(loss_cls_opts);
                // _loss_bbox = build_loss(loss_bbox_opts);
                
                register_module("conv", mConv);
            }

            std::vector<ClassBBoxOutputPair> forward(std::vector<torch::Tensor> feats) {
                std::vector<torch::Tensor> clsOuts, bboxOuts;
                std::vector<ClassBBoxOutputPair> outputs;
                // loop for all feature levels to get class/bbox for each of them
                for(auto &x: feats) {
                    x = mConv->forward(x).relu_();
                }

                return outputs;
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