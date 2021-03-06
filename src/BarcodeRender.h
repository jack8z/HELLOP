﻿#pragma once

#include "afxTodop.h"

#include <zint.h>
#pragma comment(lib, "libzint.lib")

namespace hellop {
    class BarcodeRender
    {
    public:
        BarcodeRender(Graphics* graphics);
        ~BarcodeRender();

        // 绘制条码.自动选择CODE128A,CODE128B,CODE128C.单位:像素
        int drawCode128Auto(TString content, int x, int y, int width = 0, int height = 0, int rotateAngle = 0, bool autoZoom = false);
        // 绘制条码CODE128B
        int drawCode128B(TString content, int x, int y, int width = 0, int height = 0, int rotateAngle = 0, bool autoZoom = false);
        
        // 绘制QR码.
        int drawQrcode(TString content, int x, int y, int width = 0, int height = 0, bool autoZoom = false);

    private:
        // 绘制Barcode，第一个参数symbology的值为：zint_symbol.symbology,具体请参考zint。rotateAngle旋转的角度，支持:0,90,180,270.autoZoom是否自动缩放,为true且width/height大于1时,将自动缩放条码以按width/height完整显示
        int drawBarcode(int symbology, TString content, int x, int y, int width = 0, int height = 0, int rotateAngle = 0, bool autoZoom = false);

    private:
        struct zint_symbol *m_pZintSymbol;
        Graphics *m_pGraphics;
    };
}
