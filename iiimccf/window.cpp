// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          window.cpp  -  description
                             -------------------
    begin                : Sun Mar 18 2001
    copyright            : (C) 2001 by huyong
    email                : ccpaging@online.sh.cn
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cassert>
#include <algorithm>
#include <sys/time.h>
#include <algorithm>
#include "global.h"
#include "window.h"

//static members for all the windows
Window* Window::mpConsole = NULL;
char* Window::mpOverlaps = NULL;

Window::Window(int x1, int y1, int x2, int y2, int type)
: mCol(0), mRow(0),
mCursorEnabled(false), mCursorVisible(false), mCursorIntevel(350000),
mFgColor(7), mBgColor(0) {
    mX1 = x1;
    mX2 = x2;
    mY1 = y1;
    mY2 = y2;

    mType = type;

    mpText = NULL;
    mpAttr = NULL;
    mpFlag = NULL;
    SetBuf();

    if (!(type & WS_CHILD)) { // can only be called once!
        if (mpConsole != NULL) {
            throw "Window() can not called more than once!";
        }
        int size = mMaxCols * mMaxRows;
        mpConsole = this;
        mpOverlaps = new char[size];
        memset(mpOverlaps, 0, size);
    }

    SetCursorType(CUR_DEF);
    mVisible = false;
}

void Window::SetBuf() {
    int FramePixs = 0;
    if (mType & WS_FRAMETHIN) {
        FramePixs = 2;
    } else if (mType & WS_FRAMETHICK) {
        FramePixs = 3;
    } else;

    mTextX0 = mTextY0 = 0;
    if (FramePixs > 0) {
        mTextX0 = FramePixs + ((Width() - 2 * FramePixs) % gpScreen->BlockWidth()) / 2;
        mTextY0 = FramePixs + ((Height() - 2 * FramePixs) % gpScreen->BlockHeight()) / 2;
    }

    mMaxCols = (Width() - 2 * FramePixs) / gpScreen->BlockWidth();
    mMaxRows = (Height() - 2 * FramePixs) / gpScreen->BlockHeight();

    mEndCol = mMaxCols - 1;
    mEndRow = mMaxRows - 1;

    int size = mMaxCols * mMaxRows;
    delete[] mpText;
    delete[] mpAttr;
    delete[] mpFlag;
    mpText = new char[size];
    mpAttr = new char[size];
    mpFlag = new char[size];
}

Window::~Window() {
    if (this == mpConsole) {
        delete[] mpOverlaps;
        mpConsole = NULL;
    }
    delete[] mpText;
    delete[] mpAttr;
    delete[] mpFlag;
}

void Window::SetCursorType(int CurType) {
    this->CursorRestore();
    switch (CurType) {
        case CUR_NONE:
            mCursorStart = gpScreen->BlockHeight() - 1;
            mCursorEnd = gpScreen->BlockHeight() - 1;
        case CUR_UNDERLINE:
            if (gpScreen->mBlankLineHeight > 0)
                mCursorStart = gpScreen->BlockHeight() - gpScreen->mBlankLineHeight;
            else
                mCursorStart = gpScreen->BlockHeight() - gpScreen->mBlankLineHeight - 1;
            mCursorEnd = gpScreen->BlockHeight() - 1;
            break;
        case CUR_LOWER_THIRD:
            mCursorStart = gpScreen->BlockHeight()/3;
            mCursorEnd = gpScreen->BlockHeight() - 1;
            break;
        case CUR_LOWER_HALF:
            mCursorStart = gpScreen->BlockHeight()/2;
            mCursorEnd = gpScreen->BlockHeight() - 1;
            break;
        case CUR_TWO_THIRDS:
            mCursorStart = gpScreen->BlockHeight()*2/3;
            mCursorEnd = gpScreen->BlockHeight() - 1;
            break;
        default:  // CUR_DEF, CUR_BLOCK
            mCursorStart = 0;
            mCursorEnd = gpScreen->BlockHeight() - 1;
            break;
    }
}

int Window::ColsOvered() {
    int cols = Width() / gpScreen->BlockWidth();
    if (Width() % gpScreen->BlockWidth())
        cols++;
    return cols;
}

