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

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <string>
//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE /*hModule*/)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(0, TEXT("find sum"), columnSum, NULL, false);
    
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void hello()
{
    // Open a new document
    ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;
    HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

    // Say hello now :
    // Scintilla control has no Unicode mode, so we use (char *) here
    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)"Hello, Notepad++!");
}

void helloDlg()
{
    
    ::MessageBox(NULL, TEXT("Hello, Notepad++!"), TEXT("Notepad++ Plugin Template"), MB_OK);
}

void columnSum()
{
    //HWND hCurrScintilla = getCurrentScintillaHandle();
    unsigned int codepage = ::GetACP();
   
    codepage = CP_UTF8;

    if (execute(SCI_GETSELECTIONS) > 1) // Multi-Selection || Column mode
    {
        

        ColumnModeInfos cmi = getColumnModeSelectInfo();

        for (size_t i = 0, cmiLen = cmi.size(); i < cmiLen; ++i)
        {
            const int len = cmi[i]._selRpos - cmi[i]._selLpos;
            char* srcStr = new char[len + 1];
            wchar_t* destStr = new wchar_t[len + 1];

            int start = cmi[i]._selLpos;
            int end = cmi[i]._selRpos;
            
            
            getText(srcStr, start, end);

            //int nbChar = ::MultiByteToWideChar(codepage, 0, srcStr, len, destStr, len);

            

            ::WideCharToMultiByte(codepage, 0, destStr, len, srcStr, len, NULL, NULL);

            

            delete[] srcStr;
            delete[] destStr;
        }

        //setMultiSelections(cmi);

        execute(SCI_ENDUNDOACTION);
        return;
    }
    MessageBox(
        NULL,
        (LPCWSTR)L"Resource not available\nDo you want to try again?",
        (LPCWSTR)L"Account Details",
        MB_OK
    );
}
ColumnModeInfos getColumnModeSelectInfo()
{
    ColumnModeInfos columnModeInfos;
    if (execute(SCI_GETSELECTIONS) > 1) // Multi-Selection || Column mode
    {
        int nbSel = static_cast<int32_t>(execute(SCI_GETSELECTIONS));

        for (int i = 0; i < nbSel; ++i)
        {
            int absPosSelStartPerLine = static_cast<int32_t>(execute(SCI_GETSELECTIONNANCHOR, i));
            int absPosSelEndPerLine = static_cast<int32_t>(execute(SCI_GETSELECTIONNCARET, i));
            int nbVirtualAnchorSpc = static_cast<int32_t>(execute(SCI_GETSELECTIONNANCHORVIRTUALSPACE, i));
            int nbVirtualCaretSpc = static_cast<int32_t>(execute(SCI_GETSELECTIONNCARETVIRTUALSPACE, i));

            if (absPosSelStartPerLine == absPosSelEndPerLine && execute(SCI_SELECTIONISRECTANGLE))
            {
                bool dir = nbVirtualAnchorSpc < nbVirtualCaretSpc ? TRUE : FALSE;
                columnModeInfos.push_back(ColumnModeInfo(absPosSelStartPerLine, absPosSelEndPerLine, i, dir, nbVirtualAnchorSpc, nbVirtualCaretSpc));
            }
            else if (absPosSelStartPerLine > absPosSelEndPerLine)
                columnModeInfos.push_back(ColumnModeInfo(absPosSelEndPerLine, absPosSelStartPerLine, i, FALSE, nbVirtualAnchorSpc, nbVirtualCaretSpc));
            else
                columnModeInfos.push_back(ColumnModeInfo(absPosSelStartPerLine, absPosSelEndPerLine, i, TRUE, nbVirtualAnchorSpc, nbVirtualCaretSpc));
        }
    }
    return columnModeInfos;
}
HWND getCurrentScintillaHandle() {
    int currentEdit;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
	return (currentEdit == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
}
void getText(char* dest, size_t start, size_t end) 
{
    Sci_TextRange tr;
    tr.chrg.cpMin = static_cast<long>(start);
    tr.chrg.cpMax = static_cast<long>(end);
    tr.lpstrText = dest;
    execute(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
}