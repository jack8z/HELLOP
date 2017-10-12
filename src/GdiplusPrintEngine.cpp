#include "GdiplusPrintEngine.h"

#include <Winspool.h>
#include "Strsafe.h"

#include "easylogging++.h"

GdiplusPrintEngine::GdiplusPrintEngine(){
    // do nothing
}

GdiplusPrintEngine::~GdiplusPrintEngine(){
    // do nothing
}


int GdiplusPrintEngine::DoPrint(){
    // Initialize GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    DWORD size;
    HDC hdcPrint;

    DOCINFO docInfo;
    ZeroMemory(&docInfo, sizeof(docInfo));
    docInfo.cbSize = sizeof(docInfo);
    docInfo.lpszDocName = _T("GdiplusPrint");

    // Get the size of the default printer name.
    GetDefaultPrinter(NULL, &size);

    // Allocate a buffer large enough to hold the printer name.
    TCHAR* buffer = new TCHAR[size];

    // Get the printer name.
    if(!GetDefaultPrinter(buffer, &size))
    {
        LOG(DEBUG) << "GetDefaultPrinter Failure" << std::endl;
    }
    else
    {
        // Get a device context for the printer.
        hdcPrint = CreateDC(NULL, buffer, NULL, NULL);

        StartDoc(hdcPrint, &docInfo);
        Graphics* graphics;
        Pen* pen = new Pen(Color(255, 0, 0, 0));
        SolidBrush *brush = new SolidBrush(Color(255, 0, 0, 255)); 

        // 创建字体
        FontFamily  fontFamily(L"微软雅黑");
        Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);

        StartPage(hdcPrint);
        graphics = new Graphics(hdcPrint);
        graphics->DrawRectangle(pen, 50, 50, 200, 300);
        graphics->DrawLine(pen, 20, 10, 200, 100);
        PointF pointF(10.0f, 20.0f);
        graphics->DrawString(L"您好，World!", -1, &font, pointF, brush);
        RectF rectF(30.0f, 100.0f, 200.0f, 122.0f); 
        graphics->DrawString(L"从《英伦对决》到《追凶》，近期热映的影片无意间掀起了一股“老牌对战”的浪潮，不管是邦德气质的战斗，还是港片基因的对打，其实观众所追求的快感往往就是扣动扳机那一刻——或是带着激情碰撞的暴力，或是带着快意恩仇的狠绝。以下这些电影便会给你带来这最原始的撸片爽感，可谓一道又一道有滋有味的爆裂大餐。", -1, &font, rectF, NULL, brush);
        graphics->DrawRectangle(pen, rectF);
        delete graphics;
        EndPage(hdcPrint);

        StartPage(hdcPrint);
        graphics = new Graphics(hdcPrint);
        graphics->DrawEllipse(pen, 50, 50, 200, 300);
        // 枚举所有安装的字体 ++++++++++
        /*
        InstalledFontCollection installedFontCollection; 
        int count = 0;
        count = installedFontCollection.GetFamilyCount();
        FontFamily* pFontFamily = new FontFamily[count]; 
        int found = 0;
        installedFontCollection.GetFamilies(count, pFontFamily, &found);
        WCHAR        familyName[LF_FACESIZE];
        WCHAR*       familyList = new WCHAR[count*(sizeof(familyName)+ 3)];
        StringCchCopyW(familyList, 1, L"");
        for(int j = 0; j < count; ++j) {
            pFontFamily[j].GetFamilyName(familyName);
            StringCchCatW(familyList, count*(sizeof(familyName)+ 3), familyName);
            StringCchCatW(familyList, count*(sizeof(familyName)+ 3), L",  "); 
        }
        RectF        rectF2(10.0f, 10.0f, 500.0f, 500.0f);
        graphics->DrawString(familyList, -1, &font, rectF2, NULL, brush);
        delete pFontFamily;
        delete familyList;
        */
        // 枚举所有安装的字体 ----------
        delete graphics;
        EndPage(hdcPrint);

        delete brush;
        delete pen;
        EndDoc(hdcPrint);

        DeleteDC(hdcPrint);
    }

    delete buffer;

    GdiplusShutdown(gdiplusToken);

    return 0;
}
