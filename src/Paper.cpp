#include "Paper.h"

hellop::Paper::Paper(PaperType type) {
    mType = type;

    if(paper_type_a0 == mType) {
        mWidth = 841.0;
        mHeight = 1189.0;
    } else if(paper_type_a1 == mType) {
        mWidth = 594.0;
        mHeight = 841.0;
    } else if(paper_type_a2 == mType) {
        mWidth = 420.0;
        mHeight = 594.0;
    } else if(paper_type_a3 == mType) {
        mWidth = 297.0;
        mHeight = 420.0;
    } else if(paper_type_a4 == mType) {
        mWidth = 210.0;
        mHeight = 297.0;
    } else if(paper_type_a5 == mType) {
        mWidth = 148.0;
        mHeight = 210.0;
    } else if(paper_type_a6 == mType) {
        mWidth = 105.0;
        mHeight = 148.0;
    } else if(paper_type_a7 == mType) {
        mWidth = 74.0;
        mHeight = 105.0;
    } else if(paper_type_a8 == mType) {
        mWidth = 52.0;
        mHeight = 74.0;
    } else if(paper_type_a9 == mType) {
        mWidth = 37.0;
        mHeight = 52.0;
    } else if(paper_type_a10 == mType) {
        mWidth = 26.0;
        mHeight = 37.0;
    }
}

hellop::Paper::Paper(double width, double height) {
    if(width < 10.0 || height < 10.0) {
        mType = paper_type_a4;
        mWidth = 210.0;
        mHeight = 297.0;
    } else {
        mType = paper_type_custom;
        mWidth = width;
        mHeight = height;
    }
}

hellop::Paper::~Paper() {
    // do nothing
}

unsigned int hellop::Paper::getWidthPx(unsigned int ppiX) {
    return mm_to_px(mWidth, ppiX); // 将毫米转为像素
}

unsigned int hellop::Paper::getHeightPx(unsigned int ppiY) {
    return mm_to_px(mHeight, ppiY); // 将毫米转为像素
}