int Window::RowsOvered() {
    int rows = Height() / gpScreen->BlockHeight();
    if (Height() % gpScreen->BlockHeight())
        rows++;
    return rows;
}

void Window::OutChar(int col, int row, int fg, int bg, char c) {
    if (!mVisible)
        return;
    if (this == mpConsole && mpOverlaps[Index(col, row)] > 0)
        return;
    gpScreen->OutChar(mX1 + mTextX0 + col * gpScreen->BlockWidth(),
                      mY1 + mTextY0 + row * gpScreen->BlockHeight(),
                      fg, bg, c);
}

void Window::OutChar(int col, int row, int fg, int bg, char c1, char c2) {
    if (!mVisible)
        return;
    if (this != mpConsole) {
        gpScreen->OutChar(mX1 + mTextX0 + col * gpScreen->BlockWidth(),
                          mY1 + mTextY0 + row * gpScreen->BlockHeight(),
                          fg, bg, c1, c2);
        return;
    }
    int idx = Index(col, row);
    if (mpOverlaps[idx] == 0 && mpOverlaps[idx+1] == 0 ) {
        gpScreen->OutChar(mX1 + mTextX0 + col * gpScreen->BlockWidth(),
                          mY1 + mTextY0 + row * gpScreen->BlockHeight(),
                          fg, bg, c1, c2);
        return;
    }
    if (mpOverlaps[idx] > 0) {
        if (mpOverlaps[idx+1] == 0) {
            // left is overlapped, draw right
            gpScreen->OutChar(mX1 + mTextX0 + (col+1) * gpScreen->BlockWidth(),
                              mY1 + mTextY0 + row * gpScreen->BlockHeight(),
                              fg, bg, c2);
        }
        // whole chinese is in overlaped
        return;
    }
    // left is not overlaped
    if (mpOverlaps[idx+1] > 0) {
        // right is overlapped, draw left
        gpScreen->OutChar(mX1 + mTextX0 + col * gpScreen->BlockWidth(),
                          mY1 + mTextY0 + row * gpScreen->BlockHeight(),
                          fg, bg, c1);
    }
    // both left & right are not overlapped
}

//put c into inner text buffer col and row are relative position
void Window::PutChar(char attr,char flag,char c){
    int i = Index(mCol, mRow);
    mpText[i] = c;
    mpAttr[i] = attr;
    mpFlag[i] = flag;
}

//put a dbl char
void Window::PutChar(char attr,char flag,char c1,char c2){
    int i = Index(mCol, mRow);
    mpText[i] = c1;
    mpAttr[i] = attr;
    mpFlag[i++] = flag;
    mpText[i] = c2;
    mpAttr[i] = attr;
    mpFlag[i] = flag;
}

//return index of (r,c) in mpText,mpAttr,mpFlag
int Window::Index(int c, int r) {
    return c + r * mMaxCols;
}

//do Hanzi recognize then update screen
//if isUpdateAll is true then update all screen
void Window::Redraw(bool isUpdateAll) {
    CursorRestore();
    //then we update console
    int row, indexCol;
    for (row = 0, indexCol = 0; row < mMaxRows; row++) {
        RedrawRow(row, mpText + indexCol, mpAttr + indexCol,
            mpFlag + indexCol, isUpdateAll);
        indexCol += mMaxCols;
    }
    CursorSet();
}

