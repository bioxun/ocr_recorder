/*

*/
#ifndef __ORCSCREEN_H__
#define __ORCSCREEN_H__

#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/photo/photo.hpp"
//#include "opencv2/imgcodecs/imgcodecs_c.h"
#include <objidl.h>
#include <Gdiplus.h>
#include <vector>	   // std::vector
#include <fstream>
#include <string>
#include <regex>
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include "basic_funcs_for_ocr_recorder.hpp"

using namespace std;
using namespace cv;
using namespace Gdiplus;



#ifdef __cplusplus  
extern "C" {  
#endif
//void __declspec (dllexport) ocr_region(char*& text, int x, int y, int w, int h, HWND hWnd);
int __declspec (dllexport) ocr_region(char*& text, int x, int y, int w, int h, HWND hWnd, int HexColorTarget, int dist_colorTarget, int auto_color);
void __declspec (dllexport) saveshot(int x, int y, int w, int h, HWND hWnd, const char* fileOut);
void __declspec (dllexport) apiLoadLua(const char* LuaFile);
extern void __declspec (dllexport) destroyApi();
extern int __declspec (dllexport) loadTessData(const char* pathTessdataCh,const char* langCh, short PageSegMode=6);
#ifdef __cplusplus  
} 
#endif  


/** General variables */
//const char* LuaFile="ocr_recorder.lua";
//const char* LuaFile="dll_ocr_recorder.lua";
string LuaFile="";
int user_defined_dpi=100;
int PageSegMode=3;
int engine_oem=3;
int MaxCharNum=3000;
int modeScript=0;
int doDebug=0;
int doShow=0;
float alpha_contrast=1;
float beta_contrast=0;
float canny_thresh1=25;
float canny_thresh2=75;
int blockSize1=5;
int doBlur=0;
string lang="eng";
string path_tessdata="D:/ProgKits/msys2/mingw64/share/tessdata";
string whitelist="";
string blacklist="";
string ConfigFile="";
vector<int> VtargetLoc;
vector<float> Rrc_seekColor{0.375,0.25,0.25,0.5};
cv::Scalar colorTarget(255,255,255,50);
cv::Scalar colorFlood(255,255,255);
int dist_colorTarget=20;
int N_floodPointX=20;
int N_floodPointY=10;
int Is_DarkChar=0;
int preprocessType=1;
int doFlood1=1;
int doFlood2=1;
int flood_fillMode=2;
int flood_connect=4;
int colorFlood_lowDiff=5;
int colorFlood_upDiff=5;
string imgFile="";
int doDilate=0;
float r_near=0.1;
int auto_colorTarget=0;
int doLab=1;
cv::Scalar colorTargetSc(255,255,255);
int min_area=10;
int tmp=0;

/*---------------------------------------*/
//把HBITMAP型转成Mat型
Mat fun_HBitmapToMat(HBITMAP& _hBmp){
	BITMAP bmp;	
	GetObject(_hBmp,sizeof(BITMAP),&bmp);	
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8 ;   
//	printf("[channel=%d]\n",nChannels);
//	int nChannels = 3;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;	
	Mat v_mat;
	v_mat.create(cvSize(bmp.bmWidth,bmp.bmHeight), CV_MAKETYPE(CV_8U,nChannels));
	GetBitmapBits(_hBmp,bmp.bmHeight*bmp.bmWidth*nChannels,v_mat.data);  
	DeleteObject(_hBmp);
	return v_mat;
}


void saveshot(int x=0, int y=0, int w=0, int h=0, HWND hWnd=0x0, const char* fileOut=""){
	if (hWnd==0x0){
		hWnd = GetDesktopWindow();
	}
	HBITMAP hBmp=func_GdipPrintWindow(hWnd,x,y,w,h);
	Mat cvImg00=fun_HBitmapToMat(hBmp);
	Mat cvImg(cvImg00.rows, cvImg00.cols, CV_8UC3, Scalar(255, 255, 255));// Declaring a white matrix//
	cvtColor(cvImg00, cvImg, COLOR_BGRA2BGR, 3);
	printf("[%s]----fileout---\n",fileOut);
	if (string(fileOut).length()){
		imwrite(fileOut, cvImg);
	}
}


cv::Mat screenshot(int x=0, int y=0, int w=0, int h=0, HWND hWnd=0x0, const char* fileOut=""){
	if (hWnd==0x0){
		hWnd = GetDesktopWindow();
	}
	HBITMAP hBmp=func_GdipPrintWindow(hWnd,x,y,w,h);
	Mat cvImg00=fun_HBitmapToMat(hBmp);
	Mat cvImg(cvImg00.rows, cvImg00.cols, CV_8UC3, Scalar(255, 255, 255));// Declaring a white matrix//
	cvtColor(cvImg00, cvImg, COLOR_BGRA2BGR, 3);
	if (string(fileOut).length()){
		imwrite(fileOut, cvImg);
	}
	return cvImg;
}



/*  select area with similiar color, like the function of magic-stick in Photoshop
https://blog.csdn.net/cv_ronny/article/details/17438131  */
class colorDetect{
private:
	int minDist; //minium acceptable distance
//	Scalar target;//target color;	
	Vec3b target;//target color;
public:
	colorDetect();
	colorDetect(cv::Scalar color,int dist);
	colorDetect(cv::Vec3b color,int dist);
	void SetMinDistance(int dist);
	void SetTargetColor(uchar blue, uchar green, uchar red);
	void SetTargetColor(cv::Vec3b color);
	void SetTargetColor(cv::Scalar color);
	Mat process(const Mat image, int doLab=0, int ValPos=255); 
};

