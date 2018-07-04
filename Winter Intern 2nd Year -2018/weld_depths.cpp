#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<fstream>
#include<math.h>

using namespace std;
using namespace cv;

Mat inputimage,altimage;
vector<Point2f>L1pts;
vector<Point2f>L2pts;
vector<Point2f>contourPt1;
vector<Point2f>contourPt2;
vector<Point2f>contourPt3;
vector<Point2f>contourPt4;
Point2f intersectionofLines;
int activeclicksdone=0;
int curvesDrawn=0;
int contourSelectionOn=0;
Point2f p1,p2,p3,p4;
int thickness=2;

void putDistance(float r,Point2f edge,Point2f centre){
  char DistanceValue[1000];
  sprintf(DistanceValue, "Distance: %f", r);
  Point2f text_appear_point; //For drag option to be available for angle value change this point
  text_appear_point.x = edge.x/2 + centre.x/2;
  text_appear_point.y = edge.y/2 + centre.y/2 + 20;
  Point2f rectCorner = text_appear_point;
  //drawRect(text_appear_point);
  int baseline = 0;
  Size textSize = getTextSize(DistanceValue, CV_FONT_HERSHEY_SIMPLEX,0.5, 1, &baseline);
  rectangle(altimage,rectCorner + Point2f(-2, 2),rectCorner + Point2f(textSize.width, -textSize.height)+Point2f(2,-2),Scalar(100,0,0),-1,8,0);
  putText(altimage,DistanceValue,text_appear_point,CV_FONT_HERSHEY_SIMPLEX,0.5,Scalar(255,255,255),1,8,false);
}

float dist(Point2f p,Point2f q){
  float d= sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
  return d;
}

void pttocurve(Point2f p,vector<Point2f>c){
  float maximumDistance=0;
  float distanceofPttocurve;
  Point2f maxDistPt;
  for(int i=0;i<c.size();i++){
    distanceofPttocurve=dist(intersectionofLines,c[i]);
    if(distanceofPttocurve>maximumDistance){
      maximumDistance=distanceofPttocurve;
      maxDistPt=c[i];
    }
  }
  circle(altimage,maxDistPt,2,Scalar(0,0,0),-1,8,0);
  line(altimage,maxDistPt,intersectionofLines,Scalar(0,0,0),thickness,8,0);
  putDistance(maximumDistance,maxDistPt,intersectionofLines);
  cout<<"Max point dist: "<<maximumDistance<<endl;
}

void linetoCurve(vector<Point2f>l,vector<Point2f>c){

  Point2f l1,l2,c0;
  Point2f maximumDistancePt;
  l1.x=intersectionofLines.x;
  l1.y=intersectionofLines.y;
  l2.x=l[l.size()-1].x;
  l2.y=l[l.size()-1].y;
  /*float x1=l[0].x;
  float x2=l[l.size()-1].x;
  float y1=l[0].y;
  float y2=l[l.size()-1].y;*/
  float maximumDistance=0;
  for(int i=0;i<c.size();i++){
    c0.x=c[i].x;
    c0.y=c[i].y;
    float s=(dist(l1,l2)+dist(l1,c0)+dist(l2,c0))/2;
    float area = sqrt(s*(s-dist(l1,l2))*(s-dist(l1,c0))*(s-dist(c0,l2)));
    float distanceofcurvePttoLine = 2*area/dist(l1,l2);
    if(distanceofcurvePttoLine > maximumDistance){
      maximumDistancePt.x=c0.x;
      maximumDistancePt.y=c0.y;
      maximumDistance=distanceofcurvePttoLine;
    }
  }
  if(maximumDistance>3){
    float dl=sqrt(dist(l1,maximumDistancePt)*dist(l1,maximumDistancePt)-maximumDistance*maximumDistance);
    Point2f perponLine1,perponLine2,perponLine;
      perponLine1.y=l1.y+dl*(l2.y-l1.y)/dist(l1,l2);
      perponLine1.x=l1.x+dl*(l2.x-l1.x)/dist(l1,l2);
      perponLine2.x=l1.x-dl*(l2.x-l1.x)/dist(l1,l2);
      perponLine2.y=l1.y-dl*(l2.y-l1.y)/dist(l1,l2);
    if(dist(perponLine1,maximumDistancePt)<dist(perponLine2,maximumDistancePt)){
      perponLine.x=perponLine1.x;
      perponLine.y=perponLine1.y;
    }
    else{
      perponLine.x=perponLine2.x;
      perponLine.y=perponLine2.y;
    }
    cout<<"maximum distance: "<<maximumDistance<<endl;
    circle(altimage,maximumDistancePt,3,Scalar(200,0,0),-1,8,0);
    circle(altimage,perponLine,3,Scalar(200,0,0),-1,8,0);
    line(altimage,perponLine,maximumDistancePt,Scalar(0,0,0),thickness,8,0);
    putDistance(maximumDistance,maximumDistancePt,perponLine);
    //char* Distance;
    //sprintf(Distance,"Distance: %f",maximumDistance);
    //putText(altimage,Distance,perponLine/*Point2f(perponLine.x/2+maximumDistancePt.x/2,perponLine.y/2+maximumDistancePt.y/2)*/,FONT_HERSHEY_SIMPLEX,0.5,Scalar(255,255,255),1,8,0);
  }
}

