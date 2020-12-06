// WindowsChess.cpp : Определяет точку входа для приложения.
//

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "WindowsChess.h"
#include "../ChessDll/ChessDll.h";


// TODO: Добавить ресурсы
// TODO: Добавить отрисовку фигур
// TODO: Настроить определение размера поля
// TODO: добавить ходы 
// TODO: Добавить главное меню


#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

void DrawBoard(HDC hdc, RECT boardRect);
void DrawButton(HWND parent);
HWND DrawPanel(HWND parent,RECT rect);
HWND DrawPanel(HWND parent, int X, int Y, int Width, int Height);

HWND g_myStatic;




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



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
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

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

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

void InitSquare(RECT* rect, int offsetX,int offsetY) {
	rect->left += offsetX;rect->top += offsetY;
	int offset;
	if ((rect->right - rect->left) > (rect->bottom - rect->top)) {
		offset = ((rect->bottom - rect->top)% BORDER_SIZE)/2;
		rect->right = rect->left + rect->bottom - rect->top;	
	}
	else {
		offset = ((rect->right - rect->left) % BORDER_SIZE) / 2;
		rect->bottom = rect->top + rect->right - rect->left;
	}
}
//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
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
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		InitSquare(&r,4,70);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hBM = CreateCompatibleBitmap(hdc, r.right, r.bottom);
		HANDLE hOld = SelectObject(hMemDC, hBM);

		DrawBoard(hMemDC, r);
		BitBlt(hdc, 0, 0, r.right, r.bottom, hMemDC, 0, 0, SRCCOPY);
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
		mmi->ptMinTrackSize = {800,600};
		//mmi->ptMaxSize.x = 800;
		//mmi->ptMaxSize.y = 600;						                      
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:

		DrawPanel(hWnd,0,0,r.right,68);
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

void DrawButton(HWND parent,HDC hdc, POINT start,POINT end, LPCWSTR text) {
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

HWND DrawPanel(HWND parent, int X,int Y,int Width,int Height) {
	HWND panel = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, _T("STATIC"), _T("Panel 1"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_SUNKEN, X, Y, Width, Height, parent, (HMENU)NULL, GetModuleHandle(NULL), (LPVOID)NULL);
	return panel;
}HWND DrawPanel(HWND parent,RECT rect) {
	HWND panel =	CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, _T("STATIC"), _T("Panel 1"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_SUNKEN, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, parent, (HMENU)NULL, GetModuleHandle(NULL), (LPVOID)NULL);
	return panel;
}

void DrawBoard(HDC hdc, RECT boardRect,int board[BOARD_SIZE]) {

	//FillRect(hdc, &boardRect, (HBRUSH)CreateSolidBrush(RGB(0, 0, 255)));
	// CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	// CreateSolidBrush(RGB(0x0, 0x0, 0x0));

	
	HBRUSH lightCell = CreateSolidBrush(0xCDFAFF);
	HBRUSH darkCell = CreateSolidBrush(0xB46411);
	RECT rect;
	for (int i = 0; i < BORDER_SIZE; i++) {
		for (int j = 0; j < BORDER_SIZE; j++) {
			InitRect(i, j, &rect, boardRect);
			if ((i + j) % 2 == 0) {
				FillRect(hdc, &rect, lightCell);
			}
			else {
				FillRect(hdc, &rect, darkCell);
			}
		}
	}
	DeleteObject(lightCell);
	DeleteObject(darkCell);
}
