#include "Win32.h"

HINSTANCE g_hInst;
RECT g_buffer = {22, 195, 267, 402};
LPSTR lpszClass=TEXT("Win32");
char g_arrItem[][13] = { "WHITE", "GRAY", "RED","GREEN", "BLUE"};
int g_nIndex;
int g_nPosX,g_nPosY;
TBBUTTON g_arrtoolBtn[2] = {
		{0, ID_TOOLBAR_PEN, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0, 0, 0},
		{1, ID_TOOLBAR_CANCEL, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0, 0, 0}
	};

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
					 ,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MENU2);
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);

	HACCEL hAccel;
	hAccel = LoadAccelerators(hInstance, MAKEINTATOM(IDR_ACCELERATOR1));

	while(GetMessage(&Message,0,0,0)) {
		if(!TranslateAccelerator(hWnd, hAccel, &Message))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
	return Message.wParam;
}


BOOL CALLBACK InfoDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage) {
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg,IDC_EDIT1,g_nPosX,FALSE);
		SetDlgItemInt(hDlg,IDC_EDIT2,g_nPosY,FALSE);
		return TRUE;
	case WM_COMMAND:
		switch (wParam) {
		case IDOK:
			g_nPosX=GetDlgItemInt(hDlg,IDC_EDIT1,NULL,FALSE);
			g_nPosY=GetDlgItemInt(hDlg,IDC_EDIT2,NULL,FALSE);
			EndDialog(hDlg,1);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, bkhDC, MemDC;
	static int nRed ,nGreen, nBlue;
	int nTempPos=0;
	static int nRecX = 0, nTemp = 0, nBkIndex = -1, g_nRecX = 0;
	static char arrstr[128];
	RECT rc = {0, 0, 0, 0}; 
	HBRUSH bkbrush;
	static BOOL bRectangle = FALSE, bEllipse = FALSE;
	static BOOL bPen = FALSE, bNowDraw = FALSE, bIsBit = FALSE, bBkRed = FALSE;

	static HWND hRed, hGreen, hBlue;					//스크롤 핸들러
	static HWND hTrack;
	static HWND hChckBox1, hChckBox2;								//체크박스 핸들러
	static HWND hToolbar;								//툴바 핸들러
	static HWND hR1, hR2;								//라디오 버튼 핸들러
	static HWND hCombo;
	static HBITMAP hBit;
	static HBITMAP myBitMap, oldBitMap;
	static HBRUSH myBrush, oldBrush;

	switch(iMessage) {
		//WM_CREATE
	case WM_CREATE:
		InitCommonControls();
		g_nPosX=100;
		g_nPosY=100;
		
		SetController(hWnd, &hTrack, &hChckBox1, &hChckBox2, &hToolbar, &hR1, &hCombo, &hRed, &hGreen, &hBlue);

		//WM_COMMAND
	case WM_COMMAND :
		//ID체크
		switch(LOWORD(wParam))
		{
			//Rectangle을 그리는 체크박스
		case ID_CHECKBOX_RECTANGLE :
			//현재 체크박스가 체크되어 있지 않다면
			CheckedReactangle(&hChckBox1,&hChckBox2, &bRectangle, &bEllipse);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
			//Ellipse를 그리는 체크박스
		case  ID_CHECKBOX_ELLIPSE :
			//현재 체크박스가 체크되어 있지 않다면
			CheckedEllipse(&hChckBox1, &hChckBox2, &bRectangle, &bEllipse);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
			//메뉴
		case ID_MB1 :
			MessageBox(hWnd, TEXT("첫번째 메세지 박스입니다."), TEXT("MB1"), MB_OK);
			break;
		case ID_MB2 :
			MessageBox(hWnd, TEXT("두번째 메세지 박스입니다."), TEXT("MB2"), MB_OK);
			break;
		case ID_EXIT :
			PostQuitMessage(0);
			break;
			//툴바
		case ID_TOOLBAR_PEN :
			bPen = TRUE;
			break;
		case ID_TOOLBAR_CANCEL:
			bPen = FALSE;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_RADIO_NORMAL :
			bIsBit = FALSE;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_RADIO_ZOOM :
			bIsBit = TRUE;
			InvalidateRect(hWnd, NULL, TRUE);
			break;	
		case ID_COMBOBOX:
			CommboBoXSelChange(hWnd, &hCombo, wParam, &nBkIndex, arrstr);
		}
		return 0;
		
		//WM_HSCROLL
	case WM_HSCROLL :
		GetScroll(&hTrack, wParam, lParam, &hRed, &hGreen, &hBlue, &nTempPos, &nRed, &nGreen, &nBlue, &nRecX);
		InvalidateRect(hWnd, &g_buffer, TRUE);
		return 0;

		//WM_LBUTTONDOWN
	case WM_LBUTTONDOWN :
		g_nPosX = LOWORD(lParam);
		g_nPosY = HIWORD(lParam);
		bNowDraw = TRUE;
		return 0;
		//WM_LBUTTONUP
	case WM_LBUTTONUP :
		bNowDraw = FALSE;
		return 0;
	case WM_MOUSEMOVE :
		if(bNowDraw == TRUE && bPen == TRUE)
		{
			hdc = GetDC(hWnd);
			DrawPen(hWnd, hdc, lParam);
			ReleaseDC(hWnd, hdc);
		}
		return 0;
		//WM_MBUTTONDOWN
	case WM_MBUTTONDOWN :
		g_nPosY = LOWORD(lParam);
		g_nPosY = HIWORD(lParam);
		DialogBox(g_hInst,MAKEINTRESOURCE(IDD_DIALOG1),hWnd,InfoDlgProc); 
		return 0;

		//WM_PAINT
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		MemDC = CreateCompatibleDC(hdc);

		 WM_PaintFunction(hWnd, &hRed, &hGreen, &hBlue, &hdc, &MemDC, 
					  &myBrush, &oldBrush, &bkbrush, &myBitMap, &oldBitMap, 
					  &nRed, &nGreen, &nBlue, &nBkIndex, &nRecX, rc, 
					 &bIsBit, &bRectangle, &bEllipse);

		EndPaint(hWnd, &ps);
		return 0;
		//WM_DESTROY
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam)); 
}

