#ifndef _NET_H_
#define _NET_H_

#define PADDING_SAME 0
#define PADDING_VALID 1

typedef struct tgTensor {
    float *data;
    short shape[16];
    short len;

  public:
    tgTensor() {
      data = 0;
      len = 0;
    }

    void release() {
      if(data != 0) {
        delete[] data;
        data = 0;
        len = 0;
      }
    }

    ~tgTensor() {
      release();
    }
} Tensor;

int conv2d(const float *input, int iw, int ih,
  const float *filter, int fw, int fh, int strideW, int strideH,
  int padding, float **output, int &ow, int &oh);

#endif
