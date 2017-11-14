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
        LOG(ERROR) << "No Printer" << std::endl;
        return -1;
    } else {
        // Get a device context for the printer.
        m_hdcPrinter = CreateDCW(NULL, m_printerName.c_str(), NULL, NULL);
    
        if (!m_hdcPrinter) { // 获取打印机上下文失败
            LOG(ERROR) << "CreateDC Failure" << std::endl;
            return -2;
        }

        int startResult = StartDoc(m_hdcPrinter, &docInfo);
		if (startResult < 1) {
            LOG(DEBUG) << "StartDoc Failure : " << startResult << std::endl;
			return -3;
		}

		// 解析ECMAScript，并执行渲染指令
        ECMAScriptProcessor *pScriptProcessor = new ECMAScriptProcessor(m_hdcPrinter);
        int scriptRet = pScriptProcessor->doRun();
        delete pScriptProcessor;

        EndDoc(m_hdcPrinter);

        if (m_hdcPrinter) {
            DeleteDC(m_hdcPrinter);
        }

        if (0!=scriptRet) {
            return -4;
        }
    }

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
