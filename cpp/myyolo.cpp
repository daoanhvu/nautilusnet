#include <iostream>
#include <opencv2/core/core.hpp>
#include <cmath>
#include "net.h"

double loss() {
  return 0.0;
}

int main(int argc, char **args) {
  char *label[] = {"dog", "cat", "car", "people", "bus"};
  int B = 2;
  int S = 7;
  int ih = 448;
  int iw = 448;

  int i, j;
  cv::Mat m;

  float in[] = {  2, 3, 1, 2, 5,
                  4, 6, 1, 2, 3,
                  3, 4, 1, 2, 4,
                  2, 1, 3, 2, 3};

  float f[] = { 2, 2, 3,
                1, 2, 3,
                3, 1, 2};

  float *output[1];
  int h, w;

  // conv2d(in, 5, 4, f, 3, 3, 1, 1, PADDING_SAME, output, w, h);
  conv2d(in, 5, 4, f, 3, 3, 1, 1, PADDING_VALID, output, w, h);

  printf("Rows: %d, Cols: %d\n ", h, w);
  for(i=0; i<h; i++) {
    for(j=0; j<w; j++) {
      printf("%f ", output[0][i*w + j]);
    }
    printf("\n");
  }

  delete[] output[0];

  return 0;
}
