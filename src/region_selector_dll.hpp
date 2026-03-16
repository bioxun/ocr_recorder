/*

*/
#pragma once


#include <windows.h>
#include <uxtheme.h>
#include <string>
using namespace std;


//#define MyAPI __declspec(dllexport)

#define WM_MY_QUIT 4001

const int BORDER_WIDTH = 2;
//const COLORREF colorKey=RGB(200, 228, 0);
const COLORREF colorKey=RGB(20, 28, 220);

struct PARAM_create_RS {
	int x_gui, y_gui, w_gui, h_gui, r, g, b, thickness;
};

class RegionSelector{
public:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	RegionSelector();
	~RegionSelector();
	void set(int x, int y, int w, int h, int r, int g, int b, int thick);
	int CreateRun();
	HANDLE threadCreate();
	std::string wcClassName="myRegionSelector";
	HINSTANCE hInstance;
	HWND m_hwnd=0;
	HANDLE hThread=0;
	DWORD thread_id=-1;
	COLORREF color_border=RGB(181,230,29);
	COLORREF color_border2=RGB(50,100,255);
	int thickness_border=3;
	int x_gui=1000,y_gui=200,w_gui=300,h_gui=200;
private:
	HDC m_hdc;
	HPEN m_hPen;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	int m_start=0;
};


struct PARAM_RS {
	RegionSelector* pRS;
};

void RegionSelector::set(int x, int y, int w, int h, int r, int g, int b, int thick){
	if (x>=0){
		x_gui=x;
	}
	if (y>=0){
		y_gui=y;
	}
	if (w>0){
		w_gui=w;
	}
	if (h>0){
		h_gui=h;
	}
	if (thick>0){
		thickness_border=thick;
	}
	if (r>=0 && g>=0 && b>=0){
		r=r>255 ? 255:r;
		g=g>255 ? 255:g;
		b=b>255 ? 255:b;
		color_border=RGB(r,g,b);
	}
}

std::string func_get_title_by_hwnd(HWND hwnd){
	char wnd_title[512];
	GetWindowText(hwnd,wnd_title,sizeof(wnd_title));
	return std::string(wnd_title);
}

void printf_rect(RECT X, const char* head){
	printf("%s\t(%d,%d)\t(%d,%d)\n",head, X.left,X.top,X.right,X.bottom);
}


void func_print_hw_related(HWND HW, std::string tag){
	std::string title=func_get_title_by_hwnd(HW);
	DWORD* pID = new DWORD;
	GetWindowThreadProcessId(HW, pID);
//	printf("%x<%d>[0x%02x][%s]\n",*pID,*pID,HW,title.c_str());
	printf("%s%x<%d>[0x%02x][%d]\t",tag.c_str(),*pID,*pID,HW,HW);
	RECT rect;
	GetWindowRect(HW, &rect);
	printf_rect(rect,title.c_str());
}


