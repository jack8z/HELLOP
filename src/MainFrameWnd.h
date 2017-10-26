#pragma once

#include "afxTodop.h"

using namespace DuiLib;

#ifdef _DEBUG
    #ifdef _UNICODE
        #pragma comment(lib, "DuiLib_ud.lib")
    #else
        #pragma comment(lib, "DuiLib_d.lib")
    #endif
#else
    #ifdef _UNICODE
        #pragma comment(lib, "DuiLib_u.lib")
    #else
        #pragma comment(lib, "DuiLib.lib")
    #endif
#endif

class CMainFrameWnd : public WindowImplBase
{
public:
    virtual LPCTSTR     GetWindowClassName() const  { return _T("MainFrameWnd"); }
    virtual CDuiString  GetSkinFile()               { return _T("main.xml"); }
    virtual CDuiString  GetSkinFolder()             { return _T(""); }
};
