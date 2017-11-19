#include "HtmlContainer.h"

hellop::HtmlContainer::HtmlContainer(litehtml::context *pContext) {
	m_pLiteContext = pContext;
}

hellop::HtmlContainer::~HtmlContainer() {
	// do nothing
}

void hellop::HtmlContainer::initHtml(std::wstring html) {
	if(m_doc) {
		// delete m_doc;
	}

	std::string buff = todop_to_string(html);
	if(html.empty()){
		buff = "<h1>Something Wrong</h1>";
	}
	m_doc = litehtml::document::createFromUTF8(buff.c_str(), this, m_pLiteContext);
}

void hellop::HtmlContainer::draw(HDC hdc, int x, int y, int width, int height) {
	int best_width = m_doc->render(width);
	if(best_width < width) {
		m_doc->render(best_width);
	}

	LOG(DEBUG) << "best_width : " << best_width << std::endl;
	
	//litehtml::position clip(x, y, width, height);
	litehtml::position clip(0, 0, 400, 400);
	m_doc->draw((litehtml::uint_ptr)hdc, x, y, &clip);
}

litehtml::uint_ptr hellop::HtmlContainer::create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) {
	std::wstring fnt_name = L"微软雅黑";

	litehtml::string_vector fonts;
	litehtml::split_string(faceName, fonts, _t(","));

	if (!fonts.empty()) {
		litehtml::trim(fonts[0]);
		#ifdef LITEHTML_UTF8
			wchar_t* f = utf8_to_wchar(fonts[0].c_str());
			fnt_name = f;
			delete f;
		#else
			fnt_name = fonts[0];
			if (fnt_name.front() == '"') {
				fnt_name.erase(0, 1);
			}
			if (fnt_name.back() == '"') {
				fnt_name.erase(fnt_name.length() - 1, 1);
			}
		#endif
	}

	int fntStyle = FontStyle::FontStyleRegular;
	fntStyle = fntStyle | (weight>=700 ? FontStyle::FontStyleBold : FontStyle::FontStyleRegular);
	fntStyle = fntStyle | ((italic==litehtml::fontStyleItalic) ? FontStyleItalic : FontStyleRegular);
	fntStyle = fntStyle | ((decoration & litehtml::font_decoration_linethrough) ? FontStyleStrikeout : FontStyleRegular);
	fntStyle = fntStyle | ((decoration & litehtml::font_decoration_underline) ? FontStyleUnderline : FontStyleRegular);

	FontFamily fontFamily(fnt_name.c_str());
	Font* pFont = new Font(&fontFamily, (REAL)size, fntStyle, UnitPoint);

	return (litehtml::uint_ptr) pFont;
}

void hellop::HtmlContainer::delete_font( litehtml::uint_ptr hFont ) {
	Font* pFont = (Font *) hFont;
	delete pFont;
}

int hellop::HtmlContainer::text_width( const litehtml::tchar_t* text, litehtml::uint_ptr hFont )
{
	PointF pointF(0, 0);
	RectF boundingBox;

	HDC dc = GetDC(NULL);
	Graphics g(dc);
	g.MeasureString(utf8_to_wchar(text), -1, (Font *)hFont, pointF, StringFormat::GenericTypographic(), &boundingBox);
	
	int width = (int)(boundingBox.GetRight() - boundingBox.GetLeft());

    //LOG(DEBUG) << "text : " << text << " , text_width : " << width << std::endl;

	return width;
}

void hellop::HtmlContainer::draw_text( litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos )
{
	PointF pointF((REAL)pos.left(), (REAL)pos.top());
    // StringFormat sf;
    // double w = pos.right() - pos.left();
    // double h = pos.bottom();
    // RectF rectF(pos.left(), pos.top(), w, h);
	SolidBrush brush(Color(255, color.red, color.green, color.blue)); 
	Graphics g((HDC)hdc);
	g.DrawString(utf8_to_wchar(text), -1, (Font *)hFont, pointF, &brush);
	// g.DrawString(utf8_to_wchar(text), -1, (Font *)hFont, rectF, &sf, &brush);
    // LOG(DEBUG) << "text : " << text << " , pos : " << pos.left() << "," << pos.top() << "," << w << "," << h << std::endl;
}

int hellop::HtmlContainer::pt_to_px( int pt )
{
	HDC dc = GetDC(NULL);
	int ret = MulDiv(pt, GetDeviceCaps(dc, LOGPIXELSY), 72);
    LOG(DEBUG) << "pt : " << pt << " to px : " << ret << std::endl;
	ReleaseDC(NULL, dc);
	return ret;
}

int hellop::HtmlContainer::get_default_font_size() const
{
	return 12;
}

const litehtml::tchar_t* hellop::HtmlContainer::get_default_font_name() const
{
	return _t("Times New Roman");
}

