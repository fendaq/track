#include "dataset_utils.hpp"
#include <string>
#include <sstream>
#include <utility>
#include <opencv2/opencv.hpp>
#include "image_util.hpp"
#include "pugixml.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace dataset;
using namespace pugi;
using namespace tree;
using namespace cv;

Result& dataset::Result::operator+=(const Result& other){
  countGT += other.countGT;
  correct += other.correct;
  incorrect += other.incorrect;
  pure += other.pure;
  complete += other.complete;  
  return *this;
}

string dataset::getBase(string str){
  return str.substr(0, str.find_last_of("_"));
}

string dataset::getName(string str){
  return str.substr(str.find_last_of("/")+1);
}

int dataset::getNumber(string str){
  int first = str.find_last_of("_")+1;
  int last = str.find_last_of(".");
  int numb;
  istringstream(str.substr(first, last-first)) >> numb;
  return numb;
}

string dataset::boundName(string str){
  return getBase(str) + "_" + "z"; 
}

void dataset::Document::insert(int pageNumber, Page p){
  pages[pageNumber] = p;
}

void dataset::Document::insertGT(int pageNumber, Rect table){
  pages[pageNumber].gt.push_back(table);
}

void dataset::Result::update(bool isPure, bool isComplete){
  if (isPure)
    pure++;
  if (isComplete)
    complete++;
  if (isPure && isComplete)
    correct++;
  else
    incorrect++;
}

Result dataset::Page::evaluate(){
  Result result(gt.size());
  RTBox tree;
  for (Rect& table : gt){
    insert2tree(tree, table);    
  }
  for (Rect& table : tables){
    bool pure, complete = false;
    list<Rect> hits = search_tree(tree, table);
    pure = hits.size() == 1;
    for (Rect hit : hits){
      if  (hit == (hit & table)){
	complete = true;
	break;
      }
    }
    result.update(pure, complete);
  }
  return result;
}

Result dataset::Document::evaluate(){
  Result result(0);
  for (pair<int, Page> p : pages){
    result += p.second.evaluate();
  }
  return result;
}


Rect scaleAndTranslate(Rect& r){
  int bottom = 842;
  int sf = 150/72;
  int by = (bottom - r.y)*sf;
  int ty = (bottom - r.br().y)*sf;
  int lx = r.x*sf;
  int rx = r.br().x*sf;
  return pos2rect(lx, ty, rx, by);
}

void dataset::attachGT(Document& doc){
  const string reg = "-reg.xml";
  const string text = "-str.xml";
  const string regname = doc.name + reg;
  const string textname = doc.name + text;
  xml_document xml;
  xml.load_file(regname.c_str());
  xpath_node_set rns = xml.select_nodes("//region");
  for (xpath_node rn : rns){
    xml_node region = rn.node();
    int number =  region.attribute("page").as_int();
    xml_node b = region.child("bounding-box");
    Rect r = pos2rect(b.attribute("x1").as_int(),
		      b.attribute("y1").as_int(),
		      b.attribute("x2").as_int(),
		      b.attribute("y2").as_int());
    Rect fixedBox =  scaleAndTranslate(r);
    doc.insertGT(rn.node().attribute("page").as_int(), fixedBox);
  }
}
