#pragma once

#include <UIlib.h>
#include <windows.h>

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

class CDuiFrameWnd : public WindowImplBase
{
public:
    virtual LPCTSTR     GetWindowClassName() const  { return _T("DUIMainFrame"); }
    virtual CDuiString  GetSkinFile()               { return _T("duilib.xml"); }
    virtual CDuiString  GetSkinFolder()             { return _T(""); }
};
