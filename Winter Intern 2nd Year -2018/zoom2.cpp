#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<math.h>
#include<string>
#include<string.h>
#include<stdlib.h>

using namespace std;
using namespace cv;

Mat inputimage,zoomedImage,originalImage;
Point2f zoomedAreaCentre;
int magnification=200;              //in percent
int clearance=50;
int zoomRec;
Size newWin;

void blend(Mat A,Mat B,int min_x,int min_y){
  // min_x, min_y should be valid in A and [width height] = size(B)
  cv::Rect roi = cv::Rect(min_x, min_y, B.cols, B.rows);

// "out_image" is the output ; i.e. A with a part of it blended with B
cv::Mat out_image = A.clone();

// Set the ROIs for the selected sections of A and out_image (the same at the moment)
cv::Mat A_roi= A(roi);
cv::Mat out_image_roi = out_image(roi);
int alpha=0;
// Blend the ROI of A with B into the ROI of out_image
cv::addWeighted(A_roi,alpha,B,1-alpha,0.0,out_image_roi);
imshow("Input",out_image);
}

void drawPlus(Mat image,Point2f p){
  int linelength=20;
  line(image,p,Point2f(p.x+linelength/2,p.y),Scalar(0,0,0),1,8,0);
  line(image,p,Point2f(p.x-linelength/2,p.y),Scalar(0,0,0),1,8,0);
  line(image,p,Point2f(p.x,p.y+linelength/2),Scalar(0,0,0),1,8,0);
  line(image,p,Point2f(p.x,p.y-linelength/2),Scalar(0,0,0),1,8,0);
}

Mat zoomIn(int x, int y)
{
  //zoomRec = (int)(newWin.width*100/magnification);
	int width = zoomRec, height = zoomRec;
	int ptoX = x-(zoomRec/2), ptoY = y-(zoomRec/2);

	if((x+(zoomRec/2)) > inputimage.size().width)
		width = width - ((x+(zoomRec/2)) - inputimage.size().width);

	if((y+(zoomRec/2)) > inputimage.size().height)
		height = height - ((y+(zoomRec/2)) - inputimage.size().height);

	if((x-(zoomRec/2)) < 0)
		ptoX = 0;

	if((y-(zoomRec/2)) < 0)
		ptoY = 0;

	Rect roi = Rect(ptoX, ptoY, width, height);
	Mat inputimage_roi = inputimage(roi);
	resize(inputimage_roi, inputimage_roi, newWin /*Size(inputimageOriginal.size().width, inputimageOriginal.size().height)*/, 0, 0, CV_INTER_AREA);
  copyMakeBorder(inputimage_roi,inputimage_roi,2,2,2,2,BORDER_CONSTANT,Scalar(0,0,0));
  drawPlus(inputimage_roi,Point2f(inputimage_roi.cols/2,inputimage_roi.rows/2));
	return inputimage_roi;
}

void mousecallback(int event,int x,int y, int flags,void* param){
  if(event==EVENT_MOUSEMOVE){
    inputimage=originalImage.clone();
    zoomedAreaCentre=Point2f(x,y);
    zoomedImage=zoomIn(x,y);
    cout<<zoomedAreaCentre<<endl;
  }
  int xnew=x+clearance;
  int ynew=y+clearance;
  if(x+30+clearance+newWin.width>originalImage.size().width){
    //moveWindow("zoomed",x-100-newWin.width,y-100-newWin.height);
    xnew=x-clearance-newWin.width;
    //cout<<"Called"<<endl;
  }
  if(y+30+clearance+newWin.height>originalImage.size().height){
    //moveWindow("zoomed",x-100-newWin.width,y-100-newWin.height);
    ynew=y-clearance-newWin.height;
    //cout<<"Called"<<endl;
  }
  //zoomedImage.copyTo(inputimage(cv::Rect(x,y,zoomedImage.cols, zoomedImage.rows)));
  //imshow("zoomed",zoomedImage);
  //moveWindow("zoomed",xnew,ynew);
  //imshow("Input",inputimage);
  blend(inputimage,zoomedImage,xnew,ynew);
}

int track=100;

void change_mag(int,void*){
  magnification=track+100;
  zoomRec = (int)(newWin.width*100/magnification);
}

int main(int argc, char* argv[]){
  if(argc>1){
    inputimage=imread(argv[1]);
  }
  originalImage=inputimage.clone();
  newWin.width=200;
  newWin.height=200;
  zoomRec = (int)(newWin.width*100/magnification);
  namedWindow("Input",WINDOW_NORMAL);
  createTrackbar("Magnification percent","Input",&track,400,change_mag);
  //cvSetWindowProperty("Input", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  imshow("Input",inputimage);
  setMouseCallback("Input",mousecallback);
  waitKey(0);
  return 0;
}
