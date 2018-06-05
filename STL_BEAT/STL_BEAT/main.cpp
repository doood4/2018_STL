//  유니코드 기반입니다.
#include <Windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <fstream>
#include <string>
#include <atlimage.h>	
#include <gdiplus.h>	
#include "note.h"
#include "sound.h"

using namespace std;
using namespace Gdiplus;

#pragma commnet(lib,"Gdiplus.lib")

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000 ? 1 : 0))

#define MAX_LOADSTRING 100

#define CLIENT_WIDTH 480		
#define CLIENT_HEIGHT 640		

#define WINTITLE	TEXT("STL BEAT")		
#define WINCLASSNAME	TEXT("winMain")		

// 게임 STATE 구분
#define TITLE_STATE		1
#define GAME_STATE		2
#define LOAD_STATE		3
#define	FINISH_STATE	4
#define REPLAY_STATE	5

// 타이머 시간
#define TIMER 20

HINSTANCE hInst;							
TCHAR szTitle[MAX_LOADSTRING];				
TCHAR szWindowClass[MAX_LOADSTRING];								
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,
						_In_ LPTSTR    lpCmdLine,_In_ int       nCmdShow) {

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;

	GdiplusStartupInput         m_GdiplusStartupInput;
	ULONG_PTR                   m_GdiplusToken;
	GdiplusStartup(&m_GdiplusToken, &m_GdiplusStartupInput, NULL);

	wsprintf(szTitle, WINTITLE);
	wsprintf(szWindowClass, WINCLASSNAME);

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style =
		  CS_HREDRAW 	
		| CS_VREDRAW	;
	wcex.lpfnWndProc = WndProc;	
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;	

	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {

	HWND hWnd;

	//	윈도우 스타일
	DWORD dwStyle =
  WS_OVERLAPPED 	
		| WS_CAPTION 		
		| WS_SYSMENU		
		| WS_MINIMIZEBOX	
		| WS_BORDER	;					

						
	hInst = hInstance; 

	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	AdjustWindowRect(&rc, dwStyle, FALSE);

	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - rc.right) / 2;
	ptClientWorld.y = (getWinSize.bottom - rc.bottom) / 2;

	//	윈도우 생성
	hWnd = CreateWindow(szWindowClass, szTitle, dwStyle,
		ptClientWorld.x, ptClientWorld.y, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL);


	if (!hWnd)	return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

// 글로벌 변수들
UINT STATE;
UINT CONTROLER;

UINT savedata_count;

vector<Note> v_notes;
vector<Note>::iterator it;

//  <시간, 버튼>
multimap<int, char> ic_map;		// 플레이한 data
multimap<int, char> play_map;	// 재생하는 곡 data

CImage g_cimgBackBuff;
CImage g_background;

TCHAR 	score_buff[20];
TCHAR   time_buff[20];
TCHAR   perfect_buff[20];
TCHAR   good_buff[20];
TCHAR   miss_buff[20];

UINT	g_score = 0;
UINT	g_time = 0;
UINT	judge = 0; // 1 : perfect , 2 : good , 3 : miss
UINT	perfect_count = 0;
UINT	good_count = 0;
UINT	miss_count = 0;

bool g_keyS = false;				
bool hit_S = false;
bool g_keyT = false;				
bool hit_T = false;
bool g_keyL = false;				
bool hit_L = false;


void press_key(char key_type) {
	
	if (key_type == 'S') {
		do_sound();
		g_keyS = true;
		for (it = v_notes.begin(); it != v_notes.end();) {
			if (it->getx() == 30 && 560 <= it->gety() + 20 && it->gety() + 20 <= 570) {
				hit_S = true;
				g_score += 100;
				perfect_count++;
				judge = 1;
				v_notes.erase(it);
			}
			else if (it->getx() == 30 && ((540 <= it->gety() + 20 && it->gety() + 20 < 560) || (570 < it->gety() + 20 && it->gety() + 20 <= 580))) {
				hit_S = true;
				g_score += 30;
				good_count++;
				judge = 2;
				v_notes.erase(it);
			}
			else if (it->getx() == 30 && (580 < it->gety() + 20 )) {
				hit_S = true;
				g_score += 0;
				miss_count++;
				judge = 3;
				v_notes.erase(it);
			}
			++it;
		}
	}
	else if (key_type == 'T') {
		mi_sound();
		g_keyT = true;
		for (it = v_notes.begin(); it != v_notes.end();) {
			if (it->getx() == 130 && 560 <= it->gety() + 20 && it->gety() + 20 <= 570) {
				hit_T = true;
				g_score += 100;
				perfect_count++;
				judge = 1;
				v_notes.erase(it);
			}
			else if (it->getx() == 130 && ((540 <= it->gety() + 20 && it->gety() + 20 < 560) || (570 < it->gety() + 20 && it->gety() + 20 <= 580))) {
				hit_T = true;
				g_score += 30;
				good_count++;
				judge = 2;
				v_notes.erase(it);
			}
			else if (it->getx() == 130 && (580 < it->gety() + 20)) {
				hit_T = true;
				g_score += 0;
				miss_count++;
				judge = 3;
				v_notes.erase(it);
			}
			++it;
		}
	}
	else if (key_type == 'L') {
		sol_sound();
		g_keyL = true;
		for (it = v_notes.begin(); it != v_notes.end();) {
			if (it->getx() == 230 && 560 <= it->gety() + 20 && it->gety() + 20 <= 570) {
				hit_L = true;
				g_score += 100;
				perfect_count++;
				judge = 1;
				v_notes.erase(it);
			}
			else if (it->getx() == 230 && ((540 <= it->gety() + 20 && it->gety() + 20 < 560) || (570 < it->gety() + 20 && it->gety() + 20 <= 580))) {
				hit_L = true;
				g_score += 30;
				good_count++;
				judge = 2;
				v_notes.erase(it);
			}
			else if (it->getx() == 230 && (580 < it->gety() + 20)) {
				hit_L = true;
				g_score += 0;
				miss_count++;
				judge = 3;
				v_notes.erase(it);
			}
			++it;
		}
	}
}

// 저장 함수
void save_data(multimap<int,char>& m) {
	if (savedata_count < 3) {
		string name1, number, datatype;
		name1 = "data";
		number = to_string(++savedata_count);
		datatype = ".txt";
		ofstream out(name1 + number + datatype);
		for (auto p = m.begin(); p != m.end(); ++p) {
			out << p->first << '\t' << p->second << endl;
		}
		out.close();
	}
	else if (savedata_count == 3) {
		string name1, number, datatype;
		name1 = "data";
		number = to_string(savedata_count);
		datatype = ".txt";
		ofstream out(name1 + number + datatype);
		for (auto p = m.begin(); p != m.end(); ++p) {
			out << p->first << '\t' << p->second << endl;
		}
		out.close();
	}
}

// 로드 함수
void load_data(multimap<int, char>& m,int num)  {
	string name1,number,datatype;
	name1 = "data";
	number = to_string(num);
	datatype = ".txt";
	ifstream in(name1 + number + datatype);
	int time;
	char key;
	while (!in.eof()) {
		in >> time >> key;
		m.emplace(time, key);
	}
	for (auto p = m.begin(); p != m.end(); ++p) {
		cout << p->first << ' ' << p->second << endl;
	}
	in.close();
}

void Init_savedata_count() {
	ifstream in("savedata_count.txt");
	int count;
	while (!in.eof()) {
		in >> count;
		if (count <= 3) {
			savedata_count = count;
		}
	}
	in.close();
}

void Save_savedata_count() {
	ofstream out("savedata_count.txt");
	out << savedata_count << endl;
	out.close();
}

// I 클릭시 초기화
void Init_game(HWND hWnd) {
	g_time = 0;
	judge = 0;
	g_score = 0;
	perfect_count = 0;
	good_count = 0;
	miss_count = 0;
	v_notes.clear();
	Init_Notes(v_notes);
	KillTimer(hWnd, 1);
	KillTimer(hWnd, 2);
}

// TITLE _STATE
void Draw_Title(HWND hWnd)
{
	// 윈도우 전체 크기
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HDC memDC = g_cimgBackBuff.GetDC();

	Graphics graphics(memDC);

	Image		background(L"background_title.png");
	Image		title_menu(L"title_menu.png");
	Image       title(L"stl_beat.png");


	{
		// 초기화
		FillRect(memDC, &rcClient, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		// 배경 
		graphics.DrawImage(&background, 0, 0, 480, 640);

		// STL BEAT
		graphics.DrawImage(&title, 40, 80, 400, 80);

		// NEW
		if (CONTROLER == 1) {
			graphics.DrawImage(&title_menu, Rect(160, 340, 160, 50), 0, 0, 200, 50, UnitPixel);
		}
		else {
			graphics.DrawImage(&title_menu, Rect(160, 340, 160, 50), 200, 0, 200, 50, UnitPixel);
		}

		// LOAD
		if (CONTROLER == 2) {
			graphics.DrawImage(&title_menu, Rect(160, 440, 160, 50), 0, 50, 200, 50, UnitPixel);
		}
		else {
			graphics.DrawImage(&title_menu, Rect(160, 440, 160, 50), 200, 50, 200, 50, UnitPixel);
		}

		// EXIT
		if (CONTROLER == 3) {
			graphics.DrawImage(&title_menu, Rect(160, 540, 160, 50), 0, 100, 200, 50, UnitPixel);
		}
		else {
			graphics.DrawImage(&title_menu, Rect(160, 540, 160, 50), 200, 100, 200, 50, UnitPixel);
		}

		g_cimgBackBuff.Draw(hdc, 0, 0);
		g_cimgBackBuff.ReleaseDC();
		EndPaint(hWnd, &ps);
	}
}

// GAME_STATE and REPLAY_STATE
void Draw_InGame(HWND hWnd)
{
	// 윈도우 전체 크기
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HDC memDC = g_cimgBackBuff.GetDC();

	Graphics graphics(memDC);

	Image		background(L"background.png");
	Image		judge_letter(L"judge.png");

	Image       light(L"light.png");
	Image       frame(L"frame.png");
	Image		note(L"note.png");

	Image		red(L"red.png");
	Image		yellow(L"yellow.png");
	Image		green(L"green.png");

	Image		button(L"stl_button.png");

	{
		// 초기화
		FillRect(memDC, &rcClient, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		// 배경 
		graphics.DrawImage(&background, 0, 0, 480, 640);

		// 프레임
		graphics.DrawImage(&frame, 0, 0, frame.GetWidth(), frame.GetHeight());


		// 점수 출력
		HFONT myFont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, 0);
		HFONT oldFont = (HFONT)SelectObject(memDC, myFont);
		wsprintf(score_buff, TEXT("SCORE: %d"), g_score);
		TextOut(memDC, 340, 100, score_buff, lstrlen(score_buff));

		// 카운트들
		wsprintf(perfect_buff, TEXT("PERFECT: %d"), perfect_count);
		TextOut(memDC, 340, 200, perfect_buff, lstrlen(perfect_buff));

		wsprintf(good_buff, TEXT("GOOD: %d"), good_count);
		TextOut(memDC, 340, 250, good_buff, lstrlen(good_buff));

		wsprintf(miss_buff, TEXT("MISS: %d"), miss_count);
		TextOut(memDC, 340, 300, miss_buff, lstrlen(miss_buff));


		//// 시간 출력
		//wsprintf(time_buff, TEXT("%d"), int(g_time));
		//TextOut(memDC, 360, 500, time_buff, lstrlen(time_buff));

		// 버튼 
		graphics.DrawImage(&button, Rect(30, 580, 50, 50), 0, 0, 50, 50, UnitPixel);		// S
		graphics.DrawImage(&button, Rect(130, 580, 50, 50), 50, 0, 50, 50, UnitPixel);		// T
		graphics.DrawImage(&button, Rect(230, 580, 50, 50), 100, 0, 50, 50, UnitPixel);		// L


																							// 노트 그리기
		for (auto i = v_notes.begin(); i != v_notes.end(); ++i) {
			if (i->gety() >= 0) {
				graphics.DrawImage(&note, i->getx() - 5, i->gety(), 60, 40);
			}
		}

		// 키입력 이펙트
		if (g_keyS) {
			graphics.DrawImage(&light, 0, 0, 100, 570);
		}
		if (g_keyT) {
			graphics.DrawImage(&light, 100, 0, 100, 570);
		}
		if (g_keyL) {
			graphics.DrawImage(&light, 200, 0, 100, 570);
		}

		// 히트 이펙트
		if (hit_S) {
			switch (judge) {
			case 1:
				graphics.DrawImage(&green, 0, 0, 100, 570);
				break;
			case 2:
				graphics.DrawImage(&yellow, 0, 0, 100, 570);
				break;
			case 3:
				graphics.DrawImage(&red, 0, 0, 100, 570);
				break;
			}
		}
		if (hit_T) {
			switch (judge) {
			case 1:
				graphics.DrawImage(&green, 100, 0, 100, 570);
				break;
			case 2:
				graphics.DrawImage(&yellow, 100, 0, 100, 570);
				break;
			case 3:
				graphics.DrawImage(&red, 100, 0, 100, 570);
				break;
			}
		}
		if (hit_L) {
			switch (judge) {
			case 1:
				graphics.DrawImage(&green, 200, 0, 100, 570);
				break;
			case 2:
				graphics.DrawImage(&yellow, 200, 0, 100, 570);
				break;
			case 3:
				graphics.DrawImage(&red, 200, 0, 100, 570);
				break;
			}
		}

		// 판정선 540
		graphics.DrawImage(&note, 0, 540, 310, 40);


		// judge 판정
		if (judge == 1) {
			graphics.DrawImage(&judge_letter, Rect(60, 100, 180, 40), 0, 0, 200, 50, UnitPixel);
		}
		else if (judge == 2) {
			graphics.DrawImage(&judge_letter, Rect(60, 100, 180, 40), 0, 50, 200, 50, UnitPixel);
		}
		else if (judge == 3) {
			graphics.DrawImage(&judge_letter, Rect(60, 100, 180, 40), 0, 100, 200, 50, UnitPixel);
		}


		SelectObject(memDC, oldFont);
		DeleteObject(myFont);
	}

	g_cimgBackBuff.Draw(hdc, 0, 0);
	g_cimgBackBuff.ReleaseDC();
	EndPaint(hWnd, &ps);
}

// LOAD_STATE
void Draw_Load(HWND hWnd) {
	// 윈도우 전체 크기
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HDC memDC = g_cimgBackBuff.GetDC();

	Graphics graphics(memDC);

	Image		background(L"background_title.png");
	Image		load_menu(L"load_menu.png");


	{
		// 초기화
		FillRect(memDC, &rcClient, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		// 배경 
		graphics.DrawImage(&background, 0, 0, 480, 640);

		switch (savedata_count) {
		case 0:
			if (CONTROLER == 1) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 0, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);

			}
			else if (CONTROLER == 2) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 0, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);

			}
			else if (CONTROLER == 3) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 0, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);

			}
			else if (CONTROLER == 4) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 0, 200, 200, 50, UnitPixel);

			}
			break;
		case 1:
			if (CONTROLER == 1) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 0, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 2) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 0, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 3) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 0, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 4) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 0, 200, 200, 50, UnitPixel);
			}
			break;
		case 2:
			if (CONTROLER == 1) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 0, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 50, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 2) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 0, 50, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 3) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 50, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 0, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 4) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 50, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 150, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 0, 200, 200, 50, UnitPixel);
			}
			break;
		case 3:
			if (CONTROLER == 1) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 0, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 50, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 100, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 2) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 0, 50, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 100, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 3) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 50, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 0, 100, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 200, 200, 200, 50, UnitPixel);
			}
			else if (CONTROLER == 4) {
				graphics.DrawImage(&load_menu, Rect(40, 80, 400, 60), 200, 0, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 230, 400, 60), 200, 50, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(40, 380, 400, 60), 200, 100, 200, 50, UnitPixel);
				graphics.DrawImage(&load_menu, Rect(140, 530, 200, 40), 0, 200, 200, 50, UnitPixel);
			}
			break;
		}

		g_cimgBackBuff.Draw(hdc, 0, 0);
		g_cimgBackBuff.ReleaseDC();
		EndPaint(hWnd, &ps);
	}
}

