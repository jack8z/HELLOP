﻿#include "ECMAScriptProcessor.h"
#include "BarcodeRender.h"

hellop::ECMAScriptProcessor::ECMAScriptProcessor(HDC hdcPrinter) {
	m_hdcPrinter = hdcPrinter;
	StartPage(m_hdcPrinter);
	m_pGraphics = new Graphics(m_hdcPrinter);
	// m_pGraphics->SetPageUnit(UnitMillimeter); // 设置页面尺寸的单位为：毫米
	m_pGraphics->SetPageUnit(UnitPixel); // 设置页面尺寸的单位为：毫米

	m_pDukContext = duk_create_heap_default();
	if (!m_pDukContext) {
		LOG(ERROR) << "Failed to create a Duktape heap!\n" << std::endl;
	}
	
	m_pDefaultPen = new Pen(Color(255, 0, 0, 0));
	m_pDefaultBrush = new SolidBrush(Color(255, 0, 0, 0)); 

	// 创建字体
	FontFamily fontFamily(L"微软雅黑");
	m_pDefaultFont = new Font(&fontFamily, 12, FontStyleRegular, UnitPoint);

	initDuktape();
}

hellop::ECMAScriptProcessor::~ECMAScriptProcessor() {
	EndPage(m_hdcPrinter);
	if (NULL!=m_pGraphics) {
		delete m_pGraphics;
	}
	
	try {
		dukglue_invalidate_object(m_pDukContext, this);
	} catch(...) {
		LOG(ERROR) << "Failed to unregister c++ this object!!"  << std::endl;
	}
	
	if (NULL!=m_pDukContext) {
		duk_destroy_heap(m_pDukContext);
	}

	if (NULL!=m_pDefaultPen) {
		delete m_pDefaultPen;
	}
	if (NULL!=m_pDefaultBrush) {
		delete m_pDefaultBrush;
	}
	if (NULL!=m_pDefaultFont) {
		delete m_pDefaultFont;
	}
}

int hellop::ECMAScriptProcessor::initDuktape() {
	try {
		dukglue_register_global(m_pDukContext, this, "hellop");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::log_debug, "debug");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addText, "addText");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addLine, "addLine");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addRectangle, "addRectangle");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addEllipse, "addEllipse");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addBarCode, "addBarCode");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addQrCode, "addQrCode");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addNewPage, "addNewPage");
	} catch(...) {
		LOG(ERROR) << "Failed to register c++ object and method!!"  << std::endl;
		return -1;
	}

	return 0;
}

void hellop::ECMAScriptProcessor::log_debug(std::string text) {
	LOG(DEBUG) << text << std::endl;
}

void hellop::ECMAScriptProcessor::push_file_as_string(std::string fileName) {
	/*
	std::string data;
	if(0==read_file(fileName, data)){
		duk_push_lstring(m_pDukContext, data.c_str(), data.size());
		LOG(DEBUG) << "Js File( " << fileName << " ) size : " << data.size() << std::endl;
	} else {
		LOG(ERROR) << "Failed to Open file : " << fileName << std::endl;
        duk_push_undefined(m_pDukContext);
	}
	*/
	FILE *f;
    size_t len;
    char buf[16384];

    f = fopen(fileName.c_str(), "rb");
    if (f) {
        len = fread((void *) buf, 1, sizeof(buf), f);
		fclose(f);

		LOG(DEBUG) << "Js File( " << fileName << " ) size : " << len << std::endl;

        duk_push_lstring(m_pDukContext, (const char *) buf, (duk_size_t) len);
    } else {
		LOG(ERROR) << "Failed to Open file : " << fileName << std::endl;
        duk_push_undefined(m_pDukContext);
	}
}

