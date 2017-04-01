#include <cmath>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <string>

#include <ZLUnicodeUtil.h>
#include <ZLTimeManager.h>
#include <ZLTextSelectionModel.h>

#include "BookshelfView.h"
#include "Fbookshelf.h"
#include "../bookmodel/BookModel.h"
#include "../options/FBTextStyle.h"

const int ELEMENTS_ON_X = 2;
const int ELEMENTS_ON_Y = 2;
const ZLColor ELEMENT_COLOR = ZLColor(190,190,190);
const ZLColor ELEMENT_FRAME_COLOR = ZLColor(250,250,250);
const ZLColor BACKGROUND_COLOR = ZLColor(255,255,255);
const std::string CAPTION = "Bookshelf";

BookshelfView::BookshelfView(ZLPaintContext &context) : ZLView(context),
                                                        mCaption(CAPTION),
                                                        mViewWidth(context.width()),
                                                        mViewHeight(context.height()),
                                                        mBackgroundColor(BACKGROUND_COLOR),
                                                        mElementsOnX(ELEMENTS_ON_X),
                                                        mElementsOnY(ELEMENTS_ON_Y),
                                                        mRenderingElementsCount(mElementsOnX * mElementsOnY),
                                                        mElementWidth(mViewWidth / mElementsOnX),
                                                        mElementHeight(mViewHeight / mElementsOnY),
                                                        mFontSize(std::min(mElementWidth/20, mElementHeight/20))
{
  //  init();
}

void BookshelfView::init()
{
    int x1 = 0;
    int y1 = 0;
    int x2 = mElementWidth;
    int y2 = mElementHeight;

    shared_ptr<BookModel> model;
    BookshelfElement element;

    BooksMap::const_iterator it = Fbookshelf::Instance().getLibrary().begin();
    BooksMap::const_iterator itEnd = Fbookshelf::Instance().getLibrary().end();

    for(; it != itEnd; ++it)
    {

        model = new BookModel((*it).second);

        element.mImageData = ZLImageManager::Instance().imageData(*(model->imageMap().begin()->second));
        element.mBook = (*it).second;
        element.mTopLeft.x = x1;
        element.mTopLeft.y = y1;
        element.mBottomRight.x = x2;
        element.mBottomRight.y = y2;
        element.mElementColor = ELEMENT_COLOR;
        element.mFrameColor = ELEMENT_FRAME_COLOR;

        mBookshelfElements.push_back(make_pair((*it).first, element));

        x1 += mElementWidth;
        x2 += mElementWidth;

        if(x2 > mViewWidth)
        {
            x1 = 0;
            x2 = mElementWidth;
            y1 += mElementHeight;
            y2 += mElementHeight;
        }
    }


    setScrollbarEnabled(VERTICAL, true);
    setScrollbarParameters(VERTICAL, mBookshelfElements.size()/mElementsOnY, 0, 1);

    mScrollBarPos = 0;

    mItFirstRendering = mItLastRendering = mBookshelfElements.begin();
    mItLastRendering += mBookshelfElements.size() > mRenderingElementsCount ? mRenderingElementsCount : mBookshelfElements.size();


}


bool BookshelfView::onStylusPress(int x, int y) {
    mStartPoint.x = x;
    mStartPoint.y = y;

    return true;
}


bool BookshelfView::onStylusMovePressed(int x, int y) {
    mEndPoint.x = x;
    mEndPoint.y = y;

    Fbookshelf::Instance().refreshWindow();

    return true;
}


//What is it?
void BookshelfView::onScrollbarStep(ZLView::Direction direction, int steps)
{
    std::cout << "onscrollstep\n";
}



void BookshelfView::onScrollbarMoved(ZLView::Direction direction, size_t full, size_t from, size_t to)
{

    if(from < mScrollBarPos)
    {
        UpdateScrollUp();
        std::cout << "update scroll up\n";
    }
    else
    {
        UpdateScrollDown();
        std::cout << "update scroll down\n";
    }

    mScrollBarPos = from;
}



void BookshelfView::onScrollbarPageStep(ZLView::Direction direction, int steps)
{

    if(steps < 0)
    {
        UpdateScrollUp();
        std::cout << "update scroll step up\n";
    }
    else
    {
        UpdateScrollDown();
        std::cout << "update scroll step down\n";
    }
}


