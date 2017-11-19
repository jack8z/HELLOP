#pragma once

#include "afxTodop.h"
#include "dukglue/dukglue.h"
#include "Paper.h"

#include "easylogging++.h"

namespace hellop {
	class ECMAScriptProcessor {
	public:
		ECMAScriptProcessor(HDC hdcPrinter);
		~ECMAScriptProcessor();

		// 运行ECMAScript脚本，在GdiplusPrintEngine类里调用
		int doRun();

		// 添加要打印的文本. style:文本的样式，JSON格式
		void addText(std::string text, double x, double y, std::string style);
		// 添加要打印的线条. style:文本的样式，JSON格式
		void addLine(double x1, double y1, double x2, double y2, std::string style);
		// 添加要打印的长方形. style:文本的样式，JSON格式
		void addRectangle(double x1, double y1, double x2, double y2, std::string style);
		// 添加要打印的圆形. style:文本的样式，JSON格式
		void addEllipse(double x1, double y1, double x2, double y2, std::string style);
		// 添加要打印的条码. style:文本的样式，JSON格式
		void addBarCode(std::string text, double x, double y, std::string style);
		// 添加要打印的QR码. style:文本的样式，JSON格式
		void addQrCode(std::string text, double x, double y, std::string style);
		// 添加要打印的Html代码.
		void addHtml(std::string html, double x, double y, double width, double height);
		
		// 添加新的页面
		void addNewPage();

		void log_debug(std::string text);

	private:
		// 初始化Duktape，初始化后才能正常使用
		int initDuktape();
		void push_file_as_string(std::string fileName);
		
	private:
		duk_context *m_pDukContext; // Duktape上下文

		HDC m_hdcPrinter; // 打印机上下文
		Graphics *m_pGraphics;
		
		Pen *m_pDefaultPen; // 默认画笔
		SolidBrush *m_pDefaultBrush; // 默认画刷
		Font *m_pDefaultFont; // 默认字体
	};
}
