#pragma once

#include "afxTodop.h"
#include <string>

namespace hellop {
    enum PaperType {
        paper_type_custom,
        paper_type_a0,
        paper_type_a1,
        paper_type_a2,
        paper_type_a3,
        paper_type_a4,
        paper_type_a5,
        paper_type_a6,
        paper_type_a7,
        paper_type_a8,
        paper_type_a9,
        paper_type_a10
    };


    class Paper {
    public:
        explicit Paper(PaperType type=paper_type_a4);
        explicit Paper(double width, double height);
        ~Paper();

        unsigned int getWidthPx(unsigned int ppiX); // 获取纸张宽度（根据PPI(每英尺像素数)自动计算），单位：像素数
        unsigned int getHeightPx(unsigned int ppiY); // 获取纸张高度（根据PPI(每英尺像素数)自动计算），单位：像素数

    private:
        double mWidth; // 纸张的宽度，单位：毫米.
        double mHeight; // 纸张的高度，单位：毫米.
        PaperType mType; // 纸张的类型，A4,A5,custom(自定义)等
    };
}
