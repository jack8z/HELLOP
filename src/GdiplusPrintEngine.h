#pragma once

#include "afxTodop.h"

// gdi+ ++++++++++
#define NOMINMAX
#include <algorithm>
/*
namespace Gdiplus
{
  using std::min;
  using std::max;
};
#ifndef ULONG_PTR   
#define ULONG_PTR unsigned long* 
#endif
*/
#include <gdiplus.h>

using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")
// gdi+ ----------

namespace hellop {
    class GdiplusPrintEngine
    {
    public:
        GdiplusPrintEngine(); // 取系统默认打印机
        GdiplusPrintEngine(TString printerName);
        ~GdiplusPrintEngine();
        
        int doPrint(); // 打印
        std::list<TString> getSystemFontFamilys(); // 获取系统字体列表
        std::list<TString> getLocalPrinters(); // 获取本机安装的打印机列表

    private:
        void initInstance(TString printerName); // 初始化实例
        void logErrorMsg(LPTSTR lpszFunction);

    private:
        TString m_printerName; // 打印机名称
        HDC m_hdcPrinter; // 打印机上下文件
        HANDLE m_printerHandle; // 打印机句柄
        ULONG_PTR m_gdiplusToken;
    };
}
