﻿#include "GdiplusPrintEngine.h"

#include <exception>
#include <Winspool.h>
#include "Strsafe.h"

#include "ECMAScriptProcessor.h"
#include "BarcodeRender.h"

#include "easylogging++.h"

hellop::GdiplusPrintEngine::GdiplusPrintEngine() {
    TString printerName;
    initInstance(printerName);
}

hellop::GdiplusPrintEngine::GdiplusPrintEngine(TString printerName) {
    initInstance(printerName);    
}

void hellop::GdiplusPrintEngine::initInstance(TString printerName) {
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

hellop::GdiplusPrintEngine::~GdiplusPrintEngine() {
    if (m_gdiplusToken) {
        GdiplusShutdown(m_gdiplusToken);
    }
}

void hellop::GdiplusPrintEngine::logErrorMsg(LPTSTR lpszFunction) { 
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s Failed with Error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    LOG(ERROR) << (LPCTSTR)lpDisplayBuf << std::endl;

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}


int hellop::GdiplusPrintEngine::doPrint() {
    DOCINFO docInfo;
    ZeroMemory(&docInfo, sizeof(docInfo));
    docInfo.cbSize = sizeof(docInfo);
    docInfo.lpszDocName = _T("todop");

    // 如果打印机为空，则返回-1
    if (m_printerName.empty()) {
        LOG(ERROR) << "No Printer" << std::endl;
        return -1;
    } else {
		// Get a printer handle.
		if (!OpenPrinterW(const_cast<LPWSTR>(m_printerName.c_str()), &m_printerHandle, NULL)) {
			logErrorMsg(_T("OpenPrinter"));
			return -3;
		}

		LPDEVMODEW   pDevMode;
		DWORD       dwNeeded, dwRet;

		/*
		* Step 1:
		* Allocate a buffer of the correct size.
		*/
		dwNeeded = DocumentPropertiesW(NULL,
			m_printerHandle,       /* Handle to our printer. */
			const_cast<LPWSTR>(m_printerName.c_str()),        /* Name of the printer. */
			NULL,           /* Asking for size, so */
			NULL,           /* these are not used. */
			0);             /* Zero returns buffer size. */
		pDevMode = (LPDEVMODEW)malloc(dwNeeded);

		/*
		* Step 2:
		* Get the default DevMode for the printer and
		* modify it for your needs.
		*/
		dwRet = DocumentPropertiesW(NULL,
			m_printerHandle,
			const_cast<LPWSTR>(m_printerName.c_str()),
			pDevMode,       /* The address of the buffer to fill. */
			NULL,           /* Not using the input buffer. */
			DM_OUT_BUFFER); /* Have the output buffer filled. */
		if (dwRet != IDOK)
		{
			/* If failure, cleanup and return failure. */
			free(pDevMode);
			ClosePrinter(m_printerHandle);
			return -4;
		}

		/*
		* Make changes to the DevMode which are supported.
		*/
		if (pDevMode->dmFields & DM_ORIENTATION) {
			/* If the printer supports paper orientation, set it.*/
            pDevMode->dmOrientation = DMORIENT_LANDSCAPE; // 打印方向：横向
            pDevMode->dmOrientation = DMORIENT_PORTRAIT; // 打印方向：纵向
		}
		if (pDevMode->dmFields & DM_DUPLEX) {
			/* If it supports duplex printing, use it. */
			pDevMode->dmDuplex = DMDUP_HORIZONTAL;
		}
		if (pDevMode->dmFields & DM_PAPERSIZE) {
			pDevMode->dmFields = pDevMode->dmFields | DM_PAPERSIZE | DM_PAPERLENGTH | DM_PAPERWIDTH;
			//pDevMode->dmPaperSize = DMPAPER_USER; // 自定义纸张，其值只能为0或DMPAPER-prefixed
            pDevMode->dmPaperSize = 0; // 自定义纸张，只有为0时，下面两个参数（dmPaperWidth和dmPaperLength）才有效
			pDevMode->dmPaperWidth = 1000; // 纸张的宽度，以0.1mm为单位
			pDevMode->dmPaperLength = 1800; // 纸张的高度，以0.1mm为单位
		}

		/*
		* Step 3:
		* Merge the new settings with the old.
		* This gives the driver an opportunity to update any private
		* portions of the DevMode structure.
		*/
		dwRet = DocumentPropertiesW(NULL,
			m_printerHandle,
			const_cast<LPWSTR>(m_printerName.c_str()),
			pDevMode,       /* Reuse our buffer for output. */
			pDevMode,       /* Pass the driver our changes. */
			DM_IN_BUFFER |  /* Commands to Merge our changes and */
			DM_OUT_BUFFER); /* write the result. */

		if (dwRet != IDOK) {
			ClosePrinter(m_printerHandle);
			free(pDevMode);
			return -5;
		}

        // Get a device context for the printer.
        m_hdcPrinter = CreateDCW(NULL, m_printerName.c_str(), NULL, pDevMode);
    
        if (!m_hdcPrinter) { // 获取打印机上下文失败
            LOG(ERROR) << "CreateDC Failure" << std::endl;
            return -2;
        }

        int startResult = StartDoc(m_hdcPrinter, &docInfo);
		if (startResult < 1) {
            LOG(DEBUG) << "StartDoc Failure : " << startResult << std::endl;
			return -10;
		}

        // 测试 +++
        hellop::TString formsBuff;
        std::list<hellop::TString> formList = getPrinterForms(m_hdcPrinter);
        for (std::list<TString>::iterator it=formList.begin(); it!=formList.end(); ++it) {
            formsBuff += *it;
            if (it!=formList.end()) {
                formsBuff += L",";
            }
        }

        LOG(DEBUG) << formsBuff.c_str() << endl;
        // 测试 ---

		// 解析ECMAScript，并执行渲染指令
        ECMAScriptProcessor *pScriptProcessor = new ECMAScriptProcessor(m_hdcPrinter);
        int scriptRet = pScriptProcessor->doRun();
        delete pScriptProcessor;

        EndDoc(m_hdcPrinter);

        if (m_hdcPrinter) {
			free(pDevMode);
            ClosePrinter(m_printerHandle);
            DeleteDC(m_hdcPrinter);
        }

        if (0!=scriptRet) {
            return -100;
        }
    }

    return 0;
}

std::list<hellop::TString> hellop::GdiplusPrintEngine::getSystemFontFamilys() {
    std::list<hellop::TString> fontFamilyList;

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
                hellop::TString strFamilyName(tmpFamilyName);
                fontFamilyList.push_back(strFamilyName);
            }
        }
    }
    delete []pFontFamily;

    return fontFamilyList;
}

std::list<hellop::TString> hellop::GdiplusPrintEngine::getLocalPrinters() {
    std::list<hellop::TString> printerList;

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

std::list<hellop::TString> hellop::GdiplusPrintEngine::getPrinterForms(HANDLE printerHandle) {
    std::list<hellop::TString> formList;

    DWORD pcbNeeded, pcbReturned;

    if(!EnumForms(printerHandle, 1, NULL, 0, &pcbNeeded, &pcbReturned)){
        LOG(ERROR) << "EnumForms Error " << std::endl;
        return formList;
    }

    FORM_INFO_1 *pFormOne = NULL;

    try {
        if (EnumForms(printerHandle, 1, LPBYTE(pFormOne), pcbNeeded, &pcbNeeded, &pcbReturned)) {
            throw exception("Could not Enumerate forms");
        }

        if (pcbReturned>0) {
            for (unsigned int i=0; i<pcbReturned; i++) {
                std::string formName(pFormOne[i].pName);
                formList.push_back(todop_to_wstring(formName));
            }
        }
    } catch (...) {
        LOG(ERROR) << "EnumForms Error " << std::endl;
    }

    return formList;
}