colorDetect::colorDetect(){
}


colorDetect::colorDetect(cv::Vec3b color,int dist){
	minDist=dist;
	target=color;
}

colorDetect::colorDetect(cv::Scalar color,int dist){
	SetMinDistance(dist);
	SetTargetColor(color[0], color[1], color[2]);
}

//colorDetect::colorDetect(){
//}

void colorDetect::SetTargetColor(uchar blue, uchar green, uchar red){
	target=cv::Vec3b(blue, green, red);
}

void colorDetect::SetTargetColor(cv::Vec3b color){
	target=color;
}

void colorDetect::SetTargetColor(cv::Scalar color){
	target=cv::Vec3b(color[0], color[1], color[2]);
}

void colorDetect::SetMinDistance(int dist){
	minDist=dist;
}

Mat colorDetect::process(const Mat image, int doLab, int ValPos){
	Mat img1=image.clone();
	Mat result(image.rows,image.cols,CV_8UC1);
	//将目标颜色由BGR转换为Lab
	Mat temp(1,1,CV_8UC3);
	temp.at<Vec3b>(0,0)=target;//创建了一张1*1的临时图像并用目标颜色填充
	if (doLab>0){	//将image转换为Lab格式存储在ImageLab中
		cvtColor(img1,img1,COLOR_BGR2Lab);
		cvtColor(temp,temp,COLOR_BGR2Lab);
	}
	Vec3b target1=temp.at<Vec3b>(0,0);//再从临时图像的Lab格式中取出目标颜色
 
	// 创建处理用的迭代器
	cv::Mat_<cv::Vec3b>::iterator it=img1.begin<cv::Vec3b>();
	cv::Mat_<cv::Vec3b>::iterator itend=img1.end<cv::Vec3b>();
	cv::Mat_<uchar>::iterator itout=result.begin<uchar>();
	while(it!=itend) {
		//两个颜色值之间距离的计算
		int dist=static_cast<int>(cv::norm<int,3>(Vec3i((*it)[0]-target1[0], (*it)[1]-target1[1], (*it)[2]-target1[2])));
		if(dist<minDist){
			(*itout)=ValPos;
		}else{
			(*itout)=0;
		}
		it++; itout++;
	}
	return result;
}



class myFloodSetting{
public:
	myFloodSetting(cv::Scalar,int,int,int,int);
	int ffillMode=2;
	int connectivity=4;
	int lo;
	int up;
	int flags;
	cv::Scalar colorFlood;
	cv::Scalar colorLow;
	cv::Scalar colorUp;
};

myFloodSetting::myFloodSetting(cv::Scalar colorF, int color_lowD, int color_upD, int fMode=2, int conn=4){
	colorFlood=colorF;
	ffillMode=fMode;
	connectivity=conn;
	int lo = ffillMode == 0 ? 0 : color_lowD;
	int up = ffillMode == 0 ? 0 : color_upD;
	int newMaskVal = 255;
	flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);
	colorLow=cv::Scalar(lo, lo, lo);
	colorUp=cv::Scalar(up, up, up);
}


/* --------launch tess api--------- */
class myPowerStation : public tesseract::TessBaseAPI{
public:
	myPowerStation();
//	myPowerStation(const char* settingFileCh);
	int LoadLua(const char* settingFileCh);
	ULONG_PTR gdiplusToken;
	~myPowerStation();
	const char* lua_file="";
};


myPowerStation::~myPowerStation(){
	End();
//	delete this;
}

