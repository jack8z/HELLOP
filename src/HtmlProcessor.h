#pragma once

#include "afxTodop.h"
#include "HtmlContainer.h"

#include "easylogging++.h"

// Html渲染类，使用litehtml库渲染Html代码
class HtmlProcessor {
public:
    HtmlProcessor(HDC hdcPrinter);
    ~HtmlProcessor();

private:
    HDC m_hdcPrinter; // 打印机上下文
    
    litehtml::context *m_pLiteContext;
    HtmlContainer *m_pHtmlContainer;
};
