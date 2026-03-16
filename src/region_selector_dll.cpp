/*

*/
#ifndef __REGION_SELECTOR_H__
#define __REGION_SELECTOR_H__


//#include <uxtheme.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <comdef.h>
#include <thread>
#include <regex>
#include <vector>
#include <iterator>

#include "region_selector_dll.hpp"


using namespace std;



#define MyAPI __declspec(dllexport)

#define WM_OUT_THREAD_MSG 2001
#define WM_CROSS_THREAD_MSG 2010

#define id_Btn1a  2011
#define id_Btn1b  2012
#define id_Edt1  2013
#define id_Btn2a  2021
#define id_Btn2b  2022
#define id_Edt2  2023



//int __declspec (dllexport) ocr_region(char*& text, int x, int y, int w, int h, HWND hWnd, int HexColorTarget, int dist_colorTarget);
//extern void __declspec (dllexport) destroyApi();

#ifdef __cplusplus  
extern "C" {  
#endif  
  
extern HWND MyAPI threadCreate_RS(int id=0, int x_gui=-1, int y_gui=-1, int w_gui=-1, int h_gui=-1, int r=-1, int g=-1, int b=-1, int thickness=-1);
int MyAPI test_1(int x);

#ifdef __cplusplus  
}  
#endif  

void MyAPI func_print_hw_related(HWND HW, std::string tag="");



//std::vector<HANDLE> hThreads_tmp;

HWND HWbtn1a,HWbtn1b,HWbtn2a,HWbtn2b,HWedt1,HWedt2;

HANDLE hWindowReadyEvent;
//RegionSelector mySelector1;
//RegionSelector mySelector2;
//std::vector<RegionSelector> RSelectors{mySelector1,mySelector2};
std::vector<RegionSelector> RSelectors(2);


std::vector<int> split_into_num(const std::string& s, const std::string& delimStr, int Ndef=0) {
	const std::regex delim(delimStr);
    std::vector<int> elems;
    for (std::sregex_token_iterator it(s.begin(), s.end(), delim, -1); it != std::sregex_token_iterator(); ++it) {
        if (!it->str().empty()) {
			try {
				elems.push_back(std::stoi(it->str().c_str()));
			} catch (...) {
				elems.push_back(Ndef);
			}
		}
    }
    return elems;
}


void fun_set_params_RS(const char* EdtText, int& x_gui, int& y_gui, int& w_gui, int& h_gui, int& r, int& g, int& b, int& thickness){
	auto arr=split_into_num(EdtText,"[, ]+");
	int n=arr.size();
	if (n>=1){
		x_gui=arr[0];
	}
	if (n>=2){
		y_gui=arr[1];
	}
	if (n>=3){
		w_gui=arr[2];
	}
	if (n>=4){
		h_gui=arr[3];
	}
	if (n>=5){
		r=arr[4];
	}
	if (n>=6){
		g=arr[5];
	}
	if (n>=7){
		b=arr[6];
	}
	if (n>=8){
		thickness=arr[7];
	}
}


DWORD GetThreadIdFromWindow(HWND hWnd) {
	DWORD dwProcessId = 0;
	DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
	return dwThreadId;
}

HANDLE GetThreadHandleFromWindow(HWND hWnd) {
	DWORD dwThreadId = GetThreadIdFromWindow(hWnd);
	return OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
}




HWND threadCreate_RS(int id, int x_gui, int y_gui, int w_gui, int h_gui, int r, int g, int b, int thickness){
	std::string wcClassName="RegionSelectorC"+std::to_string(id);
	RSelectors[id].wcClassName=wcClassName;
	RSelectors[id].set(x_gui, y_gui, w_gui, h_gui, r, g, b, thickness);
	RSelectors[id].threadCreate();
//	while (RSelectors[id].m_hwnd==0){}   // 可能有用
//	SetWindowPos(RSelectors[id].m_hwnd, NULL, RSelectors[id].x_gui, RSelectors[id].y_gui, RSelectors[id].w_gui, RSelectors[id].h_gui, SWP_SHOWWINDOW);
	SetWindowPos(RSelectors[id].m_hwnd, NULL, RSelectors[id].x_gui, RSelectors[id].y_gui, RSelectors[id].w_gui, RSelectors[id].h_gui, SWP_DRAWFRAME);
	ShowWindow(RSelectors[id].m_hwnd, SW_SHOW);  // 显示窗口
//	hThreads_tmp.push_back(RSelectors[id].hThread);
	return RSelectors[id].m_hwnd;
}




