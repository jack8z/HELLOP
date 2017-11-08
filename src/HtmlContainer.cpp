#include "HtmlContainer.h"

HtmlContainer::HtmlContainer(HDC hdcPrinter) {
	m_hdcPrinter = hdcPrinter;
}

HtmlContainer::~HtmlContainer() {
	// do nothing
}

litehtml::uint_ptr HtmlContainer::create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) {
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
	
	LOGFONTW lf;
	ZeroMemory(&lf, sizeof(lf));
	wcscpy_s(lf.lfFaceName, LF_FACESIZE, fnt_name.c_str());

	lf.lfHeight			= -size;
	lf.lfWeight			= weight;
	lf.lfItalic			= (italic == litehtml::fontStyleItalic) ? TRUE : FALSE;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfQuality		= DEFAULT_QUALITY;
	lf.lfStrikeOut		= (decoration & litehtml::font_decoration_linethrough) ? TRUE : FALSE;
	lf.lfUnderline		= (decoration & litehtml::font_decoration_underline) ? TRUE : FALSE;
	HFONT hFont = CreateFontIndirectW(&lf);

	if (fm) {/*
		fm->ascent		= fnt->metrics().ascent;
		fm->descent		= fnt->metrics().descent;
		fm->height		= fnt->metrics().height;
		fm->x_height	= fnt->metrics().x_height;
		if(italic == litehtml::fontStyleItalic || decoration)
		{
			fm->draw_spaces = true;
		} else
		{
			fm->draw_spaces = false;
		}*/
	}

	return (litehtml::uint_ptr) hFont;
}

void HtmlContainer::delete_font( litehtml::uint_ptr hFont )
{
	DeleteObject((HFONT) hFont);
}

int HtmlContainer::text_width( litehtml::uint_ptr hdc, const wchar_t* text, litehtml::uint_ptr hFont )
{
	HFONT oldFont = (HFONT) SelectObject((HDC) hdc, (HFONT) hFont);

	SIZE sz = {0, 0};

	GetTextExtentPoint32W((HDC) hdc, text, lstrlenW(text), &sz);

	SelectObject((HDC) hdc, oldFont);

	return (int) sz.cx;
}

void HtmlContainer::draw_text( litehtml::uint_ptr hdc, const wchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos )
{
	apply_clip((HDC) hdc);

	HFONT oldFont = (HFONT) SelectObject((HDC) hdc, (HFONT) hFont);

	SetBkMode((HDC) hdc, TRANSPARENT);

	SetTextColor((HDC) hdc, RGB(color.red, color.green, color.blue));

	RECT rcText = { pos.left(), pos.top(), pos.right(), pos.bottom() };
	DrawTextW((HDC) hdc, text, -1, &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_BOTTOM | DT_NOCLIP);

	SelectObject((HDC) hdc, oldFont);

	release_clip((HDC) hdc);
}

int HtmlContainer::pt_to_px( int pt )
{
	HDC dc = GetDC(NULL);
	int ret = MulDiv(pt, GetDeviceCaps(dc, LOGPIXELSY), 72);
	ReleaseDC(NULL, dc);
	return ret;
}

int HtmlContainer::get_default_font_size() const
{
	return 16;
}

const litehtml::tchar_t* HtmlContainer::get_default_font_name() const
{
	return _t("Times New Roman");
}

void HtmlContainer::draw_list_marker( litehtml::uint_ptr hdc, const litehtml::list_marker& marker )
{
	apply_clip((HDC) hdc);

	if (!marker.image.empty()) {
		// TODO draw image
	} else
	{
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
				fill_rect((HDC) hdc, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color, css_border_radius());
			}
			break;
		}
	}

	release_clip((HDC) hdc);
}

void litehtml::win32_container::load_image( const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready )
{
	std::wstring url;
	make_url(src, baseurl, url);
	if(m_images.find(url.c_str()) == m_images.end())
	{
		litehtml::uint_ptr img = get_image(url.c_str());
		if(img)
		{ 
			m_images[url.c_str()] = img;
		}
	}
}