void hellop::HtmlContainer::draw_list_marker( litehtml::uint_ptr hdc, const litehtml::list_marker& marker )
{
	if (!marker.image.empty()) {
		// TODO draw image
	} else {
		switch(marker.marker_type)
		{
		case litehtml::list_style_type_circle:
			{
				draw_ellipse((HDC) hdc, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color, 1);
			}
			break;
		case litehtml::list_style_type_disc:
			{
				fill_ellipse((HDC) hdc, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color);
			}
			break;
		case litehtml::list_style_type_square:
			{
				litehtml::css_border_radius radius;
				fill_rect((HDC) hdc, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color, radius);
			}
			break;
		}
	}
}

void hellop::HtmlContainer::load_image( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready ) {
	std::wstring url;
	make_url(utf8_to_wchar(src), utf8_to_wchar(baseurl), url);
	if(m_images.find(url.c_str()) == m_images.end()) {
	/*
		litehtml::uint_ptr img = get_image(url.c_str());
		if(img)
		{ 
			m_images[url.c_str()] = img;
		}
	*/
	}
}

void hellop::HtmlContainer::get_image_size( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz ) {
	std::wstring url;
	make_url(utf8_to_wchar(src), utf8_to_wchar(baseurl), url);
/*
	images_map::iterator img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		get_img_size(img->second, sz);
	}
*/
}

void hellop::HtmlContainer::draw_background( litehtml::uint_ptr hdc, const litehtml::background_paint& bg ) {
	// TODO
}

void hellop::HtmlContainer::draw_borders( litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root ) {
	// TODO
}

void hellop::HtmlContainer::set_caption( const litehtml::tchar_t* caption ) {
	// do nothing
}

void hellop::HtmlContainer::set_base_url( const litehtml::tchar_t* base_url ) {
	// do nothing
}

void hellop::HtmlContainer::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) {
	// do nothing
}

void hellop::HtmlContainer::on_anchor_click( const litehtml::tchar_t* url, const litehtml::element::ptr& el ) {
	// do nothing
}

void hellop::HtmlContainer::set_cursor( const litehtml::tchar_t* cursor ) {
	// do nothing
}

void hellop::HtmlContainer::transform_text( litehtml::tstring& text, litehtml::text_transform tt ) {
	// do nothing
}

void hellop::HtmlContainer::import_css( litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl ) {
	// do nothing
}

void hellop::HtmlContainer::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y)
{
	// do nothing
}

void hellop::HtmlContainer::del_clip()
{
	// do nothing
}

void hellop::HtmlContainer::get_client_rect( litehtml::position& client ) const
{
	client.x		= 0;
	client.y		= 0;
	client.width	= 400;
	client.height	= 600;
}

std::shared_ptr<litehtml::element> hellop::HtmlContainer::create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc)
{
	return 0;
}

void hellop::HtmlContainer::get_media_features(litehtml::media_features& media)  const
{
	litehtml::position client;
	get_client_rect(client);
	HDC hdc = GetDC(NULL);

	media.type			= litehtml::media_type_screen;
	media.width			= client.width;
	media.height		= client.height;
	media.color			= 8;
	media.monochrome	= 0;
	media.color_index	= 256;
	media.resolution	= GetDeviceCaps(hdc, LOGPIXELSX);
	media.device_width	= GetDeviceCaps(hdc, HORZRES);
	media.device_height	= GetDeviceCaps(hdc, VERTRES);

	ReleaseDC(NULL, hdc);

	LOG(DEBUG) << "media.resolution : " << media.resolution << " , media.device_width : " << media.device_width << " , media.device_height : " << media.device_height << std::endl;
}

void hellop::HtmlContainer::get_language(litehtml::tstring& language, litehtml::tstring & culture) const
{
	language = _t("en");
	culture = _t("");
}

litehtml::tstring hellop::HtmlContainer::resolve_color(const litehtml::tstring& color) const
{
	return litehtml::tstring();
}

void hellop::HtmlContainer::make_url( LPCWSTR url, LPCWSTR basepath, std::wstring& out )
{
	// TODO
}

void hellop::HtmlContainer::draw_ellipse( HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color, int line_width ) {
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::LinearGradientBrush* brush = NULL;

	graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	Gdiplus::Pen pen( Gdiplus::Color(color.alpha, color.red, color.green, color.blue) );
	graphics.DrawEllipse(&pen, x, y, width, height);
}

void hellop::HtmlContainer::fill_ellipse( HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color ) {
	Gdiplus::Graphics graphics(hdc);

	graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	Gdiplus::SolidBrush brush( Gdiplus::Color(color.alpha, color.red, color.green, color.blue) );
	graphics.FillEllipse(&brush, x, y, width, height);
}

void hellop::HtmlContainer::fill_rect( HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color, const litehtml::css_border_radius& radius ) {
	Gdiplus::Graphics graphics(hdc);

	Gdiplus::SolidBrush brush( Gdiplus::Color(color.alpha, color.red, color.green, color.blue) );
	graphics.FillRectangle(&brush, x, y, width, height);
}
