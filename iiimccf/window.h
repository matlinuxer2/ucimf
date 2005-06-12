// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          window.h  -  description
                             -------------------
    begin                : Fri July 20 2001
    copyright            : (C) 2001 by huyong,ejoy
    email                : ccpaging@online.sh.cn
                           ejoy@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

// text mask
const unsigned char
    txtUpdated = 1,
    txtASCII = 2,
    txtDblCode = 4,
    txtDblCode1 = 8,
    txtDblCode2 = 16,
    txtPrimary = 32;

class Window;

#define CUR_DEF         0
#define CUR_NONE        1
#define CUR_UNDERLINE   2
#define CUR_LOWER_THIRD 3
#define CUR_LOWER_HALF  4
#define CUR_TWO_THIRDS  5
#define CUR_BLOCK       6
#define CUR_DEFAULT CUR_UNDERLINE

#define WS_DEF          0
#define WS_CHILD        1
#define WS_FRAMETHIN    2
#define WS_FRAMETHICK   4

class Window {
    public:
        Window(int x1, int y1, int x2, int y2, int type = WS_DEF);
        virtual ~Window();

        void SetFrameColor(int light, int dark) {
            mFrameLight = light;
            mFrameDark = dark;
        }
        void SetCursorType(int CurType);
        void SetFgColor(int color) { mFgColor = color; }
        void SetBgColor(int color) { mBgColor = color; }
        int  Col() { return mCol; }
        int  Row() { return mRow; }
        void Goto(int c, int r);

        bool Visible() { return mVisible; }
        void Clear();
        void Clear(int c1, int r1, int c2, int r2, char attr);
        void Show();
        void Hide();
        int GetX1() { return mX1; }
        int GetY1() { return mY1; }
        void MoveTo(int x, int y);
        
        void CursorShow();   // turn cursor on
        void CursorHide();  // turn cursor off
        bool CursorOnOff() { return mCursorEnabled; }
        void CursorBlink();

        int Width() { return mX2-mX1+1; }
        int Height() { return mY2-mY1+1; }
        int MaxCols() { return mMaxCols; }
        int MaxRows() { return mMaxRows; }
        int CorrectCol(int c) {
            if (c < 0) return 0;
            if (c > mEndCol) return mEndCol;
            return c;
        }
        int CorrectRow(int r) {
            if (r < 0) return 0;
            if (r > mEndRow) return mEndRow;
            return r;
        }
        int ColsOvered();
        int RowsOvered();
        
        void ResetFlagAll();
        void Redraw(bool isUpdateAll = false);

        void DrawColorStr(int col,int row,string s);
        void PutStr(int col,int row,string s);
        void PutStr(int col,int row,string s,char fg);

    protected:
        int Index(int c, int r);
        void ResetFlagRow(char *pText, char *pFlag,int len);
        void RedrawRow(int row, char *pText, char *pAttr,
            char *pFlag, bool isUpdateAll);
        void RedrawChar(int col, int row);

        void PutChar(char attr,char flag,char c1,char c2);
        void PutChar(char attr,char flag,char c);
        void UpdateBlinkAttr(bool show);

        // set update and overlap flag for popwin's show/hide, by pixel
        void SetOverlaped(int x1, int y1, int x2, int y2, bool isOvered);
        void Resize(int w,int h);
        int mMaxCols, mMaxRows;  // lines of cols and rows
        int mEndCol, mEndRow;   // last line
        // screen buffers
        char *mpText;                  // Text Buffer
        char *mpAttr;                  // Attr Buffer
        char *mpFlag;
    private:
        void SetBuf();
        void OutChar(int col, int row, int fg, int bg, char c);
        void OutChar(int col, int row, int fg, int bg, char c1, char c2);

        int mType;
        // cursor position
        int mCol, mRow;

        bool mVisible;

        int mX1, mX2, mY1, mY2; // console position in pixels
        int mTextX0, mTextY0; // relate pixel postion of text start

        bool mCursorEnabled, mCursorVisible;
        unsigned mCursorIntevel;  //intevel in microseconds
        int mCursorStart, mCursorEnd;
        // inner used when text/cursor change
        void CursorSet();
        void CursorRestore();
        void CursorDraw();

        int mFrameLight, mFrameDark; // frame color

        char mFgColor, mBgColor;

        static Window* mpConsole;
        // static PopWin* mPopWinList[2];
        // not merge in mpFlag because scroll easy
        static char* mpOverlaps;
};

#endif

