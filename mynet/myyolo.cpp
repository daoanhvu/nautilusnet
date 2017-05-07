#include <iostream>
#include <opencv2/core/core.hpp>

typedef struct tgTensor {
  float *data;
  int shape[16];
  int dimen;

  tgTensor() {
    data = NULL;
    dimen = 0;
  }

} Tensor;

double loss() {
  return 0.0;
}

int main(int argc, char **args) {
  char *label[] = {"dog", "cat", "car", "people", "bus"};
  int B = 2;
  int S = 7;
  int ih = 448;
  int iw = 448;

  int i;

  cv::Mat m;

  for(i=0; i<S; i++) {
    for(j=0; j<S; j++) {
      for(k=0; k<B; k++) {

      }
    }
  }

  return 0;
}
