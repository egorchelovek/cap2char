/*
 *  imagecollection.h
 *
 *  Created by Egor Zamotaev on 11/11/18.
 *  Copyright 2018 Joyful Magic Company. All rights reserved.
 *
 */

#ifndef IMAGECOLLECTION
#define IMAGECOLLECTION

#define MIN_SAMPLES 42

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

using namespace cv;
using namespace std;

/**
 * This class will accumulate labeled images
 */
class ImageCollection {

  vector<Mat> images;    // collected images
  vector<string> labels; // the images labels
  int samplesToReady;    // suppose the collection is ready for the purpose

public:
  ImageCollection(
      int samplesToReady = MIN_SAMPLES); // default collection volume

  void setVolume(int samplesToReady);

  /// shows when data set is ready by size
  bool isReady();

  /// put some set of labeled data
  void put(vector<Mat> data, string label);

  /// returns collected data
  vector<Mat> getImages();
  vector<string> getLabels();

  /// clears gathered collection
  void clear();
};

#endif // IMAGECOLLECTION
