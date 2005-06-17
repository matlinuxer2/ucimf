// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          console.cpp  -  description
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

#include <cassert>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include "console.h"
int Console::mColorTable[16] =
{ 0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15 };

Console::Console(int x1, int y1, int x2, int y2)
:Window(x1, y1, x2, y2, WS_DEF), // set window console pointer
mHistMode(false),
mDefColor(0x07),
mUlColor(0x0f),
mHalfColor(0x08),
mEscState(NORMAL)
{
    mConMaxCols = mMaxCols;
    mConMaxRows = mMaxRows;
    mConEndCol = mEndCol;
    mConEndRow = mEndRow;
    int size = mMaxCols * mMaxRows;
    mpSavTextBuf = new char[size];
    mpSavAttrBuf = new char[size];
    mpSavFlagBuf = new char[size];

    Reset();
    Show();
    CursorShow();

    size = mMaxCols * HISTORY_LINES;
    mpHistText = new char[size];
    mpHistAttr = new char[size];
    mpHistFlag = new char[size];
    memset(mpHistText, ' ', size);
    memset(mpHistAttr, mAttr, size);
    memset(mpHistFlag, txtASCII | txtUpdated, size);
    mHistCurRow = 0;

    mMouseMask = 0x77;  // 0x80 when mono
    mMouseIdx = -1;
    mSelStart = -1;  // cleared by clear_selection
    mpSelBuf = NULL;
}

Console::~Console() {
    delete[] mpHistText;
    delete[] mpHistAttr;
    delete[] mpHistFlag;
    delete[] mpSavTextBuf;
    delete[] mpSavAttrBuf;
    delete[] mpSavFlagBuf;
    delete[] mpSelBuf;
}