void litehtml::win32_container::get_image_size( const wchar_t* src, const wchar_t* baseurl, litehtml::size& sz )
{
	std::wstring url;
	make_url(src, baseurl, url);

	images_map::iterator img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		get_img_size(img->second, sz);
	}
}

void cairo_container::draw_background( litehtml::uint_ptr hdc, const litehtml::background_paint& bg )
{
	apply_clip((HDC) hdc);
	
	std::wstring url;
	make_url(bg.image.c_str(), bg.baseurl.c_str(), url);

	images_map::iterator img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		litehtml::size img_sz;
		get_img_size(img->second, img_sz);

		litehtml::position pos = draw_pos;

		if(bg_pos.x.units() != css_units_percentage)
		{
			pos.x += (int) bg_pos.x.val();
		} else
		{
			pos.x += (int) ((float) (draw_pos.width - img_sz.width) * bg_pos.x.val() / 100.0);
		}

		if(bg_pos.y.units() != css_units_percentage)
		{
			pos.y += (int) bg_pos.y.val();
		} else
		{
			pos.y += (int) ( (float) (draw_pos.height - img_sz.height) * bg_pos.y.val() / 100.0);
		}

		draw_img_bg((HDC) hdc, img->second, draw_pos, pos, repeat, attachment);
	}

	release_clip((HDC) hdc);
}

