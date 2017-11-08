#include <iostream>
#include <vector>
//nclude <Magick++.h> 
#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


#include "detect_table.hpp"
#include "detect_cell.hpp"

using namespace std;

//TODO
//unresolved conflict with opencv and imagemagick. Need solution
//IMRPOVEMENT
//use stream instead of writing to file
void doc2png(string pdf, string png){
  /*using namespace Magick;
  InitializeMagick(NULL);  
  Image img(pdf);
  img.write(png);*/
  return;
}    

vector<string> identify_content(string path, vector<cv::Rect> rv){
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(NULL, "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }
  Pix *image = pixRead(path.c_str());
  api->SetImage(image);
  vector<string> v;
  for (cv::Rect r : rv){
    cv::Point p = r.tl();
    api->SetRectangle(p.x, p.y, r.width, r.height);
    v.push_back(string(api->GetUTF8Text()));    
  }
  api->End();
  pixDestroy(&image);  
  return v;
}

//TODO
//add RDF-logic
vector<string> logic_layout(vector<cv::Rect> cells, vector<string> content){
  return content;
}

int main(int argc, char** argv){
  if (argc != 3){
    cout << "Invalid number of arguments" << endl;
    return 0;
  }
  string pdf(argv[1]);
  string png(argv[2]);
  doc2png(pdf, png);
  vector<cv::Rect> tables = detect_tables(png);
  for (cv::Rect r : tables){
    //IMPROVEMENT
    //Read image file once for all tables in it instead of once for every table
    vector<cv::Rect> cells = detect_cells(png, r);
    vector<string> content = identify_content(png, cells);
    //logic_layout(cells, content);
    for (string s : content){
      cout << s << endl;
    }
  }
  return 0;
}
