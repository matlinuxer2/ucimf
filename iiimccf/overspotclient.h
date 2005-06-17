// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          overspotclient.h  -  description
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

#ifndef OVERSPOTCLIENT_H
#define OVERSPOTCLIENT_H

#include <inputclient.h>
#include <window.h>
#include <string>



/**
  *@author ejoy
  */

struct Candilist {
public:
    Candilist();
    ~Candilist();
    void Reset();

    struct {
        std::string mText;
	char mKey;
    }
    mList[10];
    int mCount;
    bool mHavePrev;
    bool mHaveNext;
};



class OverSpotClient : public Window,public InputClient {
    public:
        OverSpotClient(string& sColors);
        ~OverSpotClient();
        void Update();
        void Hide();
        void Show();
        void VtSizeDelta(int &ColDelta, int &RowDelta);
        void Connect(InputServer *pServer);
    private:
        string mTitle;
        int mMinCols;
        bool mTitleUpside;
        void PutTitle();
        void PutList();
        string GetPromptStr();
        bool AdjustWinPos(int row, int col, int maxRows, int maxCols);
        void Draw();
        void MoveToConner();
        Candilist mList;
};

#endif