void cairo_container::draw_borders( litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root )
{
	cairo_t* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);

	cairo_new_path(cr);

	int bdr_top		= 0;
	int bdr_bottom	= 0;
	int bdr_left	= 0;
	int bdr_right	= 0;

	if(borders.top.width != 0 && borders.top.style > litehtml::border_style_hidden)
	{
		bdr_top = (int) borders.top.width;
	}
	if(borders.bottom.width != 0 && borders.bottom.style > litehtml::border_style_hidden)
	{
		bdr_bottom = (int) borders.bottom.width;
	}
	if(borders.left.width != 0 && borders.left.style > litehtml::border_style_hidden)
	{
		bdr_left = (int) borders.left.width;
	}
	if(borders.right.width != 0 && borders.right.style > litehtml::border_style_hidden)
	{
		bdr_right = (int) borders.right.width;
	}

	// draw right border
	if (bdr_right)
	{
		set_color(cr, borders.right.color);

		double r_top	= (double) borders.radius.top_right_x;
		double r_bottom	= (double) borders.radius.bottom_right_x;

		if(r_top)
		{
			double end_angle	= 2.0 * M_PI;
			double start_angle	= end_angle - M_PI / 2.0  / ((double) bdr_top / (double) bdr_right + 0.5);

			if (!add_path_arc(cr,
					draw_pos.right() - r_top,
					draw_pos.top() + r_top,
					r_top - bdr_right,
					r_top - bdr_right + (bdr_right - bdr_top),
					end_angle,
					start_angle, true))
			{
				cairo_move_to(cr, draw_pos.right() - bdr_right, draw_pos.top() + bdr_top);
			}

			if (!add_path_arc(cr,
					draw_pos.right() - r_top,
					draw_pos.top() + r_top,
					r_top,
					r_top,
					start_angle,
					end_angle, false))
			{
				cairo_line_to(cr, draw_pos.right(), draw_pos.top());
			}
		} else
		{
			cairo_move_to(cr, draw_pos.right() - bdr_right, draw_pos.top() + bdr_top);
			cairo_line_to(cr, draw_pos.right(), draw_pos.top());
		}

		if(r_bottom)
		{
			cairo_line_to(cr, draw_pos.right(),	draw_pos.bottom() - r_bottom);

			double start_angle	= 0;
			double end_angle	= start_angle + M_PI / 2.0  / ((double) bdr_bottom / (double) bdr_right + 0.5);

			if (!add_path_arc(cr,
				draw_pos.right() - r_bottom,
				draw_pos.bottom() - r_bottom,
				r_bottom,
				r_bottom,
				start_angle,
				end_angle, false))
			{
				cairo_line_to(cr, draw_pos.right(), draw_pos.bottom());
			}

			if (!add_path_arc(cr,
				draw_pos.right() - r_bottom,
				draw_pos.bottom() - r_bottom,
				r_bottom - bdr_right,
				r_bottom - bdr_right + (bdr_right - bdr_bottom),
				end_angle,
				start_angle, true))
			{
				cairo_line_to(cr, draw_pos.right() - bdr_right, draw_pos.bottom() - bdr_bottom);
			}
		} else
		{
			cairo_line_to(cr, draw_pos.right(),	draw_pos.bottom());
			cairo_line_to(cr, draw_pos.right() - bdr_right,	draw_pos.bottom() - bdr_bottom);
		}

		cairo_fill(cr);
	}

	// draw bottom border
	if(bdr_bottom)
	{
		set_color(cr, borders.bottom.color);

		double r_left	= borders.radius.bottom_left_x;
		double r_right	= borders.radius.bottom_right_x;

		if(r_left)
		{
			double start_angle	= M_PI / 2.0;
			double end_angle	= start_angle + M_PI / 2.0  / ((double) bdr_left / (double) bdr_bottom + 0.5);

			if (!add_path_arc(cr,
				draw_pos.left() + r_left,
				draw_pos.bottom() - r_left,
				r_left - bdr_bottom + (bdr_bottom - bdr_left),
				r_left - bdr_bottom,
				start_angle,
				end_angle, false))
			{
				cairo_move_to(cr, draw_pos.left() + bdr_left, draw_pos.bottom() - bdr_bottom);
			}

			if (!add_path_arc(cr,
				draw_pos.left() + r_left,
				draw_pos.bottom() - r_left,
				r_left,
				r_left,
				end_angle,
				start_angle, true))
			{
				cairo_line_to(cr, draw_pos.left(), draw_pos.bottom());
			}
		} else
		{
			cairo_move_to(cr, draw_pos.left(), draw_pos.bottom());
			cairo_line_to(cr, draw_pos.left() + bdr_left, draw_pos.bottom() - bdr_bottom);
		}

		if(r_right)
		{
			cairo_line_to(cr, draw_pos.right() - r_right,	draw_pos.bottom());

			double end_angle	= M_PI / 2.0;
			double start_angle	= end_angle - M_PI / 2.0  / ((double) bdr_right / (double) bdr_bottom + 0.5);

			if (!add_path_arc(cr,
				draw_pos.right() - r_right,
				draw_pos.bottom() - r_right,
				r_right,
				r_right,
				end_angle,
				start_angle, true))
			{
				cairo_line_to(cr, draw_pos.right(), draw_pos.bottom());
			}

			if (!add_path_arc(cr,
				draw_pos.right() - r_right,
				draw_pos.bottom() - r_right,
				r_right - bdr_bottom + (bdr_bottom - bdr_right),
				r_right - bdr_bottom,
				start_angle,
				end_angle, false))
			{
				cairo_line_to(cr, draw_pos.right() - bdr_right, draw_pos.bottom() - bdr_bottom);
			}
		} else
		{
			cairo_line_to(cr, draw_pos.right() - bdr_right,	draw_pos.bottom() - bdr_bottom);
			cairo_line_to(cr, draw_pos.right(),	draw_pos.bottom());
		}

		cairo_fill(cr);
	}

	// draw top border
	if(bdr_top)
	{
		set_color(cr, borders.top.color);

		double r_left	= borders.radius.top_left_x;
		double r_right	= borders.radius.top_right_x;

		if(r_left)
		{
			double end_angle	= M_PI * 3.0 / 2.0;
			double start_angle	= end_angle - M_PI / 2.0  / ((double) bdr_left / (double) bdr_top + 0.5);

			if (!add_path_arc(cr,
				draw_pos.left() + r_left,
				draw_pos.top() + r_left,
				r_left,
				r_left,
				end_angle,
				start_angle, true))
			{
				cairo_move_to(cr, draw_pos.left(), draw_pos.top());
			}

			if (!add_path_arc(cr,
				draw_pos.left() + r_left,
				draw_pos.top() + r_left,
				r_left - bdr_top + (bdr_top - bdr_left),
				r_left - bdr_top,
				start_angle,
				end_angle, false))
			{
				cairo_line_to(cr, draw_pos.left() + bdr_left, draw_pos.top() + bdr_top);
			}
		} else
		{
			cairo_move_to(cr, draw_pos.left(), draw_pos.top());
			cairo_line_to(cr, draw_pos.left() + bdr_left, draw_pos.top() + bdr_top);
		}

		if(r_right)
		{
			cairo_line_to(cr, draw_pos.right() - r_right,	draw_pos.top() + bdr_top);

			double start_angle	= M_PI * 3.0 / 2.0;
			double end_angle	= start_angle + M_PI / 2.0  / ((double) bdr_right / (double) bdr_top + 0.5);

			if (!add_path_arc(cr,
				draw_pos.right() - r_right,
				draw_pos.top() + r_right,
				r_right - bdr_top + (bdr_top - bdr_right),
				r_right - bdr_top,
				start_angle,
				end_angle, false))
			{
				cairo_line_to(cr, draw_pos.right() - bdr_right, draw_pos.top() + bdr_top);
			}

			if (!add_path_arc(cr,
				draw_pos.right() - r_right,
				draw_pos.top() + r_right,
				r_right,
				r_right,
				end_angle,
				start_angle, true))
			{
				cairo_line_to(cr, draw_pos.right(), draw_pos.top());
			}
		} else
		{
			cairo_line_to(cr, draw_pos.right() - bdr_right,	draw_pos.top() + bdr_top);
			cairo_line_to(cr, draw_pos.right(),	draw_pos.top());
		}

		cairo_fill(cr);
	}

	// draw left border
	if (bdr_left)
	{
		set_color(cr, borders.left.color);

		double r_top	= borders.radius.top_left_x;
		double r_bottom	= borders.radius.bottom_left_x;

		if(r_top)
		{
			double start_angle	= M_PI;
			double end_angle	= start_angle + M_PI / 2.0  / ((double) bdr_top / (double) bdr_left + 0.5);

			if (!add_path_arc(cr,
				draw_pos.left() + r_top,
				draw_pos.top() + r_top,
				r_top - bdr_left,
				r_top - bdr_left + (bdr_left - bdr_top),
				start_angle,
				end_angle, false))
			{
				cairo_move_to(cr, draw_pos.left() + bdr_left, draw_pos.top() + bdr_top);
			}

			if (!add_path_arc(cr,
				draw_pos.left() + r_top,
				draw_pos.top() + r_top,
				r_top,
				r_top,
				end_angle,
				start_angle, true))
			{
				cairo_line_to(cr, draw_pos.left(), draw_pos.top());
			}
		} else
		{
			cairo_move_to(cr, draw_pos.left() + bdr_left, draw_pos.top() + bdr_top);
			cairo_line_to(cr, draw_pos.left(), draw_pos.top());
		}

		if(r_bottom)
		{
			cairo_line_to(cr, draw_pos.left(),	draw_pos.bottom() - r_bottom);

			double end_angle	= M_PI;
			double start_angle	= end_angle - M_PI / 2.0  / ((double) bdr_bottom / (double) bdr_left + 0.5);

			if (!add_path_arc(cr,
				draw_pos.left() + r_bottom,
				draw_pos.bottom() - r_bottom,
				r_bottom,
				r_bottom,
				end_angle,
				start_angle, true))
			{
				cairo_line_to(cr, draw_pos.left(), draw_pos.bottom());
			}

			if (!add_path_arc(cr,
				draw_pos.left() + r_bottom,
				draw_pos.bottom() - r_bottom,
				r_bottom - bdr_left,
				r_bottom - bdr_left + (bdr_left - bdr_bottom),
				start_angle,
				end_angle, false))
			{
				cairo_line_to(cr, draw_pos.left() + bdr_left, draw_pos.bottom() - bdr_bottom);
			}
		} else
		{
			cairo_line_to(cr, draw_pos.left(),	draw_pos.bottom());
			cairo_line_to(cr, draw_pos.left() + bdr_left,	draw_pos.bottom() - bdr_bottom);
		}

		cairo_fill(cr);
	}
	cairo_restore(cr);
}

