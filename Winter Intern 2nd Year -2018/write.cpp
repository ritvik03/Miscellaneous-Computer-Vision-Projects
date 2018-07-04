#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<math.h>
#include<string>
#include<string.h>
#include<stdlib.h>

using namespace std;
using namespace cv;

Mat inputimage,altimage,prevTextimage;
Point2f AddTextPt;
vector<string>strs;
vector<Point2f>strPts;
bool isWrittingOn=false;
std::string str;
int fontType=1;
int fontSize=1;
int textThickness=1;

void strStoring(char c,Point2f p);

void strsPrint(vector<string>s,vector<Point2f>p){
  inputimage=prevTextimage.clone();
  for(int i=0;i<p.size();i++){
    putText(inputimage,s[i],p[i],fontType,fontSize,Scalar(255,0,0),textThickness,8,0);
  }
  imshow("Input",inputimage);
}

void strStoring(char character,Point2f p){
  inputimage=prevTextimage.clone();
  strsPrint(strs,strPts);
  if(character=='~'){
    //inputimage=prevTextimage;
    if(str.begin()!=str.end()){
    str.erase(str.end()-1);
  }
  }
  else str.insert(str.end(),1,character);
  int baseline=0;
  Size textSizeDefault = getTextSize("Hello World Sir!!", fontType,fontSize, textThickness, &baseline);
  Size textSize = getTextSize(str, fontType,fontSize, textThickness, &baseline);
  if(textSize.width<textSizeDefault.width){
    textSize=textSizeDefault;
  }
  rectangle(inputimage,AddTextPt+Point2f(-10,10),AddTextPt+Point2f(textSize.width,-textSize.height)+Point2f(10,-10),Scalar(0,100,0),2,8,0);
  putText(inputimage,str,p,fontType,fontSize,Scalar(255,0,0),textThickness,8,0);
  cout<<str<<endl;
  imshow("Input",inputimage);
  imshow("PrevImage",prevTextimage);
  //int baseline=0;
  //Size textSize = getTextSize(str, FONT_HERSHEY_SIMPLEX,fontSize, textThickness, &baseline);
  //baseline += textThickness;
  //rectangle(inputimage, p ,p + Point2f(textSize.width, -textSize.height),Scalar(0,0,255));
}

void charTakingFunc(Point2f p){
  while(1){
    int characterInput;
    characterInput=waitKey(0);
    if((characterInput== 27)||(characterInput== 13)){
      cout<<"Escape or Enter Pressed"<<endl;
      isWrittingOn=false;
      strs.push_back(str);
      strPts.push_back(p);
      str.clear();
      strsPrint(strs,strPts);
      break;
    }
    //cout<<characterInput<<endl;
    char character = char(characterInput);

    //cout<<character<<endl;
    if(characterInput==32){
      character = ' ';
    }
    if(characterInput==8){
      //str.erase(str.end()-1);
      //inputimage=prevTextimage;
      strStoring('~',p);
    }
    else strStoring(character,p);
  }
}

void mousecallback(int event, int x, int y,int flags,void* param){

  if(event==EVENT_LBUTTONDOWN){
    //prevTextimage=inputimage;
    if(1){
      cout<<"Click Done"<<endl;
      isWrittingOn=false;
      strs.push_back(str);
      strPts.push_back(AddTextPt);
      strsPrint(strs,strPts);
      str.clear();
    }
    AddTextPt=Point2f(x,y);
    circle(inputimage,AddTextPt,2,Scalar(0,0,0),-1,8,0);
    //clear the string
    isWrittingOn=true;
    charTakingFunc(AddTextPt);
    //refreshingImage();
    //event=EVENT_MOUSEMOVE;
  }
  else if(event==EVENT_MOUSEMOVE){
    altimage=inputimage.clone();
  }
  imshow("Input",altimage);
}

int main(int argc,char* argv[]){
  if(argc>1){
    inputimage=imread(argv[1]);
  }
  else cout<<"Enter correct image address"<<endl;
  int n;
  cout<<"Enter font type (1-8)"<<endl;
  cin>>n;
  fontType=n-1;
  cout<<"Enter font size"<<endl;
  cin>>fontSize;
  cout<<"Enter font textThickness"<<endl;
  cin>>textThickness;
  prevTextimage=inputimage.clone();
  altimage=inputimage.clone();
  namedWindow("Input",WINDOW_NORMAL);
  namedWindow("PrevImage",WINDOW_NORMAL);
  imshow("Input",altimage);
  setMouseCallback("Input",mousecallback);
  waitKey(0);
  return 0;
}
