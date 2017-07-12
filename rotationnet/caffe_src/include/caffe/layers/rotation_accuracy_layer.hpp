#ifndef ROTATIONNET_ACCURACY_H
#define ROTATIONNET_ACCURACY_H

#include <vector>

#include "caffe/blob.hpp"
#include "caffe/layer.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/layers/loss_layer.hpp"

namespace caffe {
	template<typename Dtype> 
	class RotationNetAccuracyLayer: public Layer<Dtype> {
		protected:
			int nClassLabel;
			int nRotation;
			int nSample;
			bool using_upright;
			vector< vector<int> > ang;

			virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top);
			virtual void Backward_cpu(const vector<Blob<Dtype>*>& top,
      				const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom) {
    			// NOT_IMPLEMENTED;
  			}

		public:
			explicit RotationNetAccuracyLayer(const LayerParameter &param): Layer<Dtype>(param){};
			virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top);
			virtual void Reshape(const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) {
				// NOT_IMPLEMENTED;
			}
			//https://github.com/BVLC/caffe/issues/4535
			virtual inline const char* type() const { return "RotationNetAccuracy"; }
	};
}

#endif