// FINISH_STATE
void Draw_Finish(HWND hWnd)
{
	// 윈도우 전체 크기
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);


	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HDC memDC = g_cimgBackBuff.GetDC();

	Graphics graphics(memDC);

	Image		background(L"background.png");
	Image		black_out(L"black_out.png");

	Image		finish_menu(L"finish_menu.png");
	Image       replay_white(L"replay_white.png");
	Image       replay_gray(L"replay_gray.png");
	Image       title_white(L"title_white.png");
	Image       title_gray(L"title_gray.png");

	Image       frame(L"frame.png");
	Image		note(L"note.png");

	Image		button(L"stl_button.png");

	{
		// 초기화
		FillRect(memDC, &rcClient, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		// 배경 

		graphics.DrawImage(&background, 0, 0, 480, 640);

		// 프레임
		graphics.DrawImage(&frame, 0, 0, frame.GetWidth(), frame.GetHeight());

		// 판정선 540
		graphics.DrawImage(&note, 0, 540, 310, 40);

		// 점수 출력
		HFONT myFont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, 0);
		HFONT oldFont = (HFONT)SelectObject(memDC, myFont);
		wsprintf(score_buff, TEXT("SCORE: %d"), g_score);
		TextOut(memDC, 340, 100, score_buff, lstrlen(score_buff));

		// 카운트들
		wsprintf(perfect_buff, TEXT("PERFECT: %d"), perfect_count);
		TextOut(memDC, 340, 200, perfect_buff, lstrlen(perfect_buff));

		wsprintf(good_buff, TEXT("GOOD: %d"), good_count);
		TextOut(memDC, 340, 250, good_buff, lstrlen(good_buff));

		wsprintf(miss_buff, TEXT("MISS: %d"), miss_count);
		TextOut(memDC, 340, 300, miss_buff, lstrlen(miss_buff));

		

		// 버튼 
		graphics.DrawImage(&button, Rect(30, 580, 50, 50), 0, 0, 50, 50, UnitPixel);		// S
		graphics.DrawImage(&button, Rect(130, 580, 50, 50), 50, 0, 50, 50, UnitPixel);		// T
		graphics.DrawImage(&button, Rect(230, 580, 50, 50), 100, 0, 50, 50, UnitPixel);		// L

		// 블랙아웃
		graphics.DrawImage(&black_out, 0, 0, 480, 640);


		// REPLAY
		if (CONTROLER == 1) {
			graphics.DrawImage(&finish_menu, Rect(120, 350, 240, 60), 0, 0, 200, 50, UnitPixel);
		}
		else {
			graphics.DrawImage(&finish_menu, Rect(120, 350, 240, 60), 200, 0, 200, 50, UnitPixel);
		}

		// TITLE
		if (CONTROLER == 2) {
			graphics.DrawImage(&finish_menu, Rect(120, 470, 240, 60), 0, 50, 200, 50, UnitPixel);
		}
		else {
			graphics.DrawImage(&finish_menu, Rect(120, 470, 240, 60), 200, 50, 200, 50, UnitPixel);
		}

		g_cimgBackBuff.Draw(hdc, 0, 0);
		g_cimgBackBuff.ReleaseDC();
		EndPaint(hWnd, &ps);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	#pragma region 초기화
	case WM_CREATE:
		setup_sound();
		title_bgm_on();
		STATE = TITLE_STATE; 
		g_time = 0;
		CONTROLER = 1;

		// 세이브데이타 갯수 설정
		Init_savedata_count();

		// 백 버퍼 생성
		g_cimgBackBuff.Create(CLIENT_WIDTH, CLIENT_HEIGHT, 24);

		break;		
	#pragma endregion


	#pragma region 타이머.
	case WM_TIMER:
		switch (wParam) {
		case 1: // GAME_STATE
			g_time++;

			for (auto i = v_notes.begin(); i != v_notes.end();++i) {
				i->down_note();
				if (i->gety() >= 580) {
					if (i->getx() == 30) {
						hit_S = true;
						g_score += 0;
						miss_count++;
						judge = 3;
						v_notes.erase(i);
					}
					else if (i->getx() == 130) {
						hit_T = true;
						g_score += 0;
						miss_count++;
						judge = 3;
						v_notes.erase(i);
					}
					else if (i->getx() == 230) {
						hit_L = true;
						g_score += 0;
						miss_count++;
						judge = 3;
						v_notes.erase(i);
					}
					
				}
			}

			if ((g_time % 10) == 0) {
				judge = 0;
				g_keyS = false;
				hit_S = false;
				g_keyT = false;
				hit_T = false;
				g_keyL = false;
				hit_L = false;
			}
			
			if (KEYDOWN(0x53)) { // S
				press_key('S');
				ic_map.emplace(g_time, 'S');
			}


			if (KEYDOWN(0x54)) { // T
				press_key('T');
				ic_map.emplace(g_time, 'T');
			}
			

			if (KEYDOWN(0x4C)) { // L
				press_key('L');
				ic_map.emplace(g_time, 'L');
			}

			if (g_time == 400) {
				title_bgm_on();
				save_data(ic_map);
				STATE = FINISH_STATE;
				KillTimer(hWnd, 1);
				v_notes.clear();
			}


			break;

		case 2: // REPLAY_STATE
			g_time++;

			for (auto i = v_notes.begin(); i != v_notes.end(); ++i) {
				i->down_note();
				if (i->gety() >= 580) {
					if (i->getx() == 30) {
						hit_S = true;
						g_score += 0;
						miss_count++;
						judge = 3;
						v_notes.erase(i);
					}
					else if (i->getx() == 130) {
						hit_T = true;
						g_score += 0;
						miss_count++;
						judge = 3;
						v_notes.erase(i);
					}
					else if (i->getx() == 230) {
						hit_L = true;
						g_score += 0;
						miss_count++;
						judge = 3;
						v_notes.erase(i);
					}

				}
			}

			auto p = play_map.begin();
			if (p->first == g_time) {
				while (p->first == g_time) {
					press_key(p->second);
					play_map.erase(p);
					++p;
				}
			}

			if ((g_time % 10) == 0) {
				judge = 0;
				g_keyS = false;
				hit_S = false;
				g_keyT = false;
				hit_T = false;
				g_keyL = false;
				hit_L = false;
			}

			if (g_time == 400) {
				title_bgm_on();
				STATE = FINISH_STATE;
				CONTROLER = 1;
				KillTimer(hWnd, 2);
				v_notes.clear();
				play_map.clear();
			}
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	#pragma endregion


	#pragma region 키입력.
	case WM_KEYDOWN:
		if (STATE == TITLE_STATE) {
			pFmod->update();
			if (GetAsyncKeyState(VK_DOWN)) {
				if (CONTROLER < 3) {
					CONTROLER++;
					move_sound();
				}
			}
			else if (GetAsyncKeyState(VK_UP)) {
				if (CONTROLER > 1) {
					CONTROLER--;
					move_sound();
				}
			}
			else if (GetAsyncKeyState(VK_SPACE)) {
				select_sound();
				if (CONTROLER == 1) { // NEW
					STATE = GAME_STATE;
					Init_game(hWnd);
					ic_map.clear();
					bgm_off();
					SetTimer(hWnd, 1, TIMER, NULL);
				}
				else if (CONTROLER == 2) { // LOAD
					STATE = LOAD_STATE;
					CONTROLER = 1;
					
				}
				else if (CONTROLER == 3) { // EXIT
					DestroyWindow(hWnd);
				}
			}
		}
		else if (STATE == LOAD_STATE) {
			if (GetAsyncKeyState(VK_DOWN)) {
				pFmod->update();
				if (CONTROLER < 4) {
					CONTROLER++;
					move_sound();
				}
			}
			if (GetAsyncKeyState(VK_UP)) {
				if (CONTROLER > 1) {
					CONTROLER--;
					move_sound();
				}
			}
			if (GetAsyncKeyState(VK_SPACE)) {
				select_sound();
				if (CONTROLER == 1) { 
					if (savedata_count > 0) {
						ic_map.clear();
						play_map.clear();
						load_data(ic_map,1);
						play_map.insert(ic_map.begin(), ic_map.end());
						Init_game(hWnd);
						SetTimer(hWnd, 2, TIMER, NULL);
						STATE = REPLAY_STATE;
						bgm_off();
					}
				}
				else if (CONTROLER == 2) { 
					if (savedata_count > 1) {
						ic_map.clear();
						play_map.clear();
						load_data(ic_map, 2);
						play_map.insert(ic_map.begin(), ic_map.end());
						Init_game(hWnd);
						SetTimer(hWnd, 2, TIMER, NULL);
						STATE = REPLAY_STATE;
						bgm_off();
					}
				}
				else if (CONTROLER == 3) { 
					if (savedata_count > 2) {
						ic_map.clear();
						play_map.clear();
						load_data(ic_map, 3);
						play_map.insert(ic_map.begin(), ic_map.end());
						Init_game(hWnd);
						SetTimer(hWnd, 2, TIMER, NULL);
						STATE = REPLAY_STATE;
						bgm_off();
					}
				}
				else if (CONTROLER == 4) {
					CONTROLER = 1;
					STATE = TITLE_STATE;
				}
			}
		}
		else if (STATE == FINISH_STATE) {
			pFmod->update();
			if (GetAsyncKeyState(VK_DOWN)) {
				if (CONTROLER < 2) {
					move_sound();
					CONTROLER++;
				}
			}
			else if (GetAsyncKeyState(VK_UP)) {
				if (CONTROLER > 1) {
					move_sound();
					CONTROLER--;
				}
			}
			else if (GetAsyncKeyState(VK_SPACE)) {
				select_sound();
				if (CONTROLER == 1) { // REPLAY
					play_map.clear();
					play_map.insert(ic_map.begin(), ic_map.end());
					Init_game(hWnd);
					SetTimer(hWnd, 2, TIMER, NULL);
					STATE = REPLAY_STATE;
					bgm_off();
				}
				else if (CONTROLER == 2) { // TITLE
					STATE = TITLE_STATE;
					CONTROLER = 1;
				}
			}
		}
		
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	#pragma endregion
	

	#pragma region 랜더링. 그리기 이외의 연산은 절대로 하지 말 것.
	case WM_PAINT:
		if (STATE == TITLE_STATE) {
			Draw_Title(hWnd);
		}
		else if (STATE == GAME_STATE) {
			Draw_InGame(hWnd);
		}
		else if (STATE == LOAD_STATE) {
			Draw_Load(hWnd);
		}
		else if (STATE == FINISH_STATE) {
			Draw_Finish(hWnd);
		}
		else if (STATE == REPLAY_STATE) {
			Draw_InGame(hWnd);
		}
		
		break;
	#pragma endregion


	#pragma region 소멸 시 호출. 윈도우 프로시저를 소멸하고 싶으면 DestroyWindow(hWnd)를 호출.
	case WM_DESTROY:
		Save_savedata_count();
		ic_map.clear();
		v_notes.clear();
		g_cimgBackBuff.Destroy();
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		break;
	#pragma endregion


	//	해당 윈도우 프로시저에서 정의되지 않은 다른 작업들을 처리합니다.
	//	해당 윈도우 프로시저에서 임의의 작업을 한 뒤 Windows API가 제공하는 기본 동작을 처리하고 싶을 때 호출 할 수도 있습니다.
	default: return DefWindowProc(hWnd, message, wParam, lParam);

	}

	return 0;
}
