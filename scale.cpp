#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<stdlib.h>
#include<math.h>

using namespace std;
using namespace cv;

Mat inputimage;
Point2f maxP1,maxP2;
vector<Point2f>whitePts;
float perBarLen;

double median( cv::Mat channel )
    {
        double m = (channel.rows*channel.cols) / 2;
        int bin = 0;
        double med = -1.0;

        int histSize = 256;
        float range[] = { 0, 256 };
        const float* histRange = { range };
        bool uniform = true;
        bool accumulate = false;
        cv::Mat hist;
        cv::calcHist( &channel, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );

        for ( int i = 0; i < histSize && med < 0.0; ++i )
        {
            bin += cvRound( hist.at< float >( i ) );
            if ( bin > m && med < 0.0 )
                med = i;
        }

        return med;
    }

Mat correctLighting(Mat image){
	Mat bgr_image=image;
	cv::Mat lab_image;
  cv::cvtColor(bgr_image, lab_image, CV_BGR2Lab);

	std::vector<cv::Mat> lab_planes(3);
  cv::split(lab_image, lab_planes);

	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(4);
    cv::Mat dst;
    clahe->apply(lab_planes[0], dst);

		dst.copyTo(lab_planes[0]);
    cv::merge(lab_planes, lab_image);

		cv::Mat image_clahe;
   cv::cvtColor(lab_image, image_clahe, CV_Lab2BGR);

	 return image_clahe;

}

float max(float a,float b){
	if(a>b) return a;
	else return b;
}

float min(float a,float b){
	if(a>b) return b;
	else return a;
}

float dist(Point2f p,Point2f q){
	float d;
	d=sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
	return d;
}

