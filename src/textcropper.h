/*
 *  textcropper.h
 *
 *  Created by Egor Zamotaev on 11/11/18.
 *  Copyright 2018 Joyful Magic Company. All rights reserved.
 *
 */

#ifndef TEXTCROPPER
#define TEXTCROPPER

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <vector>

using namespace cv;
using namespace std;

/**
 * Crop image with text by symbols
 */
class TextCropper {

  int nSymbols;    // number of crop picies
  Size outputSize; // output image size

  /*
   * Main worflow process
   */
  /// make image denoized and binary
  Mat preprocess(Mat &image);
  /// select only a text region at image
  Mat selectTextROI(Mat &image);
  /// crops simple rotated rectangle image
  Mat cropMinAreaRect(Mat &image, RotatedRect rect);
  /// make square bounds for better normalization
  Mat squarize(Mat &image);
  /// make image sizes equals
  vector<Mat> normalizeSquaredSymbolsSizes(vector<Mat> symbols, Size newSizes);

public:
  /**
   * @nSymbols expected number of symbols in text
   * @outputSize size of output symbol image (outputSize,outputSize)
   */
  TextCropper(int nSymbols = 5, Size outputSize = Size(32, 32));

  /// set cropper parameters (look above ^)
  void setSymbolsNumber(int nSymbols);
  void setOutputSize(Size outputSize);

  /// get symbols images
  vector<Mat> getSymbols();

  /// crop image text to the symbols images
  vector<Mat> crop(Mat &textImage);
};

#endif // TEXTCROPPER
