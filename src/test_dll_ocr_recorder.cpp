/*

*/

#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

//#include "Windows.h"  // required for DWORD
#include <objidl.h>
#include <Gdiplus.h>
#include <vector>       // std::vector
#include <fstream>
#include <string>
#include <regex>
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui_c.h"
#include <tesseract/baseapi.h>
#include <lua/lua.hpp>
#include "basic_funcs_for_ocr_recorder.hpp"


using namespace std;
using namespace cv;
using namespace Gdiplus;



#ifdef __cplusplus  
extern "C" {  
#endif
int __declspec (dllexport) ocr_region(char*& text, int x, int y, int w, int h, HWND hWnd, int HexColorTarget, int dist_colorTarget, int auto_color);
void __declspec (dllexport) saveshot(int x, int y, int w, int h, HWND hWnd, const char* fileOut);
void __declspec (dllexport) apiLoadLua(const char* LuaFile);
extern void __declspec (dllexport) destroyApi();
extern int __declspec (dllexport) loadTessData(const char* pathTessdataCh, const char* langCh, short PageSegMode=6);
#ifdef __cplusplus  
} 
#endif  


/** General variables */
int user_defined_dpi=100;
int PageSegMode=3;
int MaxCharNum=3000;
int hwTarget=0;
int doDebug=0;
int doShow=0;
int engine_oem=3;
string lang="eng";
string path_tessdata="D:/ProgKits/msys2/mingw64/share/tessdata";
string whitelist="";
string blacklist="";
const char* LuaFile="dll_ocr_recorder.lua";
cv::Scalar colorTarget(255,255,255,50);
int dist_colorTarget=20;
int auto_colorTarget=0;



int main( int argc, char** argv ){
//	string SimName=regex_replace (string(argv[0]),regex("\\.exe$"),"");
//	string LuaFile=SimName+".lua";
//	myLoadLua(LuaFile.c_str());
//	/*-------------------------*/
	lua_State *LuaP = luaL_newstate();
	luaL_openlibs(LuaP);
	/*-------------load the lua file------------*/
	if (luaL_loadfile(LuaP, LuaFile) || lua_pcall(LuaP, 0, 0, 0)){
		printf("error %s\n", lua_tostring(LuaP, -1));
		return -1;
	}
	sub_get_lua_var<string>(lang, LuaP, "lang");	// target languge
	sub_get_lua_var<string>(path_tessdata, LuaP, "path_tessdata");	// 
	sub_get_lua_var<string>(whitelist, LuaP, "whitelist");	// 
	sub_get_lua_var<string>(blacklist, LuaP, "blacklist");	// 
	sub_get_lua_var<int>(user_defined_dpi, LuaP, "user_defined_dpi");
	sub_get_lua_var<int>(engine_oem, LuaP, "engine_oem");
	sub_get_lua_var<int>(PageSegMode, LuaP, "PageSegMode");
	sub_get_lua_var<int>(MaxCharNum, LuaP, "MaxCharNum");
	sub_get_lua_var<int>(hwTarget, LuaP, "hwTarget");
	sub_get_lua_var<int>(doDebug, LuaP, "doDebug");
	sub_get_lua_var<int>(doShow, LuaP, "doShow");
	vector<int> VtargetLoc=func_readLuaArray<int>(LuaP, "target_loc");
	vector<int> VcolorTarget=func_readLuaArray<int>(LuaP, "colorTarget");
	if (VcolorTarget.size()>0){ colorTarget=cv::Scalar(VcolorTarget[0],VcolorTarget[1],VcolorTarget[2]);}
	sub_get_lua_var<int>(dist_colorTarget, LuaP, "dist_colorTarget");
	sub_get_lua_var<int>(auto_colorTarget, LuaP, "auto_colorTarget");
	apiLoadLua(LuaFile);
	if (loadTessData(path_tessdata.c_str(), lang.c_str(), PageSegMode)){
		return 1;
	}
//	myPowerStation *api = new myPowerStation(path_tessdata.c_str(), lang.c_str());
	HWND hWnd=0x00;
	if (hwTarget>0){
		POINT cursor;	
		GetCursorPos(&cursor);
		hWnd=WindowFromPoint(cursor);
		if (hwTarget==1){
			hWnd=GetNonChildParent(hWnd);
		}
		printf("[hwnd=0x%x,%d]-----------\n",hWnd,hWnd);
	}
	printf("target=%d[%d,%d,%d,%d]-----------\n",hwTarget,VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3]);
	char *outText = new char[MaxCharNum];
//	ocr_region(outText, 451,830,915,44);
//	ocr_region(outText, VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd);
	int HexColorTarget=bgrToHexInt(colorTarget[0],colorTarget[1],colorTarget[2]);

	printf("HexColorTarget=%d\n",HexColorTarget);
	if (auto_colorTarget>0){
		int HexColorTarget1=ocr_region(outText, VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd, HexColorTarget, dist_colorTarget, auto_colorTarget);
	}
	if (doDebug!=9){
		ocr_region(outText, VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd, -1, dist_colorTarget, 0);
	}


//	ocr_region(outText, VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd);
//	screenshot(VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd, "test_out.png");
	saveshot(VtargetLoc[0], VtargetLoc[1], VtargetLoc[2], VtargetLoc[3], hWnd, "test_out.png");
	printf("<%s>-----------\n",outText);

	if (doDebug&&doShow){
		waitKey(0);
	}
	destroyApi();

	return 0;
}