//DrawRow for Console
void Window::RedrawRow(int row, char *pText, char *pAttr, char *pFlag, bool isUpdateAll) {
    for (int col = 0; col < mMaxCols; pText++, pAttr++, pFlag++, col++) {
        if (isUpdateAll == false && !(*pFlag & txtUpdated))
            continue;

        *pFlag &= ~txtUpdated;
        if ((!(*pFlag & txtPrimary) || (*pFlag & txtASCII))) {
            OutChar(col, row, FgColor(*pAttr), BgColor(*pAttr), *pText);
            continue;
        }

        if (*pFlag & txtDblCode) {
            *pFlag &= ~txtDblCode;
            if (col == 0) {
                if (gpDecoder->IsCode1(*pText)) {
                    *pFlag |= txtDblCode1;
                } else {
                    *pFlag |= txtASCII;
                }
            }
            else if (*(pFlag - 1) & txtDblCode1) {
                *pFlag |= txtDblCode2;
            }                          // prev char is ASCII or double code2
            else {
                if (gpDecoder->IsCode1(*pText)) {
                    *pFlag |= txtDblCode1;
                } else {
                    *pFlag |= txtASCII;
                }
            }
            // txtDblCode may convert to txtASCII, special in BIG5
            if (*pFlag & txtASCII) {
                OutChar(col, row, FgColor(*pAttr), BgColor(*pAttr), *pText);
                continue;
            }
        }

        if (*pFlag & txtDblCode1) {
            // absolate double code 1 at last col
            if (col >= mMaxCols - 1) {
                OutChar(col, row, FgColor(*pAttr), BgColor(*pAttr), *pText);
                continue;
            }
            if ( (*(pFlag + 1) & txtASCII) || !(*(pFlag + 1) & txtPrimary)
                || (*(pFlag + 1) & txtDblCode1)
                || !gpDecoder->IsCode2(*(pText + 1)) ) {
                // absolate double code 1
                OutChar(col, row, FgColor(*pAttr), BgColor(*pAttr), *pText);
                continue;
            }
            // if next char is not set to txtDblCode
            if (*(pFlag + 1) & txtDblCode) {
                *(pFlag + 1) &= ~txtDblCode;
                *(pFlag + 1) |= txtDblCode2;
            }
            // next char is or was txtDblCode2
            OutChar(col, row, FgColor(*pAttr), BgColor(*pAttr), *pText, *(pText + 1));
            pText++;
            pAttr++;
            pFlag++;
            col++;
            continue;
        }

        if ( (*pFlag & txtDblCode2) && isUpdateAll == false
            && col != 0 && (*(pFlag-1) & txtDblCode1) ) {
            OutChar(col - 1, row, FgColor(*pAttr), BgColor(*pAttr), *(pText-1), *pText);
        }
    }
}

void Window::RedrawChar(int col, int row) {
    assert(col >= 0 && col <= mEndCol && row >= 0 && row <= mEndRow);

    int idx = Index(col, row);

    mpFlag[idx] &= ~txtUpdated;
    if ((mpFlag[idx] & txtDblCode1) && col < mpConsole->MaxCols() - 1
        && (mpFlag[idx + 1] & txtDblCode2) ) {
        //full hz out
        OutChar(col, row, FgColor(mpAttr[idx]), BgColor(mpAttr[idx]),
                mpText[idx], mpText[idx + 1]);
        return;
    }
    if ((mpFlag[idx] & txtDblCode2) && col > 0
        && (mpFlag[idx - 1] & txtDblCode1) ) {
        //full hz out
        OutChar(col-1, row, FgColor(mpAttr[idx]), BgColor(mpAttr[idx]),
                mpText[idx-1], mpText[idx]);
        return;
    }
    
    //ascii or single double code
    OutChar(col, row, FgColor(mpAttr[idx]), BgColor(mpAttr[idx]),
            mpText[idx]);
}

//implement blink attribute on console
void Window::UpdateBlinkAttr(bool show) {
    int size = mpConsole->MaxCols() * mpConsole->MaxRows();
    char *pFlag = mpConsole->mpFlag;
    char *pAttr = mpConsole->mpAttr;
    char *pText = mpConsole->mpText;
    int r, c;
    for (int i = 0; i < size; i++) {
        if (mpOverlaps[i] == 0 && pAttr[i] & 0x80)
            continue;

        r = i / mpConsole->MaxCols();
        c = i % mpConsole->MaxCols();

        if (show) {
            if ((pFlag[i] & txtDblCode1) && c < mpConsole->MaxCols() - 1
                && (pFlag[i + 1] & txtDblCode2) ) {
                //full hz out
                Window::OutChar(c, r,FgColor(pAttr[i]), BgColor(pAttr[i]),
                    pText[i], pText[i + 1]);
                i++;
            } else {
                //ascii or code2
                Window::OutChar(c, r,FgColor(pAttr[i]), BgColor(pAttr[i]),
                        pText[i]);
            }
        } else {
            //show = false,fill with blank
            Window::OutChar(c, r,BgColor(pAttr[i]), BgColor(pAttr[i]),' ');
        }
    }
}

