#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include<iostream>

#include <stdio.h>

using namespace cv;
using namespace std;

int edgeThresh = 1;
int edgeThreshScharr=1;

bool isFirstPtSelected=false;

Mat image, gray, blurImage, edge1, edge2, cedge ;
Mat imgbw,imgth;
Point2f p1,p2;
vector<Point2f>edgePts;

//const char* window_name1 = "Edge map : Canny default (Sobel gradient)";
const char* window_name2 = "Edge map : Canny with custom gradient (Scharr)";

Mat erosion_dst, dilation_dst;

Vec3b color=(0,0,255);
//color[0]=0;
//color[1]=0;
//color[2]=255;
//int erosion_elem = 2;
//int erosion_size = 1;

//int const max_elem = 2;
//int const max_kernel_size = 21;


float dist(Point2f p,Point2f q){
  float d = sqrt((q.x-p.x)*(q.x-p.x)+(q.y-p.y)*(q.y-p.y));
  return d;
}


void Erosion( Mat src,int, void* )
 {
  int erosion_type = 0;
  int erosion_size1=2;
  int erosion_size2=3;
  //if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
  //else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
  //else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
  Mat element1 = getStructuringElement(  MORPH_ELLIPSE,
                       Size( 2*erosion_size1 + 1, 2*erosion_size1+1 ),
                       Point( erosion_size1, erosion_size1 ) );
  Mat element2 = getStructuringElement(  MORPH_ELLIPSE,
                      Size( 2*erosion_size2 + 1, 2*erosion_size2+1 ),
                      Point( erosion_size2, erosion_size2 ) );
  erode( src, erosion_dst, element1 );
  erode( src, src, element2 );
  erosion_dst = erosion_dst-src;
  //imshow("Subs Image", erosion_dst-src);
  //imshow( "Erosion Demo", erosion_dst );
}
void Dilation( Mat src,int, void* )
 {
  //int dilation_type = 0;
  //if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
  //else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
  //else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
  int dilation_elem = 2;
  int dilation_size = 2;
  Mat element = getStructuringElement( MORPH_ELLIPSE,
                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                       Point( dilation_size, dilation_size ) );
  dilate( src, dilation_dst, element );
  //imshow( "Dilation Demo", dilation_dst );
  Erosion(dilation_dst,0,0);
}


// define a trackbar callback
static void onTrackbar(int, void*){
    blur(gray, blurImage, Size(3,3));

    // Run the edge detector on grayscale
    Canny(blurImage, edge1, edgeThresh, edgeThresh*3, 3);
    cedge = Scalar::all(0);

    image.copyTo(cedge, edge1);
    //imshow(window_name1, cedge);

    /// Canny detector with scharr
    Mat dx,dy;
    Scharr(blurImage,dx,CV_16S,1,0);
    Scharr(blurImage,dy,CV_16S,0,1);
    Canny( dx,dy, edge2, edgeThreshScharr, edgeThreshScharr*3 );
    /// Using Canny's output as a mask, we display our result
    cedge = Scalar::all(0);
    image.copyTo(cedge, edge2);
    imshow(window_name2, cedge);
    Dilation(cedge,0,0);
    imshow("Cedge+Dilate+Erode",erosion_dst);

}

/*static void help()
{
    printf("\nThis sample demonstrates Canny edge detection\n"
           "Call:\n"
           "    /.edge [image_name -- Default is ../data/fruits.jpg]\n\n");
}

const char* keys =
{
    "{help h||}{@image |../data/fruits.jpg|input image name}"
};*/

//int drawEdge(Point2f p,Point2f q);

void scan(Point2f t){
  int kernel_size = 10;
  for(int i=t.x-kernel_size;i<=t.x+kernel_size;i++){
    for(int j=t.y-kernel_size;j<=t.y+kernel_size;j++){
      if(imgth.at<uchar>(j,i)<50){
        //image.at<Vec3b>(j,i)=color;
        //edgePts.push_back(Point2f(i,j));
        //cout<<"Pixel changed: "<<Point2f(i,j);
        //return Point2f(i,j);
        //drawEdge(Point2f(i,j),q);
        int minDist = 5;
        //for(int k=0;k<edgePts.size();k++){
          if((dist(edgePts[edgePts.size()-1],Point2f(i,j))<minDist)){//||(dist(p1,Point2f(i,j))<minDist)||(dist(p2,Point2f(i,j))<minDist)){
            edgePts.push_back(Point2f(i,j));
            for(int l=-1;l<=1;l++){
              for(int m=-1;m<=1;m++){
              image.at<Vec3b>(j+l,i+m)=color;
            }
          }
            //image.at<Vec3b>(j,i)=color;
          }
        //}
      }
      /*else{
        cout<<"No point found"<<endl;
      }*/
    }
  }
  //cout<<"Done"<<endl;
}

