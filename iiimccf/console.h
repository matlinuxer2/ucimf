// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          console.h  -  description
                             -------------------
    begin                : Sun Mar 18 2001
    copyright            : (C) 2001 by ejoy
    email                : ejoy@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSOLE_H
#define CONSOLE_H

/**
 *@author ejoy
 */
#include <cassert>
#include "global.h"
#include "basefont.h"
#include "window.h"

const int MAX_ESC_PARAMS = 5;
const int HISTORY_LINES = 2000;

class Console : public Window {
    friend class Window;
    public:
        Console(int x1, int y1, int x2, int y2);
        ~Console();

        void VtSizeDelta(int ColDelta, int RowDelta);
        void GetVtSize(int &cols, int &rows);
        /** Write num chars in pBuf to console */
        void Write(const char *pBuf, int num);
        void Cr();
        Encode DetectBufferEncode();
        enum ScrollFlag { PAGE_UP, PAGE_DOWN, LINE_UP, LINE_DOWN };
        void ScrollDelta(ScrollFlag f);

        // mouse paste selection
        void SelClear();
        void SelCopy(int c1, int r1, int c2, int r2, int mode);
        void SelPaste(int fd);

    private:
        void ParseEscape(char c);
        void DoEscape(char c);
        // do ANSI control function
        void DoControl(char c);
        void CopyLines(int r1, int r2, int count);
        void DeleteLine(int n);
        void InsertLine(int n);
        void DeleteChar(int n);
        void InsertBlank(int n);
        void CopyChar(int c1, int r1, int c2, int r2);
        void SendChar(char c);

        enum DIR { UP, DOWN };
        void ScrollScr(DIR dir);
        void DefaultAttr();
        void Reset();
        
        void UnSaveCursor();
        void SaveCursor();
        
        void SetMode(bool f);
        void Checkxy();
        void ConGoto(int c, int r);
        void AbsGoto(int c, int r);
        void Lf();
        void PushHistoryLine();
        void ShowHistory(int offset);
        void UpdateAttr();

        // history buffers
        char *mpHistText;
        char *mpHistAttr;
        char *mpHistFlag;
        //saved buffers in history mode
        char *mpSavTextBuf;
        char *mpSavAttrBuf;
        char *mpSavFlagBuf;
        // history current row
        int mHistCurRow;
        bool mHistMode;

        int mConMaxCols, mConMaxRows;  // lines of cols and rows
        int mConEndCol, mConEndRow;   // last line
        /** cursor position,write next char from here */
        //int mCol, mRow;
        int mOldCol, mOldRow;
        
        int mScrollStart;
        int mScrollEnd;

        char mColor;
        char mDefColor;
        char mUlColor;
        char mHalfColor;
        char mIntensity;
        char mAttr;
        bool mUnderline;
        bool mBlink;
        bool mNeedWrap;
        bool mBold;
        bool mReverse;
        bool mAutoWrap;
        bool mInsertMode;
        // Origin relative/absolute
        bool mDecom;
        char mOldColor;
        bool mOldBlink;
        bool mOldUnderline;
        char mOldIntensity;
        bool mOldBold;
        bool mOldReverse;
        enum ESC_STATE { NORMAL, ESC, SQUARE, NONSTD };
        ESC_STATE mEscState;
        /** array to store escape params */
        int mEscParam[MAX_ESC_PARAMS];
        char mEscIntro;
        bool mEscQuestion;
        /** point to current esc param */
        int *mpEscParam;
        static int mColorTable[16];
        int mTabStop[5];

        enum CharSet { PRIMARY = 0, ALT1 = 0x10, ALT2 = 0x20 };
        CharSet mCharSet;
        CharSet mOldCharSet;

        // mouse pointer
        int mMouseIdx;
        char mMouseMask;
        // mouse paste selection
        void SelHighlight(const int begin, const int end);
        void SelPointer(const int offset);
        int  InWord(const unsigned char c);
        static __u32 mInWordLut[8];
        int  AtColEdge(const int p);
        
        int   mSelStart;  // cleared by clear_selection
        int   mSelEnd;
        int   mSelBufLen;
        char* mpSelBuf;
};
#endif