// only used when decoder is changed, GB->BIG5
void Window::ResetFlagAll() {
    int row, indexCol;
    for (row = 0, indexCol = 0; row < mMaxRows; row++) {
        ResetFlagRow(mpText + indexCol, mpFlag + indexCol,mMaxCols);
        indexCol += mMaxCols;
    }
}

//used when decoder is changed
//or it's useful when you want to reset a whole row's flag
void Window::ResetFlagRow(char *pText, char *pFlag,int len) {
    for (int col = 0; col < len; col++) {
        if (pFlag[col] & txtPrimary) {
           if (gpDecoder->IsCode1(pText[col]) || gpDecoder->IsCode2(pText[col])) {
               pFlag[col] = txtDblCode | txtPrimary | txtUpdated;
           } else {
               pFlag[col] = txtASCII | txtPrimary | txtUpdated;
           }
        } else {
           pFlag[col] = txtASCII | txtUpdated;
        }
    }
}

// set update and overlap flag for popwin's show/hide, by pixel
void Window::SetOverlaped(int x1, int y1, int x2, int y2, bool isOvered) {
    if (this != mpConsole)
        return;
    // debug<<"popwin "<<x1<<','<<y1<<' '<<x2<<','<<y2<<endl;
    int OverX1,OverY1,OverX2,OverY2;
    OverX1 = max( x1, mX1 );
    OverY1 = max( y1, mY1 );
    OverX2 = min( x2, mX2 );
    OverY2 = min( y2, mY2 );
    //debug<<"popwin over"<<OverX1<<','<<OverY1
    //     <<' '<<OverX2<<','<<OverY2<<endl;
    // console window's mTextX0, mTextY0 must be 0
    int c1, r1, c2, r2;
    c1 = (OverX1 - mX1) / gpScreen->BlockWidth();
    r1 = (OverY1 - mY1) / gpScreen->BlockHeight();
    c2 = (OverX2 - mX1) / gpScreen->BlockWidth();
    r2 = (OverY2 - mY1) / gpScreen->BlockHeight();
    if (r1 > mEndRow) r1 = mEndRow;
    if (r2 > mEndRow) r2 = mEndRow;

    //modify flags to POP_CHAR
    int r, c;
    int idx;
    if (isOvered) {
        //debug<<"Over "<<c1<<','<<r1<<' '<<c2<<','<<r2<<endl;
        for (r = r1;r <= r2;r++) {
            idx = Index(c1, r);
            for (c = c1; c <= c2;c++, idx++) {
                (mpOverlaps[idx])++;
            }
        }
    } else {
        //debug<<"Restore "<<c1<<','<<r1<<' '<<c2<<','<<r2<<endl;
        for (r = r1;r <= r2;r++) {
            idx = Index(c1, r);
            for (c = c1; c <= c2;c++, idx++) {
                (mpOverlaps[idx])--;
                mpFlag[idx] |= txtUpdated;
            }
        }
    }
}

