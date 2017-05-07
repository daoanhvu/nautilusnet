#include "net.h"
#include <cmath>

#include <iostream>

/*

*/
int conv2d(const float *input, int iw, int ih,
  const float *filter, int fw, int fh, int strideW, int strideH,
  int padding, float **output, int &ow, int &oh) {

  int i, j;
  int xc, yc, x, y;
  int xpos;
  int ypos;
  float val;
  float *out;

  if(strideW >= iw || strideH >= ih)
    return 0;

  if(padding == PADDING_SAME) {
    ow = ceil( (float)iw / strideW );
    oh = ceil( (float)ih / strideH );
    out = new float[ow * oh];

    i = 0;
    xpos = 0;
    while(i < oh) {
      yc = i * strideH - fh/2;
      j = 0;
      while( j < ow ) {
        val = 0;
        xc = j * strideW - fw/2;
        //printf("start x: %d, start y: %d\n", xc, yc);
        for(ypos=0; ypos<fh; ypos++) {
          for(xpos=0; xpos<fw; xpos++) {
            x = xc + xpos;
            y = yc + ypos;
            if( (x >= 0 && x < iw) && (y >= 0 && y < ih) )
              val += filter[ypos * fw + xpos] * input[y * iw + x];
          }
        }
        out[i * ow + j] = val;
        j++;
      }
      i++;
    }
  } else { //VALID
    ow = ceil( (float)(iw - fw + 1) / strideW );
    oh = ceil( (float)(ih - fh + 1) / strideH );
    out = new float[ow * oh];

    i = 0;
    xpos = 0;
    yc = 0;
    while(i < oh) {
      j = 0;
      xc = 0;
      while( j < ow ) {
        val = 0;
        for(ypos=0; ypos<fh; ypos++) {
          for(xpos=0; xpos<fw; xpos++) {
            x = xc + xpos;
            y = yc + ypos;
            val += filter[ypos * fw + xpos] * input[y * iw + x];
          }
        }
        out[i * ow + j] = val;
        xc += strideW;
        j++;
      }
      yc += strideH;
      i++;
    }
  }
  *output = out;
  return 1;
}

int maxpool(const float *input, int iw, int ih, float **output, int &ow, int &oh) {
  return 0;
}