/** handle ansi esc command */
void Console::DoEscape(char c) {

    if (mEscIntro == '\0') {
        //ESC seen
        switch (c) {
            case 'D':
                Lf();
                break;
            case 'E':
                Cr();
                Lf();
                break;
            case 'H':  // Set Tab <ESC>H
                mTabStop[Row() >> 5] |= (1 << (Row() & 31));
                break;
            case 'M':  // Reverse Index,Scroll Up
            case 'T':  // FreeBSD, cons25
                if (Row() == mScrollStart)
                    ScrollScr(DOWN);
                else if (Row())
                    ConGoto(Col(), Row() - 1);
                mNeedWrap = false;
                break;
            case 'c':
                Reset();
                // Redraw(); // why need? called by write
                break;
            case '>':
                assert(!">");
                break;
            case '=':
                assert(!"=");
                break;
            case '7':  // Save Cursor & Attrs <ESC>7
                SaveCursor();
                break;
            case '8':  // Restore Cursor & Attrs  <ESC>8
                UnSaveCursor();
                break;
            default:
                break;
        }
    }
    else if (mEscIntro == '[') {
        //ESC [ seen
        int num;
                                       //param count
        num = mpEscParam - mEscParam + 1;
        switch (c) {
            case 'A':  // Restore Cursor & Attrs  <ESC>8
                if (!mEscParam[0])
                    mEscParam[0]++;
                ConGoto(Col(), Row() - mEscParam[0]);
                break;
            case 'B':  // Cursor Down <ESC>[{COUNT}B
            case 'e':
                if (!mEscParam[0])
                    mEscParam[0]++;
                ConGoto(Col(), Row() + mEscParam[0]);
                break;
            case 'C':  // Cursor Forward <ESC>[{COUNT}C
            case 'a':
                if (!mEscParam[0])
                    mEscParam[0]++;
                ConGoto(Col() + mEscParam[0], Row());
                break;
            case 'D':  // Cursor Backward <ESC>[{COUNT}D
                if (!mEscParam[0])
                    mEscParam[0]++;
                ConGoto(Col() - mEscParam[0], Row());
                break;
            case 'E':
                if (!mEscParam[0])
                    mEscParam[0]++;
                ConGoto(0, Row() + mEscParam[0]);
                break;
            case 'F':
                if (!mEscParam[0])
                    mEscParam[0]++;
                ConGoto(0, Row() - mEscParam[0]);
                break;
            case 'G':  // Cursor Horizontal Absolute
            case '`':
                if (mEscParam[0])
                    mEscParam[0]--;
                ConGoto(mEscParam[0], Row());
                break;
            case 'H':  // ESC [m;nH moves cursor to (m,n)
            case 'f':
                if (mEscParam[0])
                    mEscParam[0]--;
                if (mEscParam[1])
                    mEscParam[1]--;
                AbsGoto(mEscParam[1], mEscParam[0]);
                break;
            case 'J':  // ESC [sJ clears in display
                switch (mEscParam[0]) {
                    case 0:  // clear from cursor to end of screen
                        Clear(Col(), Row(), mConEndCol, Row(), mAttr);
                        if (Row() < mConEndRow)
                            Clear(0, Row() + 1, mConEndCol, mConEndRow, mAttr);
                        break;
                    case 1:  // clear from start to cursor
                        Clear(0, Row(), Col(), Row(), mAttr);
                        if (Row() > 0)
                            Clear(0, 0, mConEndCol, Row() - 1, mAttr);
                        break;
                    case 2:  // clear whole screen
                        Clear(0, 0, mConEndCol, mConEndRow, mAttr);
                        break;
                    default:  // do nothing
                        break;
                }
                mNeedWrap = false;
                break;
            case 'K':  // ESC [sK clears lines from cursor
                switch (mEscParam[0]) {
                    case 0:  // clear from cursor to end of line
                        Clear(Col(), Row(), mConEndCol, Row(), mAttr);
                        break;
                    case 1:  // clear from beinning of line to cursor
                        Clear(0, Row(), Col(), Row(), mAttr);
                        break;
                    case 2:  // clear whole line
                        Clear(0, Row(), mConEndCol, Row(), mAttr);
                        break;
                    default:  // do nothing
                        break;
                }
                mNeedWrap = false;
                break;
            case 'L':  // ESC [nL inserts n lines at cursor
                InsertLine(mEscParam[0]);
                break;
            case 'M':  // ESC [nM deletes n lines at cursor
                DeleteLine(mEscParam[0]);
                break;
            case 'P':  // ESC [nP deletes n chars at cursor
                DeleteChar(mEscParam[0]);
                break;
            case 'X':
                {
                    int n = mEscParam[0];
                    if (!n)
                        n++;
                    Clear(Col(), Row(),
                          Col() + n > mConEndCol ? mConEndCol : Col() + n,
                          Row(),
                          mAttr);
                    mNeedWrap = false;
                }
                break;
            case '@':  // ESC [n@ insert n chars at cursor
                InsertBlank(mEscParam[0]);
                break;
            case ']':
                assert(!"]");
                break;
            case 'c':
                if (mEscQuestion) {
                    if (mEscParam[0])
                        SetCursorType(mEscParam[0] | (mEscParam[1]<<8) | (mEscParam[2]<<16));
                    else
                        SetCursorType(CUR_DEFAULT);
                }
                break;
            case 'd':
                if (mEscParam[0])
                    mEscParam[0]--;
                AbsGoto(Col(), mEscParam[0]);
                break;
            case 'g':  // Clear Tab  <ESC>[{3}g
                if (num == 0)
                    mTabStop[Col() >> 5] &= ~(1 << (Col() & 31));
                else if (mEscParam[0] == 3)
                    fill_n(mTabStop, 5, 0);
                break;
            case 'h':
                SetMode(true);
                break;
            case 'l':
                SetMode(false);
                break;
            case 'm':  // ESC [nm set attribute
                for (int i = 0; i < num; i++) {
                    int value = mEscParam[i];
                    switch (value) {
                        case 0:
                            DefaultAttr();
                            break;
                        case 1:
                            mIntensity = 2;
                            break;
                        case 2:
                            mIntensity = 0;
                            break;
                        case 4:  // underline
                            mUnderline = true;
                            break;
                        case 5:  // blinking
                            mBlink = true;
                            break;
                        case 7:  // reverse
                            mReverse = true;
                            break;
                        case 8:  // hidden
                            break;
                        case 10:
                            /* ANSI X3.64-1979 (SCO-ish?)
                             * Select primary font, don't display
                             * control chars if defined, don't set
                             * bit 8 on output.
                             */
                            mCharSet = PRIMARY;
                            break;
                        case 11:
                            /* ANSI X3.64-1979 (SCO-ish?)
                             * Select first alternate font, lets
                             * chars < 32 be displayed as ROM chars.
                             */
                            mCharSet = ALT1;
                            break;
                        case 12:
                            /* ANSI X3.64-1979 (SCO-ish?)
                             * Select second alternate font, toggle
                             * high bit before displaying as ROM char.
                             */
                            mCharSet = ALT2;
                            break;
                        case 21:
                        case 22:
                            mIntensity = 1;
                        case 24:
                            mUnderline = false;
                            break;
                        case 25:
                            mBlink = false;
                            break;
                        case 27:
                            mReverse = false;
                            break;
                        case 38:
                            /* ANSI X3.64-1979 (SCO-ish?)
                             * Enables underscore, white foreground
                             * with white underscore (Linux - use
                             * default foreground).
                             */
                            mColor = BuildColor(FgColor(mDefColor),BgColor(mColor));
                            mUnderline = true;
                            break;
                        case 39:
                            /* ANSI X3.64-1979 (SCO-ish?)
                             * Disable underline option.
                             * Reset colour to default? It did this
                             * before...
                             */
                            mColor = BuildColor(FgColor(mDefColor),BgColor(mColor));
                            mUnderline = false;
                            break;
                        case 49:
                            mColor = BuildColor(FgColor(mColor),BgColor(mDefColor));
                            break;
                        default:
                            //color
                            if (value >= 30 && value <= 37)
                                mColor = BuildColor(mColorTable[(value - 30)],
                                    BgColor(mColor));
                            else if (value >= 40 && value <= 47)
                                mColor = BuildColor(FgColor(mColor),
                                    mColorTable[(value - 40)]);
                            else {
                                //what to do?
                            }
                            break;
                    }//switch
                }//for
                UpdateAttr();
                break;
            case 'n':  // not implemented
                assert(!"n");
                break;
            case 'p':  // Define Key
                assert(!"Define Key not implemented yet!");
                break;
            case 'q':
                assert(!"esc q");
                break;
            case 'r':  // set scrolling range
                if (!mEscParam[0])
                    mEscParam[0]++;
                if (!mEscParam[1])
                    mEscParam[1] = mConMaxRows;
                /* Minimum allowed region is 2 lines */
                if (mEscParam[0] < mEscParam[1] &&
                    mEscParam[1] <= mConMaxRows) {
                    mScrollStart = mEscParam[0] - 1;
                    mScrollEnd = mEscParam[1];
                    AbsGoto(0, 0);
                }
                break;
            case 's':  // Save Cursor <ESC>[s
                SaveCursor();
                break;
            case 'u':  // Unsave Cursor <ESC>[u
                UnSaveCursor();
        } //switch
    }
    else if (mEscIntro == ']') {
        //unfinished
    }
    mEscState = NORMAL;
}

