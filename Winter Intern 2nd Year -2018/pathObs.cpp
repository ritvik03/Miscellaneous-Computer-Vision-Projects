#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void main(int argc,char* argv[]){
  if(argc>1){
    originalImage= imread(argv[1]);
  }
  namedWindow("Original",WINDOW_NORMAL);
  imshow("Original",originalImage);
  waitKey(0);
}
