#include <opencv2/imgproc/imgproc.hpp>    
#include <opencv2/core/core.hpp>          
#include <opencv2/highgui/highgui.hpp>   
#include<math.h>
#include <iostream>
#include <ctime>

using namespace cv;
using namespace std;
float M_PI = 3.14159265358979323846f;
float faceTransform[6][2] =
{
	{ 0, 0 },
	{ M_PI / 2,0 },
	{ M_PI,0 },
	{ -M_PI / 2,0 },
	{ 0,-M_PI / 2 },
	{ 0,M_PI / 2 }
};


inline void createCubeMapFace(const Mat &in, Mat &face, int faceId = 0, const int width = -1, const int height = -1)
{

	float inWidth = in.cols;
	float inHeight = in.rows;

	Mat mapx(height, width, CV_32F);
	Mat mapy(height, width, CV_32F);

	const float an = sin(M_PI / 4);
	const float ak = cos(M_PI / 4);

	const float ftu = faceTransform[faceId][0];
	const float ftv = faceTransform[faceId][1];

	// For each point in the target image, 
	// calculate the corresponding source coordinates.         
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			// Map face pixel coordinates to [-1, 1] on plane      
			float nx = ((float)y / (float)height - 0.5f)*2.0*an;
			float ny = ((float)x / (float)width - 0.5f)*2.0*an;

			//nx *= 2;
			//ny *= 2;

			// Map [-1, 1] plane coord to [-an, an]                          
			// thats the coordinates in respect to a unit sphere 
			// that contains our box. 
			//nx *= an;
			//ny *= an;

			float u, v;

			// Project from plane to sphere surface.
			if (ftv == 0) {
				// Center faces
				u = atan2(nx, ak);
				v = atan2(ny * cos(u), ak);
				u += ftu;
			}
			else if (ftv > 0) {
				// Bottom face 
				float d = sqrt(nx * nx + ny * ny);
				v = M_PI / 2 - atan2(d, ak);
				u = atan2(ny, nx);
			}
			else {
				// Top face
				//cout << "aaa";
				float d = sqrt(nx * nx + ny * ny);
				v = -M_PI / 2 + atan2(d, ak);
				u = atan2(-ny, nx);
			}

			// Map from angular coordinates to [-1, 1], respectively.
			u = u / (M_PI);
			v = v / (M_PI / 2);

			// Warp around, if our coordinates are out of bounds. 
			while (v < -1) {
				v += 2;
				u += 1;
			}
			while (v > 1) {
				v -= 2;
				u += 1;
			}

			while (u < -1) {
				u += 2;
			}
			while (u > 1) {
				u -= 2;
			}

			// Map from [-1, 1] to in texture space
			u = u / 2.0f + 0.5f;
			v = v / 2.0f + 0.5f;

			u = u*(inWidth - 1);
			v = v*(inHeight - 1);
			if (x == width / 2 && y == height / 2) { cout << u << "  " << v << endl; }
			mapx.at<float>(x, y) = u;
			mapy.at<float>(x, y) = v;

		}
	}

	// Recreate output image if it has wrong size or type. 
	if (face.cols != width || face.rows != height ||
		face.type() != in.type()) {
		face = Mat(width, height, in.type());
		/*  system("pause");*/
	}

	// Do actual  using OpenCV's remap
	remap(in, face, mapx, mapy, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));

	if (faceId == 0)
	{
		imwrite("./out/front.jpg", face);
	}
	else if (faceId == 1)
	{
		imwrite("./out/back.jpg", face);
	}
	else if (faceId == 2)
	{
		imwrite("./out/left.jpg", face);
	}
	else if (faceId == 3)
	{
		imwrite("./out/right.jpg", face);
	}
	else if (faceId == 4)
	{
		imwrite("./out/top.jpg", face);
	}
	else if (faceId == 5)
	{
		imwrite("./out/bottom.jpg", face);
	}
	/*  waitKey(10000);*/

}
double dur;
clock_t time_start, time_end;

int main()
{

	int faceId = 0;
	int width = 1000;
	int height = 1000;
	cv::Mat srcimage = cv::imread("wenti4.jpg");

	cv::Mat resultImage;
	//resultImage= srcimage(Rect(800, 350, srcimage.cols, srcimage.rows)); 
	time_start = clock();
	for (int i = 0; i < 6; i++) {
		createCubeMapFace(srcimage, resultImage, i, width, height);
	}
	time_end = clock();
	dur = (double)(time_end - time_start);
	printf("Use Time:%f\n", (dur / CLOCKS_PER_SEC));
}
