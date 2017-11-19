#pragma once

#include "afxTodop.h"
#include "HtmlContainer.h"

#include "easylogging++.h"

namespace hellop {
    // Html渲染类，使用litehtml库渲染Html代码
    class HtmlRender {
    public:
        HtmlRender(HDC hdcPrinter);
        ~HtmlRender();

        void drawHtml(std::wstring html, double x, double y, double width, double height);

    private:
        HDC m_hdcPrinter; // 打印机上下文
        Graphics *m_pGraphics;
        
        litehtml::context *m_pLiteContext;
        HtmlContainer *m_pHtmlContainer;
    };
}