int myPowerStation::LoadLua(const char* settingFileCh){
	imgFile="";
//	string settingFile(settingFileCh);
	lua_State *LuaP = luaL_newstate();
	luaL_openlibs(LuaP);
	if (luaL_loadfile(LuaP, settingFileCh) || lua_pcall(LuaP, 0, 0, 0)){
		printf("error %s\n", lua_tostring(LuaP, -1));
		return 1;
	}
	sub_get_lua_var<string>(lang, LuaP, "lang");	// target languge
	sub_get_lua_var<string>(path_tessdata, LuaP, "path_tessdata");	// 
	sub_get_lua_var<string>(whitelist, LuaP, "whitelist");	// 
	sub_get_lua_var<string>(blacklist, LuaP, "blacklist");	// 
	sub_get_lua_var<string>(ConfigFile, LuaP, "ConfigFile");	// 
	sub_get_lua_var<int>(user_defined_dpi, LuaP, "user_defined_dpi");
	sub_get_lua_var<int>(engine_oem, LuaP, "engine_oem");
	sub_get_lua_var<int>(PageSegMode, LuaP, "PageSegMode");
	sub_get_lua_var<int>(MaxCharNum, LuaP, "MaxCharNum");
	sub_get_lua_var<int>(modeScript, LuaP, "modeScript");
	sub_get_lua_var<int>(doDebug, LuaP, "doDebug");
	sub_get_lua_var<int>(doShow, LuaP, "doShow");
	sub_get_lua_var<float>(alpha_contrast, LuaP, "alpha_contrast");
	sub_get_lua_var<float>(beta_contrast, LuaP, "beta_contrast");
	sub_get_lua_var<float>(canny_thresh1, LuaP, "canny_thresh1");
	sub_get_lua_var<float>(canny_thresh2, LuaP, "canny_thresh2");
	sub_get_lua_var<int>(blockSize1, LuaP, "blockSize1");
	sub_get_lua_var<int>(doBlur, LuaP, "doBlur");
	sub_get_lua_var<int>(N_floodPointX, LuaP, "N_floodPointX");
	sub_get_lua_var<int>(N_floodPointY, LuaP, "N_floodPointY");
	sub_get_lua_var<int>(Is_DarkChar, LuaP, "Is_DarkChar");
	sub_get_lua_var<int>(colorFlood_lowDiff, LuaP, "colorFlood_lowDiff");
	sub_get_lua_var<int>(colorFlood_upDiff, LuaP, "colorFlood_upDiff");
	sub_get_lua_var<int>(preprocessType, LuaP, "preprocessType");
	sub_get_lua_var<int>(doFlood1, LuaP, "doFlood1");
	sub_get_lua_var<int>(doFlood2, LuaP, "doFlood2");
	sub_get_lua_var<int>(doDilate, LuaP, "doDilate");
	sub_get_lua_var<int>(flood_fillMode, LuaP, "flood_fillMode");
	sub_get_lua_var<int>(flood_connect, LuaP, "flood_connect");
	sub_get_lua_var<string>(imgFile, LuaP, "imgFile");
	VtargetLoc=func_readLuaArray<int>(LuaP, "target_loc");
	vector<int> VcolorTarget=func_readLuaArray<int>(LuaP, "colorTarget");
	if (VcolorTarget.size()>0){ colorTarget=cv::Scalar(VcolorTarget[0],VcolorTarget[1],VcolorTarget[2]);}
	sub_get_lua_var<int>(dist_colorTarget, LuaP, "dist_colorTarget");
	vector<int> VcolorFloodB=func_readLuaArray<int>(LuaP, "colorFloodB");
	if (VcolorFloodB.size()>0){ colorFlood=cv::Scalar(VcolorFloodB[0],VcolorFloodB[1],VcolorFloodB[2]);}
	sub_get_lua_var<float>(r_near, LuaP, "r_near");
	sub_get_lua_var<int>(auto_colorTarget, LuaP, "auto_colorTarget");
	sub_get_lua_var<int>(doLab, LuaP, "doLab");
	Rrc_seekColor=func_readLuaArray<float>(LuaP, "Rrc_seekColor");
	sub_get_lua_var<int>(min_area, LuaP, "min_area");
	sub_get_lua_var<int>(tmp, LuaP, "tmp");

	lua_close (LuaP);
	return 0;
}


myPowerStation::myPowerStation(){
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}


//myPowerStation *api = new myPowerStation();
myPowerStation api;

void destroyApi(){
	GdiplusShutdown(api.gdiplusToken);
	api.End();
//	delete api;
}



void apiLoadLua(const char* LFile){
	api.LoadLua(LFile);
	LuaFile=LFile;
}

int loadTessData(const char* pathTessdataCh, const char* langCh, short PageSegMode){
	printf("config=%s\nlua=%s\n",ConfigFile.c_str(),LuaFile.c_str());
	api.End();
	api.LoadLua(LuaFile.c_str());
	int res=1;
	char *configs[]={constChar2Char(ConfigFile.c_str())};
	if (ConfigFile.length()){
		res=api.Init(pathTessdataCh, langCh, (tesseract::OcrEngineMode)engine_oem, configs, 1, NULL, NULL, false);
	}else{
		res=api.Init(pathTessdataCh, langCh, (tesseract::OcrEngineMode)engine_oem);
	}
	if (res){
		fprintf(stderr, "Could not initialize tesseract.\n");
	}else{
		api.SetVariable("user_defined_dpi", to_string(user_defined_dpi).c_str()); 
		api.SetPageSegMode((tesseract::PageSegMode)PageSegMode);
		if (whitelist.length()){
			api.SetVariable("tessedit_char_whitelist", whitelist.c_str());
		}
		if (blacklist.length()){
			api.SetVariable("tessedit_char_blacklist", blacklist.c_str());
		}
//		api.SetVariable("oem", to_string(engine_oem).c_str());
	}
	return res;
}


Mat func_adaptiveThreshold(Mat Img, int dim=0, int blockSize1=5, bool doBlur=0, float C=-2){
// blockSize1: Size of a pixel neighborhood that is used to calculate a threshold value for the pixel: 3, 5, 7, and so on.
	if (blockSize1>=0){
		if (Img.channels() == 3){
			cvtColor(Img, Img, COLOR_BGR2GRAY);
		}
		if (doBlur){
			cv::blur(Img, Img, cv::Size(3,3) );   // important
		}
		if (blockSize1==0){
			Canny(Img, Img, canny_thresh1, canny_thresh2); 
		}else if (blockSize1>0){
			adaptiveThreshold(~Img, Img, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, blockSize1, C);
		}else{
		}
	}
	return Img;
}



cv::Scalar hexToCvBGR(unsigned int hexValue) {
	int b = (hexValue >> 16) & 0xFF; // 提取R分量
	int g = (hexValue >> 8) & 0xFF;  // 提取G分量
	int r = hexValue & 0xFF;		 // 提取B分量
	return cv::Scalar(b,g,r);
}


int CvBGRtoHexInt(cv::Scalar bgr) {
	unsigned int b = std::clamp(static_cast<int>(bgr[0]), 0, 255);
	unsigned int g = std::clamp(static_cast<int>(bgr[1]), 0, 255);
	unsigned int r = std::clamp(static_cast<int>(bgr[2]), 0, 255);
	return (b << 16) | (g << 8) | r;
}



