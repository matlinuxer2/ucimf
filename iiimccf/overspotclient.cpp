// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          overspotclient.cpp  -  description
                             -------------------
    begin                : Mon Sep 10 2001
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

#include "global.h"
#include "console.h"
#include "inputserver.h"
#include "overspotclient.h"
#include "zhcon.h"

OverSpotClient::OverSpotClient(string& sColors)
:Window(0, 0, 22 * gpScreen->BlockWidth() - 1,
    2 * gpScreen->BlockHeight() - 1,
    WS_CHILD | WS_FRAMETHICK),
InputClient()
{
    mMinCols = mMaxCols;
    mTitleUpside = true;
    SetColor(sColors);
    SetFgColor(InputClient::mFgColor);
    SetBgColor(InputClient::mBgColor);
    SetFrameColor(InputClient::mPixLight, InputClient::mPixDark);
}

OverSpotClient::~OverSpotClient() {}


void OverSpotClient::Show() {
    Draw();
    InputClient::mVisible = true;
}

void OverSpotClient::Hide() {
    Window::Hide();
    InputClient::mVisible = false;
}

//update window's content
//only update display when visible
void OverSpotClient::Update() {
    if (mpServer == NULL) {
        //prompt mode
        mTitle = GetPromptStr();
        mList.Reset();
        MoveToConner();
    } else {
        //input mode
        mpServer->GetCandilist(mList);

        if (mList.mCount == 0) {
            mTitle = _("[");
            mTitle += (mpServer->IsFullChar() ? _("Full") : _("Half"));
            mTitle += (mpServer->IsFullComma() ? _("Ch") : _("En"));
            mTitle += _("]") + (mpServer->GetImmInfo()).mName;
        } else {
            mTitle = _("[") + (mpServer->GetImmInfo()).mName + _("]");
        }

        char buf[64];
        buf[0] = '\0';
        mpServer->GetInputBuf(buf, 63);
        mTitle += buf;

        AdjustWinPos(mpCon->Row(), mpCon->Col(),
                     mpCon->MaxRows(), mpCon->MaxCols());
    }
    if (!InputClient::mVisible) return ;
    Draw();
}

// adjust overspotwin's position&size according to
// cursor position at (row,col)
// when cursor is moving, the pos is previous.
// resize if needed
bool OverSpotClient::AdjustWinPos(int CursorRow, int CursorCol, int MaxRows, int MaxCols) {
    //adjust window size
    int rows, cols;
    if (!mList.mCount)
        rows = 1;
    else
        rows = 11;
    cols = mTitle.size();
    for (int i = 0; i < mList.mCount; i++)
        if (mList.mList[i].mText.size() + 2 > (size_t)cols)
            cols = mList.mList[i].mText.size() + 2;
    if (cols < mMinCols)
        cols = mMinCols;
    else if (cols > 50) //50 cols is wide enough I think
        cols = 50;
    bool resized = false;
    if (mMaxCols != cols || mMaxRows != rows) {
        //need resize first
        Window::Hide();
        MoveTo(0, 0);   //to ensure Resize() successful
        Resize((cols + 1)*gpScreen->BlockWidth(),
               (rows + 1)*gpScreen->BlockHeight());
        resized = true;
    }

    cols = ColsOvered();
    rows = RowsOvered();
    int r, c;
    r = CursorRow + 2;
    if (r + 11 >= MaxRows) {
        // out of bottom, set win up than cursor
        r = CursorRow - rows - 1;
        mTitleUpside = false;
    } else
        mTitleUpside = true;

    c = CursorCol;
    if (c + cols - 1 >= MaxCols) // out of right
        c = MaxCols - cols;
    //check whether position need not change
    int ConCol, ConRow;
    ConCol = GetX1() / gpScreen->BlockWidth();
    ConRow = GetY1() / gpScreen->BlockHeight();
    if (!resized && r == ConRow && c >= ConCol - 3 && c <= ConCol + 6)
        return false;

    MoveTo(c * gpScreen->BlockWidth(), r * gpScreen->BlockHeight());
    return true;
}

void OverSpotClient::PutList() {
    if (!mList.mCount) return ;
    int col;
    string str;
    //draw candilist
    int row,end_row,step;
    if (mTitleUpside) {
        row = 1;
        end_row = 11;
        step = 1;
    } else {
        row = 9;
        end_row = -1;
        step = -1;
    }
    for (int i = 0; row != end_row; row += step, i++) {
        if (i >= mList.mCount) { //append blank line
            PutStr(0, row, str.assign(mMaxCols, ' '));
            continue;
        }
        col = 0;

        str = ((i == 9) ? '0' : '1' + i);
        str += '.';
        PutStr(0, row, str, mColor2);
        col += 2;

        str = mList.mList[i].mText;
        PutStr(col, row, str);
        col += str.size();

        if (mList.mList[i].mKey != '\0' && col < mMaxCols) {
            str = mList.mList[i].mKey;
            PutStr(col, row, str, mColor1);
            col++;
        }
        if (col < mMaxCols) {
            str.assign(mMaxCols - col, ' ');
            PutStr(col, row, str);
        }
    }
}

void OverSpotClient::PutTitle() {
    mTitle.resize(mMaxCols, ' ');
    if (!mTitleUpside && mList.mCount)
        PutStr(0, 10, mTitle, InputClient::mColor1);
    else
        PutStr(0, 0, mTitle, InputClient::mColor1);
}

void OverSpotClient::Draw() {
    PutTitle();
    PutList();

    if (!Window::Visible())
        Window::Show();
    else
        Window::Redraw();
}

void OverSpotClient::VtSizeDelta(int &ColDelta, int &RowDelta) {
    ColDelta = 0;
    RowDelta = 0;
}

void OverSpotClient::Connect(InputServer *pServer) {
    mpServer = pServer;
    //overspot client has no buf size limit
    pServer->SetClientBufLen(100);
}

string OverSpotClient::GetPromptStr() {
    string s;
    s = _("[");
    s += gpZhcon->GetEncode();
    s += _("]");
    return s;
}

//resize client to corner in prompt mode
void OverSpotClient::MoveToConner() {
    Window::Hide();
    MoveTo(0, 0);   //to ensure Resize() successful
    int rows, cols;
    cols = mTitle.size();
    rows = 1;
    Resize((cols + 1)*gpScreen->BlockWidth(),
           (rows + 1)*gpScreen->BlockHeight());
    int c = mpCon->MaxCols() - cols - 2;
    int r = mpCon->MaxRows() - rows - 2;
    int CursorRow = mpCon->Row();
    if (CursorRow < mpCon->MaxRows() - rows - 3)
        MoveTo(c * gpScreen->BlockWidth(), r * gpScreen->BlockHeight());
    else
        MoveTo(c * gpScreen->BlockWidth(), 0);
}
