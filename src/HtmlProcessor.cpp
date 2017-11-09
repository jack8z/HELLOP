#include "HtmlProcessor.h"
#include<boost/filesystem.hpp>

HtmlProcessor::HtmlProcessor(HDC hdcPrinter) {
	m_hdcPrinter = hdcPrinter;

	m_pHtmlContainer = new HtmlContainer();

	m_pLiteContext = new litehtml::context();
	
	{
		boost::filesystem::path curr_path = boost::filesystem::current_path(); //取得当前程序所在目录
		boost::filesystem::path css_file_path = curr_path / "res/master.css"; //path支持重载/运算符
		if (boost::filesystem::exists(css_file_path)) {
			std::wstring css;
			read_file(css_file_path.string(), css);
			if(css)
			{
				m_pLiteContext.load_master_stylesheet(css);
				delete css;
			}
		} else {
			LOG(DEBUG) << "Can not found file : " << css_file_path.string() << std::endl;
		}
	}
}

HtmlProcessor::~HtmlProcessor() {
	if (m_pLiteContext) {
		delete m_pLiteContext;
	}
	if (m_pHtmlContainer) {
		delete m_pHtmlContainer;
	}
}
