#include "image_processor.h"

int detectBoundingBox(cv::Mat img, cv::Vec3b bgcolor, cv::Rect &bbox) {
  cv::Size size = img.size();
  int ih = size.height;
  int iw = size.width;
  bool should_stop = false;
  int r, c;
  r = 0;
  while( (r < ih) && !should_stop ) {
    c = 0;
    while( c < iw) {
      if(img.at<cv::Vec3b>(r, c) != bgcolor) {
        bbox.y = (r > 0)? r-1 : r;
        should_stop = true;
        break;
      }
      c++;
    }
    r++;
  }

  should_stop = false;
  r = ih - 1;
  while( (r >= 0) && !should_stop ) {
    c = 0;
    while( c < iw ) {
      if(img.at<cv::Vec3b>(r, c) != bgcolor) {
        bbox.height = ( (r < (ih-1))?r+1:r ) - bbox.y;
        should_stop = true;
        break;
      }
      c++;
    }
    r--;
  }

  ////////////////
  should_stop = false;
  c = 0;
  while( (c < iw) && !should_stop ) {
    r = 0;
    while( r < ih) {
      if(img.at<cv::Vec3b>(r, c) != bgcolor) {
        bbox.x = (c > 0)? c-1 : c;
        should_stop = true;
        break;
      }
      r++;
    }
    c++;
  }

  should_stop = false;
  c = iw - 1;
  while( (c >= 0) && !should_stop ) {
    r = 0;
    while( r < ih ) {
      if(img.at<cv::Vec3b>(r, c) != bgcolor) {
        bbox.width = c - bbox.x + 1;
        should_stop = true;
        break;
      }
      r++;
    }
    c--;
  }
  return 0;
}