void web_page::set_caption( const litehtml::tchar_t* caption ) {
	// do nothing
}

void web_page::set_base_url( const litehtml::tchar_t* base_url )
{
#ifndef LITEHTML_UTF8
	if(base_url)
	{
		if(PathIsRelative(base_url) && !PathIsURL(base_url))
		{
			make_url(base_url, m_url.c_str(), m_base_path);
		} else
		{
			m_base_path = base_url;
		}
	} else
	{
		m_base_path = m_url;
	}
#else
	LPWSTR bu = cairo_font::utf8_to_wchar(base_url);

	if(bu)
	{
		if(PathIsRelative(bu) && !PathIsURL(bu))
		{
			make_url(bu, m_url.c_str(), m_base_path);
		} else
		{
			m_base_path = bu;
		}
	} else
	{
		m_base_path = m_url;
	}
#endif
}

void cairo_container::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) {
	// do nothing
}

void web_page::on_anchor_click( const litehtml::tchar_t* url, const litehtml::element::ptr& el ) {
	// do nothing
}

void web_page::set_cursor( const litehtml::tchar_t* cursor ) {
	// do nothing
}

void cairo_container::transform_text( litehtml::tstring& text, litehtml::text_transform tt )
{
	if(text.empty()) return;

#ifndef LITEHTML_UTF8
	switch(tt)
	{
	case litehtml::text_transform_capitalize:
		if(!text.empty())
		{
			text[0] = (WCHAR) CharUpper((LPWSTR) text[0]);
		}
		break;
	case litehtml::text_transform_uppercase:
		for(size_t i = 0; i < text.length(); i++)
		{
			text[i] = (WCHAR) CharUpper((LPWSTR) text[i]);
		}
		break;
	case litehtml::text_transform_lowercase:
		for(size_t i = 0; i < text.length(); i++)
		{
			text[i] = (WCHAR) CharLower((LPWSTR) text[i]);
		}
		break;
	}
#else
	LPWSTR txt = cairo_font::utf8_to_wchar(text.c_str());
	switch(tt)
	{
	case litehtml::text_transform_capitalize:
		CharUpperBuff(txt, 1);
		break;
	case litehtml::text_transform_uppercase:
		CharUpperBuff(txt, lstrlen(txt));
		break;
	case litehtml::text_transform_lowercase:
		CharLowerBuff(txt, lstrlen(txt));
		break;
	}
	LPSTR txtA = cairo_font::wchar_to_utf8(txt);
	text = txtA;
	delete txtA;
	delete txt;
#endif
}

