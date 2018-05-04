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
bool contineousSelection =false;
bool isL1selected=false;
bool isL2selected=false;

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

void linetoCurve2(vector<Point2f>l,vector<Point2f>c){
  float perpSlope;
  Point2f farPoint;
  //Point2f nearPoint;
  if(dist(l[0],intersectionofLines)>dist(l[l.size()-1],intersectionofLines)){
    perpSlope = ((intersectionofLines.y-l[0].y)/(intersectionofLines.x-l[0].x));
    farPoint=l[0];
  }
  else {
    perpSlope = ((intersectionofLines.y-l[l.size()-1].y)/(intersectionofLines.x-l[l.size()-1].x));
    farPoint=l[l.size()-1];
  }
  float slope = -1/perpSlope;
  if(perpSlope<0.02){
    slope=99999;
  }
  Point2f maximumDistancePtline,maximumDistancePtCurve;
  float maximumDistance=0;
  cout<<"Slope of line to curve: "<<slope<<endl;
  for(int j=0;j<l.size();j++){
    for(int i=0;i<c.size();i++){
      float inst_slope=((l[j].y-c[i].y)/(l[j].x-c[i].x));
      if(slope!=99999){
        if(fabs(inst_slope-slope)<0.01){
        //m++;
        //cout<<m<<endl;
        if((dist(l[j],farPoint))<dist(intersectionofLines,farPoint)){
          //cout<<"inst_slope-slope: "<<inst_slope-slope<<endl;
          float inst_dist= dist(l[j],c[i]);
          if(inst_dist>maximumDistance){
            maximumDistancePtline=l[j];
            maximumDistancePtCurve=c[i];
            maximumDistance=inst_dist;
        }
      }
        //break;
        //break;
      }
      }
      else{
        if(fabs(c[i].x-l[j].x)<2){
          float inst_dist=dist(l[j],c[i]);
          if(inst_dist>maximumDistance){
            maximumDistance=inst_dist;
            maximumDistancePtline=l[j];
            maximumDistancePtCurve=c[i];
          }
        }

      }
    }
  }
  cout<<"maximum distance: "<<maximumDistance<<endl;
  circle(altimage,maximumDistancePtline,3,Scalar(200,0,0),-1,8,0);
  circle(altimage,maximumDistancePtCurve,3,Scalar(200,0,0),-1,8,0);
  line(altimage,maximumDistancePtline,maximumDistancePtCurve,Scalar(0,0,0),thickness,8,0);
  putDistance(maximumDistance,maximumDistancePtline,maximumDistancePtCurve);
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
  //intersectionofLines.x=(float)(((((x3)*(y4-y3)/(x4-x3))-((x1)*(y2-y1)/(x2-x1)))+(y1-y3))/(((y4-y3)/(x4-x3))-((y2-y1)/(x2-x1))));
  //intersectionofLines.y=(float)(y1+(((y2-y1)/(x2-x1))*(intersectionofLines.x-x1)));
  //cout<<"Intersection"<<intersectionofLines<<endl;
  for(int i=0;i<L1pts.size();i++){
    for(int j=0;j<L2pts.size();j++){
      if(dist(L1pts[i],L2pts[j])<1){
        intersectionofLines = L1pts[i];
        break;
      }
    }
  }
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
    if(isL1selected==false){
      if(activeclicksdone==0){
        p1.x=x;
        p1.y=y;
        circle(altimage,p1,2,Scalar(0,0,0),-1,8,0);
        activeclicksdone=1;
        if(contineousSelection==true){
          drawL1(p2,p1);
        }
      }
      else if(activeclicksdone==1){
        p2.x=x;
        p2.y=y;
        circle(altimage,p2,2,Scalar(0,0,0),-1,8,0);
        contineousSelection=true;
        activeclicksdone=0;
        drawL1(p1,p2);
      }
    }
    else if(isL2selected==false){
      if(activeclicksdone==0){
        p3.x=x;
        p3.y=y;
        circle(altimage,p3,2,Scalar(0,0,0),-1,8,0);
        activeclicksdone=1;
        if(contineousSelection==true){
          drawL2(p4,p3);
        }
      }
      else if(activeclicksdone==1){
        p4.x=x;
        p4.y=y;
        circle(altimage,p4,2,Scalar(0,0,0),-1,8,0);
        activeclicksdone=0;
        contineousSelection=true;
        drawL2(p3,p4);
      }
    }
    //contour select on mouse move
    else if((isL1selected==true)&&(isL2selected==true)){
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
    line(altimage,contourPt1[0],contourPt3[contourPt3.size()-1],Scalar(0,0,255),2,8,0);
    contourSelectionOn=1;
    activeclicksdone=4;
    //cout<<"contourSelectionOn= 1"<<endl;
  }
    else if(activeclicksdone==4){
    contourSelectionOn=0;
    activeclicksdone=0;
    float perpSlope = (contourPt3[contourPt3.size()-1].y-contourPt1[0].y)/(contourPt3[contourPt3.size()-1].x-contourPt1[0].x);
    float slope = -1/perpSlope;
    cout<<"slope: "<<slope<<endl;
    Point2f C4perpPoint;
    for(int i=0;i<contourPt4.size();i++){
      float instantaneous_slope = ((contourPt4[i].y-intersectionofLines.y)/(contourPt4[i].x-intersectionofLines.x));
      if(fabs(instantaneous_slope-slope)<0.05){
        line(altimage,contourPt4[i],intersectionofLines,Scalar(0,0,0),thickness,8,0);
        //cout<<"called"<<endl;
        putDistance(dist(contourPt4[i],intersectionofLines),intersectionofLines,contourPt4[i]);
        break;
      }
    }
    //cout<<"contourSelectionOn= 0  "<<endl;
    //linetoCurve(L2pts,contourPt1);
    //linetoCurve(L1pts,contourPt3);
    linetoCurve2(L1pts,contourPt1);
    linetoCurve2(L2pts,contourPt3);
    pttocurve(intersectionofLines,contourPt2);
    //pttocurve(intersectionofLines,contourPt4);
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
  if(waitKey(0)==13){
    isL1selected=true;
    activeclicksdone=0;
    contineousSelection=false;
  }
  if(waitKey(0)==13){
    isL2selected=true;
    contineousSelection=false;
    activeclicksdone=0;
    findInterSection();
  }


  waitKey(0);
  return 0;
}
