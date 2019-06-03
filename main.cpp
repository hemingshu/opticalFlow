#include<iostream>
#include <fstream>  
#include <iostream>  
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> 
#include "opencv2/video/tracking.hpp"

using namespace std;  
using namespace cv;


const int width = 352;  
const int height = 288;  
const int framesize = width * height * 3 / 2; 
typedef struct planet
{
	char name[framesize];
	double population;
	double g;
} PLANET;
int countframe(string file);
void opticalflow(Mat Img, Mat ImgRef);
int main(){
	string file = "football_352x288_30.yuv";
	int frames = countframe(file);
	const char* filename = nullptr;
	filename = file.c_str();

	FILE* fileIn = fopen(filename, "rb+");
	unsigned char* pYuvBuf = new unsigned char[framesize];

	Mat rgbImg0;
	Mat yuvImg0;
	fread(pYuvBuf, framesize*sizeof(unsigned char), 1, fileIn);
	yuvImg0.create(height * 3 / 2, width, CV_8UC1);
	memcpy(yuvImg0.data, pYuvBuf, framesize*sizeof(unsigned char));
	cvtColor(yuvImg0, rgbImg0, CV_YUV2BGR_I420);

	for (int i = 1; i < frames; ++i)
	{
		fread(pYuvBuf, framesize*sizeof(unsigned char), 1, fileIn);       
		Mat yuvImg;
		yuvImg.create(height * 3 / 2, width, CV_8UC1); 
		memcpy(yuvImg.data, pYuvBuf, framesize*sizeof(unsigned char));   
		Mat rgbImg;      
		cvtColor(yuvImg, rgbImg, CV_YUV2BGR_I420);
		opticalflow(rgbImg, rgbImg0);
		printf("frame:%d\n", i);

	    rgbImg0 = rgbImg;		
	}
	fclose(fileIn);
}
int countframe(string file)
{
	PLANET pl;      
	ifstream fin;   
	fin.open(file, ios_base::in | ios_base::binary);
	if (fin.fail())      
	{ 
		cout << "the file is error" << endl;          
		exit(-1);
	}    
	fin.seekg(0, ios::end);       
	streampos ps = fin.tellg();     
	unsigned long NumberPixe = ps;         
	unsigned FrameCount = ps / framesize;        
	fin.close(); 
	return FrameCount;
	
}
void opticalflow(Mat Img, Mat ImgRef)
{
	Mat grayImg, grayImgRef;
	cvtColor(Img, grayImg, CV_RGB2GRAY);
	cvtColor(ImgRef, grayImgRef, CV_RGB2GRAY);
	Mat flow;
	calcOpticalFlowFarneback(grayImg, grayImgRef, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
	cout << flow.size() << endl;

	for (size_t y = 0; y<Img.rows; y += 8){
		for (size_t x = 0; x<Img.cols; x += 8){
			Point2f fxy = flow.at<Point2f>(y, x);
			if (fxy.x != 0 || fxy.y != 0)
				line(Img, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), CV_RGB(0, 255, 0), 1, 8);
			  

		}
	}


	imshow("rgbImg", Img);
	int c = waitKey(30);
	if ((char)c == 27)
	{
		return;
	}
}
