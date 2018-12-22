#include "textcropper.h"

TextCropper::TextCropper(int nSymbols, Size outputSize) {
  setSymbolsNumber(nSymbols);
  setOutputSize(outputSize);
}

// set crop numbers
void TextCropper::setSymbolsNumber(int nSymbols) {
  if (nSymbols > 0) {
    this->nSymbols = nSymbols;
  }
}

// set output image size
void TextCropper::setOutputSize(Size outputSize) {
  if (outputSize.height > 0 && outputSize.width > 0) {
    this->outputSize = outputSize;
  }
}

vector<Mat> TextCropper::crop(Mat &textImage) {

  // here we crop
  Mat gray;
  cvtColor(textImage, gray, CV_BGR2GRAY);

  Mat textROI = selectTextROI(gray);
  Mat thresh;
  threshold(textROI, thresh, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

  int h = thresh.size().height;
  int w = thresh.size().width;
  int subW = float(w) / nSymbols;

  vector<Mat> symbols;
  for (int x = 0; x < nSymbols * subW; x += subW) {
    Mat symbol = thresh(Rect(x, 0, subW, h));
    symbols.push_back(squarize(symbol));
  }

  return normalizeSquaredSymbolsSizes(symbols, outputSize);
}

// make image denoized and binary
Mat TextCropper::preprocess(Mat &image) {
  Mat median, otsu;

  medianBlur(image, median, 3);

  threshold(median, otsu, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

  return otsu;
}

// select only a text region at image
Mat TextCropper::selectTextROI(Mat &image) {
  Mat bwImage = preprocess(image);

  Mat blur, thresh, bordered;
  GaussianBlur(bwImage, blur, Size(41, 41), 0);
  threshold(blur, thresh, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  copyMakeBorder(thresh, bordered, 2, 2, 2, 2, BORDER_CONSTANT, 255);

  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  findContours(bordered, contours, hierarchy, CV_RETR_TREE,
               CV_CHAIN_APPROX_SIMPLE, Point());

  return cropMinAreaRect(image, minAreaRect(contours[1]));
}

// crops simple rotated rectangle image
Mat TextCropper::cropMinAreaRect(Mat &image, RotatedRect rect) {
  Mat croppedRotated;

  Point2f vertices[4];
  rect.points(vertices);

  int W = rect.size.width;
  int H = rect.size.height;

  int x1 = vertices[0].x;
  int x2 = x1;
  int y1 = vertices[0].y;
  int y2 = y1;
  for (int i = 0; i < 4; i++) {
    int x = vertices[i].x;
    int y = vertices[i].y;
    if (x1 > x)
      x1 = x;
    if (x2 < x)
      x2 = x;
    if (y1 > y)
      y1 = y;
    if (y2 < y)
      y2 = y;
  }

  // correct angle
  float angle = rect.angle < -45 ? rect.angle + 90 : rect.angle;
  // center of rectangle in source image
  Point2f center((x1 + x2) / 2, (y1 + y2) / 2);
  // size of the upright rectangle bounding the rotated rectangle
  Size patchSize(x2 - x1, y2 - y1);
  Mat M = getRotationMatrix2D(
      Point2f(patchSize.width / 2, patchSize.height / 2), angle, 1.);
  // cropped upright rectangle
  Mat cropped, warped;
  getRectSubPix(image, patchSize, center, cropped);
  warpAffine(cropped, warped, M, patchSize);
  int croppedW = H > W ? H : W;
  int croppedH = H < W ? H : W;
  getRectSubPix(warped, Size(int(croppedW), int(croppedH)),
                Point2f(patchSize.width / 2, patchSize.height / 2),
                croppedRotated);

  return croppedRotated;
}

// make square bounds for better normalization
Mat TextCropper::squarize(Mat &image) {
  Mat squaredImage = image.clone();
  int h = image.size().height, w = image.size().width;

  if (w != h) {
    int maxSide = max(h, w);
    int x = float(maxSide - w) / 2;
    int y = float(maxSide - h) / 2;

    // make 1px correction by side
    int newSizeX = w + 2 * x;
    int newSizeY = h + 2 * y;
    int xCorrection = 0;
    int yCorrection = 0;
    if (newSizeX != newSizeY) {
      if (min(newSizeX, newSizeY) == newSizeX) {
        xCorrection = 1;
      } else {
        yCorrection = 1;
      }
    }

    copyMakeBorder(image, squaredImage, y, y + yCorrection, x, x + xCorrection,
                   BORDER_CONSTANT, Scalar(255, 255, 255));
  }

  return squaredImage;
}

// input: square symbol images of different sizes
// output: square images with the same sizes
vector<Mat> TextCropper::normalizeSquaredSymbolsSizes(vector<Mat> symbols,
                                                      Size newSizes) {
  vector<Mat> resizedSymbols;
  // get max size
  int maxSize = symbols[0].size().height;
  for (int i = 0; i < symbols.size(); i++) {
    int size = symbols[i].size().height;
    if (maxSize < size) {
      maxSize = size;
    }
  }

  // add size to max size of image symbols and then fit to the sizes square
  for (int i = 0; i < symbols.size(); i++) {
    int dif = maxSize - symbols[0].size().height;
    int half = float(dif) / 2;
    int add = dif - half;
    Mat bordered, resizedSymbol;
    copyMakeBorder(symbols[i], bordered, add, half, add, half, BORDER_CONSTANT,
                   Scalar(255, 255, 255));
    resize(bordered, resizedSymbol, newSizes);
    resizedSymbols.push_back(resizedSymbol);
  }

  return resizedSymbols;
}