int main(int argc,char* argv[]){
	if(argc>1){
		inputimage=imread(argv[1]);
	}
	int n;
	cout<<"if lines white press 1\nif lines black 0"<<endl;
	cin>>n;
	namedWindow("Input",WINDOW_NORMAL);
	imshow("Input",inputimage);
	Mat inputimage2=inputimage.clone();
	Mat grayinput,cannyout;
	vector<vector<Point> > contours;
	vector<Vec4i>hierarchy;
	namedWindow("Canny",WINDOW_NORMAL);
	namedWindow("Drawing",WINDOW_NORMAL);
	namedWindow("Drawing1",WINDOW_NORMAL);
	namedWindow("Gray",WINDOW_NORMAL);
	cvtColor(inputimage,grayinput,CV_BGR2GRAY);
	if(n==1){
	threshold(grayinput,grayinput,220,255,0);
	copyMakeBorder(grayinput,grayinput,10,10,50,50,BORDER_CONSTANT,0);
}
else if(n==0){
	threshold(grayinput,grayinput,120,255,1);
	copyMakeBorder(grayinput,grayinput,10,10,50,50,BORDER_CONSTANT,0);
}
	//adaptiveThreshold(grayinput, grayinput, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,11,0);
	float majority = (float)median(grayinput);
	//imshow("Medin",median);
	cout<<"Median: "<<majority<<endl;

	GaussianBlur(grayinput,grayinput,Size(3,3),0,0);
	imshow("Gray",grayinput);
	Canny(grayinput,cannyout,50,50*3,3);
	findContours(cannyout,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,Point(0,0) );
		Mat drawing = Mat::zeros(cannyout.size(),CV_8UC1);
	for(int i=0;i<contours.size();i++){
		drawContours(drawing,contours,i,Scalar(255),-1,8,hierarchy,0,Point());
		if((arcLength(contours[i],true)<70)||(contourArea(contours[i]))<50){
			drawContours(drawing,contours,i,Scalar(0),-1,8,hierarchy,0,Point());
		}
	}
	threshold(drawing,drawing,220,255,0);
	GaussianBlur(drawing,drawing,Size(3,3),0,0);
	contours.clear();
	hierarchy.clear();
	findContours(drawing,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,Point(0,0) );
	vector<vector<Point> >hull( contours.size() );
   for( int i = 0; i < contours.size(); i++ )
      {  convexHull( Mat(contours[i]), hull[i], false ); }
	Mat drawing2 = Mat::zeros(cannyout.size(),CV_8UC3);
	for(int i=0;i<contours.size();i++){
		drawContours(drawing2,contours,i,Scalar(255,255,255),-1,8,hierarchy,0,Point());
		drawContours(drawing2, hull, i, Scalar(100,0,0), 2, 8, vector<Vec4i>(), 0, Point() );
	}
	vector<Moments> mu(contours.size());
	for(int i=0;i<contours.size();i++){
		mu[i] = moments(hull[i],false);
	}
	vector<Point2f>mc(contours.size());
	for(int i=0;i<contours.size();i++){
		mc[i] = Point2f(mu[i].m10/mu[i].m00,mu[i].m01/mu[i].m00);
		circle(drawing2,mc[i],3,Scalar(0,0,255),-1,8,0);
	}
	float MaxDist=0;
	for(int i=0;i<contours.size();i++){
		for(int j=0;j<contours.size();j++){
			Point2f p1,p2;
			p1.x=mc[i].x;
			p1.y=mc[i].y;
			p2.x=mc[j].x;
			p2.y=mc[j].y;
			float distBtwPts;
			distBtwPts=dist(p1,p2);
			//cout<<"Distances "<<i<<","<<j<<" "<<distBtwPts<<endl;
			if(distBtwPts>MaxDist){
				MaxDist=distBtwPts;
				maxP1.x=p1.x;
				maxP1.y=p1.y;
				maxP2.x=p2.x;
				maxP2.y=p2.y;
			}

		}
	}
	imshow("Drawing1",drawing);
	perBarLen = MaxDist/(mc.size()-1);
	if(hull.size()<6){
		/*MaxDist=0;
		for(int i=0;i<contours.size();i++){
			for(int j=0;j<contours.size();j++){
				Point2f p;
				p.x=contours[i][j].x;
				p.y=contours[i][j].y;
				whitePts.push_back(p);
			}
		}
		for(int i=0;i<whitePts.size();i++){
			for(int j=0;j<whitePts.size();j++){
				float distBtwPts=dist(whitePts[i],whitePts[j]);
				if(distBtwPts>MaxDist){
					MaxDist=distBtwPts;
					maxP1.x=whitePts[i].x;
					maxP1.y=whitePts[i].y;
					maxP2.x=whitePts[j].x;
					maxP2.y=whitePts[j].y;
				}
			}
		}
		cout<<"Special Per bar length: "<<MaxDist/100<<endl;*/
		//cout<<"Hull<6"<<endl;
		vector<vector<Point> > contours_poly( contours.size() );
		  vector<Rect> boundRect( contours.size() );
			//vector<RotatedRect> minRect( contours.size() );

		for( size_t i = 0; i < contours.size(); i++ )
  {
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    //minEnclosingCircle( contours_poly[i], center[i], radius[i] );
		//rectangle( drawing2, boundRect[i].tl(), boundRect[i].br(), Scalar(0,0,255), 2, 8, 0 );
  }
	Point2f NearMost,FarMost;
	NearMost=boundRect[0].tl();
	FarMost=boundRect[0].br();
	float leastDist,maxDist;
	leastDist=dist(boundRect[0].tl(),Point2f(0,0));
	maxDist=dist(boundRect[0].br(),Point2f(0,0));
	for(int i=0;i<contours.size();i++){
		float currentDistMin=dist(boundRect[i].tl(),Point2f(0,0));
				if(currentDistMin<leastDist){
					leastDist=currentDistMin;
					NearMost=boundRect[i].tl();
				}
				float currentDistMax=dist(boundRect[i].br(),Point2f(0,0));
						if(currentDistMax>maxDist){
							maxDist=currentDistMax;
							FarMost=boundRect[i].br();
						}
	}
	float x1,x2,y1,y2;
	x1=min(FarMost.x,NearMost.x);
	x2=max(FarMost.x,NearMost.x);
	y1=min(FarMost.y,NearMost.y);
	y2=max(FarMost.y,NearMost.y);
	rectangle( drawing2, NearMost, FarMost, Scalar(255,0,255), 2, 8, 0 );
	perBarLen = fabs((x1-x2)/100);
	cout<<"Length 1: "<<perBarLen<<endl;
	if(fabs(x1-x2)<fabs(y1-y2)){
		perBarLen = fabs((y1-y2)/100);
		cout<<"Length 2: "<<perBarLen<<endl;
	}

	/*vector<RotatedRect> minRect( contours.size() );
	for(size_t i=0;i<contours.size();i++){
	Point2f rect_points[4]; minRect[i].points( rect_points );
       for( int j = 0; j < 4; j++ )
          line( drawing, rect_points[j], rect_points[(j+1)%4], Scalar(0,255,255), 1, 8 );
					cout<<"Line drawn"<<endl;
				}
				cout<<"hull<6"<<endl;
				imshow("Drawing1",drawing);*/


	}
	cout<<"Hull size; "<<hull.size()<<endl;
	line(drawing2,maxP1,maxP2,Scalar(0,255,0),1,8,0);
	cout<<"Maximum distance: "<<MaxDist<<endl;
	cout<<"Centre count: "<<mc.size()<<endl;
	cout<<"Per bar length in Pixel: "<<perBarLen<<endl;
	imshow("Canny",cannyout);
	imshow("Drawing",drawing2);
	waitKey(0);
	return 0;
}
