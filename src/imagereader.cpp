#include "imagereader.h"

#include "strswitch.h"

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem::v1;

// check file existence
bool fileExists(string path) { return fs::exists(path); }

// get file (name,extension)
pair<string, string> getFilenameExt(string path) {
  int filenameStart = path.find_last_of("\\/") + 1;
  int filenameStop = path.rfind(".");
  string name = path.substr(filenameStart, filenameStop - filenameStart);
  string ext = path.substr(filenameStop + 1, 3);
  return make_pair(name, ext);
}

bool ImageReader::isAllowedExtension(string extension) {
  SWITCH(extension) { CASE("png") : CASE("jpg") : CASE("jpeg") : return true; }
  return false;
}

bool ImageReader::read(string pathString) {
  // check existence
  if (!fileExists(pathString)) {
    cerr << "Warning: Image file path doesn't exists. Nothing to do.\n";
    return false;
  }

  // clean last files buffers
  fileNames.clear();
  fileImages.clear();

  // check directory or file
  if (fs::is_directory(pathString)) {

    // loop over the directory files
    for (auto &p : fs::directory_iterator(pathString)) {

      // get file name
      string matPath;
      matPath = p.path();
      pair<string, string> basename = getFilenameExt(matPath);

      // check extension
      if (isAllowedExtension(basename.second)) {
        // write image to vector
        fileImages.push_back(imread(matPath));
        fileNames.push_back(basename.first);
      }
    }
  } else {
    pair<string, string> basename = getFilenameExt(pathString);

    // check extension
    if (isAllowedExtension(basename.second)) {

      // write image to vector
      fileImages.push_back(imread(pathString));
      fileNames.push_back(basename.first);
    } else {
      cerr << "Warning: Use only png, jpg file formats.\n";
      return false;
    }
  }
  return (fileImages.size() >= 0);
}

vector<string> ImageReader::getLastFileNames() { return fileNames; }

vector<Mat> ImageReader::getLastFileImages() { return fileImages; }