cv::Mat fun_convert_8bppPixToMat(Pix* pix) {
	if(!pix) throw std::runtime_error("Null Pix pointer");	
	l_int32 width = pixGetWidth(pix);
	l_int32 height = pixGetHeight(pix);
	l_int32 depth = pixGetDepth(pix);
	l_uint32* data = pixGetData(pix);
	l_int32 wpl = pixGetWpl(pix);
	cv::Mat mat(height, width, CV_8UC1);
	for(int y=0; y<height; ++y) {
		l_uint32* line = data + y*wpl;
		const l_uint8* pline = reinterpret_cast<l_uint8*>(data + y * wpl);
		uchar* dst = mat.ptr<uchar>(y);
		for(int x=0; x<width; ++x) {
			dst[x] =*(pline + x-2*(x%4)+3);
//			dst[x] = GET_DATA_BYTE(line, x);
		}
	}
	return mat;
}


cv::Mat fun_GetThresholdedImage(tesseract::TessBaseAPI& api) {
	// api.SetVariable("tessedit_write_images", "1");
	Pix* processed = api.GetThresholdedImage();  // 得到1位Pix图像
	Pix* pix8bpp=pixConvertTo8(processed, 0);   // 转换为8位8pix图像
	cv::Mat cvImgP=fun_convert_8bppPixToMat(pix8bpp);
	// 清理资源
	pixDestroy(&processed);
	return cvImgP;
}



template<typename T1>
std::vector<T1> fun_extract_cvMat_by_mask(cv::Mat src, cv::Mat mask){
	std::vector<cv::Point> locations;
	cv::findNonZero(mask, locations);
	std::vector<T1> values;
	for(auto& pt : locations) {
		values.push_back(src.at<T1>(pt));
	}
	return values;
}


#include <algorithm>

template<typename T1>
double compute_var(const std::vector<T1>& vals) {
	double mean = std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
	double variance = 0.0;
	for (double val : vals) {
		variance += (val - mean) * (val - mean);
	}
	return variance / vals.size();
};

template<typename T1>
double calculateDeviation(const std::vector<T1>& pixels) {
    // 提取通道数据
    std::vector<double> b_vals, g_vals, r_vals;
    for (const auto& pixel : pixels) {
        b_vals.push_back(pixel[0]);
        g_vals.push_back(pixel[1]);
        r_vals.push_back(pixel[2]);
    }
	return std::sqrt((compute_var(b_vals)+compute_var(g_vals)+compute_var(r_vals))/3);
}

template<typename T1>
T1 robustMedian_vecVec(const std::vector<T1>& pixels, float r1=0.4, float r2=0.6) {
	std::vector<uchar> b, g, r;
	for (const auto& pixel : pixels) {
		b.push_back(pixel[0]); // B通道
		g.push_back(pixel[1]); // G通道
		r.push_back(pixel[2]); // R通道
	}
	std::sort(b.begin(), b.end());
	std::sort(g.begin(), g.end());
	std::sort(r.begin(), r.end());
	int n=b.size();
	int n1=n*r1;
	int n2=n*r2;
	std::vector<int> bms(b.begin() + n1, b.begin() + n2 + 1);
	std::vector<int> gms(g.begin() + n1, g.begin() + n2 + 1);
	std::vector<int> rms(r.begin() + n1, r.begin() + n2 + 1);
	int bm=std::reduce(bms.begin(), bms.end())/bms.size();
	int gm=std::reduce(gms.begin(), gms.end())/gms.size();
	int rm=std::reduce(rms.begin(), rms.end())/rms.size();
	return T1(bm,gm,rm);
}



template<typename T1>
cv::Scalar func_get_colorMedian(Mat cvImg0, std::vector<T1>Rrc_seekColor, cv::Scalar color0=cv::Scalar(255,255,255)){
	size_t w_img=cvImg0.cols, h_img=cvImg0.rows;
	api.SetImage(cvImg0.data, w_img, h_img, cvImg0.channels(), cvImg0.step);
	cv::Mat maskT=fun_GetThresholdedImage(api)==0;
	cv::Rect rcUseful(w_img*Rrc_seekColor[0],h_img*Rrc_seekColor[1],w_img*Rrc_seekColor[2],h_img*Rrc_seekColor[3]);
	if (doDebug&&doShow){
		cv::imshow("maskT", maskT);
		cv::imshow("cvImg0(part)", cvImg0(rcUseful));
		cv::Mat cvImg1=cvImg0.clone();
		cvImg1.setTo(cv::Scalar(0, 255, 0), maskT);
		cv::imshow("Img1mask", cvImg1);
	}
	auto v1p=fun_extract_cvMat_by_mask<Vec3b>(cvImg0(rcUseful), maskT(rcUseful));
	if (v1p.size()>0){
		cv::Vec3b colorMedianVec=robustMedian_vecVec(v1p);
		return cv::Scalar(colorMedianVec[0], colorMedianVec[1], colorMedianVec[2]);
	}
	return color0;
}



