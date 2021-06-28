#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#pragma comment(lib,"Comctl32.lib")


//Draw
#define ID_CHECKBOX_RECTANGLE 10
#define ID_CHECKBOX_ELLIPSE 11
//Color
#define ID_SCR_RED 20
#define ID_SCR_GREEN 21
#define ID_SCR_BLUE 22
//Toolbar
#define ID_TOOLBAR 30
#define ID_TOOLBAR_PEN 31
#define ID_TOOLBAR_CANCEL 32
//RadioButton
#define ID_RADIO_NORMAL 40
#define ID_RADIO_ZOOM 41
//TrackBar
#define ID_TRACKBAR 50
//ComboBox
#define ID_COMBOBOX 60
//COLOR
#define BKWHITE 0
#define BKGRAY 1
#define BKRED 2
#define BKGREEN 3
#define BKBLUE 4


int BkColorIndex(int nIndex);
//void CheckedReactangle(HWND hChckBox1, HWND hChckBox2, BOOL* bRectangle, BOOL* bEllipse);
//void CommboBoXSelChange(HWND hWnd, WPARAM wParam, int *nBkValu, char* arr);
//void CheckedEllipse(HWND hChckBox1, HWND hChckBox2, BOOL* bRectangle, BOOL* bEllipse);
//void HiddenScrollbar();
//void ShowScrollbar(HDC hdc);
//void SetController(HWND hWnd);
void FillBackground(HWND hWnd, HDC hdc, RECT rc, HBRUSH bkbrush);
void DrawPen(HWND hWnd, HDC hdc, LPARAM lParam);
//void WM_PaintFunction(HWND hWnd, HDC hdc, HDC MemDC, HBRUSH bkbrush, BOOL bRectangle, BOOL bEllipse, int nRed, int nGreen, int nBlue, int nBkIndex, int nRecX, RECT rc);


void CheckedEllipse(HWND* hChckBox1, HWND* hChckBox2, BOOL* bRectangle, BOOL* bEllipse);
void CheckedReactangle(HWND* hChckBox1, HWND *hChckBox2, BOOL* bRectangle, BOOL* bEllipse);
void HiddenScrollbar(HWND* hRed, HWND* hGreen ,HWND* hBlue);
void ShowScrollbar(HDC hdc, HWND* hRed, HWND* hGreen ,HWND* hBlue);
void SetController(HWND hWnd, HWND* hTrack, HWND* hChckBox1, HWND* hChckBox2, HWND* hToolbar, HWND* hR1, HWND* hCombo, HWND* hRed, HWND* hGreen, HWND* hBlue);
void GetScroll(HWND* hTrack, WPARAM wParam, LPARAM lParam, HWND* hRed, HWND* hGreen, HWND* hBlue, int* nTempPos, int* nRed, int* nGreen, int* nBlue, int* nRecX);
void CommboBoXSelChange(HWND hWnd, HWND* hCombo, WPARAM wParam, int *nBkValu, char* arr);
void WM_PaintFunction(HWND hWnd, HWND* hRed, HWND* hGreen, HWND* hBlue,
					  HDC* hdc, HDC* MemDC, HBRUSH *myBrush, HBRUSH *oldBrush, HBRUSH* bkbrush, HBITMAP* myBitMap, HBITMAP* oldBitMap,
					  int* nRed, int* nGreen, int* nBlue, int* nBkIndex, int *nRecX, RECT rc, 
					  BOOL* bIsBit, BOOL* bRectangle, BOOL* bEllipse);

BOOL CALLBACK InfoDlgProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);