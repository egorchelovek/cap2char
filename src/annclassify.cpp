#include "annclassify.h"

string AnnClassify::symbolToString(Mat symbol) {
  symbol.convertTo(symbol, CV_32F);
  int index = ann->predict(symbol.reshape(1, 1), noArray());
  return classNames.substr(index, 1);
}

void AnnClassify::train(vector<Mat> images, vector<string> labels) {
  // prepare data to train ANN
  Mat trainData, trainLabels;
  Mat testData, testLabels;
  int nTestSamples = images.size() / 5;
  int nTrainSamples = images.size() - nTestSamples;

  // scale data to [0,1]
  for (int i = 0; i < images.size(); i++) {
    images[i].convertTo(images[i], CV_32F);
  }

  // divide data by train and test subsets
  for (int i = 0; i < nTrainSamples; i++) {
    trainData.push_back(images[i].reshape(1, 1));
    int classIndex = classNames.find(labels[i]);
    trainLabels.push_back(classIndex);
  }
  for (int i = nTrainSamples; i < images.size(); i++) {
    testData.push_back(images[i].reshape(1, 1));
    int classIndex = classNames.find(labels[i]);
    testLabels.push_back(classIndex);
  }

  // "one-hot" encoding of class labels
  int nInputs = trainData.cols;
  int nClasses = classNames.size();
  Mat trainClasses = Mat::zeros(trainData.rows, nClasses, CV_32FC1);
  for (int i = 0; i < trainClasses.rows; i++) {
    trainClasses.at<float>(i, trainLabels.at<int>(i)) = 1.f;
  }

  // initalize the ANN
  ann = ml::ANN_MLP::create();
  // init layers
  Mat_<int> layers(4, 1);
  layers(0) = nInputs;      // input
  layers(1) = nClasses * 8; // hidden
  layers(2) = nClasses * 4; // hidden
  layers(3) = nClasses;     // output
  ann->setLayerSizes(layers);
  //
  ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
  // init train params
  ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 20, 0));
  ann->setTrainMethod(ANN_MLP::BACKPROP, 0.0001);
  int64 t = getTickCount(); // measure computation time
  // ANN train
  ann->train(trainData, ROW_SAMPLE, trainClasses);

  // check accuracy after the ANN train
  accuracy = 0;
  for (int i = 0; i < testData.rows; i++) {

    int pred = ann->predict(testData.row(i), noArray());
    int truth = testLabels.at<int>(i);
    if (truth == pred)
      accuracy++;
  }
  accuracy /= testData.rows;
}

void AnnClassify::save(string filePath) {
  FileStorage fs;
  fs.open(filePath, FileStorage::WRITE);
  ann->write(fs);
  fs.release();
}

void AnnClassify::load(string filePath) {
  ann = ml::ANN_MLP::create();
  FileStorage fs;
  fs.open(filePath, FileStorage::READ);
  ann->read(fs.root());
  fs.release();
}

float AnnClassify::getAccuracy() { return accuracy; }
