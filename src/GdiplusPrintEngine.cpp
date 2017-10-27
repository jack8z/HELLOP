#include "GdiplusPrintEngine.h"

#include <exception>
#include <Winspool.h>
#include "Strsafe.h"

#include "ECMAScriptProcessor.h"
#include "BarcodeRender.h"

#include "easylogging++.h"

GdiplusPrintEngine::GdiplusPrintEngine() {
    TString printerName;
    initInstance(printerName);
}

GdiplusPrintEngine::GdiplusPrintEngine(TString printerName) {
    initInstance(printerName);    
}

void GdiplusPrintEngine::initInstance(TString printerName) {
    if (printerName.empty()) {
        DWORD printerNameSize; // 打印机名字的长度
        
        // Get the size of the default printer name.
        GetDefaultPrinter(NULL, &printerNameSize);
        
        // Allocate a buffer large enough to hold the printer name.
        TCHAR* pBufferPrinterName = new TCHAR[printerNameSize];
    
        // Get the printer name.
        if(!GetDefaultPrinter(pBufferPrinterName, &printerNameSize)) {
            LOG(DEBUG) << "GetDefaultPrinter Failure" << std::endl;
        } else {
            LOG(DEBUG) << "GetDefaultPrinter Success : " << pBufferPrinterName << std::endl;
            std::string tmpPrinterName(pBufferPrinterName);
            m_printerName = todop_to_wstring(tmpPrinterName);
        }

        delete pBufferPrinterName;
    } else {
        m_printerName = printerName;
    }

    // Initialize GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

GdiplusPrintEngine::~GdiplusPrintEngine() {
    if (m_gdiplusToken) {
        GdiplusShutdown(m_gdiplusToken);
    }
}


int GdiplusPrintEngine::doPrint() {
    DOCINFO docInfo;
    ZeroMemory(&docInfo, sizeof(docInfo));
    docInfo.cbSize = sizeof(docInfo);
    docInfo.lpszDocName = _T("todop");

    // 如果打印机为空，则返回-1
    if (m_printerName.empty()) {
        LOG(DEBUG) << "No Printer" << std::endl;
        return -1;
    } else {
        // Get a device context for the printer.
        m_hdcPrinter = CreateDCW(NULL, m_printerName.c_str(), NULL, NULL);
    
        if (!m_hdcPrinter) { // 获取打印机上下文失败
            LOG(DEBUG) << "CreateDC Failure" << std::endl;
            return -2;
        }

        StartDoc(m_hdcPrinter, &docInfo);
        Graphics* graphics;
        Pen* pen = new Pen(Color(255, 0, 0, 0));
        SolidBrush *brush = new SolidBrush(Color(255, 0, 0, 255)); 

        // 创建字体
        FontFamily  fontFamily(L"微软雅黑");
        Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);

        // 第一页 +++
        StartPage(m_hdcPrinter);
        graphics = new Graphics(m_hdcPrinter);
        graphics->DrawRectangle(pen, 50, 310, 200, 300);
        graphics->DrawLine(pen, 20, 10, 200, 100);
        PointF pointF(10.0f, 20.0f);
        graphics->DrawString(L"您好，World!", -1, &font, pointF, brush);
        RectF rectF(30.0f, 100.0f, 300.0f, 200.0f); 
        graphics->DrawString(L"从《英伦对决》到《追凶》，近期热映的影片无意间掀起了一股“老牌对战”的浪潮，不管是邦德气质的战斗，还是港片基因的对打，其实观众所追求的快感往往就是扣动扳机那一刻——或是带着激情碰撞的暴力，或是带着快意恩仇的狠绝。以下这些电影便会给你带来这最原始的撸片爽感，可谓一道又一道有滋有味的爆裂大餐。", -1, &font, rectF, NULL, brush);
        graphics->DrawRectangle(pen, rectF);
        delete graphics;
        EndPage(m_hdcPrinter);
        // 第一页 ---

        // 第二页 +++
        StartPage(m_hdcPrinter);
        graphics = new Graphics(m_hdcPrinter);
        graphics->DrawEllipse(pen, 10, 10, 200, 200);
        
        // 绘制条码 +++
        BarcodeRender barcodeRender(graphics);
        TString sid_content(L"886821653780468974");
        barcodeRender.drawCode128Auto(sid_content, 50, 210);
		// 绘制QR码 +++
        TString qr_content(L"“我打”诞生于2010年，是千牛服务平台上一款专门帮助中小淘宝卖家打单发货的应用软件，一直以来，它始终深耕于解决打印面单这一核心功能。这让它在该细分服务市场领域内立有一席之地。");
        barcodeRender.drawQrcode(qr_content, 50, 350);

        delete graphics;
        EndPage(m_hdcPrinter);
        // 第二页 ---

        // 第三页 +++
        StartPage(m_hdcPrinter);
        graphics = new Graphics(m_hdcPrinter);
        ECMAScriptProcessor *pScriptProcessor = new ECMAScriptProcessor(graphics);
        pScriptProcessor->_DO_RUN_RUN();
        delete pScriptProcessor;
        delete graphics;
        EndPage(m_hdcPrinter);
        // 第三页 ---

        delete brush;
        delete pen;
        EndDoc(m_hdcPrinter);

        if (m_hdcPrinter) {
            DeleteDC(m_hdcPrinter);
        }
    }

    // delete buffer;

    // GdiplusShutdown(gdiplusToken);

    return 0;
}

