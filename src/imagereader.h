/*
 *  imagereader.h
 *
 *  Created by Egor Zamotaev on 11/11/18.
 *  Copyright 2018 Joyful Magic Company. All rights reserved.
 *
 */

#ifndef IMAGEREADER
#define IMAGEREADER

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

using namespace cv;
using namespace std;

/**
 * This class to read images from files
 */
class ImageReader {

  vector<Mat> fileImages;   // last readed images
  vector<string> fileNames; // last readed images filenames

  /// only png and jpeg images
  bool isAllowedExtension(string extension);

public:
  /// read image from path (returns true if succeed)
  bool read(string path);

  /// return last readed files names
  vector<string> getLastFileNames();

  /// return last readed files names
  vector<Mat> getLastFileImages();
};

bool fileExists(string path); // tiny additional function

#endif // IMAGEREADER