void Console::ParseEscape(char c) {
    switch (mEscState) {
        case ESC:
            mEscIntro = '\0';          //ESC seen
            mEscQuestion = false;
            mpEscParam = mEscParam;
            for (int i = 0; i < MAX_ESC_PARAMS; i++)
                mEscParam[i] = 0;
            switch (c) {
                case '#':  // DEC obsolete hacks
                    //anyone want to implement them?
                    break;
                case '[':              //control sequence introducer
                    mEscIntro = c;
                    mEscState = SQUARE;
                    break;
                case ']':
                    mEscIntro = c;
                    mEscState = NONSTD;
                    break;
                case '%':  // unfinished

                    break;
                case '(':              //Char Set G0
                case ')':              //Char Set G1
                    //unfinished
                    //charset change not implemented
                    break;
                default:
                    DoEscape(c);
            }                          //swicth
            break;
        case SQUARE:                   //ESC [ seen
            if (c == '?')
                mEscQuestion = true;
            else if (c >= '0' && c <= '9') {
                if (mpEscParam < mEscParam + MAX_ESC_PARAMS)
                    *mpEscParam = *mpEscParam * 10 + (c - '0');
            }
            else if (c == ';') {
                if (++mpEscParam < mEscParam + MAX_ESC_PARAMS)
                    *mpEscParam = 0;
            }
            else {
                DoEscape(c);
            }
            break;
        case NONSTD:
            DoEscape(c);
            break;
        default:
            break;
    }                                  //switch
}

//send c to buffer
void Console::SendChar(char c) {
    if (mHistMode) {
        mHistMode = false;
        //restore saved Buffer
        memcpy(mpText, mpSavTextBuf, mConMaxCols * mConMaxRows);
        memcpy(mpAttr, mpSavAttrBuf, mConMaxCols * mConMaxRows);
        ResetFlagAll();
        // Redraw(); // why need? called by write
    }

    if (mNeedWrap) {
        Cr();
        Lf();
    }
    assert(mInsertMode == false);
    if (mCharSet == PRIMARY) {
        // txtDblCode could be code1, code2, even ASCII
        // It will be identified in UpdateScreen()
        if (gpDecoder->IsCode1(c) || gpDecoder->IsCode2(c)) {
            PutChar(mAttr,txtDblCode | txtPrimary | txtUpdated,c);
        } else {
            PutChar(mAttr,txtASCII | txtPrimary | txtUpdated,c);
        }
    }
    else {  // always table line char
        PutChar(mAttr,txtASCII | txtUpdated,c);
    }

    //update cursor
    if (Col() == mConEndCol)
        mNeedWrap = mAutoWrap;
    else
        Goto(Col() + 1, Row());
}

//#include <sys/time.h>
//#include <sys/types.h>
//#include <unistd.h> //for debug
/** Write num chars in pBuf to console */
void Console::Write(const char *pBuf, int num) {
    SelClear(); // hide mouse
    bool isCursorOn = CursorOnOff();
    // turn off cursor for rapid display
    if (isCursorOn) CursorHide();

    char c;
    for (; num; pBuf++, num--) {
        c = *pBuf;
        //debug<<c<<flush;
        if (mEscState != NORMAL)
            ParseEscape(c);
        else if (c < ' ')
            DoControl(c);
        else {
            SendChar(c);
//for debug
//            Redraw();
//            fd_set set;
//            struct timeval tv;
//            FD_ZERO(&set);
//            tv.tv_sec = 0;
//            tv.tv_usec = 10000; // 0.1 sec
//            select(FD_SETSIZE, &set, NULL, NULL, &tv);
        }
    }
    Redraw();

    if (isCursorOn) CursorShow();
}

