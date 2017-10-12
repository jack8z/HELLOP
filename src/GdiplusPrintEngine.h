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

class GdiplusPrintEngine
{
public:
    GdiplusPrintEngine();
    ~GdiplusPrintEngine();
    
    int DoPrint(); // 打印

private:
    TString m_printerName; // 打印机名称
    HDC m_hdcPrinter; // 打印机上下文件
};
