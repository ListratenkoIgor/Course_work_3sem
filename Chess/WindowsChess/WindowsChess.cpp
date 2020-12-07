// WindowsChess.cpp : Определяет точку входа для приложения.
//
#include <array>
#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "WindowsChess.h"
#include "../ChessDll/ChessDll.h";

// +++TODO: Настроить определение размера поля
// +++TODO: добавить отрисовку поля
// +++TODO: добавить выбор\нажатие клеток
// +-TODO: Добавить отрисовку фигур
// TODO: добавить ходы 
// TODO: Добавить ресурсы
// TODO: добавить историю ходов 
// TODO: редактировать панель
// TODO: добавить кнопки на панель 
// TODO: Добавить главное меню
int INIT_BOARD[BOARD_SIZE] = {
	OFFBOARD,	OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,
	OFFBOARD,	OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,
	OFFBOARD,	ROOK | WHITE,	KNIGHT | WHITE,	BISHOP | WHITE,	QUEEN | WHITE,	KING | WHITE,	BISHOP | WHITE,	KNIGHT | WHITE,	ROOK | WHITE,	OFFBOARD,
	OFFBOARD,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	OFFBOARD,
	OFFBOARD,	EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			OFFBOARD,
	OFFBOARD,	EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			OFFBOARD,
	OFFBOARD,	EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			OFFBOARD,
	OFFBOARD,	EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			OFFBOARD,
	OFFBOARD,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	OFFBOARD,
	OFFBOARD,	ROOK | BLACK,	KNIGHT | BLACK,	BISHOP | BLACK,	QUEEN | BLACK,	KING | BLACK,	BISHOP | BLACK,	KNIGHT | BLACK,	ROOK | BLACK,	OFFBOARD,
	OFFBOARD,	OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,
	OFFBOARD,	OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD
};

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
RECT boardRect = {-1,-1,-1,-1};

void DrawBoard(HDC hdc, RECT boardRect, int board[]);
void DrawButton(HWND parent);
HWND DrawPanel(HWND parent, RECT rect);
HWND DrawPanel(HWND parent, int X, int Y, int Width, int Height);
bool isCellSellected = false;
POINT sellectedCell = { -1,-1 };
//HWND g_myStatic;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWSCHESS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSCHESS));

	MSG msg;

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSCHESS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSCHESS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; 

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void InitSquare(RECT* rect, int offsetX, int offsetY) {
	rect->left += offsetX; rect->top += offsetY;
	int offset;
	if ((rect->right - rect->left) > (rect->bottom - rect->top)) {
		offset = ((rect->bottom - rect->top) % BORDER_SIZE) / 2;
		rect->right = rect->left + rect->bottom - rect->top;
	}
	else {
		offset = ((rect->right - rect->left) % BORDER_SIZE) / 2;
		rect->bottom = rect->top + rect->right - rect->left;
	}
}

bool PointInRect(int x, int y,RECT rect) {
	return (x <= rect.right) & (x >= rect.left) & (y >= rect.top) & (y <= rect.bottom);
}
POINT GetCellectedCell(int x, int y, RECT rect) {
	int size = (rect.right - rect.left) / BORDER_SIZE;
	x -= rect.left;
	y -= rect.top;
	POINT result = { (int)(x / size) ,BORDER_SIZE-1-(int)(y / size) };
	return result;
}

