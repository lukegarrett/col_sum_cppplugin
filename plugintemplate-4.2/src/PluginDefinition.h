//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

//
// All difinitions of plugin interface
//
#include "PluginInterface.h"
//#include "GoToLineDlg.h"
#include <vector>
//#include <vector>
//-------------------------------------//
//-- STEP 1. DEFINE YOUR PLUGIN NAME --//
//-------------------------------------//
// Here define your plugin name
//
const TCHAR NPP_PLUGIN_NAME[] = TEXT("Get Sum");

//-----------------------------------------------//
//-- STEP 2. DEFINE YOUR PLUGIN COMMAND NUMBER --//
//-----------------------------------------------//
//
// Here define the number of your plugin commands
//
const int nbFunc = 1;


//
// Initialization of your plugin data
// It will be called while plugin loading
//
void pluginInit(HANDLE hModule);

//
// Cleaning of your plugin
// It will be called while plugin unloading
//
void pluginCleanUp();

//
//Initialization of your plugin commands
//
void commandMenuInit();

//
//Clean up your plugin commands allocation (if any)
//
void commandMenuCleanUp();

//
// Function which sets your command 
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false);


//
// Your plugin command functions
//
void hello();
void helloDlg();
void columnSum();


typedef sptr_t(*SCINTILLA_FUNC) (void*, unsigned int, uptr_t, sptr_t);
typedef void* SCINTILLA_PTR;
static SCINTILLA_FUNC _pScintillaFunc = nullptr;
static SCINTILLA_PTR  _pScintillaPtr = nullptr;

inline LRESULT execute(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0)  {
	try {
		return _pScintillaFunc(_pScintillaPtr, Msg, wParam, lParam);
	}
	catch (...)
	{
		return -1;
	}
};

struct ColumnModeInfo {
	int _selLpos;
	int _selRpos;
	int _order; // 0 based index
	bool _direction; // L2R or R2L
	int _nbVirtualCaretSpc;
	int _nbVirtualAnchorSpc;
	bool L2R = true;
	bool R2L = false;

	ColumnModeInfo() : _selLpos(0), _selRpos(0), _order(-1), _direction(TRUE), _nbVirtualAnchorSpc(0), _nbVirtualCaretSpc(0) {};
	ColumnModeInfo(int lPos, int rPos, int order, bool dir = TRUE, int vAnchorNbSpc = 0, int vCaretNbSpc = 0)
		: _selLpos(lPos), _selRpos(rPos), _order(order), _direction(dir), _nbVirtualAnchorSpc(vAnchorNbSpc), _nbVirtualCaretSpc(vCaretNbSpc) {};

	bool isValid() const {
		return (_order >= 0 && _selLpos >= 0 && _selRpos >= 0 && _selLpos <= _selRpos);
	};
};
struct SortInPositionOrder {
	bool operator() (ColumnModeInfo& l, ColumnModeInfo& r) {
		return (l._selLpos < r._selLpos);
	}
};

typedef std::vector<ColumnModeInfo> ColumnModeInfos;

typedef LRESULT(WINAPI* CallWindowProcFunc) (WNDPROC, HWND, UINT, WPARAM, LPARAM);
//const bool L2R = true;
//const bool R2L = false;
HWND getCurrentScintillaHandle();
void getText(char* dest, size_t start, size_t end);
ColumnModeInfos getColumnModeSelectInfo();
#endif //PLUGINDEFINITION_H
