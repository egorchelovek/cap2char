#include "imagecollection.h"

ImageCollection::ImageCollection(int samplesToReady) {
  setVolume(samplesToReady);
}

void ImageCollection::put(vector<Mat> data, string label) {
  images.insert(images.end(), data.begin(), data.end());

  // divide label string by symbols
  for (int i = 0; i < label.size(); i++) {
    labels.push_back(string(1, label[i]));
  }
}

void ImageCollection::setVolume(int samplesToReady) {
  if (samplesToReady > 0) {
    this->samplesToReady = samplesToReady;
  }
}

bool ImageCollection::isReady() { return images.size() >= samplesToReady; }

vector<Mat> ImageCollection::getImages() { return images; }

vector<string> ImageCollection::getLabels() { return labels; }

void ImageCollection::clear() {
  images.clear();
  labels.clear();
}
