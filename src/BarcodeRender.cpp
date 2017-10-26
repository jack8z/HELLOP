#include "BarcodeRender.h"

#include "easylogging++.h"

BarcodeRender::BarcodeRender(Graphics* graphics) {
    m_pGraphics = graphics;
    
    m_pZintSymbol = ZBarcode_Create();
    if (NULL==m_pZintSymbol) {
        LOG(ERROR) << "m_pZintSymbol cannot created!\n" << std::endl;
    }
}

BarcodeRender::~BarcodeRender() {
    if (NULL!=m_pZintSymbol) {
        ZBarcode_Delete(m_pZintSymbol);
    }
}

// 绘制Barcode，第一个参数symbology的值为：zint_symbol.symbology,具体请参考zint。rotateAngle旋转的角度，支持:0,90,180,270.autoZoom是否自动缩放,为true且width/height大于1时,将自动缩放条码以按width/height完整显示
int BarcodeRender::drawBarcode(int symbology, TString content, int x, int y, int width, int height, int rotateAngle, bool autoZoom) {
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
        m_pGraphics->DrawImage(&b, x, y);

        return 0;
    }
    return -1;
}

int BarcodeRender::drawCode128Auto(TString content, int x, int y, int width, int height, int rotateAngle, bool autoZoom) {
    return drawBarcode(BARCODE_CODE128, content, x, y, width, height, rotateAngle, autoZoom);
}

int BarcodeRender::drawCode128B(TString content, int x, int y, int width, int height, int rotateAngle, bool autoZoom) {
    return drawBarcode(BARCODE_CODE128B, content, x, y, width, height, rotateAngle, autoZoom);
}

int BarcodeRender::drawQrcode(TString content, int x, int y, int width, int height, bool autoZoom) {
    return drawBarcode(BARCODE_QRCODE, content, x, y, width, height, 0, autoZoom);
}