std::list<TString> GdiplusPrintEngine::getSystemFontFamilys() {
    std::list<TString> fontFamilyList;

    // 枚举所有安装的字体
    InstalledFontCollection installedFontCollection; 
    int familyCount = 0;
    familyCount = installedFontCollection.GetFamilyCount();
    FontFamily* pFontFamily = new FontFamily[familyCount];
    int foundCount = 0;
    installedFontCollection.GetFamilies(familyCount, pFontFamily, &foundCount);
    LOG(DEBUG) << "familyCount=" << familyCount <<" , foundCount=" << foundCount << std::endl;
    if (foundCount > 0) {
        for (int i=0; i<foundCount; i++) {
            WCHAR tmpFamilyName[LF_FACESIZE];
            ZeroMemory(tmpFamilyName, sizeof(tmpFamilyName));

            Status ret = pFontFamily[i].GetFamilyName(tmpFamilyName);
            
            // LOG(DEBUG) << "[ " << i << " ] " << "GetFamilyName Status : " << ret << std::endl;
            if (Ok == ret) {
                // LOG(DEBUG) << "[ " << i << " ] " << tmpFamilyName << std::endl;
                TString strFamilyName(tmpFamilyName);
                fontFamilyList.push_back(strFamilyName);
            }
        }
    }
    delete []pFontFamily;

    return fontFamilyList;
}

std::list<TString> GdiplusPrintEngine::getLocalPrinters() {
    std::list<TString> printerList;

    LPBYTE pPrinterEnum;
    DWORD pcbNeeded, pcbReturned;
    PRINTER_INFO_2 *piTwo = NULL;

    // get the list of printers installed on this system
    EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &pcbNeeded, &pcbReturned);

    try {
        pPrinterEnum = new BYTE[pcbNeeded];
        if (!EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, pPrinterEnum, pcbNeeded, &pcbNeeded, &pcbReturned)) {
            throw exception("Could not Enumerate printers");
        }

        piTwo = (PRINTER_INFO_2 *)pPrinterEnum;

        for (unsigned int i=0; i<pcbReturned; i++) {
            std::string printerName(piTwo[i].pPrinterName);
            printerList.push_back(todop_to_wstring(printerName));
        }
    } catch (...) {
        LOG(ERROR) << "EnumPrinters Error " << std::endl;
    }

    if (pPrinterEnum) {
        delete[] pPrinterEnum;
        pPrinterEnum = NULL;
    }

    return printerList;
}