int BkColorIndex(int nIndex)
{
	return nIndex;
}

void CheckedReactangle(HWND* hChckBox1, HWND* hChckBox2, BOOL* bRectangle, BOOL* bEllipse)
{

	if(SendMessage(*hChckBox1, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
	{
		//체크박스를 체크상태로 변경
		SendMessage(*hChckBox1, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(*hChckBox2, BM_SETCHECK, BST_UNCHECKED, 0);
		*bRectangle = TRUE;
		*bEllipse = FALSE;
	}
	else
	{
		SendMessage(*hChckBox1, BM_SETCHECK, BST_UNCHECKED, 0);
		*bRectangle = FALSE;
	}

}

void CheckedEllipse(HWND* hChckBox1, HWND* hChckBox2, BOOL* bRectangle, BOOL* bEllipse)
{
if(SendMessage(*hChckBox2, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
			{
				//체크박스를 체크상태로 변경
				SendMessage(*hChckBox2, BM_SETCHECK, BST_CHECKED, 0);
				SendMessage(*hChckBox1, BM_SETCHECK, BST_UNCHECKED, 0);
				*bEllipse = TRUE;
				*bRectangle = FALSE;
			}
			else
			{
				SendMessage(*hChckBox2, BM_SETCHECK, BST_UNCHECKED, 0);
				*bEllipse = FALSE;
			}
}

void HiddenScrollbar(HWND* hRed, HWND* hGreen ,HWND* hBlue)
{
	ShowScrollBar(*hRed, SB_CTL, FALSE);
	ShowScrollBar(*hGreen, SB_CTL, FALSE);
	ShowScrollBar(*hBlue, SB_CTL, FALSE);
}

void ShowScrollbar(HDC hdc, HWND* hRed, HWND* hGreen ,HWND* hBlue)
{
	ShowScrollBar(*hRed, SB_CTL, TRUE);
	ShowScrollBar(*hGreen, SB_CTL, TRUE);
	ShowScrollBar(*hBlue, SB_CTL, TRUE);
	TextOut(hdc, 400, 200, TEXT("RED"), 3);
	TextOut(hdc, 395, 260, TEXT("GREEN"), 5);
	TextOut(hdc, 397, 320, TEXT("BLUE"), 4);
}

void FillBackground(HWND hWnd, HDC hdc, RECT rc, HBRUSH bkbrush)
{
	GetClientRect(hWnd, &rc); 
	FillRect(hdc, &rc, bkbrush); 
}

void SetController(HWND hWnd, HWND* hTrack, HWND* hChckBox1, HWND* hChckBox2, HWND* hToolbar, HWND* hR1, HWND* hCombo, HWND* hRed, HWND* hGreen, HWND* hBlue)
{
	*hChckBox1 = CreateWindow(TEXT("button"), TEXT("RECTANGLE"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 40, 80, 160, 25, hWnd, (HMENU)ID_CHECKBOX_RECTANGLE, g_hInst, NULL);
	*hChckBox2 = CreateWindow(TEXT("button"), TEXT("ELLIPSE"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 40, 110, 160, 25, hWnd, (HMENU)ID_CHECKBOX_ELLIPSE, g_hInst, NULL);
	//스크롤바 생성
	*hRed = CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 310, 220, 200, 20, hWnd, (HMENU)ID_SCR_RED, g_hInst, NULL);
	*hGreen = CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 310, 280, 200, 20, hWnd, (HMENU)ID_SCR_GREEN, g_hInst, NULL);
	*hBlue = CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 310, 340, 200, 20, hWnd, (HMENU)ID_SCR_BLUE, g_hInst, NULL);
	//스크롤바 범위 설정
	SetScrollRange(*hRed, SB_CTL, 0, 255, TRUE);
	SetScrollRange(*hGreen, SB_CTL, 0, 255, TRUE);
	SetScrollRange(*hBlue, SB_CTL, 0, 255, TRUE);
	//스크롤바의 현재위치
	SetScrollPos(*hRed, SB_CTL, 0, TRUE);
	SetScrollPos(*hGreen, SB_CTL, 0, TRUE);
	SetScrollPos(*hBlue, SB_CTL, 0, TRUE);
	//툴바생성
	*hToolbar = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE | WS_BORDER, ID_TOOLBAR, 2, g_hInst, IDB_BITMAP2, g_arrtoolBtn, 2, 16, 16, 16, 16, sizeof(TBBUTTON));
	//라이도 버튼 생성
	*hR1 = CreateWindow(TEXT("button"), TEXT("원본"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 1300, 40, 100, 30, hWnd, (HMENU)ID_RADIO_NORMAL, g_hInst, NULL);
	*hR1 = CreateWindow(TEXT("button"), TEXT("확대"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 1300, 80, 100, 30, hWnd, (HMENU)ID_RADIO_ZOOM, g_hInst, NULL);
	//슬라이드바
	*hTrack = CreateWindowW(TRACKBAR_CLASSW, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 50, 430, 170, 30, hWnd, (HMENU)ID_TRACKBAR, g_hInst, NULL);
	SendMessageW(*hTrack, TBM_SETRANGE,  TRUE, MAKELONG(0, 10)); 
	SendMessageW(*hTrack, TBM_SETPAGESIZE, 0,  2); 
	SendMessageW(*hTrack, TBM_SETPOS, FALSE, 0); 

	*hCombo=CreateWindow("combobox",NULL,WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,1150,50,100,200,hWnd,(HMENU)ID_COMBOBOX,g_hInst,NULL);
	for (g_nIndex=0; g_nIndex<5; g_nIndex++)
		SendMessage(*hCombo,CB_ADDSTRING,0,(LPARAM)g_arrItem[g_nIndex]);
}

void CommboBoXSelChange(HWND hWnd, HWND* hCombo, WPARAM wParam, int *nBkValu, char* arr)
{

	if(HIWORD(wParam)) {
		if(CBN_SELCHANGE)
		{
			g_nIndex=SendMessage(*hCombo, CB_GETCURSEL,0,0);
			SendMessage(*hCombo, CB_GETLBTEXT, g_nIndex, (LPARAM)arr);
			SetWindowText(hWnd, arr);
			*nBkValu = BkColorIndex(g_nIndex);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if(CBN_EDITCHANGE)
		{
			GetWindowText(*hCombo, arr, 128);
			SetWindowText(hWnd,arr);
		}
	}
}

void DrawPen(HWND hWnd, HDC hdc, LPARAM lParam)
{
	MoveToEx(hdc, g_nPosX, g_nPosY, NULL);
	g_nPosX = LOWORD(lParam);
	g_nPosY = HIWORD(lParam);
	LineTo(hdc, g_nPosX, g_nPosY);
}


void GetScroll(HWND* hTrack, WPARAM wParam, LPARAM lParam, HWND* hRed, HWND* hGreen, HWND* hBlue, int* nTempPos, int* nRed, int* nGreen, int* nBlue, int* nRecX)
{
	if((HWND)lParam == *hRed) *nTempPos = *nRed;
		if((HWND)lParam == *hGreen) *nTempPos = *nGreen;
		if((HWND)lParam == *hBlue) *nTempPos = *nBlue;

		//스크롤바의 어디를 눌렀는지 체크
		switch(LOWORD(wParam))
		{
		case SB_LINELEFT :
			*nTempPos = max(0, *nTempPos-1);
			break;
		case SB_LINERIGHT :
			*nTempPos = min(255, *nTempPos+1);
			break;
		case SB_PAGELEFT :
			*nTempPos = max(0,* nTempPos-10);
			break;
		case SB_PAGERIGHT :
			*nTempPos = min(255, *nTempPos+10);
			break;
		case SB_THUMBTRACK :
			*nTempPos = HIWORD(wParam);				//스크롤을 누르고 있으면 현재위치를 저장
			break;
		}

		if((HWND)lParam == *hTrack)
		{
			switch(LOWORD(wParam))
			{
			case TB_LINEUP :
				*nRecX = max(0, *nRecX-1);
				break;
			case TB_LINEDOWN :
				*nRecX = min(10, *nRecX+1);
				break;
			case TB_PAGEUP :
				*nRecX = max(0, *nRecX-3);
				break;
			case TB_PAGEDOWN :
				*nRecX = min(10, *nRecX+3);
				break;
			case TB_THUMBTRACK :
				*nRecX = HIWORD(wParam);
				break;
			}
		}


		if((HWND)lParam == *hRed) *nRed = *nTempPos;
		if((HWND)lParam == *hGreen) *nGreen = *nTempPos;
		if((HWND)lParam == *hBlue) *nBlue = *nTempPos;

		SetScrollPos((HWND)lParam, SB_CTL, *nTempPos, TRUE);
}

void WM_PaintFunction(HWND hWnd, HWND* hRed, HWND* hGreen, HWND* hBlue, HDC* hdc, HDC* MemDC, 
					  HBRUSH *myBrush, HBRUSH *oldBrush, HBRUSH* bkbrush, HBITMAP* myBitMap, HBITMAP* oldBitMap, 
					  int* nRed, int* nGreen, int* nBlue, int* nBkIndex, int *nRecX, RECT rc, 
					  BOOL* bIsBit, BOOL* bRectangle, BOOL* bEllipse)
{
	TextOut(*hdc, 95, 50, TEXT("Draw"), 4);
	Rectangle(*hdc, 30,70,210,145);

	*myBrush = CreateSolidBrush(RGB(*nRed, *nGreen, *nBlue));
	*oldBrush = (HBRUSH)SelectObject(*hdc, *myBrush);

	*myBitMap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
	*oldBitMap = (HBITMAP)SelectObject(*MemDC, *myBitMap);

	if(nBkIndex == BKWHITE)
	{
		*bkbrush = CreateSolidBrush(RGB(255, 255, 255)); 
		FillBackground(hWnd, *hdc, rc, *bkbrush);
	}
	if(*nBkIndex == BKGRAY)
	{
		*bkbrush = CreateSolidBrush(RGB(128, 128, 128)); 
		FillBackground(hWnd, *hdc, rc, *bkbrush);
	}
	if(*nBkIndex == BKRED)
	{
		*bkbrush = CreateSolidBrush(RGB(255, 0, 0)); 
		FillBackground(hWnd, *hdc, rc, *bkbrush);
	}
	if(*nBkIndex == BKGREEN)
	{
		*bkbrush = CreateSolidBrush(RGB(0, 255, 0)); 
		FillBackground(hWnd, *hdc, rc, *bkbrush);
	}
	if(*nBkIndex == BKBLUE)
	{
		*bkbrush = CreateSolidBrush(RGB(0, 0, 255)); 
		FillBackground(hWnd, *hdc, rc, *bkbrush);
	}

	if(*bIsBit)
	{
		BitBlt(*hdc, 1270, 150, 123, 160, *MemDC, 0, 0, SRCCOPY);
	}
	if(*bIsBit)
	{
		StretchBlt(*hdc, 1100, 150, 300, 400, *MemDC, 0, 0, 123, 160, SRCCOPY);
	}
	if((*bRectangle == FALSE) && (*bEllipse == FALSE))
	{
		HiddenScrollbar(hRed, hGreen, hBlue);
	}

	if((*bRectangle == TRUE) || (*bEllipse == TRUE))
	{
		ShowScrollbar(*hdc, hRed, hGreen, hBlue);
	}

	if(*bRectangle)
	{
		Rectangle(*hdc, 130-(*nRecX*10), 200, 160+(*nRecX*10), 400);
	}
	if(*bEllipse)
	{
		Ellipse(*hdc, 130-(*nRecX*10), 200, 160+(*nRecX*10), 400);
	}

	SelectObject(*hdc,*oldBrush);
	DeleteObject(*myBrush);

	SelectObject(*hdc, *oldBitMap);
	DeleteObject(*myBitMap);
}