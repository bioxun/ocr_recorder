#pragma once

//#define WINVER 0x0A00
//#define _WIN32_WINNT 0x0A00

//#include <windows.h>
#include <memory>
#include <stdexcept>
#include <numeric>      // std::iota, std::accumulate
#include <memory>
#include <lua/lua.hpp>
const std::vector<std::string> dict_correction = {"One", "confvse", "orange", "correlation"};



template<class T>
char* constChar2Char(T cC1){
	char *C2 = new char[strlen(cC1) + 1];
//	char *C2 = (char *)malloc(strlen(cC1) + 1);
	strcpy_s(C2, strlen(cC1) + 1, cC1); // sizeof() is equivalent to strlen()
//	strcpy(C2, cC1); // sizeof() is equivalent to strlen()
//	memcpy ((void*)C2, cC1, strlen(cC1) + 1);
	return C2;
}

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args ){
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

template<typename T>
constexpr bool is_string_v = std::is_same_v<std::decay_t<T>, std::string>;

template<class T>
std::string func_format_join_array(std::vector<T> X, std::string format="", std::string sep=""){
	if (X.size()>0){
		if (is_string_v<T>){
			auto lambdaF = [format,sep](std::string a, auto b) { return a + sep+ string_format(format,b.c_str()); };
			return accumulate(next(X.begin()), X.end(), string_format(format,X[0].c_str()), lambdaF);
		}else{
			auto lambdaF = [format,sep](std::string a, auto b) { return a + sep+ string_format(format,b); };
			return accumulate(next(X.begin()), X.end(), string_format(format,X[0]), lambdaF);
		}
	}else{
		return "";
	}
}


bool fileExists(const char* filename) {
	DWORD attrib = GetFileAttributesA(filename);
	return (attrib != INVALID_FILE_ATTRIBUTES && 
		   !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}



template<class T1>
std::vector<T1> func_readLuaArray(lua_State *Lp, const char* arrName){
	lua_settop(Lp,0);
	lua_getglobal(Lp, arrName);
	if (lua_istable(Lp, -1)){
		int n = luaL_len(Lp, 1);
		std::vector<T1> OutArr;
		for (int i = 1; i <= n; ++i) {
			lua_pushnumber(Lp, i);
			lua_gettable(Lp, -2);  //lua_gettable也可以用lua_rawget来替换
			if constexpr (std::is_integral_v<T1>) {
				if (lua_isinteger(Lp, -1)){
					OutArr.push_back(lua_tointeger(Lp, -1));
				}
			}else if constexpr (std::is_floating_point<T1>::value){
				if (lua_isnumber(Lp, -1)){
					OutArr.push_back(lua_tonumber(Lp, -1));
				}
			}else{
				if (lua_isstring(Lp, -1)){
					OutArr.push_back(lua_tostring(Lp, -1));
				}
			}
			lua_pop(Lp, 1);
		}
		lua_remove(Lp,-1);
		return OutArr;
	}else{
		lua_remove(Lp,-1);
		return std::vector<T1>{};
	}
}

template<class T>
T func_get_lua_var(lua_State * Lp, const char * v_name, T X_default){
	lua_getglobal(Lp, v_name);
	T X=X_default;
	if constexpr (std::is_integral_v<T>) {
		if (lua_isinteger(Lp, -1)){
			X=lua_tointeger(Lp, -1);
		}
	}else if constexpr (std::is_floating_point<T>::value){
		if (lua_isnumber(Lp, -1)){
			X=lua_tonumber(Lp, -1);
		}
	}else{
		if (lua_isstring(Lp, -1)){
			X=lua_tostring(Lp, -1);
		}
	}
	lua_remove(Lp,-1);
	return X;
}

template<class T>
void sub_get_lua_var(T& X, lua_State * Lp, const char * v_name){
	lua_getglobal(Lp, v_name);
	if constexpr(std::is_integral_v<T>) {
		if (lua_isinteger(Lp, -1)){
			X=lua_tointeger(Lp, -1);
		}
	}else if constexpr(std::is_floating_point<T>::value){
		if (lua_isnumber(Lp, -1)){
			X=lua_tonumber(Lp, -1);
		}
	}else{
		if (lua_isstring(Lp, -1)){
			X=lua_tostring(Lp, -1);
		}
	}
	lua_remove(Lp,-1);
}




template <class T1>
double func_median(std::vector<T1> vec){
	typedef std::vector<int>::size_type vec_sz;
	vec_sz size = vec.size();
	sort(vec.begin(), vec.end());
	vec_sz mid = size/2;
	return size % 2 == 0 ? (vec[mid] + vec[mid-1]) / 2 : vec[mid];
}



// 编辑距离计算（Levenshtein算法）
int CalculateEditDistance(const std::string& s1, const std::string& s2) {
	const size_t len1 = s1.size(), len2 = s2.size();
	std::vector<std::vector<int>> dp(len1+1, std::vector<int>(len2+1, 0));
	for(int i=0; i<=len1; ++i) dp[i][0] = i;
	for(int j=0; j<=len2; ++j) dp[0][j] = j;
	for(int i=1; i<=len1; ++i)
		for(int j=1; j<=len2; ++j)
			dp[i][j] = std::min({dp[i-1][j]+1, dp[i][j-1]+1, dp[i-1][j-1]+(s1[i-1]!=s2[j-1])});
	return dp[len1][len2];
}

std::string CorrectInvalidWord(const std::string& invalid_word) {
	int min_distance = INT_MAX;
	std::string best_match;	
	for(const auto& word : dict_correction) {
		int dist = CalculateEditDistance(invalid_word, word);
		if(dist < min_distance) {
			min_distance = dist;
			best_match = word;
		}
	}
	return (min_distance <= 2) ? best_match : "[UNK]";
}


HWND GetNonChildParent(HWND aWnd){
	if (!aWnd) return aWnd;
	HWND parent, parent_prev;
	for (parent_prev = aWnd; ; parent_prev = parent){
		if (!(GetWindowLong(parent_prev, GWL_STYLE) & WS_CHILD))  // Found the first non-child parent, so return it.
			return parent_prev;
		// Because Windows 95 doesn't support GetAncestor(), we'll use GetParent() instead:
		if (   !(parent = GetParent(parent_prev))   )
			return parent_prev;  // This will return aWnd if aWnd has no parents.
	}
}




HBITMAP func_GdipPrintWindow(HWND hWnd, int x=0, int y=0, int w=0, int h=0, float scale=1){
	bool isNotDesktop=1;
	HWND hWndD = GetDesktopWindow();
	if (hWnd==0x0){
		hWnd = hWndD;
	}
	if (hWnd == hWndD){
		isNotDesktop=0;
	}
	// 获取窗口尺寸
	RECT rc_cli;
	GetClientRect(hWnd, &rc_cli);
	int gui_cli_w=rc_cli.right-rc_cli.left;
	int gui_cli_h=rc_cli.bottom-rc_cli.top;
	int gui_cli_x=rc_cli.left;
	int gui_cli_y=rc_cli.top;
	printf("client: [%d,%d,%d,%d]\n", gui_cli_x,gui_cli_y,gui_cli_w,gui_cli_h);
	RECT rc_gui;
	GetWindowRect(hWnd, &rc_gui);
	int gui_gui_w=rc_gui.right-rc_gui.left;
	int gui_gui_h=rc_gui.bottom-rc_gui.top;
	int gui_gui_x=rc_gui.left;
	int gui_gui_y=rc_gui.top;
	printf("gui: [%d,%d,%d,%d]\n", gui_gui_x,gui_gui_y,gui_gui_w,gui_gui_h);
	if (x<0){
		x=gui_cli_x;
	}
	if (y<0)	{
		h=gui_cli_y;
	}
	if (w<=0){
		w=gui_cli_w;
	}
	if(w+x>gui_cli_w){
		w=gui_cli_w-x;
	}
	if (h<=0){
		h=gui_cli_h;
	}
	if(h+y>gui_cli_h){
		h=gui_cli_h-y;
	}
	if (scale<=0){
		scale=1;
	}
	int w1=w*scale;
	int h1=h*scale;
	printf("target geometry: %d,%d,%d,%d\n", x,y,w1,h1);
	HDC hwindowDC = GetDC(hWnd);
	HDC hdcMem = CreateCompatibleDC(hwindowDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hwindowDC, w1, h1);
	SelectObject(hdcMem, hBitmap);
	if (isNotDesktop && IsWindow(hWnd)) {
		HDC hdcMem1 = CreateCompatibleDC(hwindowDC);
		SetStretchBltMode(hdcMem1, COLORONCOLOR);  // ?
		HBITMAP hBitmap1 = CreateCompatibleBitmap(hwindowDC, x+w1, y+h1);
		SelectObject(hdcMem1, hBitmap1);
		BOOL bResult = PrintWindow(hWnd, hdcMem1, PW_CLIENTONLY | PW_RENDERFULLCONTENT); // 尝试 PW_RENDERFULLCONTENT 以捕获复杂内容
		StretchBlt(hdcMem, 0, 0, w1, h1, hdcMem1, x, y, w1, h1, SRCCOPY);   //change SRCCOPY to NOTSRCCOPY for wacky colors !
		DeleteDC(hdcMem1);
		DeleteObject(hBitmap1);
	}else{
		StretchBlt(hdcMem, 0, 0, w1, h1, hwindowDC, x, y, w, h, SRCCOPY);   //change SRCCOPY to NOTSRCCOPY for wacky colors !
	}
	// avoid memory leak
	DeleteDC(hdcMem);
	ReleaseDC(hWnd, hwindowDC);
	return hBitmap;
}



template<class T1, class T2>
std::vector<T2> func_ArithmeticSeq(T1 start, T2 step, int n, T1 end=-99) {
	if (n<=0){
		n=1+(end-start)/abs(step);
		if (n<=0){
			n=0;
		}
	}
	if (step<=0){
		if (n<=1){
			step=0;
		}else{
			step=((T2)end-start)/(n-1);
		}
	}
//	printf("step=%f\tn=%d\n",step,n);
	std::vector<T2> seq(n);
	for(int i = 0; i < n; i++) {
		seq[i]=step*i+start;
	}
	return seq;
}


void hexToRGB(unsigned int hexValue, int& r, int& g, int& b) {
	r = (hexValue >> 16) & 0xFF; // 提取R分量
	g = (hexValue >> 8) & 0xFF;  // 提取G分量
	b = hexValue & 0xFF;		 // 提取B分量
}


unsigned int rgbToHexInt(int r, int g, int b) {
	r = std::clamp(r, 0, 255);
	g = std::clamp(g, 0, 255);
	b = std::clamp(b, 0, 255);
	return (r << 16) | (g << 8) | b;
}


void hexToBGR(unsigned int hexValue, int& b, int& g, int& r) {
	b = (hexValue >> 16) & 0xFF; // 提取R分量
	g = (hexValue >> 8) & 0xFF;  // 提取G分量
	r = hexValue & 0xFF;		 // 提取B分量
}


unsigned int bgrToHexInt(int b, int g, int r) {
	b = std::clamp(b, 0, 255);
	g = std::clamp(g, 0, 255);
	r = std::clamp(r, 0, 255);
	return (b << 16) | (g << 8) | r;
}


std::string getMatType(const cv::Mat& cvImg) {
	const std::string typeNames[] = {
		"CV_8U", "CV_8S", "CV_16U", "CV_16S",
		"CV_32S", "CV_32F", "CV_64F","CV_16F"
	};
	int type = cvImg.type();
	return typeNames[type & CV_MAT_DEPTH_MASK] + "C" + std::to_string(cvImg.channels());
}

void print_cvMatInfo(cv::Mat x, const char* s=""){
	printf("%s\t[%4d,%4d]\t%4d\t%4d\t%4d\t%4d\t%s\n", getMatType(x).c_str(), x.cols, x.rows, x.total(), x.elemSize(), x.step[1], x.step[0], s);
}