/** do ANSI control function  */
void Console::DoControl(char c) {
    switch (c) {
        case 0:
            return;
        case 7:
            Beep();
            return;
        case 8:
            //backspace
            if (Col()) {
                Goto(Col()-1, Row());
                mNeedWrap = false;
            }
            return;
        case 9:
            //tab
            while (Col() < mConEndCol) {
                Goto(Col() + 1, Row());
                if (mTabStop[Col() >> 5] & (1 << (Col() & 31)))
                    break;
            }
            return;
        case 10:                       //'\n'
        case 11:
        case 12:
            Lf();
            return;
        case 13:                       //'\r'
            Cr();
            return;
        case 14:
            //charset = 1;
            //translate = set_translate(G1_charset,currcons);
            //disp_ctrl = 1;
            return;
        case 15:
            //charset = 0;
            //translate = set_translate(G0_charset,currcons);
            //disp_ctrl = 0;
            return;
        case 24://graph arrows
        case 26:
        assert(mEscState == NORMAL);
        break;
        case 27: //ESC
            mEscState = ESC;
            return;
        case 127:
            //del(currcons);
            return;
        case 128 + 27:
            mEscState = SQUARE;
            assert(!"128+27");
            return;
        default:
            break;//assert(!"unknown control char reached!");
    }//switch
    //send out unknown control char
    SendChar(c);
}

//Copy lines from r1 to r2
void Console::CopyLines(int r1, int r2, int count) {
    assert(r1 >= 0 && r1 < mMaxRows && r2 >= 0 && r2 < mMaxRows);
    if (count == 0 || r1 == r2)
        return;

    int si, di;
    si = Index(0, r1);
    di = Index(0, r2);
    int n;
    if (r2 + count < mConMaxRows)
        n = count * mConMaxCols;
    else
        n = (mConMaxRows - r2) * mConMaxCols;

    memmove(&mpText[di], &mpText[si], n);
    memmove(&mpAttr[di], &mpAttr[si], n);
    memmove(&mpFlag[di], &mpFlag[si], n);
    for (int i = 0; i < n; i++)
        mpFlag[di + i] |= txtUpdated;
}

//insert n blank line at cursor
void Console::InsertLine(int n) {
    assert(Row() + 1 <= mScrollEnd && Row() >= mScrollStart);
    if (n < 1)
        n = 1;
    if (n > mScrollEnd - Row())
        n = mScrollEnd - Row();

    CopyLines(Row(), Row() + n, mScrollEnd - Row() - n);
    Clear(0, Row(), mConEndCol, Row() + n - 1, mAttr);
    mNeedWrap = false;
}

//delete n lines at cursor
//append blank lines
void Console::DeleteLine(int n) {
    assert(Row() + 1 <= mScrollEnd && Row() >= mScrollStart);

    if (n < 1)
        n = 1;
    if (n > mScrollEnd - Row())
        n = mScrollEnd - Row();

    CopyLines(Row() + n, Row(), mScrollEnd - Row() - n);
    Clear(0, mScrollEnd - n, mConEndCol, mScrollEnd - 1, mAttr);
    mNeedWrap = false;
}

//insert n blank char at current position
void Console::InsertBlank(int n) {
    if (n < 1)
        n = 1;
    if (Col() + n > mConEndCol)
        n = mConEndCol - Col();

    int count = mConEndCol - (Col() - 1) - n;
    for (int s = mConEndCol - n, d = mConEndCol; count--;)
        CopyChar(s--, Row(), d--, Row());
    Clear(Col(), Row(), Col() + n - 1, Row(), mAttr);
    mNeedWrap = false;
}

//delete n chars at current position
//append blank char at the end of line
void Console::DeleteChar(int n) {
    if (n < 1)
        n = 1;
    if (Col() + n > mConEndCol)
        n = mConEndCol - Col();

    int count = mConEndCol - (Col() - 1) - n;
    for (int d = Col(), s = d + n; count--;) {
        assert(s <= mEndCol);
        CopyChar(s++, Row(), d++, Row());
    }
    //append blank
    Clear(mConEndCol - n + 1, Row(), mConEndCol, Row(), mAttr);
    mNeedWrap = false;
}

//copy a char from (r1,c1) to (r2,c2)
void Console::CopyChar(int c1, int r1, int c2, int r2) {
    assert(r1 >= 0 && r1 <= mEndRow && r2 >= 0 && r2 <= mEndRow);
    int si, di;
    si = Index(c1, r1);
    di = Index(c2, r2);

    mpText[di] = mpText[si];
    mpAttr[di] = mpAttr[si];
    mpFlag[di] = mpFlag[si] | txtUpdated;
}