template<typename T1>
cv::Scalar func_get_colorMedian_of_mat(cv::Mat cvImg0, cv::Mat maskT, std::vector<T1>Rrc_seekColor, cv::Scalar color0=cv::Scalar(255,255,255)){
	size_t w_img=cvImg0.cols, h_img=cvImg0.rows;
	cv::Rect rcUseful(w_img*Rrc_seekColor[0],h_img*Rrc_seekColor[1],w_img*Rrc_seekColor[2],h_img*Rrc_seekColor[3]);
	if (doDebug&&doShow){
		cv::imshow("maskT", maskT);
		cv::imshow("cvImg0(part)", cvImg0(rcUseful));
		cv::Mat cvImg1=cvImg0.clone();
		cvImg1.setTo(cv::Scalar(0, 255, 0), maskT);
		cv::imshow("Img1mask", cvImg1);
	}
	auto v1p=fun_extract_cvMat_by_mask<Vec3b>(cvImg0(rcUseful), maskT(rcUseful));
//	for (auto&x:v1p){
//		cout<<x<<endl;
//	}
	float Dev1p=calculateDeviation(v1p);
	printf("deviation:\t%f\n",Dev1p);
	if (v1p.size()>0){
		cv::Vec3b colorMedianVec=robustMedian_vecVec(v1p);
		return cv::Scalar(colorMedianVec[0], colorMedianVec[1], colorMedianVec[2]);
	}
	return color0;
}



void func_flood_around_image(cv::Mat& cvImg, myFloodSetting myFloodSet1, int N_floodPointX, int N_floodPointY){
	cv::Rect ccomp; 
	auto x1s=func_ArithmeticSeq<float,float>(1, 0, N_floodPointX, cvImg.cols-1);
	for (auto& x: x1s){ 
		cv::floodFill(cvImg, cv::Point(round(x),1), myFloodSet1.colorFlood, &ccomp, myFloodSet1.colorLow, myFloodSet1.colorUp, myFloodSet1.flags);
		cv::floodFill(cvImg, cv::Point(round(x),cvImg.rows-1), myFloodSet1.colorFlood, &ccomp, myFloodSet1.colorLow, myFloodSet1.colorUp, myFloodSet1.flags);
	}
	auto y1s=func_ArithmeticSeq<float,float>(1, 0, N_floodPointY, cvImg.rows-1);
	for (auto& y: y1s){
		cv::floodFill(cvImg, cv::Point(1,round(y)), myFloodSet1.colorFlood, &ccomp, myFloodSet1.colorLow, myFloodSet1.colorUp, myFloodSet1.flags);
		cv::floodFill(cvImg, cv::Point(cvImg.cols-1,round(y)), myFloodSet1.colorFlood, &ccomp, myFloodSet1.colorLow, myFloodSet1.colorUp, myFloodSet1.flags);
	}
//	if (doDebug&&doShow){	imshow("after flood1", cvImg); }
}





class myParams{
public:
	float alpha_contrast,beta_contrast,canny_thresh1,canny_thresh2;
	int dist_colorTarget,user_defined_dpi,engine_oem,PageSegMode,MaxCharNum,modeScript,doDebug,doShow,blockSize1,doBlur,N_floodPointX,N_floodPointY,Is_DarkChar,colorFlood_lowDiff,colorFlood_upDiff,preprocessType,doFlood1,doFlood2,doDilate,flood_fillMode,flood_connect,doLab;
	cv::Scalar colorTargetSc,colorFlood;
};


template<class T1>
void func1_set_params(T1& H, vector<string> notes, ...) {
    va_list args;
    va_start(args, notes);   // K的值不影响后面的结果
	for (auto& x: notes){
		if (x=="alpha_contrast"){
			H.alpha_contrast=va_arg(args, double);
		}else if (x=="beta_contrast"){
			H.beta_contrast=va_arg(args, double);
		}else if (x=="canny_thresh1"){
			H.canny_thresh1=va_arg(args, double);
		}else if (x=="canny_thresh2"){
			H.canny_thresh2=va_arg(args, double);
		}else if (x=="colorTargetSc"){
			H.colorTargetSc=va_arg(args, cv::Scalar);
		}else if (x=="colorFlood"){
			H.colorFlood=va_arg(args, cv::Scalar);
		}else if (x=="dist_colorTarget"){
			H.dist_colorTarget=va_arg(args, int);
		}else if (x=="user_defined_dpi"){
			H.user_defined_dpi=va_arg(args, int);
		}else if (x=="engine_oem"){
			H.engine_oem=va_arg(args, int);
		}else if (x=="PageSegMode"){
			H.PageSegMode=va_arg(args, int);
		}else if (x=="MaxCharNum"){
			H.MaxCharNum=va_arg(args, int);
		}else if (x=="doDebug"){
			H.doDebug=va_arg(args, int);
		}else if (x=="doShow"){
			H.doShow=va_arg(args, int);
		}else if (x=="blockSize1"){
			H.blockSize1=va_arg(args, int);
		}else if (x=="doBlur"){
			H.doBlur=va_arg(args, int);
		}else if (x=="N_floodPointX"){
			H.N_floodPointX=va_arg(args, int);
		}else if (x=="N_floodPointY"){
			H.N_floodPointY=va_arg(args, int);
		}else if (x=="Is_DarkChar"){
			H.Is_DarkChar=va_arg(args, int);
		}else if (x=="colorFlood_lowDiff"){
			H.colorFlood_lowDiff=va_arg(args, int);
		}else if (x=="colorFlood_upDiff"){
			H.colorFlood_upDiff=va_arg(args, int);
		}else if (x=="preprocessType"){
			H.preprocessType=va_arg(args, int);
		}else if (x=="doFlood1"){
			H.doFlood1=va_arg(args, int);
		}else if (x=="doFlood2"){
			H.doFlood2=va_arg(args, int);
		}else if (x=="doDilate"){
			H.doDilate=va_arg(args, int);
		}else if (x=="flood_fillMode"){
			H.flood_fillMode=va_arg(args, int);
		}else if (x=="flood_connect"){
			H.flood_connect=va_arg(args, int);
		}else if (x=="doLab"){
			H.doLab=va_arg(args, int);
		}
	}
    va_end(args);
}


