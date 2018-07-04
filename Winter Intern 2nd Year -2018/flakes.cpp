#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<math.h>
#include<stdlib.h>

using namespace std;
using namespace cv;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<float>areas;
Mat inputimage,original;
int thresh=100;
RNG rng(12345);

void insertionSort(vector<float>arr)
  {
  int n=arr.size();
   int i, key, j;
   for (i = 1; i < n; i++)
   {
       key = arr[i];
       j = i-1;

       /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
       while (j >= 0 && arr[j] > key)
       {
           arr[j+1] = arr[j];
           j = j-1;
       }
       arr[j+1] = key;
   }
 }

/*void findMax3(vector<float>a){
  insertionSort(a);
  M.M1=a[a.size()-1];
  M.M2=a[a.size()-2];
  M.M3=a[a.size()-3];
}*/

float sumofFirstNArea(int n){
  float sum=0;
  for(int i=0;i<n;i++){
    sum+=areas[areas.size()-i-1];
    if(i==areas.size()-1) break;
  }
  return sum;
}

void on_track(int,void*){
  Mat inputimage2=inputimage.clone();
  threshold(inputimage,inputimage2,thresh,255,0);
  imshow("Input",inputimage2);
}

int main(int argc,char* argv[]){
  if(argc>1){
    inputimage=imread(argv[1],0);
    original=imread(argv[1]);
  }
  else cout<<"Enter correct image address"<<endl;
  Mat blurred;
  namedWindow("Input",WINDOW_NORMAL);
  namedWindow("Original",WINDOW_NORMAL);
  //namedWindow("Drawing",WINDOW_NORMAL);
  //namedWindow("Drawing2",WINDOW_NORMAL);
  GaussianBlur( inputimage, blurred, Size( 19, 19 ), 0, 0 );
  imshow("Input",inputimage);
  imshow("Original",original);
  createTrackbar("thresholdbar","Input",&thresh,255,on_track);
  on_track(0,0);
  waitKey(0);
  threshold(inputimage,inputimage,thresh,255,0);
  //imshow("Blur",blurred);
  findContours( inputimage, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Draw contours
  Mat drawing = Mat::zeros( blurred.size(), CV_8UC1 );
  vector<vector<Point> >hull( contours.size() );
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, Scalar(255,255,255), 2, 8, hierarchy, 0, Point() );
       if(contourArea(contours[i],false)<300){
         drawContours( drawing, contours, i, Scalar(0),-1, 8, hierarchy, 0, Point() );
       }
        convexHull( Mat(contours[i]), hull[i], false );
         //drawContours( drawing, hull, (int)i, color, 2, 8, vector<Vec4i>(), 0, Point() );
     }
  GaussianBlur( drawing, drawing, Size( 17, 17 ), 0, 0 );
  //imshow("Drawing",drawing);

  contours.clear();
  hierarchy.clear();
  double sumArea=0;
  Mat drawing2 = Mat::zeros( blurred.size(), CV_8UC3 );
  findContours( drawing, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  for( int i = 0; i< contours.size()-1; i++ )
     {

       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing2, contours, i, color,-1, 8, hierarchy, 0, Point() );
       areas.push_back(contourArea(contours[i],false));
       if(arcLength(contours[i],false)<100000){
         //cout<<"Length of contour "<<i<<": "<<arcLength(contours[i],false)/2<<"\t";
       }
       //cout<<"Contour Area "<<i<<": "<<contourArea(contours[i],false)<<endl;
       sumArea+=contourArea(contours[i],false);
     }
     insertionSort(areas);
     //imshow("Drawing2",drawing2);
     int n=ceil((float)(contours.size())*0.5);     //Treated as area of a&c
     float sumoffirstN = sumofFirstNArea(n);
     cout<<"Actual percent: "<<sumoffirstN/sumArea<<endl;
     if((sumoffirstN/sumArea>0.70)||(2*n>220)){
       sumoffirstN=(0.84+3*(sumArea/(inputimage.rows*inputimage.cols*10)))*sumArea;
     }
      if(sumoffirstN/sumArea>0.98){
         sumoffirstN=(0.98)*sumArea;
      }
     cout<<"PerCent of largest "<< n<< " contour areas: "<<sumoffirstN*100/sumArea<<endl;

  waitKey(0);
  return 0;
}