HWND set_RSelector(int id, int x_gui, int y_gui, int w_gui, int h_gui, int r, int g, int b, int thickness){
//	PARAM_create_RS* pParams = new PARAM_create_RS{x_gui, y_gui, w_gui, h_gui, r, g, b, thickness};
//	hThreadRS = CreateThread(NULL, 0, ThreadWrapper_create_RS, pParams, 0, NULL);
	RSelectors[id].set(x_gui, y_gui, w_gui, h_gui, r, g, b, thickness);
//	while (RSelectors[id].m_hwnd==0){}   // 可能有用
//	SetWindowPos(RSelectors[id].m_hwnd, NULL, RSelectors[id].x_gui, RSelectors[id].y_gui, RSelectors[id].w_gui, RSelectors[id].h_gui, SWP_SHOWWINDOW);
	SetWindowPos(RSelectors[id].m_hwnd, NULL, RSelectors[id].x_gui, RSelectors[id].y_gui, RSelectors[id].w_gui, RSelectors[id].h_gui, SWP_DRAWFRAME);
	ShowWindow(RSelectors[id].m_hwnd, SW_SHOW);  // 显示窗口
	return RSelectors[id].m_hwnd;
}


struct ThreadData {
	HWND* target_hwnd;
	DWORD thread_id;
};


ThreadData Data1;
ThreadData Data2;


