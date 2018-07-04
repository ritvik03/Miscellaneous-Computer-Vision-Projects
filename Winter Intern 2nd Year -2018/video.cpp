#include "opencv2/opencv.hpp"
#include<iostream>
#include "opencv2/stitching.hpp"
using namespace cv;
using namespace std;
int lowthresh=20;
vector<Mat>imgs;
Mat cannyOut(Mat image){
  Mat cannyout;
  if(lowthresh==0){
    lowthresh=10;
  }
  Canny(image, cannyout, lowthresh, 0.5*lowthresh, 3);
  return cannyout;
}
int main(int argc, char* argv[])
{
    VideoCapture cap;
    cap=VideoCapture(argv[1]); // open the default camera
    //if(!cap.isOpened())  // check if we succeeded
      //  return -1;
    Mat edges;
    //namedWindow("video",WINDOW_NORMAL);
    for(;;)
    {

        Mat frame;
        if(!cap.read(frame)){
          cout<<"End of Video"<<endl;
          break;
        }
        cap >> frame; // get a new frame from camera
        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //createTrackbar("Canny","edges",&lowthresh,100);
        //edges=cannyOut(edges);
        imgs.push_back(frame);


        //frame -> edges
        //imshow("video", frame);
        //waitKey(1);

    }
    // Define object to store the stitched image
    Mat pano;
    Stitcher::Mode mode = Stitcher::SCANS ;
    // Create a Stitcher class object with mode panoroma
    Ptr<Stitcher> stitcher = Stitcher::create(mode, false);
    cout<<"Stitcher created"<<endl;
    // Command to stitch all the images present in the image array
    Stitcher::Status status = stitcher->stitch(imgs, pano);
    cout<<"Stiched"<<endl;
    if (status != Stitcher::OK)
    {
        // Check if images could not be stiched
        // status is OK if images are stiched successfully
        cout << "Can't stitch images\n";
        return -1;
    }

    // Store a new image stiched from the given
    //set of images as "result.jpg"
    imwrite("result.jpg", pano);

    // Show the result
    namedWindow("Result",WINDOW_NORMAL);
    imshow("Result", pano);

    waitKey(0);
    return 0;
}
