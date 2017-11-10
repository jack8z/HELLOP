#include "ECMAScriptProcessor.h"
#include "BarcodeRender.h"
#include "HtmlProcessor.h"

ECMAScriptProcessor::ECMAScriptProcessor(HDC hdcPrinter) {
	m_hdcPrinter = hdcPrinter;
	StartPage(m_hdcPrinter);
    m_pGraphics = new Graphics(m_hdcPrinter);

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

ECMAScriptProcessor::~ECMAScriptProcessor() {
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

int ECMAScriptProcessor::initDuktape() {
	try {
		dukglue_register_global(m_pDukContext, this, "todop");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::log_debug, "debug");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addText, "addText");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addLine, "addLine");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addRectangle, "addRectangle");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addEllipse, "addEllipse");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addBarCode, "addBarCode");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addQrCode, "addQrCode");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addNewPage, "addNewPage");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addHtml, "addHtml");
	} catch(...) {
		LOG(ERROR) << "Failed to register c++ object and method!!"  << std::endl;
		return -1;
	}

	return 0;
}

void ECMAScriptProcessor::log_debug(std::string text) {
	LOG(DEBUG) << text << std::endl;
}

void ECMAScriptProcessor::push_file_as_string(std::string fileName) {
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

void ECMAScriptProcessor::addText(std::string text, double x, double y, std::string style) {
	if (NULL!=m_pGraphics) {
		PointF pointF(x, y);
		m_pGraphics->DrawString(todop_to_wstring(text).c_str(), -1, m_pDefaultFont, pointF, m_pDefaultBrush);
	}
}

void ECMAScriptProcessor::addLine(double x1, double y1, double x2, double y2, std::string style) {
	if (NULL != m_pGraphics) {
		m_pGraphics->DrawLine(m_pDefaultPen, (REAL)x1, (REAL)y1, (REAL)x2, (REAL)y2);
	}
}

void ECMAScriptProcessor::addRectangle(double x1, double y1, double x2, double y2, std::string style) {
	if (NULL != m_pGraphics) {
		m_pGraphics->DrawRectangle(m_pDefaultPen, (REAL)x1, (REAL)y1, (REAL)x2, (REAL)y2);
	}
}

void ECMAScriptProcessor::addEllipse(double x1, double y1, double x2, double y2, std::string style) {
	if (NULL != m_pGraphics) {
		m_pGraphics->DrawEllipse(m_pDefaultPen, (REAL)x1, (REAL)y1, (REAL)x2, (REAL)y2);
	}
}

void ECMAScriptProcessor::addBarCode(std::string text, double x, double y, std::string style) {
	if (NULL != m_pGraphics) {
		BarcodeRender barcodeRender(m_pGraphics);
		barcodeRender.drawCode128Auto(todop_to_wstring(text), x, y);
	}
}

void ECMAScriptProcessor::addQrCode(std::string text, double x, double y, std::string style) {
	if (NULL != m_pGraphics) {
		BarcodeRender barcodeRender(m_pGraphics);
		barcodeRender.drawQrcode(todop_to_wstring(text), x, y);
	}
}

void ECMAScriptProcessor::addHtml(std::string html, double x, double y, double width, double height) {
	if (NULL != m_pGraphics) {
		HtmlProcessor htmlProcessor(m_hdcPrinter);
		htmlProcessor.addHtml(todop_to_wstring(html), x, y, width, height);
	}
}

void ECMAScriptProcessor::addNewPage() {
	if (NULL != m_pGraphics) {
		delete m_pGraphics;
	}
	
	EndPage(m_hdcPrinter);
	StartPage(m_hdcPrinter);
	
	m_pGraphics = new Graphics(m_hdcPrinter);
}

void ECMAScriptProcessor::doRun() {
	std::string fileName("render.js");
	push_file_as_string(fileName);

	try {
		if (duk_peval(m_pDukContext) != 0) {
			// if an error occured while executing, print the stack trace
			duk_get_prop_string(m_pDukContext, -1, "stack");
			LOG(ERROR) << "Duktape Error: " << duk_safe_to_string(m_pDukContext, -1) << std::endl;
			duk_pop(m_pDukContext);
		}
	} catch(...) {
		LOG(ERROR) << "Failed to run ECMAScript"  << std::endl;
	}
}