LRESULT CALLBACK WindowProc1(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int Nc=256;
	char EdtText[Nc];
	int x_gui=1100,y_gui=200,w_gui=200,h_gui=100;
	int r=220, g=55, b=22;
	int thickness=3;
	switch (uMsg) {
	case WM_OUT_THREAD_MSG:
		std::cout << "Window1 received: " << wParam << std::endl;
		return 0;
	case WM_COMMAND:{
		switch(LOWORD(wParam)){
			case id_Btn1a:{
				SendMessage(HWedt1, WM_GETTEXT, Nc, (LPARAM)EdtText);
				fun_set_params_RS(EdtText,x_gui, y_gui, w_gui, h_gui, r, g, b, thickness);
				HWND hwRS1=threadCreate_RS(0, x_gui, y_gui, w_gui, h_gui, r, g, b, thickness);
				return 0;
			}case id_Btn1b:{
				PostThreadMessage(RSelectors[0].thread_id, WM_MY_QUIT, 9, 0);
				WaitForSingleObject(RSelectors[0].hThread, 1000); // INFINITE
				return 0;
			}
		}
		return 0;
	}
	case WM_CREATE:
        SetEvent(hWindowReadyEvent); // 窗口创建完成时触发事件
        return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI Thread1Func(LPVOID lpParam) {
	ThreadData* data = static_cast<ThreadData*>(lpParam);
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc1;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = "WindowClass1";
	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(WS_EX_TOPMOST, "WindowClass1", "frame1", WS_OVERLAPPEDWINDOW, 1100, 200, 450, 100, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
	*data->target_hwnd = hwnd;
    HWbtn1a = CreateWindow("button", "create", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
									10,10,50,30, hwnd, (HMENU)id_Btn1a, 0, NULL);
    HWbtn1b = CreateWindow("button", "smash", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
									65,10,50,30, hwnd, (HMENU)id_Btn1b, 0, NULL);
	HWedt1=CreateWindow("EDIT", "1300,400,100,50,20,155,210,3", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT,
									120,10,300,30, hwnd, (HMENU)id_Edt1, 0, 0);
	ShowWindow(hwnd, SW_SHOW);
    RegisterHotKey(hwnd,5, MOD_NOREPEAT, VK_F8);  //0x1B is Esc
	int k=3;
	MSG msg;
	while(true){
		if (!GetMessage(&msg,nullptr,0,0)){
//			PostThreadMessage(Data2.thread_id, WM_QUIT, 9, 0);
			PostMessage(*Data2.target_hwnd, WM_QUIT, 7, 0);
			break;
		}
		if (!IsDialogMessage(hwnd, &msg)) {  // to make WS_TABSTOP work, but make WM_CHAR not work
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_HOTKEY){
			if (msg.wParam==5){
				PostQuitMessage(0);
			}
		}else if (k>0 && msg.message == WM_CROSS_THREAD_MSG) {
//			PostMessage(hwnd, WM_OUT_THREAD_MSG, msg.wParam, 0);
//			msg.message=WM_MSG_NULL;
			k--;
			printf("-----1-----\t[%d]\t[%d\t%d]\n",k,msg.message,msg.wParam);
		}

	}
	return 0;
}


LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int Nc=256;
	char EdtText[Nc];
	int x_gui=1000,y_gui=200,w_gui=200,h_gui=100;
	int r=20, g=55, b=225;
	int thickness=3;
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:{
		switch(LOWORD(wParam)){
			case id_Btn2a:{
				SendMessage(HWedt2, WM_GETTEXT, Nc, (LPARAM)EdtText);
//				SendMessage(HWedt1, WM_SETTEXT, 0, (LPARAM)EdtText);
				fun_set_params_RS(EdtText,x_gui, y_gui, w_gui, h_gui, r, g, b, thickness);
				HWND hwRS2=threadCreate_RS(1, x_gui, y_gui, w_gui, h_gui, r, g, b, thickness);
				return 0;
			}case id_Btn2b:{
//				SendMessage(HWedt2, WM_GETTEXT, Nc, (LPARAM)EdtText);
				PostThreadMessage(RSelectors[1].thread_id, WM_MY_QUIT, 9, 0);
				return 0;
			}
		}
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


DWORD WINAPI Thread2Func(LPVOID lpParam) {
	ThreadData* data = static_cast<ThreadData*>(lpParam);
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc2;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = "WindowClass2";
	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(WS_EX_TOPMOST, "WindowClass2", "frame2", WS_OVERLAPPEDWINDOW, 1100, 300, 450, 100, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
	*data->target_hwnd = hwnd;
    HWbtn2a = CreateWindow("button", "create", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
									10,10,50,30, hwnd, (HMENU)id_Btn2a, 0, NULL);
    HWbtn2b = CreateWindow("button", "smash", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
									65,10,50,30, hwnd, (HMENU)id_Btn2b, 0, NULL);
	HWedt2=CreateWindow("EDIT", "1100,400,100,50,20,155,10,3", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT,
									120,10,300,30, hwnd, (HMENU)id_Edt2, 0, 0);
	ShowWindow(hwnd, SW_SHOW);
    RegisterHotKey(hwnd,5, MOD_NOREPEAT, VK_F9);  //0x1B is Esc
	MSG msg;
//	while(GetMessage(&msg,nullptr,0,0)){
	while(true){
		if (!GetMessage(&msg,nullptr,0,0)){
//			PostThreadMessage(Data1.thread_id, WM_QUIT, 9, 0);
			PostMessage(*Data1.target_hwnd, WM_QUIT, 7, 0);
			break;
		}
		if (!IsDialogMessage(hwnd, &msg)) {  // to make WS_TABSTOP work, but make WM_CHAR not work
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_HOTKEY){
			if (msg.wParam==5){
				PostQuitMessage(0);
			}
		}else if (msg.message == WM_OUT_THREAD_MSG) {
			PostThreadMessage(Data1.thread_id, WM_CROSS_THREAD_MSG, 13, 0);
		}
	}
	return 0;
}



int test_1(int x){
	int y=x*10;
	printf("------[%d]------\n",y);
	return y;
}





int main() {
	hWindowReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    
	HWND hwnd1 = nullptr, hwnd2 = nullptr;
	Data1={&hwnd1,0};
	Data2={&hwnd2,0};

	HANDLE hThread1 = CreateThread(nullptr, 0, Thread1Func, &Data1, 0, &Data1.thread_id);
	HANDLE hThread2 = CreateThread(nullptr, 0, Thread2Func, &Data2, 0, &Data2.thread_id);

    WaitForSingleObject(hWindowReadyEvent, INFINITE);  // wait until the window1 is created
	while (*Data2.target_hwnd==0){}  // wait until the window2 is created
//	WaitForSingleObject(hThread1, 1000);
	PostThreadMessage(Data2.thread_id, WM_OUT_THREAD_MSG, 13, 0);
	PostMessage(hwnd1, WM_OUT_THREAD_MSG, 111, 0);
	PostMessage(hwnd1, WM_OUT_THREAD_MSG, 222, 0);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hThread1);
	CloseHandle(hThread2);


//	printf("dwResult:\t");
//	for (auto& x:hThreads_tmp){
//		DWORD dwResult = WaitForSingleObject(x, 0);
//		printf("%3d\t", dwResult);
//	}
	return 0;
}



int main1(){
	int x_gui=1000,y_gui=200,w_gui=200,h_gui=100;
	int r=220, g=55, b=5;
	int thickness=2;
	HWND hwRS2=threadCreate_RS(0, x_gui, y_gui, w_gui, h_gui, 20, 220, 10, thickness);
//
//	SetWindowPos(RSelectors[0].m_hwnd, NULL, x_gui, y_gui, w_gui, h_gui, SWP_SHOWWINDOW);

//	WaitForSingleObject(hThreadRS, 1000);
//	while (RSelectors[0].m_hwnd==0){}
	printf("[%x]--------2---\n",RSelectors[0].m_hwnd);
	HANDLE hThreadRS2=GetThreadHandleFromWindow(RSelectors[0].m_hwnd);
	printf("<%x>\t(%x,%x)\n",hThreadRS2,hwRS2,RSelectors[0].m_hwnd);
//	WaitForSingleObject(hThreadRS2, INFINITE);
	MessageBox(NULL, "hello", "good", MB_OK|MB_TOPMOST);
	return 0;
}




#endif /* __REGION_SELECTOR_H__ */
