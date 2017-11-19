#include "BarcodeRender.h"

#include "easylogging++.h"

hellop::BarcodeRender::BarcodeRender(Graphics* graphics) {
    m_pGraphics = graphics;
    
    m_pZintSymbol = ZBarcode_Create();
    if (NULL==m_pZintSymbol) {
        LOG(ERROR) << "m_pZintSymbol cannot created!\n" << std::endl;
    }
}

hellop::BarcodeRender::~BarcodeRender() {
    if (NULL!=m_pZintSymbol) {
        ZBarcode_Delete(m_pZintSymbol);
    }
}

// 绘制Barcode，第一个参数symbology的值为：zint_symbol.symbology,具体请参考zint。rotateAngle旋转的角度，支持:0,90,180,270.autoZoom是否自动缩放,为true且width/height大于1时,将自动缩放条码以按width/height完整显示
int hellop::BarcodeRender::drawBarcode(int symbology, TString content, double x, double y, double width, double height, int rotateAngle, bool autoZoom) {
    // 将毫米转为像素
    int pixelX = mm_to_px(x, dpi_to_ppi(m_pGraphics->GetDpiX()));
    int pixelWidth = mm_to_px(width, dpi_to_ppi(m_pGraphics->GetDpiX()));
    int pixelY = mm_to_px(y, dpi_to_ppi(m_pGraphics->GetDpiY()));
    int pixelHeight = mm_to_px(height, dpi_to_ppi(m_pGraphics->GetDpiY()));

    if (NULL!=m_pZintSymbol) {
        ZBarcode_Clear(m_pZintSymbol);

        // 设置条码类型
        m_pZintSymbol->symbology = symbology;
        m_pZintSymbol->height = 30;

        int retEncode = ZBarcode_Encode_and_Buffer(m_pZintSymbol, (unsigned char *)todop_to_string(content).c_str(), 0, rotateAngle);
        int retBuffer = ZBarcode_Buffer(m_pZintSymbol, 0);
        
        Bitmap b(m_pZintSymbol->bitmap_width, m_pZintSymbol->bitmap_height, PixelFormat24bppRGB);
        int row, column, i = 0;
        for (row = 0; row < m_pZintSymbol->bitmap_height; row++) {
             for (column = 0; column < m_pZintSymbol->bitmap_width; column++) {
                  unsigned int red = m_pZintSymbol->bitmap[i];
                  unsigned int green = m_pZintSymbol->bitmap[i + 1];
                  unsigned int blue = m_pZintSymbol->bitmap[i + 2];
                  Color c(red, green, blue);
                  b.SetPixel(column, row, c);
                  i += 3;
             }
        }
        m_pGraphics->DrawImage(&b, pixelX, pixelY);

        return 0;
    }
    return -1;
}

int hellop::BarcodeRender::drawCode128Auto(TString content, double x, double y, double width, double height, int rotateAngle, bool autoZoom) {
    return drawBarcode(BARCODE_CODE128, content, x, y, width, height, rotateAngle, autoZoom);
}

int hellop::BarcodeRender::drawCode128B(TString content, double x, double y, double width, double height, int rotateAngle, bool autoZoom) {
    return drawBarcode(BARCODE_CODE128B, content, x, y, width, height, rotateAngle, autoZoom);
}

int hellop::BarcodeRender::drawQrcode(TString content, double x, double y, double width, double height, bool autoZoom) {
    return drawBarcode(BARCODE_QRCODE, content, x, y, width, height, 0, autoZoom);
}
