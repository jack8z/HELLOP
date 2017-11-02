#pragma once

#include "afxTodop.h"
#include "litehtml.h"
#include "gdiplus/gdiplus_container.h"

#pragma comment(lib, "litehtml.lib")

#include "easylogging++.h"

// Html渲染类，使用litehtml库渲染Html代码
class HtmlContainer : public gdiplus_container {
public:
    HtmlContainer(HDC hdcPrinter);
    ~HtmlContainer();

private:
    litehtml::document::ptr		m_doc;

    HDC m_hdcPrinter; // 打印机上下文
};
