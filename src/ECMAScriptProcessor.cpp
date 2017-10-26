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
	dukglue_register_constructor<ECMAScriptProcessor, /* constructor args: */ const char*>(m_pDukContext, "Todop");
	dukglue_register_method(m_pDukContext, &ECMAScriptProcessor::addToPrintText, "addText");

	if (duk_peval_string(m_pDukContext,
		"var gus = new Dog('Gus');"
		"gus.bark();"
		"print(gus.getName());")) {
		// if an error occured while executing, print the stack trace
		duk_get_prop_string(m_pDukContext, -1, "stack");
		LOG(ERROR) << duk_safe_to_string(m_pDukContext, -1) << std::endl;
		duk_pop(m_pDukContext);
		return -1;
	}

	return 0;
}

void ECMAScriptProcessor::addToPrintText(TString text, double x, double y, TString style) {
	PointF pointF(x, y);
	if (NULL!=m_pGraphics) {
		m_pGraphics->DrawString(text, -1, m_pDefaultFont, pointF, m_pDefaultBrush);
	}
}
