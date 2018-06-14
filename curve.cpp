#include<iostream>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<math.h>

using namespace std;
using namespace cv;

Mat input;
int clickCount=0;
vector<Point2f> clickedPoints,drawnPoints;
bool isFx=true;
float a,b,c;

/*float dist(Point2f p,Point2f q){
  float d = sqrt((q.x-p.x)*(q.x-p.x)+(q.y-p.y)*(q.y-p.y));
  return d;
}*/
float getSlope(){

  float instSlope =0;
  /*if(drawnPoints.size()<2){
    instSlope=0;
  }*/
  if(drawnPoints.size()<2){}
  else{
    instSlope = (drawnPoints[drawnPoints.size()-1].y-drawnPoints[drawnPoints.size()-2].y)/(drawnPoints[drawnPoints.size()-1].x-drawnPoints[drawnPoints.size()-2].x);
  }
  return instSlope;
}

void determineFunc(float slope,Point2f p1,Point2f p2 ){
  float perpSlope = -1/slope;
  if((p2.y-p1.y-slope*(p2.x-p1.x))*(p2.y-p1.y-perpSlope*(p2.x-p1.x))<0){ //change if wierd curve comes
    isFx=true;
  }
  else{
    isFx=false;
  }
}

void findConstx(float slope,Point2f p1,Point2f p2){
  a = (p2.y-p1.y)/((p2.x-p1.x)*(p2.x-p1.x)) - slope/(p2.x-p1.x);
  b = slope - 2*a*p1.x;
  c = p1.y - a*p1.x*p1.x - b*p1.x;
  float c2;
  c2 = p2.y - a*p2.x*p2.x - b*p2.x;
  /*if(c2!=c){
    cout<<"c values not same"<<endl;
  }
  cout<<"a"<<a<<endl<<"b"<<b<<endl<<"c"<<c<<endl;*/
}

void findConsty(float slope,Point2f p1,Point2f p2){
  a = (p2.x-p1.x)/((p2.y-p1.y)*(p2.y-p1.y)) - 1/(slope*(p2.y-p1.y));
  b = 1/slope - 2*a*p1.y;
  c = p1.x - a*p1.y*p1.y - b*p1.y;
  float c2;
  //c = p2.x - a*p2.y*p2.y - b*p2.y;
}

void drawContiArc(){
  float slope = getSlope();
  Point2f p1 = clickedPoints[clickedPoints.size()-2];
  Point2f p2 = clickedPoints[clickedPoints.size()-1];
  determineFunc(slope,p1,p2 );
  if(isFx==true){
    //y=a*x*x + b*x + c
    cout<<"Horizontal curve"<<endl;
    float xprev=p1.x;
    float xfinal=p2.x;
    /*if(p2.x<p1.x){
      xprev=p2.x;
      xfinal=p1.x;
    }*/
    findConstx(slope,p1,p2);
    //Draw curve
    if(xprev<xfinal){
    for(int i=xprev;i<=xfinal;i++){
      drawnPoints.push_back(Point2f(i,a*i*i+b*i+c));
      line(input,drawnPoints[drawnPoints.size()-1],drawnPoints[drawnPoints.size()-2],Scalar(255,0,0),1,8,0);
    }
  }
    else{
      for(int i=xprev;i>=xfinal;i--){
        drawnPoints.push_back(Point2f(i,a*i*i+b*i+c));
        line(input,drawnPoints[drawnPoints.size()-1],drawnPoints[drawnPoints.size()-2],Scalar(255,0,0),1,8,0);
      }
    }
  }
  else if(isFx==false){
    //x=a*y*y + b*y + c
    cout<<"Vertical curve"<<endl;

    float yprev=p1.y;
    float yfinal=p2.y;

    findConsty(slope,p1,p2);
    //Draw curve
    if(yprev<yfinal){
    for(int i=yprev;i<=yfinal;i++){
      drawnPoints.push_back(Point2f(a*i*i+b*i+c,i));
      line(input,drawnPoints[drawnPoints.size()-1],drawnPoints[drawnPoints.size()-2],Scalar(255,0,0),1,8,0);
    }
  }
  else{
    for(int i=yprev;i>=yfinal;i--){
      drawnPoints.push_back(Point2f(a*i*i+b*i+c,i));
      line(input,drawnPoints[drawnPoints.size()-1],drawnPoints[drawnPoints.size()-2],Scalar(255,0,0),1,8,0);
    }
  }
  }
  else{
    cout<<"None of them"<<endl;
  }
}

void drawCircArc(Point2f a,Point2f b,Point2f c){
  float slope1 = (b.y-a.y)/(b.x-a.x);
  cout<<"slope1: "<<slope1<<endl;
  float slope2 = (b.y-c.y)/(b.x-c.x);
  float m1=-1/slope1;
  float m2=-1/slope2;
  Point2f p1 = a/2+b/2;
  Point2f p2 = b/2+c/2;
  Point2f centre;
  centre.x = -(p1.y-p2.y + (m2*p2.x-m1*p1.x));
  centre.y = -(p1.y+m1*(centre.x-p1.x));
  cout<<centre<<endl;
  circle(input,centre,3,Scalar(0,0,255),-1,8,0);

}

void onMouse(int event,int x,int y,int flags,void* param){
  //refreshingInput=input.clone();
  if(event==EVENT_LBUTTONDOWN){
    //cout<<"lbutton"<<endl;
    /*if(clickedPoints[0]==Point2f(0,0)){
      clickedPoints.clear();
      drawnPoints.clear();
    }*/
    circle(input,Point2f(x,y),3,Scalar(255,0,0),-1,8,0);
    clickCount++;
    clickedPoints.push_back(Point2f(x,y));
    if(clickCount>=3){
          drawContiArc();
          //cout<<"Line length: "<<dist(clickedPoints[clickedPoints.size()-1],clickedPoints[clickedPoints.size()-2])<<endl;
        }
    else if((clickCount==2)){
      //clickedPoints.push_back(Point2f(x,y));
      //clickedPoints.push_back(Point2f(100,100));
      //drawCircArc(clickedPoints[0],clickedPoints[1],clickedPoints[2]);
      drawnPoints.push_back(clickedPoints[clickedPoints.size()-2]);
      drawnPoints.push_back(clickedPoints[clickedPoints.size()-1]);
      line(input,clickedPoints[clickedPoints.size()-1],clickedPoints[clickedPoints.size()-2],Scalar(255,0,0),1,8,0);
    }
  }
  if(event==EVENT_RBUTTONDOWN){
    clickedPoints.clear();
    clickCount=0;
    drawnPoints.clear();    
  }
  imshow("input",input);
}

int main(int argc,char* argv[]){
  if(argc<2){
    cout<<"Enter image address too"<<endl;
    return 0;
  }
  else{
    input=imread(argv[1]);
  }
  //clickedPoints.push_back(Point2f(0,0));
  //clickedPoints.push_back(Point2f(100,100));
  namedWindow("input",WINDOW_NORMAL);
  setMouseCallback("input",onMouse);
  imshow("input",input);
  waitKey(0);
  return 0;
}