void Window::Show() {
    if (mVisible == true)
        return;
    mVisible = true;
    if (this != mpConsole)
        mpConsole->SetOverlaped(mX1, mY1, mX2, mY2, true);
    int w, h;
    //debug<<mX1<<","<<mY1<<" "<<mX2<<","<<mY2<<endl;
    if (mType & WS_FRAMETHIN) {
        // upper block
        if (mTextY0 > 1)
            gpScreen->FillRect(mX1, mY1, mX2, mY1+mTextY0-1, mBgColor);
        // down block
        int h = gpScreen->BlockHeight() - mTextY0;
        if (h > 1)
            gpScreen->FillRect(mX1, mY2-h+1, mX2, mY2, mBgColor);
        // left block
        if (mTextX0 > 1)
            gpScreen->FillRect(mX1, mY1+mTextY0, mX1+mTextX0-1, mY2-h, mBgColor);
        w = gpScreen->BlockWidth() - mTextX0;
        // right block
        if (w > 1)
            gpScreen->FillRect(mX2-w+1, mY1+mTextY0, mX2, mY2-h, mBgColor);
        // inner frame
        gpScreen->FillRect(mX1, mY1, mX1, mY2, mFrameDark);
        gpScreen->FillRect(mX1, mY1, mX2, mY1, mFrameDark);
        gpScreen->FillRect(mX1+1, mY2, mX2, mY2, mFrameLight);
        gpScreen->FillRect(mX2, mY1+1, mX2, mY2, mFrameLight);
    } else if (mType & WS_FRAMETHICK) {
        // upper block
        gpScreen->FillRect(mX1, mY1, mX2, mY1+mTextY0-1, mBgColor);
        // down block
        h = gpScreen->BlockHeight() - mTextY0;
        gpScreen->FillRect(mX1, mY2-h+1, mX2, mY2, mBgColor);
        // left block
        gpScreen->FillRect(mX1, mY1+mTextY0, mX1+mTextX0-1, mY2-h, mBgColor);
        w = gpScreen->BlockWidth() - mTextX0;
        // right block
        gpScreen->FillRect(mX2-w+1, mY1+mTextY0, mX2, mY2-h, mBgColor);
        // inner frame
        gpScreen->FillRect(mX1+2, mY1+2, mX1+2, mY2-2, mFrameDark);
        gpScreen->FillRect(mX1+2, mY1+2, mX2-2, mY1+2, mFrameDark);
        gpScreen->FillRect(mX1+3, mY2-2, mX2-2, mY2-2, mFrameLight);
        gpScreen->FillRect(mX2-2, mY1+3, mX2-2, mY2-2, mFrameLight);
    }
    ResetFlagAll();
    Redraw();
}

void Window::Hide() {
    if (mVisible == false)
        return;
    mVisible = false;
        
    if (this != mpConsole) {
        mpConsole->SetOverlaped(mX1, mY1, mX2, mY2, false);
        mpConsole->Redraw();
    }
    // autoclear when mpConsole redraw
}

void Window::Clear() {
    int size = mMaxRows*mMaxCols;
    memset(mpText,' ',size);
    memset(mpAttr,BuildColor(mFgColor,mBgColor),size);
    memset(mpFlag,txtASCII | txtUpdated,size);
}

void Window::Clear(int c1, int r1, int c2, int r2, char attr) {
    assert(r1 >= 0 && r1 <= mEndRow && c1 >= 0 && c1 <= mEndCol
        && r2 >= 0 && r2 <= mEndRow && c2 >= 0 && c2 <= mEndCol);
    assert(r2 >= r1);
    assert(c2 >= c1);

    int n = c2 - c1 + 1;
    int i;
    for (int r = r1; r <= r2; r++) {
        i = Index(c1, r);
        memset(&mpText[i], ' ', n);
        memset(&mpAttr[i], attr, n);
        memset(&mpFlag[i], txtASCII | txtUpdated, n);
    }
}

void Window::MoveTo(int x, int y) {
    bool VisibleOld = mVisible;
    if (mVisible) Hide();
    int w = mX2 - mX1 + 1;
    int h = mY2 - mY1 + 1;

    mX1 = x;
    mY1 = y;
    //debug<<"mX1="<<mX1<<" mY1="<<mY1<<endl;
    assert(mX1 >= 0 && mX1 < gpScreen->Width());
    assert(mY1 >= 0 && mY1 < gpScreen->Height());

    mX2 = mX1 + w - 1;
    mY2 = mY1 + h - 1;
    //debug<<"mX2="<<mX2<<" mY2="<<mY2<<endl;
    assert(mX2 >= 0 && mX2 < gpScreen->Width());
    assert(mY2 >= 0 && mY2 < gpScreen->Height());
    if (VisibleOld) Show();
}

//after resize,the top-left position is not changed
//but all the contents will be erased!
void Window::Resize(int w,int h){
    bool VisibleOld = mVisible;
    if (mVisible) Hide();
    assert(mpConsole && mpConsole != this);//this operation do not apply to console
    mX2 = mX1 + w - 1;
    mY2 = mY1 + h - 1;
    //debug<<mX1<<","<<mY1<<" "<<mX2<<","<<mY2<<endl;
    assert(mX2 > 0 && mX2 < gpScreen->Width());
    assert(mY2 > 0 && mY2 < gpScreen->Height());

    SetBuf();

    if(VisibleOld) Show();
}

