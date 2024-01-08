#ifndef __RPN_HPP__
#define __RPN_HPP__

#include <tuple>
#include <vector>
#include <torch/torch.h>
#include "losses.hpp"

namespace rcnn {
    struct ClassBBoxOutputPair {
        torch::Tensor clsOutput;
        torch::Tensor bboxOutput;
    };

    struct GridSize {
        int width;
        int height;

        GridSize(int w, int h) {
            width = w;
            height = h;
        }
    };

    class RPNHeadImpl: torch::nn::Module {
        private:
            int mClassChannels{1};
            int mInChannels;
            int mFeatureChannels;

            int mNumOfAnchors;
    
            torch::nn::Conv2d mConv{nullptr};
            torch::nn::Conv2d mClassifier{nullptr};
            torch::nn::Conv2d mRegressor{nullptr};
            std::shared_ptr<Loss> mLossCls{nullptr};
            std::shared_ptr<Loss> mLossBbox{nullptr};

            std::vector<GridSize> getGridSize(std::vector<torch::Tensor> feats) {
                std::vector<GridSize> gridSizes;
                for(auto &f: feats) {
                    GridSize gs(f.size(2), f.size(3));
                    gridSizes.push_back(gs);
                }
                return gridSizes;
            }

            void getAnchors() {

            }

        public:
            RPNHeadImpl(int inChannels, int featChannels, int numAnchor)
            : mInChannels(inChannels), mFeatureChannels(featChannels), mNumOfAnchors(numAnchor)  {
                mConv = torch::nn::Conv2d(torch::nn::Conv2dOptions(mInChannels, mFeatureChannels, 3).stride(1).padding(1));
                mClassifier = torch::nn::Conv2d(torch::nn::Conv2dOptions(mFeatureChannels, mNumOfAnchors * mClassChannels, 1));
                mRegressor = torch::nn::Conv2d(torch::nn::Conv2dOptions(mFeatureChannels, mNumOfAnchors * 4, 1));
                mLossCls  = buildLoss("l1_loss", 1.0f);
                // _loss_bbox = buildLoss(loss_bbox_opts);
                
                register_module("conv", mConv);
                register_module("classifier", mClassifier);
                register_module("regressor", mRegressor);

                // init_weights
                torch::nn::init::normal_(mConv->weight,       0, 0.01);
                torch::nn::init::normal_(mClassifier->weight, 0, 0.01);
                torch::nn::init::normal_(mRegressor->weight,  0, 0.01);
                torch::nn::init::constant_(mConv->bias,       0);
                torch::nn::init::constant_(mClassifier->bias, 0);
                torch::nn::init::constant_(mRegressor->bias,  0);
            }

            std::vector<ClassBBoxOutputPair> forward(std::vector<torch::Tensor> feats) {
                std::vector<torch::Tensor> clsOuts, bboxOuts;
                std::vector<ClassBBoxOutputPair> outputs;
                // loop for all feature levels to get class/bbox for each of them
                for(auto &x: feats) {
                    ClassBBoxOutputPair outputPair;
                    x = mConv->forward(x).relu_();
                    outputPair.clsOutput = mClassifier->forward(x);
                    outputPair.bboxOutput = mRegressor->forward(x);
                    outputs.push_back(outputPair);
                }

                return outputs;
            }

            /**
             * This method takes the output of the FPN as it's input
            */
            std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> forwardTrain(std::vector<torch::Tensor> feats) {
                std::vector<ClassBBoxOutputPair> rpnOutput = this->forward(feats);
                std::vector<GridSize> gridSizes = getGridSize(feats);
            }

            torch::Tensor getProposals(std::vector<torch::Tensor> anchor_list, std::vector<torch::Tensor> cls_out_list, std::vector<torch::Tensor> bbox_out_list,
                const std::vector<int64_t> &img_shape) {

            }
    };

    TORCH_MODULE(RPNHead);
}

#endif