//scroll screen up or down one line
//in view: [mScrollStart,mScrollEnd)
void Console::ScrollScr(DIR dir) {
    assert(mScrollEnd > mScrollStart);

    int count = (mScrollEnd - mScrollStart - 1) * mConMaxCols;
    int si, di, blank;

    if (dir == UP) {
        si = (mScrollStart + 1) * mConMaxCols;
        di = si - mConMaxCols;
        blank = mScrollEnd - 1;
        PushHistoryLine();
    }
    else {
        si = mScrollStart * mConMaxCols;
        di = si + mConMaxCols;
        blank = mScrollStart;
    }

    memmove(&mpText[di], &mpText[si], count);
    memmove(&mpAttr[di], &mpAttr[si], count);
    memmove(&mpFlag[di], &mpFlag[si], count);
    for (int i = 0; i < count; i++)
        mpFlag[di + i] |= txtUpdated;
    Clear(0, blank, mConEndCol, blank, mAttr);
}

void Console::DefaultAttr() {
    mIntensity = 1;
    mBlink = false;
    mUnderline = false;
    mReverse = false;
    mColor = mDefColor;
}

void Console::Reset() {
    DefaultAttr();
    UpdateAttr();
    mTabStop[0] = 0x01010100;
    mTabStop[1] = mTabStop[2] = mTabStop[3] = mTabStop[4] = 0x01010101;
    mEscQuestion = false;
    mNeedWrap = false;
    mCharSet = PRIMARY;
    mInsertMode = false;
    mDecom = false;
    mAutoWrap = true;
    mScrollStart = 0;
    mScrollEnd = mConMaxRows;

    int size = mConMaxCols * mConMaxRows;
    memset(mpText, ' ', size);
    memset(mpAttr, mAttr, size);
    memset(mpFlag, txtASCII | txtUpdated, size);
    ConGoto(0, 0);
}

void Console::SaveCursor() {
    mOldCol = Col();
    mOldRow = Row();
    mOldColor = mColor;
    mOldBlink = mBlink;
    mOldUnderline = mUnderline;
    mOldIntensity = mIntensity;
    mOldReverse = mReverse;
    mOldCharSet = mCharSet;
}

void Console::UnSaveCursor() {
    ConGoto(mOldCol, mOldRow);
    mColor = mOldColor;
    mBlink = mOldBlink;
    mUnderline = mOldUnderline;
    mBold = mOldBold;
    mReverse = mOldReverse;
    mCharSet = mOldCharSet;
    UpdateAttr();
    mNeedWrap = false;
}

void Console::SetMode(bool f) {
    if (mEscQuestion)
    switch (mEscParam[0]) {
        /* DEC private modes set/reset */
        case 1:                        /* Cursor keys send ^[Ox/^[[x */
            assert(!"set_dec_cursor_keys(tty, on_off)");
            break;
        case 3:                        /* 80/132 mode switch unimplemented */
            break;
        case 4:
            /* soft scroll/Jump scroll toggle, unimplemented yet */
            break;
        case 5:                        /* Inverted screen on/off */
            //not implemented
            break;
        case 6:
            mDecom = f;
            AbsGoto(0, 0);
        case 7:                        /* Autowrap on/off */
            mAutoWrap = f;
        case 8:                        /* Autorepeat on/off not implemented yet */
            break;
        case 25:                       /* Cursor on/off */
            //deccm = on_off;
            if (f)
                CursorShow();
            else
                CursorHide();
            break;
    }
    else
    switch (mEscParam[0]) {
        /* ANSI modes set/reset */
        case 4:                        /* Insert Mode on/off */
            mInsertMode = f;
            //decim = f;
            break;
        case 20:                       /* Lf, Enter == CrLf/Lf */
            //          set_lf_mode(tty, on_off);
            assert(!"Lf, Enter == CrLf/Lf");
            break;
    }
}

void Console::ConGoto(int c, int r) {
    int col, row;
    if (c < 0)
        col = 0;
    else if (c > mConEndCol)
        col = mConEndCol;
    else
        col = c;

    int min_y, max_y;

    if (mDecom) {
        min_y = mScrollStart;
        max_y = mScrollEnd;
    }
    else {
        min_y = 0;
        max_y = mConMaxRows;
    }
    if (r < min_y)
        row = min_y;
    else if (r >= max_y)
        row = max_y - 1;
    else
        row = r;
    Goto(col, row);
    mNeedWrap = false;
}

//for absolute user moves, when decom is set
void Console::AbsGoto(int c, int r) {
    ConGoto(c, mDecom ? (mScrollStart + r) : r);
}

void Console::Lf() {
    /* don't scroll if above bottom of scrolling region, or
     * if below scrolling region
     */
    if (Row() + 1 == mScrollEnd)
        ScrollScr(UP);
    else if (Row() < mConEndRow)
        Goto(Col(), Row() + 1);
    mNeedWrap = false;
}

void Console::Cr() {
    Goto(0, Row());
    mNeedWrap = false;
}

