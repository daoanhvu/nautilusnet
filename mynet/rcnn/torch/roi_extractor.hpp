#ifndef __ROI_EXTRACTOR_HPP__
#define __ROI_EXTRACTOR_HPP__

#include <torch/torch.h>

namespace rcnn {
    class RoIExtractorImpl: torch::nn::Module {
        private:


        public:
            RoIExtractorImpl() {

            }
    };

    TORCH_MODULE(RoIExtractor);
}

#endif