inline bool PointsEqual(POINT p1, POINT p2) {
	return (p1.x == p2.x) & (p1.y == p2.y);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


	RECT r;
	GetClientRect(hWnd, &r);
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if (!isCellSellected) {
			if (PointInRect(xPos, yPos, boardRect)) {
				sellectedCell = GetCellectedCell(xPos, yPos, boardRect);
				isCellSellected = true;
			}
		}
		else {
			isCellSellected = false;
			sellectedCell = {-1,-1};
			//Move()
		}
		InvalidateRect(hWnd,&boardRect,true);
		//UpdateWindow(hWnd);
		break;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);	
		GetClientRect(hWnd, &boardRect);
		InitSquare(&boardRect, 4, 70);
		HDC hMemDC = CreateCompatibleDC(hdc);

		HBITMAP hBM = CreateCompatibleBitmap(hdc, boardRect.right, boardRect.bottom);

		HANDLE hOld = SelectObject(hMemDC, hBM);
		DrawBoard(hMemDC, boardRect, INIT_BOARD);
		BitBlt(hdc, 0, 0, boardRect.right, boardRect.bottom, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, hOld);

		DeleteObject(hBM);

		DeleteDC(hMemDC);
		EndPaint(hWnd, &ps);
		break;
	}
	/*
		case WM_DRAWITEM:
		{
			g_myStatic = CreateWindowEx(0, L"STATIC", L"Some static text",
				WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
				25, 125, 150, 20, hWnd, 0, 0, 0);
			LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
			if (pDIS->hwndItem == g_myStatic)
			{
				SetTextColor(pDIS->hDC, RGB(100, 0, 100));
				WCHAR staticText[99];
				int len = SendMessage(g_myStatic, WM_GETTEXT,
					ARRAYSIZE(staticText), (LPARAM)staticText);
				TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);
			}
			break;
		} */
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize = { 800,600 };
		//mmi->ptMaxSize.x = 800;
		//mmi->ptMaxSize.y = 600;						                      
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:

		DrawPanel(hWnd, 0, 0, r.right, 68);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void DrawButton(HWND parent, HDC hdc, POINT start, POINT end, LPCWSTR text) {
	HWND hwndButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"OK",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		10,         // x position 
		10,         // y position 
		100,        // Button width
		100,        // Button height
		parent,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

}
void DrawButton(HWND parent) {
	HWND hwndButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"OK",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		10,         // x position 
		10,         // y position 
		100,        // Button width
		100,        // Button height
		parent,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

}
void InitRect(int i, int j, RECT* rect, RECT boardRect)
{
	int cellSize = (boardRect.right - boardRect.left) / BORDER_SIZE;
	rect->left = boardRect.left + j * cellSize;
	rect->right = rect->left + cellSize;
	rect->top = boardRect.top + i * cellSize;
	rect->bottom = rect->top + cellSize;
}

HWND DrawPanel(HWND parent, int X, int Y, int Width, int Height) {
	HWND panel = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, _T("STATIC"), _T("Panel 1"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_SUNKEN, X, Y, Width, Height, parent, (HMENU)NULL, GetModuleHandle(NULL), (LPVOID)NULL);
	return panel;
}
HWND DrawPanel(HWND parent, RECT rect) {
	HWND panel = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, _T("STATIC"), _T("Panel 1"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_SUNKEN, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, parent, (HMENU)NULL, GetModuleHandle(NULL), (LPVOID)NULL);
	return panel;
}

void DrawImageStretched(HDC hDC, HBITMAP hBitmap, RECT& rcSource, RECT& rcDest)
{

	HDC hdcMem = CreateCompatibleDC(hDC);
	if (hdcMem)
	{
		//	Select new bitmap and backup old bitmap
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
		
		//	Fetch and blit bitmap
		BITMAP bm;
		if (GetObject(hBitmap, sizeof(bm), &bm) == sizeof(bm))
		{
			//	Blit!
			SetStretchBltMode(hDC, BLACKONWHITE);
			StretchBlt(hDC,
				rcDest.left, rcDest.top, (rcDest.right - rcDest.left), (rcDest.bottom - rcDest.top),
				hdcMem,
				rcSource.left, rcSource.top, (rcSource.right - rcSource.left), (rcSource.bottom - rcSource.top),
				SRCCOPY);
		}

		//	Restore old bitmap
		(HBITMAP)SelectObject(hdcMem, hbmOld);

		//	Discard MemDC
		DeleteDC(hdcMem);
	}
}
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	// Создаем монохромную (1 бит) растровую маску.  

	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// Получить несколько HDC, совместимых с драйвером дисплея

	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	(HBITMAP)SelectObject(hdcMem, hbmColour);
	(HBITMAP)SelectObject(hdcMem2, hbmMask);

	// Устанавливаем цвет фона цветного изображения на цвет
	// вы хотите быть прозрачным.
	SetBkColor(hdcMem, crTransparent);

	// Копируем биты из цветного изображения в маску B + W ... все
	// с цветом фона становится белым, а все остальное заканчивается
	// черный ... То, что мы хотели.

	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	// Берем нашу новую маску и используем ее, чтобы сделать прозрачный цвет в нашем
	// исходное цветное изображение становится черным, чтобы эффект прозрачности был
	// работаем правильно.
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	// Очистить.

	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}