//push top line into history buffers
void Console::PushHistoryLine() {
    int s = mConMaxCols;
    memcpy(mpHistText + mHistCurRow * s, mpText, s);
    memcpy(mpHistAttr + mHistCurRow * s, mpAttr, s);
    memcpy(mpHistFlag + mHistCurRow * s, mpFlag, s);
    mHistCurRow = (mHistCurRow + 1) % HISTORY_LINES;
}

//scroll into history buffer and switch to history mode if necessary
void Console::ScrollDelta(ScrollFlag f) {
    SelClear();
    static int sBackOffset = 0;
    if (!mHistMode) {
        //history mode will be closed in PutChar()
        sBackOffset = 0;
        mHistMode = true;
        //save screen buffer
        memcpy(mpSavTextBuf, mpText, mConMaxCols * mConMaxRows);
        memcpy(mpSavAttrBuf, mpAttr, mConMaxCols * mConMaxRows);
        memcpy(mpSavFlagBuf, mpFlag, mConMaxCols * mConMaxRows);
    }
    switch (f) {
        case PAGE_UP:
            sBackOffset += mConMaxRows / 2;
            break;
        case PAGE_DOWN:
            sBackOffset -= mConMaxRows / 2;
            break;
        case LINE_UP:
            sBackOffset++;
            break;
        case LINE_DOWN:
            sBackOffset--;
            break;
    }
    if (sBackOffset >= HISTORY_LINES)
        sBackOffset = HISTORY_LINES - 1;
    if (sBackOffset < 0)
        sBackOffset = 0;
    ShowHistory(sBackOffset);
}

void Console::ShowHistory(int offset) {
    int row, indexCol, histrow;
    // draw history row
    for (row = 0; row < mConMaxRows && offset; row++, offset--) {
        histrow = mHistCurRow - offset;
        if (histrow < 0)
            histrow += HISTORY_LINES;

        indexCol = Index(0, histrow);
        //copy one row from history buffer
        memcpy(mpText + Index(0, row),mpHistText + indexCol,mConMaxCols);
        memcpy(mpAttr + Index(0, row),mpHistAttr + indexCol,mConMaxCols);
        memcpy(mpFlag + Index(0, row),mpHistFlag + indexCol,mConMaxCols);
        ResetFlagRow(mpText + row * mConMaxCols,mpFlag + row * mConMaxCols,mConMaxCols);
    }
    //remain rows copied from screen buffer
    for (int r = 0; row < mConMaxRows; row++, r++) {
        memcpy(mpText + Index(0, row), mpSavTextBuf + Index(0, r),mConMaxCols);
        memcpy(mpAttr + Index(0, row), mpSavAttrBuf + Index(0, r),mConMaxCols);
        memcpy(mpFlag + Index(0, row), mpSavFlagBuf + Index(0, r),mConMaxCols);
        ResetFlagRow(mpText + row * mConMaxCols,mpFlag + row * mConMaxCols,mConMaxCols);
//        DrawRow(row, &mpText[indexCol], &mpAttr[indexCol],
//            &mpFlag[indexCol], true);
    }

    // turn off cursor for rapid display
    bool isCursorOn = CursorOnOff();
    if (isCursorOn) CursorHide();
    Redraw();
    if (isCursorOn) CursorShow();
}

//modified from Yu Guanghui's auto-converter(judge.c)
Encode Console::DetectBufferEncode() {
    char *phz;
    int c_gb = 0;
    int c_big5 = 0;

    /* first we look up "我"  and "的" ,both gb and big5
     * in the text.
     */
    for (phz = mpText; phz < (mpText + mConMaxRows * mConMaxCols);
    phz++) {
        if (*phz & 0x80) {
            if ((*phz == 0xB5 && *(phz + 1) == 0xC4)
            || ((*phz == 0xCE) && *(phz + 1) == 0xD2)) {
                c_gb++;
                phz++;
                continue;
            } else if ((*phz == 0xAA && *(phz + 1) == 0xBA)
            || ((*phz == 0xA7) && *(phz + 1) == 0xDA)) {
                c_big5++;
                phz++;
                continue;
            }
            phz++;
        }
    }

    if (c_gb > c_big5) {
        return GB2312;
    }
    else if (c_gb == c_big5) {
        //c_gb == 0,c_big5==0
        /*There is not "我" and "的" in the text
         *So we test the text with a 400 words table.
         */
        //unable to detect so return ascii
        return ::ASCII;                //j_code3(buff,count);
    }
    else {
        return BIG5;
    }
}

void Console::VtSizeDelta(int ColDelta, int RowDelta) {
    SelClear();
    int NewRows, NewEndRow;
    NewRows = mMaxRows - RowDelta;
    NewEndRow = NewRows - 1;

    if (Row() > NewEndRow) {
        //CopyLines(Row(), NewEndRow, 1);
        Goto(Col(), NewEndRow);
    }
    if (RowDelta > 0) {
        //debug<<"Clear "<<NewRows<<"-"<<mConEndRow<<endl;
        Clear(0, NewRows, mConEndCol, mConEndRow, 0);
    }
    Redraw();

    mConMaxRows = NewRows;
    mConEndRow = NewEndRow;
    mScrollEnd = mConMaxRows;
}