Mat removeSmallBlobs(Mat input, int min_area) {
    // 确保输入是二值图像
    if (input.channels() != 1) {
        cerr << "Input must be a single-channel binary image!" << endl;
        return input;
    }
    Mat output = Mat::zeros(input.size(), input.type());
    // 使用连通域分析(带统计信息)
    Mat labels, stats, centroids;
    int num_labels = connectedComponentsWithStats(input, labels, stats, centroids, 8);
    // 跳过背景(0号标签)
    for (int i = 1; i < num_labels; i++) {
        int area = stats.at<int>(i, CC_STAT_AREA);
        // 保留大于阈值的区域
        if (area >= min_area) {
            // 创建当前区域的掩码
            Mat mask = (labels == i);
            // 将保留区域复制到输出图像
            input.copyTo(output, mask);
        }
    }
    return output;
}


float fun_ocr(char*& text, const cv::Mat cvImg0, myParams Params, cv::Mat& mask1){
	float alpha_contrast=Params.alpha_contrast;
	float beta_contrast=Params.beta_contrast;
	float canny_thresh1=Params.canny_thresh1;
	float canny_thresh2=Params.canny_thresh2;
	int dist_colorTarget=Params.dist_colorTarget;
	int doDebug=Params.doDebug;
	int doShow=Params.doShow;
	cv::Scalar colorTargetSc=Params.colorTargetSc;
	cv::Scalar colorFlood=Params.colorFlood;
	int colorFlood_lowDiff=Params.colorFlood_lowDiff;
	int colorFlood_upDiff=Params.colorFlood_upDiff;
	int flood_fillMode=Params.flood_fillMode;
	int flood_connect=Params.flood_connect;
	int blockSize1=Params.blockSize1;
	int doBlur=Params.doBlur;
	int N_floodPointX=Params.N_floodPointX;
	int N_floodPointY=Params.N_floodPointY;
	int doLab=Params.doLab;
	int preprocessType=Params.preprocessType;
	int Is_DarkChar=Params.Is_DarkChar;
	/* ----------------------------- */
	Mat cvImg=cvImg0.clone();
	size_t w_img=cvImg0.cols, h_img=cvImg0.rows;
	if (doDebug){
		std::cout<<"colorTargetSc:\t"<<colorTargetSc<<std::endl;
		std::cout<<"dist_colorTarget:\t"<<dist_colorTarget<<std::endl;
	}
	/* ----------------------------- */
	colorDetect cdet_Target;
	myFloodSetting myFloodSet1(colorFlood,colorFlood_lowDiff,colorFlood_upDiff,flood_fillMode,flood_connect);
	if (doDebug){
		printf("flag=%d\t%d\t%d\n",myFloodSet1.flags, FLOODFILL_FIXED_RANGE, FLOODFILL_MASK_ONLY);
	}
//	cv::Mat mask2(h_img,w_img,CV_8UC1,255);
	if (dist_colorTarget<0){
		if (dist_colorTarget==-1){
			Mat gray;
			cvtColor(cvImg, gray, COLOR_BGR2GRAY);
			threshold(gray, cvImg, 0, 255, THRESH_BINARY | THRESH_OTSU);
		}else {
			api.SetImage(cvImg0.data, w_img, h_img, cvImg0.channels(), cvImg0.step);
			cvImg=fun_GetThresholdedImage(api);
		}
	}else{
		if (preprocessType==1){
			cvImg.convertTo(cvImg, -1, alpha_contrast, beta_contrast);
			if (doDebug&&doShow){ imshow("contrast", cvImg); }		
			cvImg=func_adaptiveThreshold(cvImg, 0, blockSize1, doBlur);
			if (Is_DarkChar){
	//			myFloodSet1.colorFlood=cv::Scalar(0,0,0);
				cvImg=cvImg==0;
			}
			if (doDebug&&doShow){ imshow("before flood", cvImg); }
			if (doFlood1>0){
				func_flood_around_image(cvImg, myFloodSet1, N_floodPointX, N_floodPointY);
				if (doDebug&&doShow){ imshow("after flood", cvImg); }
			}
			if (doDilate>0){
				Mat se12 = getStructuringElement(MORPH_RECT, cv::Size(2, 2));
				dilate(cvImg, cvImg, se12); 
				if (doDebug&&doShow){ imshow("after dilate", cvImg); }
			}
	//		print_cvMatInfo(cvImg, "cvImg_1");
		}else if (preprocessType==3){
			/*-------remove area with the similar color of seed (like magic-stick of PS)-------*/
			cdet_Target=colorDetect(colorTargetSc,dist_colorTarget);
			mask1=cdet_Target.process(cvImg,doLab);		// get the mask of selected area
			cvImg=mask1==0;
		}else{
	//		/*-------remove area with the similar color of seed (like magic-stick of PS)-------*/
	//		cdet_Target=colorDetect(colorTargetSc,dist_colorTarget);
	//		mask1=cdet_Target.process(cvImg,doLab);		// get the mask of selected area
	//		cvImg=mask1==0;
		}
	}
	if (doFlood2>0){
		func_flood_around_image(cvImg, myFloodSet1, N_floodPointX, N_floodPointY);
//		cvImg.setTo(255,mask2==0);
	}
	mask1=cvImg==0;
//	if (doFlood2>0){
//		removeSmallBlobs(binary, min_area);
//	}
	if (min_area>0){
		mask1 = removeSmallBlobs(mask1, min_area);
	}
	Mat cvImg1=cvImg0.clone();
	if (doDebug&&doShow){
		imshow("mask1a", mask1);
		imshow("original", cvImg0);
		imshow("final", cvImg);
		print_cvMatInfo(cvImg, "cvImg");
	}
//	----------------------------------------------
	api.SetImage(mask1.data, w_img, h_img, cvImg.channels(), cvImg.step);
	int* word_confs = api.AllWordConfidences();  // 获取置信度数组，并起到api.Recognize(nullptr)的作用
//	api.Recognize(NULL);  // 执行OCR识别
	if (doDebug){
		while (*word_confs != -1) {  // 置信度数组以-1结尾
			printf("%d,",  *word_confs);
			word_confs++;
		}
		printf("\n");
	}
	tesseract::ResultIterator* ri = api.GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
	float y_top=0, y_bot=h_img;
	std::vector<float> y1s,y2s,cfs;
	std::vector<int> x1s,x2s;
	std::vector<std::string> words;
	if (ri != nullptr){
		do{
			const char* word = ri->GetUTF8Text(level);
			if (word != nullptr) {
				bool isValid = api.IsValidWord(word);  // 关键校验方法
				float conf = ri->Confidence(level);
				cfs.push_back(conf);
				int x1, y1, x2, y2;
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				string corrected="";
				if (!isValid){
					corrected = CorrectInvalidWord(word);
					if (doDebug&& corrected!="[UNK]"){ 
						cout << "修正: " << word << " -> " << corrected << std::endl; 
						words.push_back(corrected);
					}else{
						words.push_back(word);
					}
				}else{
					words.push_back(word);
				}
				std::string sep="", sepType="";
				if (ri->IsAtFinalElement(tesseract::RIL_TEXTLINE, tesseract::RIL_WORD)) {
					sep="\n";sepType="Newline";
					words.push_back(sep);
				} else if (!ri->IsAtFinalElement(tesseract::RIL_WORD, tesseract::RIL_SYMBOL)) {
					sep=" ";sepType="Space";
					words.push_back(sep);
				}
//				y1s.push_back(y1);
//				y2s.push_back(y2);
//				x1s.push_back(x1);
//				x2s.push_back(x2);
					printf("%-27s\t%.2f\t%d\t[%3d,%3d,%3d,%3d]\t%12s\t%s\n", string_format("[%s]",word).c_str(), conf, isValid, x1, y1, x2, y2, corrected.c_str(), sepType.c_str());
				if (doDebug&&doShow){
	//				printf("[%s]\t%.2f\t%d\t[%3d,%3d,%3d,%3d]\t%s\n", word, conf, isValid, x1, y1, x2, y2, corrected.c_str());
					rectangle(cvImg1, cv::Point(x1,y1), cv::Point(x2,y2), Scalar(0, 255, 0), 1 );
				}
			}
			delete [] word;
		} while (ri->Next(level));
	}
	if (doDebug&&doShow){
		imshow("rected", cvImg1);
	}
	float s_conf=std::reduce(cfs.begin(), cfs.end());
	float m_conf=cfs.size()>0 ? s_conf/cfs.size() : 0;
	float x_conf=s_conf*m_conf;
	if (doDebug){
		printf("[%s]-----3----\n",func_format_join_array(words,"%s",",").c_str());
	}
	printf("conf:\t%.2f\t%.2f\t%.2f\n",s_conf,m_conf,x_conf);
	string outText=string(api.GetUTF8Text());
	outText=regex_replace(outText, regex("^[ \n\r\t]+|[ \n\r\t]+$"),"");
	strcpy_s(text, outText.length()+1, outText.c_str()); // sizeof() is equivalent to strlen()
	return m_conf;
}