void findInterSection(){
  float x1=p1.x;
  float x2=p2.x;
  float x3=p3.x;
  float x4=p4.x;
  float y1=p1.y;
  float y2=p2.y;
  float y3=p3.y;
  float y4=p4.y;
  intersectionofLines.x=(float)(((((x3)*(y4-y3)/(x4-x3))-((x1)*(y2-y1)/(x2-x1)))+(y1-y3))/(((y4-y3)/(x4-x3))-((y2-y1)/(x2-x1))));
  intersectionofLines.y=(float)(y1+(((y2-y1)/(x2-x1))*(intersectionofLines.x-x1)));
  //cout<<"Intersection"<<intersectionofLines<<endl;
  circle(altimage,intersectionofLines,3,Scalar(0,0,0),-1,8,0);
  }

void drawCurvedContour(Point2f mouseAt){
  if(activeclicksdone==1){
    contourPt1.push_back(mouseAt);
  if(contourPt1.size()>=2){
      line(altimage,contourPt1[contourPt1.size()-1],contourPt1[contourPt1.size()-2],Scalar(200,0,200),thickness,8,0);
    }
  }
  else if(activeclicksdone==2){
    contourPt2.push_back(mouseAt);
  if(contourPt2.size()>=2){
      line(altimage,contourPt2[contourPt2.size()-1],contourPt2[contourPt2.size()-2],Scalar(0,200,200),thickness,8,0);
    }
  }
  else if(activeclicksdone==3){
    contourPt3.push_back(mouseAt);
  if(contourPt3.size()>=2){
      line(altimage,contourPt3[contourPt3.size()-1],contourPt3[contourPt3.size()-2],Scalar(0,200,0),thickness,8,0);
    }
  }
  else if(activeclicksdone==4){
    contourPt4.push_back(mouseAt);
  if(contourPt4.size()>=2){
      line(altimage,contourPt4[contourPt4.size()-1],contourPt4[contourPt4.size()-2],Scalar(200,200,0),thickness,8,0);
    }
  }
}

void drawL1(Point2f p,Point2f q){
  for(float t=0;t<=1;t+=0.001){
    Point2f newlinept;
    newlinept.x=p.x+t*(q.x-p.x);
    newlinept.y=p.y+t*(q.y-p.y);
    L1pts.push_back(newlinept);
  }
  line(altimage,p,q,Scalar(0,0,200),thickness,8,0);
}

void drawL2(Point2f p,Point2f q){
  for(float t=0;t<=1;t+=0.001){
    Point2f newlinept;
    newlinept.x=p.x+t*(q.x-p.x);
    newlinept.y=p.y+t*(q.y-p.y);
    L2pts.push_back(newlinept);
  }
  line(altimage,p,q,Scalar(0,0,200),thickness,8,0);
}

void mousecallback(int event,int x, int y,int flags,void* param){
  if(event==EVENT_LBUTTONDOWN){
    circle(altimage,Point2f(x,y),2,Scalar(0,0,0),-1,8,0);
    if(L1pts.size()==0){
      if(activeclicksdone==0){
        p1.x=x;
        p1.y=y;
        circle(altimage,p1,2,Scalar(0,0,0),-1,8,0);
        activeclicksdone=1;
      }
      else if(activeclicksdone==1){
        p2.x=x;
        p2.y=y;
        circle(altimage,p2,2,Scalar(0,0,0),-1,8,0);
        activeclicksdone=0;
        drawL1(p1,p2);
      }
    }
    else if(L2pts.size()==0){
      if(activeclicksdone==0){
        p3.x=x;
        p3.y=y;
        circle(altimage,p3,2,Scalar(0,0,0),-1,8,0);
        activeclicksdone=1;
      }
      else if(activeclicksdone==1){
        p4.x=x;
        p4.y=y;
        circle(altimage,p4,2,Scalar(0,0,0),-1,8,0);
        activeclicksdone=0;
        drawL2(p3,p4);
        findInterSection();
      }
    }
    //contour select on mouse move
    else if((L1pts.size()!=0)&&(L2pts.size()!=0)){
      if((activeclicksdone==0)&&(contourPt1.size()==0)){
      contourSelectionOn=1;
      activeclicksdone=1;
      //cout<<"contourSelectionOn= 1"<<endl;
    }
    else if((activeclicksdone==1)/*&&(contourPt2.size()==0)*/){
    contourSelectionOn=1;
    activeclicksdone=2;
    //cout<<"contourSelectionOn= 1"<<endl;
  }
  else if((activeclicksdone==2)/*&&(contourPt3.size()==0)*/){
  contourSelectionOn=1;
  activeclicksdone=3;
  //cout<<"contourSelectionOn= 1"<<endl;
}
else if((activeclicksdone==3)/*&&(contourPt3.size()==0)*/){
contourSelectionOn=1;
activeclicksdone=4;
//cout<<"contourSelectionOn= 1"<<endl;
}
    else if(activeclicksdone==4){
    contourSelectionOn=0;
    activeclicksdone=0;
    //cout<<"contourSelectionOn= 0  "<<endl;
    linetoCurve(L2pts,contourPt1);
    linetoCurve(L1pts,contourPt3);
    linetoCurve(L1pts,contourPt1);
    linetoCurve(L2pts,contourPt3);
    pttocurve(intersectionofLines,contourPt2);
    pttocurve(intersectionofLines,contourPt4);
  }
    }
  }
  else if(event==EVENT_MOUSEMOVE){
    if(contourSelectionOn==1){
      Point2f c;
      c.x=x;
      c.y=y;
      drawCurvedContour(c);
    }
  }
  imshow("AltImage",altimage);
}

int main(int argc,char* argv[]){
  if(argc>1){
    inputimage=imread(argv[1]);
  }
  else cout<<"Enter image address"<<endl;
  L1pts.clear();
  L2pts.clear();
  altimage=inputimage.clone();
  imshow("AltImage",altimage);
  setMouseCallback("AltImage",mousecallback);
  waitKey(0);
  return 0;
}