void Console::GetVtSize(int &cols, int &rows) {
    cols = mConMaxCols;
    rows = mConMaxRows;
}

void Console::UpdateAttr(){
    mAttr = mColor;
    if (mUnderline)
        mAttr = (mAttr & 0xf0) | mUlColor;
    else if (mIntensity == 0)
        mAttr = (mAttr & 0xf0) | mHalfColor;
    if (mReverse)
        mAttr = ((mAttr) & 0x88) | ((((mAttr) >> 4) | ((mAttr) << 4)) & 0x77);
    if (mIntensity == 2)
        mAttr |= 0x08;
    if (mBlink)
        mAttr |= 0x80;
}

/* use complementary color to show the mouse pointer */
void Console::SelPointer(const int offset) {
    if (mMouseIdx >= 0 && mMouseIdx < mMaxCols * mMaxRows) {
        mpAttr[mMouseIdx] ^= mMouseMask;
        mpFlag[mMouseIdx] |= txtUpdated;

        // Redraw();
        RedrawChar(mMouseIdx % mMaxCols, mMouseIdx / mMaxCols);
    }

    if (offset < 0) {
        mMouseIdx = -1;
        return;
    }

    mMouseIdx = offset;
    if (mMouseIdx > mMaxCols * mMaxRows - 1) {
        mMouseIdx = mMaxCols * mMaxRows - 1;
    }
    
    mpAttr[mMouseIdx] ^= mMouseMask;
    mpFlag[mMouseIdx] |= txtUpdated;

    // Redraw();
    RedrawChar(mMouseIdx % mMaxCols, mMouseIdx / mMaxCols);
}

/* set reverse video on characters s-e of console with selection. */
void Console::SelHighlight(const int begin, const int end) {
    int count = end - begin + 1;
    char* pAttr = &mpAttr[begin];
    char* pFlag = &mpFlag[begin];
    
    char color;
    while (count--) {
         color = *pAttr;
         //color = ((color) & 0x11) | (((color) & 0xe0) >> 4) | (((color) & 0x0e) << 4);
         color = ((color) & 0x88) | (((color) & 0x70) >> 4) | (((color) & 0x07) << 4);
         *pAttr = color;
         pAttr++;
         (*pFlag) |= txtUpdated;
         pFlag++;
         
    }

    // turn off cursor for rapid display
    bool isCursorOn = CursorOnOff();
    if (isCursorOn) CursorHide();
    Redraw();
    if (isCursorOn) CursorShow();
    /*
    int count = end - beign + 2;
    unsigned short *p;

    count /= 2;
    p = screenpos(currcons, beign, viewed);
    u16 *q = p;
    int cnt = count;

    if (!can_do_color) {
        while (cnt--) *q++ ^= 0x0800;
    } else if (hi_font_mask == 0x100) {
        while (cnt--) {
            u16 a = *q;
            a = ((a) & 0x11ff) | (((a) & 0xe000) >> 4) | (((a) & 0x0e00) << 4);
            *q++ = a;
        }
    } else {
        while (cnt--) {
            u16 a = *q;
            a = ((a) & 0x88ff) | (((a) & 0x7000) >> 4) | (((a) & 0x0700) << 4);
            *q++ = a;
        }
    }
    if (DO_UPDATE)
        do_update_region(currcons, (unsigned long) p, count);
    */
}

/* remove the current selection highlight, if any,
   from the console holding the selection. */
void Console::SelClear() {
    SelPointer(-1); /* hide the pointer */
    if (mSelStart != -1) {
        SelHighlight(mSelStart, mSelEnd);
        mSelStart = -1;
    }
}

// User settable table: what characters are to be considered alphabetic?
// 256 bits
__u32 Console::mInWordLut[8] = {
  0x00000000, // control chars
  0x03FF0000, // digits
  0x87FFFFFE, // uppercase and '_'
  0x07FFFFFE, // lowercase
  0x00000000,
  0x00000000,
  0xFF7FFFFF, // latin-1 accented letters, not multiplication sign
  0xFF7FFFFF  // latin-1 accented letters, not division sign
};

int Console::InWord(const unsigned char c) {
    return ( mInWordLut[c>>5] >> (c & 0x1F) ) & 1;
}

// does screen address p correspond to character at LH/RH edge of screen?
int Console::AtColEdge(const int p)
{
    return (!(p % mMaxCols) || !((p + 1) % mMaxCols));
}

// Don't take this from <ctype.h>: 011-015 on the screen aren't spaces
#define IS_SPACE(c)  ((c) == ' ')
#define IS_ASCII(c)  ((c) & txtASCII)

