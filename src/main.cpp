/*
 *  main.cpp
 *
 *  Created by Egor Zamotaev on 11/11/18.
 *  Copyright 2018 Joyful Magic Company. All rights reserved.
 *
 */

#include <opencv2/opencv.hpp>

#include "annclassify.h"
#include "imagecollection.h"
#include "imagereader.h"
#include "textcropper.h"

#include <iostream>

using namespace std;

// program info
String about =
    "\nThe 'cap2char' program.\n"
    "\nGet text from captcha image and send it to standart output."
    "\nThis program using Artrifical Neural Network (ANN) algorithm to "
    "recognize captcha symbols.\n";

// program keys
String keys =
    "{@captcha_path |           | path to captcha image or directory}"
    "{ann a         | Annca.xml | path to the ANN, by default - 88% accuracy}"
    "{number n      | 5         | number of captchas symbols}"
    "{samples s     | 20000     | number of train-test samples for ANN train}"
    "{train t       |           | run program in train mode for ANN teaching}"
    "{help h        |           | print help}";

// start program
int main(int argc, char **argv) {

  /*
   * Declare variables
   */

  // program arguments (look up keys)
  string ann_path = "";
  string captcha_path = "";
  int n_symbols;
  int n_samples;
  bool train;

  // computational classes
  ImageReader image_reader; // read images from captcha_path
  TextCropper text_cropper; // crops readed images to the n_symbols vector
  ImageCollection image_collection; // gather cropped images for train
  AnnClassify ann_classify;         // or classification by ann_classify object

  /*
   * Parse arguments
   */

  // run the parser
  CommandLineParser parser(argc, argv, keys);
  parser.about(about);
  if (parser.has("help")) {
    parser.printMessage();
    return 0;
  }

  // get parameters
  captcha_path = parser.get<string>(0);
  ann_path = parser.get<string>("ann");
  n_symbols = parser.get<int>("number");
  n_samples = parser.get<int>("samples");
  if (parser.has("train"))
    train = true;

  // check if parameters wrong
  if (captcha_path.empty()) {
    cerr << "Error: Captcha image can't be empty.\n";
    return -1;
  }
  if (n_symbols < 0) {
    cerr << "Error: Number of symbols can't be less then zero.\n";
    return -1;
  }
  if (n_samples < 0) {
    cerr << "Error: Number of samples can't be less then zero.\n";
    return -1;
  }
  if (train && ann_path.empty()) {
    cerr << "Error: ANN path can't be empty in the train mode.\n";
    return -1;
  }

  // checking image exists
  if (!image_reader.read(captcha_path)) {
    cerr << "Error: Couldn't load the captcha image from " << captcha_path
         << ".\n";
    return -1;
  }

  // check ANN file exists
  if (!train && !fileExists(ann_path)) {
    cerr << "Error: Couldn't load the ANN from " << ann_path << ".\n";
    return -1;
  }

  /*
   * Get text from captcha or train classifier
   */

  if (train) {
    /*
     * ANN train mode to prepare captcha recognititon
     */

    cout << "Prepare data...\n";

    // setup number of samples for train
    image_collection.setVolume(n_samples);

    // loop over all gathered images (captchas)
    for (int i = 0; i < image_reader.getLastFileImages().size(); i++) {

      // crop characters images and put them to the collection
      image_collection.put(
          text_cropper.crop(image_reader.getLastFileImages()[i]),
          image_reader.getLastFileNames()[i]);

      // check if collection is ready
      if (image_collection.isReady()) {

        cout << "Start training process... May take a light years =)\n";

        // train the ANN by labeled characters images
        ann_classify.train(image_collection.getImages(),
                           image_collection.getLabels());

        cout << "ANN train completed.";
        cout << "Accuracy equals " << setprecision(2) << fixed
             << ann_classify.getAccuracy() << ".\n";

        // save ANN to file
        cout << "Save ANN to the file " << ann_path << ".\n";
        ann_classify.save(ann_path);

        // out of loop
        break;
      }
    }
  } else {
    /*
     * Classification mode (now using for capthca recognititon)
     */

    // setup number of samples for captcha return
    image_collection.setVolume(n_symbols);

    // load the ANN from file
    ann_classify.load(ann_path);

    // loop over all gathered images
    for (int i = 0; i < image_reader.getLastFileImages().size(); i++) {

      // crop characters images (like an detective =)
      image_collection.put(
          text_cropper.crop(image_reader.getLastFileImages()[i]),
          image_reader.getLastFileNames()[i]);

      if (image_collection.isReady()) {

        // loop classifier over collection
        for (int j = 0; j < image_collection.getImages().size(); j++) {

          // send text to the standart output
          cout << ann_classify.symbolToString(image_collection.getImages()[j]);
        }
        cout << "\n";

        image_collection.clear();
      }
    }
  }

  return 0;
}
