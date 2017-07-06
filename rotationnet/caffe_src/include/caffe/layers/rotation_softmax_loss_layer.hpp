#ifndef ROTATION_SOFTMAX_LOSS_H
#define ROTATION_SOFTMAX_LOSS_H

#include <vector>

#include "caffe/blob.hpp"
#include "caffe/layer.hpp"
#include "caffe/proto/caffe.pb.h"
#include "softmax_layer.hpp"

namespace caffe {
	template <typename Dtype>
	class RotationNetSoftmaxLossLayer: public Layer<Dtype> {
		protected:
			shared_ptr<SoftmaxLayer<Dtype> > softmax_layer_;
		  	// prob stores the output probability of the layer.
		  	Blob<Dtype> prob_;
		  	// Vector holders to call the underlying softmax layer forward and backward.
		  	vector<Blob<Dtype>*> softmax_bottom_vec_;
		  	vector<Blob<Dtype>*> softmax_top_vec_;
		  	int nClassLabel;
		  	int nRotation;
		  	int nSample;
		  	int num;
		  	bool using_upright;
		  	vector<int> labelR; // class + rotation
		  	vector<int> max_ang;
		  	vector< vector<int> > ang;

			virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top);
			virtual void Backward_cpu(const vector<Blob<Dtype>*>& top, 
				const vector<bool>& propagate_down,
				const vector<Blob<Dtype>*>& bottom);

		public:
			explicit RotationNetSoftmaxLossLayer(const LayerParameter &param): Layer<Dtype>(param){};
			virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top);
			virtual void Reshape(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) {
				NOT_IMPLEMENTED;
			}
			virtual inline const char* type() const { return "RotationNetSoftmaxLoss"; }

	};
}

#endif