//doesn't work
void BookshelfView::onMouseScroll(bool forward)
{
    std::cout << "mouse scroll \n";

    if(forward)
    {
        UpdateScrollUp();
        std::cout << "update scroll step up\n";
    }
    else
    {
        UpdateScrollDown();
        std::cout << "update scroll step down\n";
    }
}



void BookshelfView::UpdateBookshelfElements()
{
    mViewWidth = context().width();
    mViewHeight = context().height();

    mElementWidth = mViewWidth / mElementsOnX;
    mElementHeight = mViewHeight / mElementsOnY;

    int x1 = 0;
    int y1 = 0;
    int x2 = mElementWidth;
    int y2 = mElementHeight;

    for(std::vector<std::pair<std::string, BookshelfElement> >::iterator it = mItFirstRendering; it != mItLastRendering; ++it)
    {
        (*it).second.UpdatePosition(x1, y1, x2, y2);

        x1 += mElementWidth;
        x2 += mElementWidth;

        if(x2 > mViewWidth)
        {
            x1 = 0;
            x2 = mElementWidth;
            y1 += mElementHeight;
            y2 += mElementHeight;
        }
    }

    mFontSize = std::min(mElementHeight/20, mElementWidth/20);

    const FBTextStyle &style = FBTextStyle::Instance();
    context().setFont(style.fontFamily(), mFontSize, style.bold(), style.italic());
}



void BookshelfView::UpdateScrollDown()
{
    if(mItLastRendering + mElementsOnX <= mBookshelfElements.end()){
        mItFirstRendering += mElementsOnX;
        mItLastRendering += mElementsOnX;
    }
    else
    {
        mItLastRendering = mItFirstRendering = mBookshelfElements.end();
        mItFirstRendering -= mBookshelfElements.size() > mRenderingElementsCount ? mRenderingElementsCount : mBookshelfElements.size();
    }

    assert(mItFirstRendering <= mItLastRendering);
    assert(mItFirstRendering >= mBookshelfElements.begin() && mItFirstRendering < mBookshelfElements.end());
    assert(mItLastRendering > mBookshelfElements.begin() && mItLastRendering <= mBookshelfElements.end());

    UpdateBookshelfElements();
    Fbookshelf::Instance().refreshWindow();
}

void BookshelfView::UpdateScrollUp()
{
    if(mItFirstRendering - mElementsOnX >= mBookshelfElements.begin()){
        mItFirstRendering -= mElementsOnX;
        mItLastRendering -= mElementsOnX;
    }
    else
    {
        mItFirstRendering = mItLastRendering = mBookshelfElements.begin();
        mItLastRendering += mBookshelfElements.size() > mRenderingElementsCount ? mRenderingElementsCount : mBookshelfElements.size();
    }

    assert(mItFirstRendering <= mItLastRendering);
    assert(mItFirstRendering >= mBookshelfElements.begin() && mItFirstRendering < mBookshelfElements.end());
    assert(mItLastRendering > mBookshelfElements.begin() && mItLastRendering <= mBookshelfElements.end());

    UpdateBookshelfElements();
    Fbookshelf::Instance().refreshWindow();
}



void BookshelfView::DrawBookshelfElements()
{
    if(context().width() != mViewWidth || context().height() != mViewHeight)
    {
        UpdateBookshelfElements();
    }

    for(std::vector<std::pair<std::string, BookshelfElement> >::const_iterator it = mItFirstRendering; it != mItLastRendering; ++it)
    {
        (*it).second.DrawElement(context(),mFontSize);
    }
}



void BookshelfView::DrawBackground()
{
    context().setFillColor(mBackgroundColor);
    context().fillRectangle(0,0,context().width(),context().height());
}



const std::string &BookshelfView::caption() const {
    return mCaption;
}



ZLColor BookshelfView::backgroundColor() const {
    return mBackgroundColor;
}



void BookshelfView::paint() {

    DrawBackground();
    DrawBookshelfElements();


    mStartPoint.x = mEndPoint.x;
    mStartPoint.y = mEndPoint.y;
}
