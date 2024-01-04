#ifndef __ANCHOR_HPP__
#define __ANCHOR_HPP__

#include <torch/torch.h>

namespace rcnn {
    class AnchorGeneratorImpl: public torch::nn::Module {
        private:

        public:
            AnchorGeneratorImpl() {
                
            }

            std::vector<torch::Tensor> getAnchors(const std::vector<std::vector<int64_t>> gridSizes) {
                
            }
            
            // number of base anchors
            int numAnchors() {
                return 0;
            }
    };

    TORCH_MODULE(AnchorGenerator);
}

#endif