//put s into buffer,cut string if it's too long
//append blanks if necessary
//s use '%color' as eacspe char to change color
//use '%255' to restore original color
void Window::DrawColorStr(int col,int row,string s){
    CursorRestore();
    int n = mMaxCols;
    const char *p = s.c_str();
    int fg = mFgColor;
    string t;
    while (1) {
        t = "";
        while (n && *p && *p != '%') {
            t += *p++;
            n--;
        }
        PutStr(col,row,t,fg);
        col += t.size();
        if (n == 0 || *p == '\0')
            break;
        p++;
        if (*p != '\0') {
            if (*p == (char) 255)
                fg = mFgColor;
            else
                fg = *p;
            p++;
        }
    }//while
    if (n) {
        //append blanks
        t = string(n, ' ');
        PutStr(col,row,t,fg);
    }
    CursorSet();
}

//put string into inner buffer
void Window::PutStr(int col,int row,string s) {
    PutStr(col,row,s,mFgColor);
}

//put string into inner buffer
void Window::PutStr(int col,int row,string s,char fg) {
    assert(col < mMaxCols);
    int i = Index(col, row);
    int cols = s.size();
    if (col + cols >= mMaxCols)  // cut off window
        cols = mMaxCols - col;
    memcpy(&mpText[i], s.c_str(), cols);
    memset(&mpAttr[i], BuildColor(fg,mBgColor), cols);
    //no table char in pop win so use txtPrimary
    //fix me, set update after compare
    memset(&mpFlag[i], txtPrimary | txtUpdated, cols);
    ResetFlagRow(&mpText[i],&mpFlag[i],cols);
}

void Window::CursorShow() {
    if (mCursorEnabled == true)
        return;
    mCursorEnabled = true;
    CursorSet();
}

void Window::CursorHide() {
    if (mCursorEnabled == false)
        return;
    CursorRestore();
    mCursorEnabled = false;
}

void Window::CursorSet() {
    if (mCursorEnabled == false)
        return;
    // more rapid when fresh screen
    // CursorBlink();
    //debug<<"(CursorSet)"<<flush;
    if (mCursorVisible == false) {
        CursorDraw();
        mCursorVisible = true;
    }
}

void Window::CursorRestore() {
    if (mCursorEnabled == false)
        return;
    //debug<<"(CursorRestore)"<<flush;
    if (mCursorVisible == true) {
        CursorDraw();
        mCursorVisible = false;
    }
}

void Window::CursorBlink() {
    static timeval sLastTime;
    if (mCursorEnabled == false)
        return;

    timeval tv;
    if (gettimeofday(&tv, NULL))
        return;

    unsigned t;
    t = (tv.tv_sec - sLastTime.tv_sec) * 1000000 + tv.tv_usec -
        sLastTime.tv_usec;
    if (t >= mCursorIntevel) {
        sLastTime = tv;
        // DrawCursor(sVisible = !sVisible);
        // Window::UpdateBlinkAttr(mCursorVisible);
        CursorDraw();
        mCursorVisible = !mCursorVisible;
    }
}

void Window::CursorDraw() {
    if (!mVisible)
        return;
    //should not draw cursor on pop win
    if (mpOverlaps[Index(mCol, mRow)] > 0)
        return;
    int x, y;
    x = mX1 + mCol * gpScreen->BlockWidth();
    y = mY1 + mRow * gpScreen->BlockHeight();
    //gpScreen->FillRect(x, y + mCursorStart, x + gpScreen->BlockWidth() - 1,
    //                   y + mCursorEnd, color);
    gpScreen->RevRect(x, y + mCursorStart, x + gpScreen->BlockWidth() - 1,
                       y + mCursorEnd);
}

void Window::Goto(int c, int r) {
    CursorRestore();
    mRow = r;
    mCol = c;
    CursorSet();
}