void hellop::ECMAScriptProcessor::addText(std::string text, double x, double y, std::string style) {
	if (NULL!=m_pGraphics) {
		// 将毫米转为像素
		int pixelX = mm_to_px(x, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY = mm_to_px(y, dpi_to_ppi(m_pGraphics->GetDpiY()));

		PointF pointF((REAL)pixelX, (REAL)pixelY);
		m_pGraphics->DrawString(todop_to_wstring(text).c_str(), -1, m_pDefaultFont, pointF, m_pDefaultBrush);
	}
}

void hellop::ECMAScriptProcessor::addLine(double x1, double y1, double x2, double y2, std::string style) {
	if (NULL != m_pGraphics) {
		// 将毫米转为像素
		int pixelX1 = mm_to_px(x1, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY1 = mm_to_px(y1, dpi_to_ppi(m_pGraphics->GetDpiY()));
		int pixelX2 = mm_to_px(x2, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY2 = mm_to_px(y2, dpi_to_ppi(m_pGraphics->GetDpiY()));

		m_pGraphics->DrawLine(m_pDefaultPen, (REAL)pixelX1, (REAL)pixelY1, (REAL)pixelX2, (REAL)pixelY2);
	}
}

void hellop::ECMAScriptProcessor::addRectangle(double x1, double y1, double x2, double y2, std::string style) {
	if (NULL != m_pGraphics) {
		// 将毫米转为像素
		int pixelX1 = mm_to_px(x1, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY1 = mm_to_px(y1, dpi_to_ppi(m_pGraphics->GetDpiY()));
		int pixelX2 = mm_to_px(x2, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY2 = mm_to_px(y2, dpi_to_ppi(m_pGraphics->GetDpiY()));

		m_pGraphics->DrawRectangle(m_pDefaultPen, (REAL)pixelX1, (REAL)pixelY1, (REAL)pixelX2, (REAL)pixelY2);
	}
}

void hellop::ECMAScriptProcessor::addEllipse(double x1, double y1, double x2, double y2, std::string style) {
	if (NULL != m_pGraphics) {
		// 将毫米转为像素
		int pixelX1 = mm_to_px(x1, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY1 = mm_to_px(y1, dpi_to_ppi(m_pGraphics->GetDpiY()));
		int pixelX2 = mm_to_px(x2, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY2 = mm_to_px(y2, dpi_to_ppi(m_pGraphics->GetDpiY()));

		m_pGraphics->DrawEllipse(m_pDefaultPen, (REAL)pixelX1, (REAL)pixelY1, (REAL)pixelX2, (REAL)pixelY2);
	}
}

void hellop::ECMAScriptProcessor::addBarCode(std::string text, double x, double y, std::string style) {
	if (NULL != m_pGraphics) {
		// 将毫米转为像素
		int pixelX = mm_to_px(x, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY = mm_to_px(y, dpi_to_ppi(m_pGraphics->GetDpiY()));

		BarcodeRender barcodeRender(m_pGraphics);
		barcodeRender.drawCode128Auto(todop_to_wstring(text), pixelX, pixelY);
	}
}

void hellop::ECMAScriptProcessor::addQrCode(std::string text, double x, double y, std::string style) {
	if (NULL != m_pGraphics) {
		// 将毫米转为像素
		int pixelX = mm_to_px(x, dpi_to_ppi(m_pGraphics->GetDpiX()));
		int pixelY = mm_to_px(y, dpi_to_ppi(m_pGraphics->GetDpiY()));

		BarcodeRender barcodeRender(m_pGraphics);
		barcodeRender.drawQrcode(todop_to_wstring(text), pixelX, pixelY);
	}
}

void hellop::ECMAScriptProcessor::addNewPage() {
	if (NULL != m_pGraphics) {
		delete m_pGraphics;
	}
	
	EndPage(m_hdcPrinter);
	StartPage(m_hdcPrinter);
	
	m_pGraphics = new Graphics(m_hdcPrinter);
	// m_pGraphics->SetPageUnit(UnitMillimeter); // 设置页面尺寸的单位为：毫米
	m_pGraphics->SetPageUnit(UnitPixel); // 设置页面尺寸的单位为：毫米
}

int hellop::ECMAScriptProcessor::doRun() {
	std::string fileName("render.js");
	push_file_as_string(fileName);

	try {
		if (duk_peval(m_pDukContext) != 0) {
			// if an error occured while executing, print the stack trace
			duk_get_prop_string(m_pDukContext, -1, "stack");
			LOG(ERROR) << "Duktape Error: " << duk_safe_to_string(m_pDukContext, -1) << std::endl;
			duk_pop(m_pDukContext);
            return -1;
		}
	} catch(...) {
		LOG(ERROR) << "Failed to run ECMAScript"  << std::endl;
	}

    return 0;
}