/*
HexColorTarget>=0 :  重设colorTarget
dist_colorTarget>-300 : 重设colorTarget时，用colorMedian
dist_colorTarget<=-300 : 重设colorTarget时，用HexColorTarget
*/
int ocr_region(char*& text, int x=0, int y=0, int w=0, int h=0, HWND hWnd=0x0, int HexColorTarget=-1, int dist_colorT=10, int auto_color=0){
	Mat cvImg;
	if (modeScript>=0 && fileExists(imgFile.c_str())){
		cvImg = cv::imread(imgFile, IMREAD_COLOR);
	}else{
		cvImg=screenshot(x,y,w,h,hWnd);
	}
	myParams myP1;
	vector<string> pnames{"dist_colorTarget","doDebug","doShow","colorTargetSc","colorFlood","colorFlood_lowDiff","colorFlood_upDiff","flood_fillMode","flood_connect","alpha_contrast","beta_contrast","blockSize1","doBlur","N_floodPointX","N_floodPointY","doLab","preprocessType","canny_thresh1","canny_thresh2","doFlood2"};
	func1_set_params(myP1,pnames,dist_colorT,0,0,colorTargetSc,colorFlood,colorFlood_lowDiff,colorFlood_upDiff,flood_fillMode,flood_connect,alpha_contrast,beta_contrast,blockSize1,doBlur,N_floodPointX,N_floodPointY,doLab,1,canny_thresh1,canny_thresh2,1);
	cv::Mat mask0;
	if (HexColorTarget>=0){
		colorTargetSc=hexToCvBGR(HexColorTarget);
	}
	if (auto_color>0){
		char *textTmp = new char[MaxCharNum];
		cv::Mat mask1,mask2;
		std::vector<float> confs;
		func1_set_params(myP1,vector<string>{"Is_DarkChar"},0);
		fun_ocr(textTmp, cvImg, myP1, mask0);
		cv::Scalar color0=func_get_colorMedian_of_mat(cvImg, mask0, Rrc_seekColor, colorTargetSc);
		func1_set_params(myP1,vector<string>{"colorTargetSc","preprocessType","dist_colorTarget"},color0,3, (dist_colorT>0 ? dist_colorT:-dist_colorT));
		confs.push_back(fun_ocr(textTmp, cvImg, myP1, mask0));
		cout<<color0<<endl;
		func1_set_params(myP1,vector<string>{"Is_DarkChar","preprocessType"},1,1);
		fun_ocr(textTmp, cvImg, myP1, mask1);
		cv::Scalar color1=func_get_colorMedian_of_mat(cvImg, mask1, Rrc_seekColor, colorTargetSc);
		func1_set_params(myP1,vector<string>{"colorTargetSc","preprocessType"},color1,3);
		confs.push_back(fun_ocr(textTmp, cvImg, myP1, mask1));
		cout<<color1<<endl;
		printf("confs[1,2]=\t%.2f\t%.2f\n",confs[0],confs[1]);
//		func1_set_params(myP1,vector<string>{"dist_colorTarget"},-9);
//		int conf3=fun_ocr(textTmp, cvImg, myP1, mask2);
//		cv::Scalar color2=func_get_colorMedian_of_mat(cvImg, mask2, Rrc_seekColor, colorTargetSc);
//		cout<<color2<<endl;
//		printf("confs[1,2,3]=\t%.2f\t%.2f\t%.2f\n",confs[0],confs[1],conf3);
//		confs[2]=0;
		auto max_it = std::max_element(confs.begin(), confs.end());
		int i_max = std::distance(confs.begin(), max_it);
		if (confs[0]>confs[1]){
			Is_DarkChar=0;
		}else{
			Is_DarkChar=1;
		}
		if (i_max==0){
			colorTargetSc=color0;
//			colorTargetSc=func_get_colorMedian_of_mat(cvImg, mask0, Rrc_seekColor, colorTargetSc);
//			printf("--------confs[0]---------\n");
		}else if (i_max==1){
			colorTargetSc=color1;
//			colorTargetSc=func_get_colorMedian_of_mat(cvImg, mask1, Rrc_seekColor, colorTargetSc);
			Is_DarkChar=1;
//			printf("--------confs[1]---------\n");
//		}else{
//			colorTargetSc=color2;
////			colorTargetSc=func_get_colorMedian_of_mat(cvImg, mask2, Rrc_seekColor, colorTargetSc);
//			preprocessType=4;
////			printf("--------confs[2]---------\n");
		}
		return CvBGRtoHexInt(colorTargetSc);
//		std::cout<<"colorTargetSc1:\t"<<colorTargetSc<<std::endl;
	}
	printf("Is_DarkChar=%d\n",Is_DarkChar);
	func1_set_params(myP1,vector<string>{"colorTargetSc","doDebug","doShow","Is_DarkChar","dist_colorTarget","preprocessType","doFlood2"},colorTargetSc,doDebug,doShow,Is_DarkChar,dist_colorT,preprocessType,doFlood2);
	fun_ocr(text, cvImg, myP1, mask0);
//	func_ocr(text, cvImg, dist_colorTarget);
	return -1;
}




