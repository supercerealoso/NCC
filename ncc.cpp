#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

int main( int argc, char** argv ) {
	IplImage *buf_f = NULL;
	IplImage *buf_t = NULL;

	buf_f = cvLoadImage("f.jpg",-1);
	buf_t = cvLoadImage("t.jpg",-1);
	cvShowImage("f", buf_f);
	cvShowImage("t", buf_t);

	int mx = (int)buf_f->width;
	int my = (int)buf_f->height;
	int nx = (int)buf_t->width;
	int ny = (int)buf_t->height;

	double gamma[mx - nx][my - ny];

	int x = 0;
	int y = 0;
	int u = 0;
	int v = 0;

	double tbar = 1 / (double)(nx * ny);
	double sumtxy = 0;
	for (x = 0; x < nx; x++) {
		for (y = 0; y < ny; y++) {
			double txy = cvGetReal2D(buf_t, y, x);
			sumtxy += txy;
		}
	}
	tbar *= sumtxy;

	double maxGamma = -999999;
	int upos = 0;
	int vpos = 0;
	for (u = 0; u < mx - nx; u++) {
		for (v = 0; v < my - ny; v++) {
			double fbar = 1 / (double)(nx * ny);
			double sumfxy = 0;
			for (x = u; x < u + nx; x++) {
				for (y = v; y < v + ny; y++) {
					double fxy = cvGetReal2D(buf_f, y, x);
					sumfxy += fxy;
				}
			}
			fbar *= sumfxy;

			double sumft = 0;
			double sumf2 = 0;
			double sumt2 = 0;
			for (x = u; x < u + nx; x++) {
				for (y = v; y < v + ny; y++) {
					double fxy = cvGetReal2D(buf_f, y, x);
					double txy = cvGetReal2D(buf_t, y - v, x - u);

					double fxyerr = fxy - fbar;
					double txyerr = txy - tbar;

					sumft += fxyerr * txyerr;
					sumf2 += fxyerr * fxyerr;
					sumt2 += txyerr * txyerr;
				}
			}
			
			gamma[u][v] = sumft / sqrt(sumf2 * sumt2);
			if (gamma[u][v] > maxGamma) {
				maxGamma = gamma[u][v];
				upos = u;
				vpos = v;
			}
		}
	}

	printf("maxGamma: %f, u: %d , v: %d \n", maxGamma, upos, vpos);
	cvRectangle(buf_f, cvPoint(upos, vpos), cvPoint(upos + nx, vpos + ny), CV_RGB(0,0,0), 1);
	cvShowImage("f", buf_f);

	waitKey(0);
	return 0;
}
