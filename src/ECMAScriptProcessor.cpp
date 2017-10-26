#include "ECMAScriptProcessor.h"

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
	duk_push_c_function(m_pDukContext, ECMAScriptProcessor::addToPrintText, DUK_VARARGS);
	duk_put_global_string(m_pDukContext, "todop_addText");

	return 0;
}

void ECMAScriptProcessor::addToPrintText(TString text, double x, double y, TString style) {
	PointF pointF(x, y);
	if (NULL!=m_pGraphics) {
		m_pGraphics->DrawString(text, -1, m_pDefaultFont, pointF, m_pDefaultBrush);
	}
}
