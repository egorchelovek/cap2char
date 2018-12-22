/*
 *  anclassify.h
 *
 *  Created by Egor Zamotaev on 11/11/18.
 *  Copyright 2018 Joyful Magic Company. All rights reserved.
 *
 */

#ifndef ANNCLASSIFY
#define ANNCLASSIFY

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

using namespace cv;
using namespace ml;
using namespace std;

/**
 * This is main computing class to get text from the images
 * based on ANN_MLP OpenCV class
 */
class AnnClassify {

  Ptr<ml::ANN_MLP> ann; // simple feedforward Artifical Neural Network
  const string classNames =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
      "0123456789"; // common
                    // symbols
                    // set
  float accuracy;   // ANN test data accuracy

public:
  /// returns one-symbol string from symbol image
  string symbolToString(Mat symbol);

  /// creates ann trained on dataset
  void train(vector<Mat> images, vector<string> labels);

  /// load/save ANN from/to the file
  void load(string filePath);
  void save(string filePath);

  /// returns ANN test data accuracy
  float getAccuracy();
};

#endif // ANNCLASSIFY
