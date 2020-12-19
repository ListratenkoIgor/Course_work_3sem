#include <array>
#include <stdio.h>
#include <iostream>
#include <string>
#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "WindowsChess.h"
#include "../ChessDll/ChessDll.h";
#include "ListBox.h";
// +++TODO: Настроить определение размера поля
// +++TODO: добавить отрисовку поля
// +++TODO: добавить выбор\нажатие клеток
// +++TODO: добавить панель с кнопками для управления игрой
// +++TODO: добавить ходы 
// +++TODO: Добавить таймер
// +++TODO: Добавить контрол для истории ходов
// +++TODO: добавить историю ходов 
// ++TODO: добавить кнопки на панель 
// TODO: Добавить ресурсы
// +-TODO: Добавить отрисовку фигур
// TODO: Добавить главное меню			 
#pragma comment(lib,"Msimg32.lib")

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

void  DrawBoard(HDC hdc, RECT boardRect, int board[]);
void  DrawImageStretched(HDC hDC, HBITMAP hBitmap, RECT rcSource, RECT rcDest);

POINT GetCellectedCell(int x, int y, RECT rect);
void  InitSquare(RECT* rect, int offsetX, int offsetY);
void  InitRect(int i, int j, RECT* rect, RECT boardRect);
bool  PointInRect(int x, int y, RECT rect);
inline bool PointsEqual(POINT p1, POINT p2);

LPCWSTR GetLastMove();
bool Takeback();
bool ComputerMove();
bool Move(POINT from, POINT to);

void StartGame(HWND);
void RestartGame(HWND);
void EndGame(HWND,int);
bool CheckEndGame();

void  InitPieces(HWND);
void  FreePieces();