void web_page::import_css( litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl )
{
	std::wstring css_url;
	t_make_url(url.c_str(), baseurl.c_str(), css_url);

	if(download_and_wait(css_url.c_str()))
	{
#ifndef LITEHTML_UTF8
		LPWSTR css = load_text_file(m_waited_file.c_str(), false, L"UTF-8");
		if(css)
		{
			baseurl = css_url;
			text = css;
			delete css;
		}
#else
		LPSTR css = (LPSTR) load_utf8_file(m_waited_file.c_str(), false, L"UTF-8");
		if(css)
		{
			LPSTR css_urlA = cairo_font::wchar_to_utf8(css_url.c_str());
			baseurl = css_urlA;
			text = css;
			delete css;
			delete css_urlA;
		}
#endif
	}
}

void cairo_container::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y)
{
	litehtml::position clip_pos = pos;
	litehtml::position client_pos;
	get_client_rect(client_pos);
	if(!valid_x)
	{
		clip_pos.x		= client_pos.x;
		clip_pos.width	= client_pos.width;
	}
	if(!valid_y)
	{
		clip_pos.y		= client_pos.y;
		clip_pos.height	= client_pos.height;
	}
	m_clips.emplace_back(clip_pos, bdr_radius);
}

void cairo_container::del_clip()
{
	if(!m_clips.empty())
	{
		m_clips.pop_back();
	}
}

