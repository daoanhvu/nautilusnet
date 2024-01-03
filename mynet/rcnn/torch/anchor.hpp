#ifndef __RPN_HPP__
#define __RPN_HPP__

#include <tuple>
#include <vector>
#include <torch/torch.h>

namespace rcnn {
    class AnchorGeneratorImpl: public torch::nn::Module {
        private:

        public:
            AnchorGeneratorImpl() {
                
            }
    };

    TORCH_MODULE(AnchorGenerator);
}

#endif