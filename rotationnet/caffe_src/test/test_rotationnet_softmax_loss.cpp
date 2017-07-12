#include <algorithm>
#include <vector>

#include "gtest/gtest.h"

#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/filler.hpp"

#include "caffe/test/test_caffe_main.hpp"
#include "caffe/test/test_gradient_check_util.hpp"

#include "caffe/layers/rotation_softmax_loss_layer.hpp"

namespace caffe {
	template <typename TypeParam>
	class RotationNetSoftmaxLossTest: public MultiDeviceTest<TypeParam> {
		typedef typename TypeParam::Dtype Dtype;

	protected:

		Blob<Dtype>* const blob_bottom_;
  		Blob<Dtype>* const blob_top_;
  		vector<Blob<Dtype>*> blob_bottom_vec_;
  		vector<Blob<Dtype>*> blob_top_vec_;

		RotationNetSoftmaxLossTest(): blob_bottom_(new Blob<Dtype>(10, 10, 20, 30)), blob_top_(new Blob<Dtype>()) {
			Caffe::set_random_seed(1701);

    		//FillerParameter filler_param;
    		//GaussianFiller<Dtype> filler(filler_param);
    		//filler.Fill(this->blob_bottom_);
    		blob_bottom_vec_.push_back(blob_bottom_);
    		blob_top_vec_.push_back(blob_top_);
		}

		virtual ~RotationNetSoftmaxLossTest() {
			delete blob_top_;
			delete blob_bottom_;
		}

		void TestLayerSetUp(Dtype filler_std) {
			FillerParameter filler_param;
    		GaussianFiller<Dtype> filler(filler_param);
    		filler.Fill(this->blob_bottom_);

    		LayerParameter layer_param;
    		RotationNetSoftmaxLayer<Dtype> layer(layer_param);
    		layer.SetUp(this->blob_bottom_vec_, this->blob_top_vec_);
		}

		void testForward() {
			FillerParameter filler_param;
    		GaussianFiller<Dtype> filler(filler_param);
    		filler.Fill(this->blob_bottom_);

    		LayerParameter layer_param;
    		RotationNetSoftmaxLayer<Dtype> layer(layer_param);
    		layer.SetUp(this->blob_bottom_vec_, this->blob_top_vec_);
    		layer.Forward(this->blob_bottom_vec_, this->blob_top_vec_);
		}

	};

	TYPED_TEST_CASE(RotationNetSoftmaxLossTest, TestDtypesAndDevices);
	TYPED_TEST(RotationNetSoftmaxLossTest, TestLayerSetUp) {
  		this->TestLayerSetUp(1.0);
	}

	TYPED_TEST(RotationNetSoftmaxLossTest, TestForwardCPU) {
  		this->testForward();
	}
}
