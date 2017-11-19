#include "HtmlRender.h"
#include<boost/filesystem.hpp>

hellop::HtmlRender::HtmlRender(HDC hdcPrinter) {
	m_hdcPrinter = hdcPrinter;

	m_pLiteContext = new litehtml::context();
	{
		boost::filesystem::path curr_path = boost::filesystem::current_path(); //取得当前程序所在目录
		boost::filesystem::path css_file_path = curr_path / "res" / "master.css"; //path支持重载/运算符
		if (boost::filesystem::exists(css_file_path)) {
			LOG(DEBUG) << "File [ master.css ] " << "Path : " << css_file_path << std::endl;

			std::string css_content;
			int ret = read_file(css_file_path.string(), css_content);
			if(ret==0 && css_content.size()>0) {
				m_pLiteContext->load_master_stylesheet(css_content.c_str());
			}
		} else {
			LOG(DEBUG) << "Can not found file : " << css_file_path.string() << std::endl;
		}
	}

	m_pHtmlContainer = new HtmlContainer(m_pLiteContext);

	m_pGraphics = new Graphics(m_hdcPrinter);
}

hellop::HtmlRender::~HtmlRender() {
	if (m_pLiteContext) {
		delete m_pLiteContext;
	}
	if (m_pHtmlContainer) {
		delete m_pHtmlContainer;
	}
	if (NULL!=m_pGraphics) {
		delete m_pGraphics;
	}
}

void hellop::HtmlRender::drawHtml(std::wstring html, double x, double y, double width, double height) {
	m_pHtmlContainer->initHtml(html);
	m_pHtmlContainer->draw(m_hdcPrinter, x, y, width, height);
}