LRESULT CALLBACK RegionSelector::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	RegionSelector* pThis = nullptr;
	if (msg == WM_NCCREATE) {
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = reinterpret_cast<RegionSelector*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->m_hwnd = hwnd;
	} else {
		pThis = reinterpret_cast<RegionSelector*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	if (pThis) {
		return pThis->HandleMessage(msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

RegionSelector::RegionSelector() {
//	hThread=threadCreate();
}


RegionSelector::~RegionSelector() {
	if (m_hPen) DeleteObject(m_hPen);
}


int RegionSelector::CreateRun(){
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(colorKey);
	wc.lpszClassName = wcClassName.c_str();
//	printf("{%s}\n",wc.lpszClassName);
	hInstance=wc.hInstance;
	if (!RegisterClassEx(&wc)) return 1;
	m_hwnd = CreateWindowEx( WS_EX_TOPMOST | WS_EX_TOOLWINDOW, wc.lpszClassName, "bioxun's region selector", 
			WS_SIZEBOX , x_gui,y_gui,w_gui,h_gui, nullptr, nullptr, GetModuleHandle(nullptr), this);
	if (!m_hwnd) return 1;
	UpdateWindow(m_hwnd);
//	RegisterHotKey(m_hwnd, 5, MOD_NOREPEAT, VK_ESCAPE);  //0x1B is Esc
//	RegisterHotKey(m_hwnd, 4, MOD_CONTROL|MOD_NOREPEAT, 0xDC);  //0xDC is '\'
//	RegisterHotKey(m_hwnd, 3, MOD_CONTROL|MOD_NOREPEAT, VK_UP);  //
	MSG msg;
	while(GetMessage(&msg,nullptr,0,0)){
		if (!IsDialogMessage(m_hwnd, &msg)) {  // to make WS_TABSTOP work, but make WM_CHAR not work
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_HOTKEY){
//			if (msg.wParam==5){
////				PostQuitMessage(0);
//				DestroyWindow(m_hwnd);
//				UnregisterClass(wc.lpszClassName, wc.hInstance);
//				m_hwnd=0;
//			}else if (msg.wParam==4){
//				ShowWindow(m_hwnd, SW_HIDE);  // 隐藏窗口
//			}else if (msg.wParam==3){
//				ShowWindow(m_hwnd, SW_SHOW);  // 显示窗口
//			}
		}else if (msg.message == WM_MY_QUIT){
			DestroyWindow(m_hwnd);
			UnregisterClass(wc.lpszClassName, wc.hInstance);
			m_hwnd=0;
//			TerminateThread(hThread, 0);  // 强制终止，退出码为0
			CloseHandle(hThread);         // 关闭句柄
		}
	}
	return 0;
}


DWORD WINAPI RS_WCreate(LPVOID lpParam) {
	PARAM_RS* params = static_cast<PARAM_RS*>(lpParam);
	params->pRS->CreateRun();
	return 0;
}


HANDLE RegionSelector::threadCreate(){
	PARAM_RS* pParams = new PARAM_RS{this};
//	if (m_hwnd==0){
//		hThread = CreateThread(NULL, 0, RS_WCreate, pParams, 0, &thread_id);
//		while (m_hwnd==0){}
//	}
	while (m_hwnd==0){
		hThread = CreateThread(NULL, 0, RS_WCreate, pParams, 0, &thread_id);
	}
	printf("(%x)\t[%d]\t<%x>---123---\n",m_hwnd,thread_id,hThread);
	return hThread;
}


LRESULT RegionSelector::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	RECT rect;
	HPEN pen;
	switch (uMsg) {
		case WM_CREATE:{
//			SetWindowTheme(m_hwnd, L"", L"");
			SetWindowLong(m_hwnd, GWL_EXSTYLE, GetWindowLong(m_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED); // 允许边缘拖拽
			SetLayeredWindowAttributes(m_hwnd, colorKey, 180, LWA_COLORKEY);   // 用于设置透明色
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_NCCALCSIZE:{
			LPNCCALCSIZE_PARAMS ncParams = (LPNCCALCSIZE_PARAMS)lParam;
			ncParams->rgrc[0].top += thickness_border;
			ncParams->rgrc[0].left += thickness_border;
			ncParams->rgrc[0].bottom -= thickness_border;
			ncParams->rgrc[0].right -= thickness_border;
			return 0;
		}
		case WM_NCPAINT:{
			GetWindowRect(m_hwnd, &rect);
			HRGN region = NULL;
			if (wParam == NULLREGION) {
				region = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
			}else {
				HRGN copy = CreateRectRgn(0, 0, 0, 0);
				if (CombineRgn(copy, (HRGN)wParam, NULL, RGN_COPY)) {
					region = copy;
				}
				else {
					DeleteObject(copy);
				}
			}
			m_hdc = GetDCEx(m_hwnd, region, DCX_WINDOW | DCX_CACHE | DCX_INTERSECTRGN | DCX_LOCKWINDOWUPDATE);
//			m_hdc = GetDC(m_hwnd);
			if (!m_hdc && region) {
				DeleteObject(region);
			}
			HPEN pen = CreatePen(PS_INSIDEFRAME, thickness_border, color_border);
			HGDIOBJ old = SelectObject(m_hdc, pen);
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			Rectangle(m_hdc, 0, 0, width, height);
			SelectObject(m_hdc, old);
			ReleaseDC(m_hwnd, m_hdc);
			DeleteObject(pen);
			return 0;
		}
		case WM_NCACTIVATE:
			RedrawWindow(m_hwnd, NULL, NULL, RDW_UPDATENOW);
			return 0;
		break;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}


