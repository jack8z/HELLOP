#include "ECMAScriptProcessor.h"

#include "dukglue/dukglue.h"

ECMAScriptProcessor::ECMAScriptProcessor(Graphics* graphics) {
	m_pGraphics = graphics;

	m_pDukContext = duk_create_heap_default();
	if (!m_pDukContext) {
		LOG(ERROR) << "Failed to create a Duktape heap!\n" << std::endl;
	}
	
	m_pDefaultPen = new Pen(Color(255, 0, 0, 0));
	m_pDefaultBrush = new SolidBrush(Color(255, 0, 0, 0)); 

	// 创建字体
	FontFamily fontFamily(L"微软雅黑");
	m_pDefaultFont = new Font(&fontFamily, 12, FontStyleRegular, UnitPixel);

	initDuktape();
}

ECMAScriptProcessor::~ECMAScriptProcessor() {
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
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addText, "addText");
		dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::log_debug, "log_debug");
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
	PointF pointF(x, y);
	if (NULL!=m_pGraphics) {
		m_pGraphics->DrawString(todop_to_wstring(text).c_str(), -1, m_pDefaultFont, pointF, m_pDefaultBrush);
	}
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
