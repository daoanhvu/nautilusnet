#include <algorithm>
#include <vector>

#include "gtest/gtest"

#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/filler.hpp"

#include "caffe/test/test_caffe_main.hpp"
#include "caffe/test/test_gradient_check_util.hpp"

#include "caffe/layers/rotation_accuracy_layer.hpp"

namespace caffe {
	template <typename TypeParam>
	class RotationNetAccuracyTest: public MultiDeviceTest<TypeParam> {

	};
}