int panelHeight = 78;
bool g_isComputerPlaying = false;
int g_side = WHITE;
int g_level = 3;
bool g_isActive = false;
bool isCellSellected = false;
bool isComputerThinking = false;
bool isComputersTurn = false;
bool isComputerPlaying = false;
bool isWhiteSide = true;
int side;		   
int moveResult = 0;
POINT sellectedCell = { -1,-1 };
RECT boardRect = { -1,-1,-1,-1 };
HWND hBoardPanel = NULL;
HWND hNewGameBtn = NULL;
HWND hRestartBtn = NULL;
HWND hResignBtn = NULL;
HWND hTakebackBtn = NULL;
HBITMAP PIECE_BITMAP[16];
GameListBox* listBox = nullptr;

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
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWSCHESS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSCHESS));
	MSG msg;
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT r;
	GetClientRect(hWnd, &r);
	switch (message)
	{
	case WM_TIMER:
	{
		switch (wParam)
		{

		case IDT_GAME_TIMERID: {
			if (isComputerPlaying && isComputersTurn) {
				if (!isComputerThinking) {
					isComputerThinking = true;
					ComputerMove();
					isComputersTurn = false;
					LPCWSTR move = GetLastMove();
					listBox->AddMove(move);
					if (CheckEndGame()) {
						EndGame(hWnd, moveResult);
					}
					isComputerThinking = false;
					InvalidateRect(hWnd, NULL, true);
				}
			}
			break;
		}
		}
		break;
	}
	case WM_COMMAND:
	{	
		if (g_isActive) {
			if ((LOWORD(wParam)==ID_NEWGAME) || (HWND)lParam == hNewGameBtn) {
				if (MessageBox(hWnd, L"Вы уверенны, что хотите начать новую игру?", L"Новая игра", MB_OKCANCEL) == IDOK) {
					StartGame(hWnd);
					InvalidateRect(hWnd, &boardRect, true);
				}
			}
			if ((HWND)lParam == hTakebackBtn) {
				if (isComputerPlaying) {
					Takeback();
					listBox->DeleteMove();
					Takeback();
					listBox->DeleteMove();
				}
				else {
					if (Takeback()) {
						isWhiteSide = !isWhiteSide;
						listBox->DeleteMove();
					}
				}
				InvalidateRect(hWnd, &boardRect, true);
			}
			if ((HWND)lParam == hRestartBtn) {
				if (MessageBox(hWnd, L"Вы уверенны, что хотите начать заново?", L"Новая игра", MB_OKCANCEL) == IDOK) {
					RestartGame(hWnd);
					InvalidateRect(hWnd, &boardRect, true);
				}

			}
			if ((HWND)lParam == hResignBtn) {
				if (MessageBox(hWnd, L"Вы уверенны, что хотите сдаться?", L"Признать поражение", MB_OKCANCEL) == IDOK) {
					EndGame(hWnd, RESIGN);
				}
			}
		}
		else {
			if ((LOWORD(wParam) == ID_NEWGAME) || (HWND)lParam == hNewGameBtn) {
				StartGame(hWnd);
				InvalidateRect(hWnd, &boardRect, true);
			}
		}
		int wmId = LOWORD(wParam);
		HMENU hMenu = GetMenu(hWnd);
		switch (wmId) {
		case ID_LEVEL_EASY:
		case ID_LEVEL_MEDIUM:
		case ID_LEVEL_HARD:
			CheckMenuItem(hMenu, ID_LEVEL_EASY, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_LEVEL_MEDIUM, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_LEVEL_HARD, MF_UNCHECKED);
			break;
		case ID_COLOR_WHITE:
		case ID_COLOR_BLACK:
			CheckMenuItem(hMenu, ID_COLOR_WHITE, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_COLOR_BLACK, MF_UNCHECKED);
			break;
		case ID_ENEMY_HUMAN:
		case ID_ENEMY_COMPUTER:
			CheckMenuItem(hMenu, ID_ENEMY_HUMAN, MF_UNCHECKED);
			CheckMenuItem(hMenu, ID_ENEMY_COMPUTER, MF_UNCHECKED);
			break;
		}
		switch (wmId)
		{
		case ID_LEVEL_EASY:
			g_level = 1;
			CheckMenuItem(hMenu, ID_LEVEL_EASY, MF_CHECKED);
			break;
		case ID_LEVEL_MEDIUM:
			g_level = 3;
			CheckMenuItem(hMenu, ID_LEVEL_MEDIUM, MF_CHECKED);
			break;
		case ID_LEVEL_HARD:
			g_level = 5;
			CheckMenuItem(hMenu, ID_LEVEL_HARD, MF_CHECKED);
			break;								  		
		case ID_COLOR_WHITE:
			g_side = WHITE;
			CheckMenuItem(hMenu, ID_COLOR_WHITE, MF_CHECKED);
			break;
		case ID_COLOR_BLACK:
			g_side = BLACK;
			CheckMenuItem(hMenu, ID_COLOR_BLACK, MF_CHECKED);
			break;
		case ID_ENEMY_HUMAN:
			g_isComputerPlaying = false;
			CheckMenuItem(hMenu, ID_ENEMY_HUMAN, MF_CHECKED);
			break;
		case ID_ENEMY_COMPUTER:
			g_isComputerPlaying = true;
			CheckMenuItem(hMenu, ID_ENEMY_COMPUTER, MF_CHECKED);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{	
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if (g_isActive) {
			if (!isCellSellected) {
				if (PointInRect(xPos, yPos, boardRect)) {
					sellectedCell = GetCellectedCell(xPos, yPos, boardRect);
					isCellSellected = true;
				}
			}
			else {
				if (!isComputerThinking) {
					POINT newCell;				
					if (PointInRect(xPos, yPos, boardRect)) {
						newCell = GetCellectedCell(xPos, yPos, boardRect);
						if (isWhiteSide) {
							if (Move(sellectedCell, newCell)) {
								LPCWSTR move = GetLastMove();
								listBox->AddMove(move);
								if (CheckEndGame()) {
									EndGame(hWnd, moveResult);
								}
							}
						}
						else {
							if (Move({ BORDER_SIZE - 1 - sellectedCell.x,BORDER_SIZE - 1 - sellectedCell.y },
								{ BORDER_SIZE - 1 - newCell.x,BORDER_SIZE - 1 - newCell.y })) {
								LPCWSTR move = GetLastMove();
								listBox->AddMove(move);
								if (CheckEndGame()) {
									EndGame(hWnd,moveResult);
								}
							}

						}
						isCellSellected = false;
						sellectedCell = { -1,-1 };
					}
				}
			}
			InvalidateRect(hWnd, &boardRect, true);
		}
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &boardRect);
		InitSquare(&boardRect, 4, panelHeight + 2);
		FillRect(hdc, &r, (HBRUSH)BLACK_BRUSH);

		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hBM = CreateCompatibleBitmap(hdc, boardRect.right, boardRect.bottom);
		HANDLE hOld = SelectObject(hMemDC, hBM);

		DrawBoard(hMemDC, boardRect, board);
		BitBlt(hdc, 0, 0, boardRect.right, boardRect.bottom, hMemDC, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hOld);													 
		DeleteObject(hBM);

		DeleteDC(hMemDC);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize = { 900,600 };
		break;
	}
	case WM_SIZE:
	{
		MoveWindow(hBoardPanel, 0, 0, r.right - r.left, panelHeight, true);
		GetClientRect(hWnd, &boardRect);
		InitSquare(&boardRect, 4, panelHeight + 2);
		listBox->Move({ boardRect.right,boardRect.top,r.right,r.bottom });
		break;
	}
	case WM_DESTROY:
	{
		EndGame(hWnd, CANCEL);
		DestroyWindow(hBoardPanel);
		DestroyWindow(hTakebackBtn);
		DestroyWindow(hNewGameBtn);
		DestroyWindow(hRestartBtn);
		DestroyWindow(hResignBtn);
		delete listBox;
		FreePieces();
		PostQuitMessage(0);
		break;
	}
	case WM_CREATE:
	{
		hBoardPanel = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, _T("STATIC"), _T(""),
			WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_SUNKEN,
			r.left, r.top, r.right - r.left, panelHeight,
			hWnd, (HMENU)NULL, GetModuleHandle(NULL), (LPVOID)NULL);
		hNewGameBtn = CreateWindow(L"BUTTON", L"Новая игра",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
			5, 5, panelHeight - 10, panelHeight - 10, hWnd, NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
		hRestartBtn = CreateWindow(L"BUTTON", L"Заново",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
			5 + panelHeight - 10, 5, panelHeight - 10, panelHeight - 10, hWnd, NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
		hResignBtn = CreateWindow(L"BUTTON", L"Сдаться",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
			5 + 2 * (panelHeight - 10), 5, panelHeight - 10, panelHeight - 10, hWnd, NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
		hTakebackBtn = CreateWindow(L"BUTTON", L"Вернуть",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,
			5 + 3 * (panelHeight - 10), 5, panelHeight - 10, panelHeight - 10, hWnd, NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
		InitPieces(hWnd);
		GetClientRect(hWnd, &boardRect);
		InitSquare(&boardRect, 4, panelHeight + 2);
		listBox = new GameListBox(hWnd, { boardRect.right,boardRect.top,r.right,r.bottom }, IDC_LISTBOX_WHITE);
		//StartGame(hWnd);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
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

void DrawImageStretched(HDC hDC, HBITMAP hBitmap, RECT rcSource, RECT rcDest)
{

	HDC hdcMem = CreateCompatibleDC(hDC);
	if (hdcMem)
	{
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
		BITMAP bm;
		if (GetObject(hBitmap, sizeof(bm), &bm) == sizeof(bm))
		{
			SetStretchBltMode(hDC, BLACKONWHITE);
			TransparentBlt(hDC,
				rcDest.left, rcDest.top, (rcDest.right - rcDest.left), (rcDest.bottom - rcDest.top),
				hdcMem,
				rcSource.left, rcSource.top, (rcSource.right - rcSource.left), (rcSource.bottom - rcSource.top),
				0xc8aeff);

		}						  
		(HBITMAP)SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
	}
}
void DrawBoard(HDC hDC, RECT boardRect, int board[]) {
	HBRUSH lightCell = CreateSolidBrush(0xCDFAFF);
	HBRUSH darkCell = CreateSolidBrush(0xB46411);
	HBRUSH sellectedLightCell = CreateSolidBrush(0x00FF66);
	HBRUSH sellectedDarkCell = CreateSolidBrush(0x228B22);
	HBRUSH white = (HBRUSH)CreateSolidBrush(0xFFFFFF);
	HBRUSH black = (HBRUSH)CreateSolidBrush(0x000000);

	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP figure = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(BLACK_R));
	HGDIOBJ hOld = NULL;
	BITMAP bm;
	RECT rect;
	int k = 0;
	int startindexI = 0;
	int lastindexI = BORDER_SIZE;
	int startindexJ = BORDER_SIZE - 1;
	int lastindexJ = -1;
	int shift = 1;
	if (isWhiteSide) {
		startindexI = BORDER_SIZE - 1;
		lastindexI = -1;
		startindexJ = 0;
		lastindexJ = BORDER_SIZE;
		shift = -1;
	}
	for (int i = startindexI; i != lastindexI; i += shift) {
		for (int j = startindexJ; j != lastindexJ; j -= shift) {

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
				
				RECT sourceRect = { 0,0,200,200 };
				DrawImageStretched(hDC, PIECE_BITMAP[((board[k] & PIECE_MASK) + (board[k] & BLACK))], sourceRect, rect);
			}
			k++;

		}								 
	}
	if (hOld != NULL) {
		DeleteObject(hOld);
	}
	if (figure != NULL) {
		DeleteObject(figure);
	}
	DeleteDC(hMemDC);
	DeleteObject(sellectedLightCell);
	DeleteObject(sellectedDarkCell);

	DeleteObject(white);
	DeleteObject(black);
	DeleteObject(lightCell);
	DeleteObject(darkCell);
}

POINT GetCellectedCell(int x, int y, RECT rect) {
	int size = (rect.right - rect.left) / BORDER_SIZE;
	x -= rect.left;
	y -= rect.top;
	POINT result = { (int)(x / size) ,BORDER_SIZE - 1 - (int)(y / size) };
	return result;
}
void InitRect(int i, int j, RECT* rect, RECT boardRect)
{
	int cellSize = (boardRect.right - boardRect.left) / BORDER_SIZE;
	rect->left = boardRect.left + j * cellSize;
	rect->right = rect->left + cellSize;
	rect->top = boardRect.top + i * cellSize;
	rect->bottom = rect->top + cellSize;
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
bool PointInRect(int x, int y, RECT rect) {
	return (x <= rect.right) & (x >= rect.left) & (y >= rect.top) & (y <= rect.bottom);
}
inline bool PointsEqual(POINT p1, POINT p2) {
	return (p1.x == p2.x) & (p1.y == p2.y);
}

void InitPieces(HWND hWnd) {
	PIECE_BITMAP[0]  = NULL;
	PIECE_BITMAP[1]  = LoadBitmap(hInst,MAKEINTRESOURCE(WHITE_P));
	PIECE_BITMAP[2]  = LoadBitmap(hInst,MAKEINTRESOURCE(WHITE_R));
	PIECE_BITMAP[3]  = LoadBitmap(hInst,MAKEINTRESOURCE(WHITE_N));
	PIECE_BITMAP[4]  = LoadBitmap(hInst,MAKEINTRESOURCE(WHITE_B));
	PIECE_BITMAP[5]  = LoadBitmap(hInst,MAKEINTRESOURCE(WHITE_Q));
	PIECE_BITMAP[6]  = LoadBitmap(hInst,MAKEINTRESOURCE(WHITE_K));
	PIECE_BITMAP[7]  = NULL;										 
	PIECE_BITMAP[8]  = NULL;
	PIECE_BITMAP[9]  = LoadBitmap(hInst,MAKEINTRESOURCE(BLACK_P));
	PIECE_BITMAP[10] = LoadBitmap(hInst,MAKEINTRESOURCE(BLACK_R));
	PIECE_BITMAP[11] = LoadBitmap(hInst,MAKEINTRESOURCE(BLACK_N));
	PIECE_BITMAP[12] = LoadBitmap(hInst,MAKEINTRESOURCE(BLACK_B));
	PIECE_BITMAP[13] = LoadBitmap(hInst,MAKEINTRESOURCE(BLACK_Q));
	PIECE_BITMAP[14] = LoadBitmap(hInst,MAKEINTRESOURCE(BLACK_K));
 	PIECE_BITMAP[15] = NULL;
}
void FreePieces() {
	DeleteBitmap(PIECE_BITMAP[1]);
	DeleteBitmap(PIECE_BITMAP[2]);
	DeleteBitmap(PIECE_BITMAP[3]);
	DeleteBitmap(PIECE_BITMAP[4]);
	DeleteBitmap(PIECE_BITMAP[5]);
	DeleteBitmap(PIECE_BITMAP[6]);
	DeleteBitmap(PIECE_BITMAP[9]);
	DeleteBitmap(PIECE_BITMAP[10]);
	DeleteBitmap(PIECE_BITMAP[11]);
	DeleteBitmap(PIECE_BITMAP[12]);
	DeleteBitmap(PIECE_BITMAP[13]);
	DeleteBitmap(PIECE_BITMAP[14]);
}

int lastMove;
int lastPiece;
int lastFrom = 0;
int lastTo = 0;
int lastCaptured = 0;
int lastXCapture = 0;
void StartGame(HWND hWnd) {

	initGlobals();
	setLevel(g_level);
	restart();
	isComputersTurn = false;
	if (isComputerPlaying = g_isComputerPlaying) {
		if (!(isWhiteSide = (g_side == (int)WHITE))) {
			side = g_side;
			isComputersTurn = true;
		}
		SetTimer(hWnd, IDT_GAME_TIMERID, 2000, (TIMERPROC)NULL);
	}
	else {
		isWhiteSide = true;
	}
	listBox->DeleteAll();
	g_isActive = true;

}
void RestartGame(HWND hWnd) {
	initGlobals();
	restart();
	isComputersTurn = false;
	if (isComputerPlaying) {
		if (!(isWhiteSide = (side == (int)WHITE))) {			
			isComputersTurn = true;
		}
		SetTimer(hWnd, IDT_GAME_TIMERID, 2000, (TIMERPROC)NULL);
	}
	else {
		isWhiteSide = true;
	}
	listBox->DeleteAll();
	g_isActive = true;

}
void EndGame(HWND hWnd, int reason) {
	g_isActive = false;
	if (isComputerPlaying) {
		KillTimer(hWnd, IDT_GAME_TIMERID);
	}
	switch (reason) {
	case STALEMATE: {
		if (MessageBox(hWnd, L"Ничья!\nХотите начать новую игру?", L"Игра окончена", MB_OKCANCEL) == IDOK) {
			RestartGame(hWnd);
		}
		break;
	}
	case CHECKMATE: {
		if ((lastPiece & WHITE)!=0) {
			if (MessageBox(hWnd, L"Мат!Белые победили!\nХотите начать новую игру?", L"Игра окончена", MB_OKCANCEL) == IDOK) {
				RestartGame(hWnd);
			}
		}
		else {
			if (MessageBox(hWnd, L"Мат!Чёрные победили!\nХотите начать новую игру?", L"Игра окончена", MB_OKCANCEL) == IDOK) {
				RestartGame(hWnd);
			}
		}
		break;
	}
	case RESIGN: {
		if ((lastPiece & WHITE) != 0) {
			if (MessageBox(hWnd, L"Белые сдались!Победа чёрных!\nХотите начать новую игру?", L"Игра окончена", MB_OKCANCEL) == IDOK) {
				RestartGame(hWnd);
			}
		}
		else {
			if (MessageBox(hWnd, L"Чёрные сдались!Победа белых!\nХотите начать новую игру?", L"Игра окончена", MB_OKCANCEL) == IDOK) {
				RestartGame(hWnd);
			}
		}
		break;
	}
	case CANCEL: {

		break;
	}

	}
}
bool CheckEndGame() {
	return (moveResult == CHECKMATE) || (moveResult == STALEMATE);
}

wchar_t* StringToLPCWSTR(std::string str)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* text = new wchar_t[length];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, text, length);
	return text;
}
LPCWSTR GetLastMove() {
	std::string res;
	int temp = moveResult & CHECKMATE;
	res.append(1, (char)(PIECE_CHARS[((lastPiece & PIECE_MASK) + (lastPiece & BLACK))]));
	res.append(1, (char)('a' + (lastFrom % STRIDE - 1)));
	res.append(1, (char)('1' + (lastFrom / STRIDE - 2)));
	if (lastCaptured != 0) {
		res.append(1, ':');
	}
	res.append(1, (char)('a' + (lastTo % STRIDE - 1)));
	res.append(1, (char)('1' + (lastTo / STRIDE - 2)));
	if (moveResult == STALEMATE) {
		res.append(1, '-');
	}
	if (moveResult == CHECKMATE) {
		res.append(1, 'x');
	}		
	LPCWSTR result = StringToLPCWSTR(res);		
	return result;
}
bool Takeback() {
	if (!isComputerThinking) {
		lastMove = takeBackMove();
		if (lastMove > 0) {
			lastFrom = moveFrom(lastMove);
			lastTo = moveTo(lastMove);
			lastMove = 0;
			return true;
		}
		else {
			lastMove = -1;
		}

	}
	return false;
}
bool ComputerMove() {
	bool moved = false;
	lastMove = getComputerMove();
	if (lastMove == CHECKMATE || lastMove == STALEMATE) {
		moveResult = lastMove;
		isComputersTurn = false;
		lastMove = -1;
	}
	if (lastMove > 0) {

		lastFrom = moveFrom(lastMove);
		lastTo = moveTo(lastMove);
		lastPiece = board[lastFrom];
		lastCaptured = board[lastTo];
		lastXCapture = xCapture;

		moveResult = executeMove(lastMove);
		switch (moveResult) {
		case VALID_MOVE:
		case CHECKMATE:
		case STALEMATE: {
			registerMove(lastMove, lastPiece, lastCaptured, lastXCapture);
			moved = true;
			if (moveResult == VALID_MOVE) {
				isComputersTurn = !isComputersTurn;
			}
			else {
				isComputersTurn = false;
			}
			break;
		}

		case IN_CHECK: {
			lastMove = -1;
			break;
		}

		case INVALID_MOVE: {
			lastMove = -1;
			break;
		}
		}
	}
	if (moveResult == CHECKMATE) {
		lastMove = MATE;
	}
	else if (moveResult == STALEMATE) {
		lastMove = MATE;
	}
	return moved;
}
bool Move(POINT from, POINT to) {
	int result;
	bool moved = false;
	if (!PointsEqual(from, to)) {

		lastMove = getUserMove2(from.x, from.y, to.x, to.y);
		result = makeMove(from.x, from.y, to.x, to.y);

		if (lastMove == CHECKMATE || lastMove == STALEMATE) {
			moveResult = lastMove;
			isComputersTurn = false;
			lastMove = -1;
		}
		if (lastMove > 0) {
			lastFrom = moveFrom(lastMove);
			lastTo = moveTo(lastMove);
			lastPiece = board[lastFrom];
			lastCaptured = board[lastTo];
			lastXCapture = xCapture;

			moveResult = executeMove(lastMove);
			switch (moveResult) {
			case VALID_MOVE:
			case CHECKMATE:
			case STALEMATE: {
				registerMove(lastMove, lastPiece, lastCaptured, lastXCapture);
				moved = true;
				if (isComputerPlaying) {
					if (moveResult == VALID_MOVE) {
						isComputersTurn = !isComputersTurn;
					}
					else {
						isComputersTurn = false;
					}
				}
				else {
					if (moveResult == VALID_MOVE) {
						isWhiteSide = !isWhiteSide;
					}
					else {
						isComputersTurn = false;
					}
				}
				break;
			}

			case IN_CHECK: {
				lastMove = -1;
				break;
			}

			case INVALID_MOVE: {
				lastMove = -1;
				break;
			}
			}
		}
		if (moveResult == CHECKMATE) {
			lastMove = MATE;
		}
		else if (moveResult == STALEMATE) {
			lastMove = MATE;
		}
	}
	return moved;
}