void drawEdge(Point2f p,Point2f q){
 if(q.x<p.x){
    int temp = p.x;
    p.x=q.x;
    q.x=temp;
  }
//  for(int i=p.x;i<=q.x;i++){
    //Point2f t=Point2f(i,p.y+(q.y-p.y)*i/(q.x-p.x));
     for(float i=0;i<=1;){
    Point2f t = p + (q-p)*i;
    scan(t);
    i+=0.1;
  }
  /*if(dist(p,q)<10){
    return 1;
  }
  else{
    scan(p,q);
  }*/
}

int iterCount=0;

void recursiveFind(Point2f p,Point2f q){
  int kernel_size=5;
  for(int j=p.y;j<=p.y+kernel_size;j++){
    for(int i=p.x;i<=p.x+kernel_size;i++){
      if(imgth.at<uchar>(j,i)<50){
        //image.at<Vec3b>(j,i)=color;
        edgePts.push_back(Point2f(i,j));
        line(image,edgePts[edgePts.size()-1],edgePts[edgePts.size()-2],Scalar(0,0,255),1,8,0);
        recursiveFind(edgePts[edgePts.size()-1],q);
        break;
        break;
      }
    }
  }
  //if(dist(Point2f(i,j),q)<)
}

void findInitialPt(Point2f p,Point2f q){
  int kernel_size = 10;
  for(int i=p.x-kernel_size;i<=p.x+kernel_size;i++){
    for(int j=p.y-kernel_size;j<=p.y+kernel_size;j++){
      if(imgth.at<uchar>(j,i)<50){
        image.at<Vec3b>(j,i)=color;
        edgePts.push_back(Point2f(i,j));
        recursiveFind(Point2f(i,j),q);
        break;
        break;
      }
    }
  }
}

void drawEdge2(Point2f p,Point2f q){
  findInitialPt(p,q);
}

void onMouse(int event,int x,int y,int flags,void* params){
  if(event==EVENT_LBUTTONDOWN){
    circle(image,Point2f(x,y),3,Scalar(0,0,255),-1,8,0);
    if(!isFirstPtSelected){
      p1 = Point2f(x,y);
      isFirstPtSelected=true;
      edgePts.push_back(p1);
    }
    else if(isFirstPtSelected) {
      p2 =Point2f(x,y);
      drawEdge(p1,p2);
      //drawEdge2(p1,p2);
      edgePts.clear();
      //isFirstPtSelected=false;
      p1=p2;
      edgePts.push_back(p1);
    }
    imshow("image",image);
  }
}

int main( int argc, const char** argv )
{
    //help();
    //CommandLineParser parser(argc, argv, keys);
    //string filename = parser.get<string>(0);

    //image = imread(filename, IMREAD_COLOR);
    image = imread(argv[1]);

    if(image.empty())
    {
        //printf("Cannot read image file: %s\n", filename.c_str());
        //help();
        printf("Enter image address\n");
        return -1;
    }
    cedge.create(image.size(), image.type());
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // Create a window
    //namedWindow(window_name1, 1);
    namedWindow(window_name2, 1);

    // create a toolbar
    //createTrackbar("Canny threshold default", window_name1, &edgeThresh, 100, onTrackbar);
    createTrackbar("Canny threshold Scharr", window_name2, &edgeThreshScharr, 400, onTrackbar);

    // Show the image
    onTrackbar(0, 0);

    // Wait for a key stroke; the same function arranges events processing

    waitKey(0);
    cvDestroyAllWindows();

    namedWindow("dilated image thresh",WINDOW_NORMAL);
    //imshow("dilated image",erosion_dst);
    cvtColor(erosion_dst,imgbw,COLOR_BGR2GRAY);
    imshow("dilated image bw",imgbw);
    adaptiveThreshold(imgbw,imgth, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 5 );
    imshow("dilated image thresh",imgth);
    imshow("image",image);
    setMouseCallback("image",onMouse);
    waitKey(0);

    return 0;
}
