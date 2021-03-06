#include <vector>
#include <algorithm>
#include <queue>

#include "detect_table.hpp"
#include "RTree.h"
#include "heuristic_filter.hpp"
#include "homogenous_regions.hpp"
#include "recursive_filter.hpp"
#include "mla.hpp"
#include "mlc.hpp"
#include "noise.hpp"

using namespace std;
using namespace cv;

Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

vector<Rect> detect_tables(string filename){
  Mat img = imread(filename.c_str(), IMREAD_GRAYSCALE);  
  if (!img.data)
    cerr << "Problem loading image. " << endl;
  Mat text = gray2binary(img);
  Mat nontext(text.size(), CV_8UC1, Scalar(0));
  heuristic_filter(text, nontext);
  multi_level_analysis(text, nontext);
  multi_level_classification(text, nontext);
  remove_noise(text, nontext);
  imshow("text", text);
  imshow("nontext", nontext);
  waitKey(0);
}

 main(int argc, char** argv){
  detect_tables(string(argv[1]));
}
