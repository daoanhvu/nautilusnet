#ifndef ROTATION_SOFTMAX_H
#define ROTATION_SOFTMAX_H

#include <vector>

#include "caffe/blob.hpp"
#include "caffe/layer.hpp"
#include "caffe/proto/caffe.pb.h"
#include "softmax_layer.hpp"

namespace caffe {
	template <typename Dtype>
	class RotationNetSoftmaxLayer: public Layer<Dtype> {
		protected:
			shared_ptr<SoftmaxLayer<Dtype> > softmax_layer_;
			// prob stores the output probability of the layer.
			Blob<Dtype> prob_;
			// Vector holders to call the underlying softmax layer forward and backward.
			vector<Blob<Dtype>*> softmax_bottom_vec_;
			vector<Blob<Dtype>*> softmax_top_vec_;
			int nClassLabel;
			int nRotation;

			virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top);
			virtual void Backward_cpu(const vector<Blob<Dtype>*>& top, 
				const vector<bool>& propagate_down,
				const vector<Blob<Dtype>*>& bottom) {
				NOT_IMPLEMENTED;
			}

		public:
			explicit RotationNetSoftmaxLayer(const LayerParameter &param): Layer<Dtype>(param){
				shared_ptr<SoftmaxLayer<Dtype> > p1(new SoftmaxLayer<Dtype>(param));
				softmax_layer_ = p1;
			}
			virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top);
			virtual void Reshape(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) {
				// NOT_IMPLEMENTED;
			}
			virtual inline const char* type() const { return "RotationNetSoftmax"; }

	};
}

#endif