void DrawBoard(HDC hDC, RECT boardRect, int board[]) {

	//FillRect(hdc, &boardRect, (HBRUSH)CreateSolidBrush(RGB(0, 0, 255)));
	// CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	// CreateSolidBrush(RGB(0x0, 0x0, 0x0));
	HBRUSH lightCell = CreateSolidBrush(0xCDFAFF);
	HBRUSH darkCell = CreateSolidBrush(0xB46411);
	HBRUSH sellectedLightCell = CreateSolidBrush(0x00FF66);
	HBRUSH sellectedDarkCell = CreateSolidBrush(0x228B22);
	HDC hMemDC = CreateCompatibleDC(hDC);
	//HANDLE hFile = CreateFile(MAKEINTRESOURCE(BLACK_R), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	HBITMAP figure = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(BLACK_R));
	
	//L"e:\\BSUIR\\GITHUB\\Course_work_5sem\\Chess\\WindowsChess\\Figures\\BLACK_R.bmp"
																																
	HANDLE hOld = NULL;
	BITMAP bm;
	
	RECT rect;
	int k = 0;
	for (int i = 0; i < BORDER_SIZE; i++) {
		for (int j = 0; j < BORDER_SIZE; j++) {
			while (board[k] == OFFBOARD) {
				k++;
			}
			InitRect(i, j, &rect, boardRect);
			if ((i + j) % 2 == 0) {
				if (isCellSellected && PointsEqual(sellectedCell, { j,BORDER_SIZE - i - 1 })) {
					FillRect(hDC, &rect, sellectedLightCell);
				}
				else {
					FillRect(hDC, &rect, lightCell);
				}
			}
			else {
				if (isCellSellected && PointsEqual(sellectedCell, { j,BORDER_SIZE - i - 1 })) {
					FillRect(hDC, &rect, sellectedDarkCell);
				}
				else {
					FillRect(hDC, &rect, darkCell);
				}
			}
			if (board[k] != EMPTY) {
				HPEN pen = (HPEN)CreatePen(PS_DOT, 2, 0x000000FF);

				hOld = SelectObject(hDC, pen);
				Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);
				SelectObject(hDC, hOld);

				/*
				GetObject(figure, sizeof(BITMAP), (LPSTR)&bm);
				hOld = (HBITMAP)SelectObject(hMemDC, figure);
				StretchBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
				SelectObject(hMemDC, hOld);
				*/
			}
			k++;


		}
	}


	//SelectObject(hDC, figure);
	//BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCAND);
	//SelectObject(hDC, figure);
	//BitBlt(hDC, 0, bm.bmHeight, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCPAINT);
	if (hOld != NULL) {
		DeleteObject(hOld);
	}
	if (figure != NULL) {
		DeleteObject(figure);
	}
	DeleteDC(hMemDC);
	DeleteObject(sellectedLightCell);
	DeleteObject(sellectedDarkCell);
	DeleteObject(lightCell);
	DeleteObject(darkCell);
}
