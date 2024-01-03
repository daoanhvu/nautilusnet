#ifndef __FPN_HPP__
#define __FPN_HPP__

#include <vector>
#include <torch/torch.h>

namespace rcnn {
    class FPNImpl: torch::nn::Module {
        private:
            int mOutChannels;
            std::vector<int> mFeatureChannels;
            int mNumOfOutput;

            torch::nn::ModuleList mLateralConvs{nullptr};
            torch::nn::ModuleList mFpnConvs{nullptr};

        public:
            FPNImpl(int outChannels = 256, const std::vector<int> &featChannels = {256, 512, 1024, 2048}, int numOuts = 5)
                                                                        : mOutChannels(outChannels), mNumOfOutput(numOuts) {
                
                this->mLateralConvs = torch::nn::ModuleList();
                this->mFpnConvs = torch::nn::ModuleList();
            
                for(int i=0; i<featChannels.size(); ++i) {
                    int featChannel = featChannels[i];
                    mLateralConvs->push_back(torch::nn::Conv2d(torch::nn::Conv2dOptions(featChannel, mOutChannels, 1)));
                    mFpnConvs->push_back(torch::nn::Conv2d(torch::nn::Conv2dOptions(mOutChannels, mOutChannels, 3).padding(1)));
                }
                register_module("lateral_convs", mLateralConvs);
                register_module("fpn_convs", mFpnConvs);
                initWeights();
            }

            void initWeights() {
                for(auto& module : modules(/*include_self=*/false)) {
                    if(auto m = dynamic_cast<torch::nn::Conv2dImpl*>(module.get())) {
	                    torch::nn::init::xavier_uniform_(m->weight);
	                    torch::nn::init::constant_(m->bias, 0);
                    }
                }
            }

            std::vector<torch::Tensor> forward(std::vector<torch::Tensor> feats) {
                std::vector<torch::Tensor> lateralOuts;
                for(int i=0; i<feats.size(); i++) {
                    lateralOuts.push_back(mLateralConvs[i]->as<torch::nn::Conv2d>()->forward(feats[i]));
                }
                std::vector<torch::Tensor> upsampleOuts({lateralOuts.back()});
                for(int i=feats.size()-2; i>=0; i--) {
                    auto up_size = lateralOuts[i].sizes();
                    auto inter_opts = torch::nn::functional::InterpolateFuncOptions().mode(torch::kNearest);
                    inter_opts.size(std::vector<int64_t>({up_size[2], up_size[3]}));
                    upsampleOuts.push_back(torch::nn::functional::interpolate(upsampleOuts.back(), inter_opts) + lateralOuts[i]);
                }
                std::reverse(upsampleOuts.begin(), upsampleOuts.end());
                
                std::vector<torch::Tensor> outs;
                for(int i=0; i<feats.size(); i++) {
                    outs.push_back(mFpnConvs[i]->as<torch::nn::Conv2d>()->forward(upsampleOuts[i]));
                }

                if(mNumOfOutput > feats.size()) {
                    outs.push_back(torch::nn::functional::max_pool2d(outs.back(), torch::nn::functional::MaxPool2dFuncOptions(1).stride(2)));
                }
                return outs;
            }
    };

    TORCH_MODULE(FPN);
}

#endif