void CHTMLViewWnd::get_client_rect( litehtml::position& client ) const
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	client.x		= rcClient.left;
	client.y		= rcClient.top;
	client.width	= rcClient.right - rcClient.left;
	client.height	= rcClient.bottom - rcClient.top;
}

std::shared_ptr<litehtml::element> cairo_container::create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc)
{
	return 0;
}

void cairo_container::get_media_features(litehtml::media_features& media)  const
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
}

void cairo_container::get_language(litehtml::tstring& language, litehtml::tstring & culture) const
{
	language = _t("en");
	culture = _t("");
}

litehtml::tstring cairo_container::resolve_color(const litehtml::tstring& color) const
{
	struct custom_color 
	{
		litehtml::tchar_t*	name;
		int					color_index;
	};

	static custom_color colors[] = {
		{ _t("ActiveBorder"),          COLOR_ACTIVEBORDER},
		{ _t("ActiveCaption"),         COLOR_ACTIVECAPTION},
		{ _t("AppWorkspace"),          COLOR_APPWORKSPACE },
		{ _t("Background"),            COLOR_BACKGROUND },
		{ _t("ButtonFace"),            COLOR_BTNFACE },
		{ _t("ButtonHighlight"),       COLOR_BTNHIGHLIGHT },
		{ _t("ButtonShadow"),          COLOR_BTNSHADOW },
		{ _t("ButtonText"),            COLOR_BTNTEXT },
		{ _t("CaptionText"),           COLOR_CAPTIONTEXT },
        { _t("GrayText"),              COLOR_GRAYTEXT },
		{ _t("Highlight"),             COLOR_HIGHLIGHT },
		{ _t("HighlightText"),         COLOR_HIGHLIGHTTEXT },
		{ _t("InactiveBorder"),        COLOR_INACTIVEBORDER },
		{ _t("InactiveCaption"),       COLOR_INACTIVECAPTION },
		{ _t("InactiveCaptionText"),   COLOR_INACTIVECAPTIONTEXT },
		{ _t("InfoBackground"),        COLOR_INFOBK },
		{ _t("InfoText"),              COLOR_INFOTEXT },
		{ _t("Menu"),                  COLOR_MENU },
		{ _t("MenuText"),              COLOR_MENUTEXT },
		{ _t("Scrollbar"),             COLOR_SCROLLBAR },
		{ _t("ThreeDDarkShadow"),      COLOR_3DDKSHADOW },
		{ _t("ThreeDFace"),            COLOR_3DFACE },
		{ _t("ThreeDHighlight"),       COLOR_3DHILIGHT },
		{ _t("ThreeDLightShadow"),     COLOR_3DLIGHT },
		{ _t("ThreeDShadow"),          COLOR_3DSHADOW },
		{ _t("Window"),                COLOR_WINDOW },
		{ _t("WindowFrame"),           COLOR_WINDOWFRAME },
		{ _t("WindowText"),            COLOR_WINDOWTEXT }
	};

    if (color == L"Highlight")
    {
        int iii = 0;
        iii++;
    }

    for (auto& clr : colors)
    {
        if (!t_strcasecmp(clr.name, color.c_str()))
        {
            litehtml::tchar_t  str_clr[20];
            DWORD rgb_color =  GetSysColor(clr.color_index);
#ifdef LITEHTML_UTF8
            StringCchPrintfA(str_clr, 20, "#%02X%02X%02X", GetRValue(rgb_color), GetGValue(rgb_color), GetBValue(rgb_color));
#else
            StringCchPrintf(str_clr, 20, L"#%02X%02X%02X", GetRValue(rgb_color), GetGValue(rgb_color), GetBValue(rgb_color));
#endif // LITEHTML_UTF8
            return std::move(litehtml::tstring(str_clr));
        }
    }
    return std::move(litehtml::tstring());
}