// based on kernel, linux/drivers/char/selection.c
void Console::SelCopy(int c1, int r1, int c2, int r2, int mode) {
    int xs, ys, xe, ye, ps, pe;
    xs = CorrectCol(c1);
    ys = CorrectRow(r1);
    xe = CorrectCol(c2);
    ye = CorrectRow(r2);
    ps = ys * mMaxCols + xs;
    pe = ye * mMaxCols + xe;
    //debug<<"Mouse " <<xs<<","<<ys<<" "<<xe<<","<<ye<<" "
    //    <<ps<<","<<pe<<" "<<endl;

    if (mode == 4) {  // useful for screendump without selection highlights
        SelClear();
        return;
    }

    if (ps > pe)    // make exchange if sel_start > sel_end
    {
        int tmp = ps;
        ps = pe;
        pe = tmp;
    }

    int new_sel_start, new_sel_end;
    bool spc, ascii;
    char *bp, *obp;
    int i;

    switch (mode)
    {
        case 0: // character-by-character selection
            new_sel_start = ps;
            new_sel_end = pe;
            break;
        case 1: // word-by-word selection
            spc = IS_SPACE(mpText[ps]);
            ascii = IS_ASCII(mpFlag[ps]);
            for (new_sel_start = ps; ; ps--)
            {
                if (ascii) {
                    if (spc && !IS_SPACE(mpText[ps]))
                        break;
                    if (!spc && !InWord(mpText[ps]))
                        break;
                } else {
                    if (IS_ASCII(mpFlag[ps]))
                        break;
                }
                new_sel_start = ps;
                if (!(ps % mMaxCols))
                    break;
            }
            spc = IS_SPACE(mpText[pe]);
            ascii = IS_ASCII(mpFlag[pe]);
            for (new_sel_end = pe; ; pe++)
            {
                if (ascii) {
                    if (spc && !IS_SPACE(mpText[pe]))
                        break;
                    if (!spc && !InWord(mpText[pe]))
                        break;
                } else {
                    if (IS_ASCII(mpFlag[pe]))
                        break;
                }
                new_sel_end = pe;
                if (!((pe+1) % mMaxCols))
                    break;
            }
            break;
        case 2: // line-by-line selection
            new_sel_start = ps - ps % mMaxCols;
            new_sel_end = pe + mMaxCols - pe % mMaxCols - 1;
            break;
        case 3:
            SelPointer(pe);
            return;
        default:
            return;
    }

    // remove the pointer
    SelPointer(-1);

    // select to end of line if on trailing space
    if (new_sel_end > new_sel_start &&
        !AtColEdge(new_sel_end) &&
        IS_SPACE(mpText[new_sel_end])) {
        for (pe = new_sel_end + 1; ; pe++)
            if (!IS_SPACE(mpText[pe]) ||
                AtColEdge(pe))
                break;
        if (IS_SPACE(mpText[pe]))
            new_sel_end = pe;
    }
    if (mSelStart == -1)    // no current selection
        SelHighlight(new_sel_start, new_sel_end);
    else if (new_sel_start == mSelStart)
    {
        if (new_sel_end == mSelEnd) // no action required
            return;
        else if (new_sel_end > mSelEnd) // extend to right
            SelHighlight(mSelEnd + 1, new_sel_end);
        else                // contract from right
            SelHighlight(new_sel_end + 1, mSelEnd);
    }
    else if (new_sel_end == mSelEnd)
    {
        if (new_sel_start < mSelStart)  // extend to left
            SelHighlight(new_sel_start, mSelStart - 1);
        else                // contract from left
            SelHighlight(mSelStart, new_sel_start - 1);
    }
    else    // some other case; start selection from scratch
    {
        SelClear();
        SelHighlight(new_sel_start, new_sel_end);
    }
    mSelStart = new_sel_start;
    mSelEnd = new_sel_end;

    // Allocate a new buffer before freeing the old one ...
    bp = new char[mSelEnd-mSelStart+1];
    if (!bp) { // selection: kmalloc() failed
        SelClear();
        return;
    }
    if (mpSelBuf)
        delete[] mpSelBuf;
    mpSelBuf = bp;

    obp = bp;
    for (i = mSelStart; i <= mSelEnd; i++) {
        *bp = mpText[i];
        if (!IS_SPACE(*bp++))
            obp = bp;
        if (! ((i + 1) % mMaxCols)) {
            // strip trailing blanks from line and add newline,
            //   unless non-space at end of line.
            if (obp != bp) {
                bp = obp;
                *bp++ = '\r';
            }
            obp = bp;
        }
    }
    mSelBufLen = bp - mpSelBuf;
}

void Console::SelPaste(int fd) {
    assert(fd >= 0);
    int pasted = 0, count;
    while (mpSelBuf && (mSelBufLen > pasted)) {
        count = mSelBufLen - pasted;
        pasted = write(fd, mpSelBuf, count);
        pasted += count;
    }
}

