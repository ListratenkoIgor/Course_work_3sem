#pragma once
#include <windows.h>

class GameListBox {
private:
	HWND hListBoxWhite = NULL;
	HWND hListBoxBlack = NULL;
	int lastIndex = -1;
	RECT rect;
public:

	GameListBox(HWND hWnd, RECT rect, int ID_LIST_WHITE = NULL) {
		this->rect = rect;
		int left = rect.left;
		int top = rect.top;
		int width = (rect.right - rect.left) / 2;
		int height = rect.bottom - rect.top;
		hListBoxWhite = CreateWindow(L"listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_MULTICOLUMN |
			WS_BORDER | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOSEL | LBS_NOINTEGRALHEIGHT,
			left, top, width, height, hWnd, (HMENU)ID_LIST_WHITE,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL, NULL);
		left += width;
		hListBoxBlack = CreateWindow(L"listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_MULTICOLUMN |
			WS_BORDER | LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOSEL | LBS_NOINTEGRALHEIGHT,
			left, top, width, height, hWnd, (HMENU)(ID_LIST_WHITE),
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL, NULL);
		ListBox_SetColumnWidth(hListBoxWhite, width);
		ListBox_SetColumnWidth(hListBoxBlack, width);
	}
	~GameListBox() {
		DestroyWindow(hListBoxWhite);
		DestroyWindow(hListBoxBlack);
	}
	void AddMove(LPCWSTR text) {
		this->lastIndex += 1;
		if (text != nullptr) {
			if (this->lastIndex % 2 == 0) {
				ListBox_AddString(hListBoxWhite, text);
			}
			else {
				ListBox_AddString(hListBoxBlack, text);
			}
		}
	}
	HWND GetHandle() {
		return hListBoxWhite;
		if (this->lastIndex % 2 == 0) {
			return hListBoxWhite;
		}
		else {
			return hListBoxBlack;
		}
	}
	bool Empty() {
		return lastIndex < 0;
	}
	void DeleteMove() {
		if (lastIndex > -1) {
			if (this->lastIndex % 2 == 0) {
				ListBox_DeleteString(hListBoxWhite, lastIndex / 2);
			}
			else {
				ListBox_DeleteString(hListBoxBlack, lastIndex / 2);
			}
			this->lastIndex--;
		}
	}
	void DeleteAll() {
		while (this->lastIndex > -1) {
			DeleteMove();
		}

	}

	void Move(RECT rect) {
		this->rect = rect;
		int left = rect.left;
		int top = rect.top;
		int width = (rect.right - rect.left) / 2;
		int height = rect.bottom - rect.top;
		MoveWindow(hListBoxWhite, left, top, width, height, true);
		left += width;
		MoveWindow(hListBoxBlack, left, top, width, height, true);
		ListBox_SetColumnWidth(hListBoxWhite, width);
		ListBox_SetColumnWidth(hListBoxBlack, width);
	}

};