int main( int argc, char** argv ){
	LuaFile=regex_replace (string(argv[0]),regex("\\.exe$"),"")+".lua";
//	printf("%s\n",LuaFile.c_str());
	apiLoadLua(LuaFile.c_str());
	loadTessData(path_tessdata.c_str(), lang.c_str(), PageSegMode);
//	if (doShow>0){
//		doShow=9;
//	}
//	printf("doShow=%d-------5------\n",doShow);
	HWND hWnd=0x00;
	if (modeScript>0){
		POINT cursor;	
		GetCursorPos(&cursor);
		hWnd=WindowFromPoint(cursor);
		if (modeScript==1){
			hWnd=GetNonChildParent(hWnd);
		}
	}
	printf("[hwnd=0x%x,%d]-----------\n",hWnd,hWnd);
	printf("target=%d[%d,%d,%d,%d]-----------\n",modeScript,VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3]);

	char *outText = new char[MaxCharNum];
	int HexColorTarget=bgrToHexInt(colorTarget[0],colorTarget[1],colorTarget[2]);
	printf("HexColorTarget=%d\n",HexColorTarget);
	if (auto_colorTarget>0){
		int HexColorTarget1=ocr_region(outText, VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd, HexColorTarget, dist_colorTarget, auto_colorTarget);
	}else{
		colorTargetSc=colorTarget;
	}
	if (doDebug!=9){
		ocr_region(outText, VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd, -1, dist_colorTarget, 0);
	}
//	save_region(VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd, "test.png");
//	ocr_region(outText, 0, 0, 0, 0, hWnd);
//	saveshot(VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd, "test_out.png");
	printf("[%s]------9-----\n",outText);


	if (doDebug&&doShow){
		waitKey(0);
	}
	destroyApi();

	return 0;
}



#endif /* __ORCSCREEN_H__ */



/*

*/