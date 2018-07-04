#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

Mat imagenOriginal, imagen, imagenMostrar;

int zoomRec = 200;
int mousex, mousey;

Mat zoomIn(int x, int y)
{
	int width = zoomRec, height = zoomRec;
	int ptoX = x-(zoomRec/2), ptoY = y-(zoomRec/2);

	/*Verifica que el ROI este dentro de la la imagen*/
	if((x+(zoomRec/2)) > imagen.size().width)
		width = width - ((x+(zoomRec/2)) - imagen.size().width);

	if((y+(zoomRec/2)) > imagen.size().height)
		height = height - ((y+(zoomRec/2)) - imagen.size().height);

	if((x-(zoomRec/2)) < 0)
		ptoX = 0;

	if((y-(zoomRec/2)) < 0)
		ptoY = 0;

	Rect roi = Rect(ptoX, ptoY, width, height);
	Mat imagen_roi = imagen(roi);
	resize(imagen_roi, imagen_roi, Size(imagenOriginal.size().width, imagenOriginal.size().height), 0, 0, CV_INTER_AREA);

	return imagen_roi;
}

Mat zoomOut(int x, int y)
{
	return imagenOriginal;
}

static void onMouse( int event, int x, int y, int /*flags*/, void* /*param*/ )
{
	mousex = x;
	mousey = y;

    if(event == CV_EVENT_LBUTTONDOWN)
		imagen = zoomIn(x, y);
	else if(event == CV_EVENT_RBUTTONDOWN)
		imagen = zoomOut(x, y);
}

void info(){

    cout << "\nKeys: \n"
            "\tESC : Exit\n"
            "\t+ : Increase the zoom scale\n"
            "\t- : Decrease the zoom scale\n"
            "\tCLICK D : zoom\n"
            "\tCLICK R : Restore image\n" << endl;
}

int main( int argc, char** argv )
{
	info();

	if(argc == 1)
        imagenOriginal = imread("lena.jpg");
	else if(argc > 1)
		imagenOriginal = imread(argv[1]);

	namedWindow("Zoom", 1);
	setMouseCallback("Zoom", onMouse, 0);

	imagenOriginal.copyTo(imagen);
	imagenOriginal.copyTo(imagenMostrar);

	for(;;)
	{
		if(imagen.empty())
			break;

		imagen.copyTo(imagenMostrar);

		rectangle(imagenMostrar,
					Point(mousex-(zoomRec/2), mousey-(zoomRec/2)),
					Point(mousex+(zoomRec/2), mousey+(zoomRec/2)),
					cv::Scalar(0,255,0), 1, 8, 0);

		imshow("Zoom", imagenMostrar);

		char c = (char) waitKey(10);
		if(c == 27)
			break;
		switch(c)
        {
        case '+':
			if(zoomRec < 500)
            zoomRec = zoomRec + 10;
			break;
		case '-':
			if(zoomRec > 10)
	            zoomRec = zoomRec - 10;
			break;
        default:
            ;
        }
	}

	return 0;
}
