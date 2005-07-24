// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          inputclient.h  -  description
                             -------------------
    begin                : Sun Sep 9 2001
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

#ifndef INPUTCLIENT_H
#define INPUTCLIENT_H

#include <string>
#include "console.h"

/**
  *@author ejoy
  */
using namespace std;

class InputClient {
    public:
        InputClient();
        virtual ~InputClient();
        static void SetConsole(Console* pCon) {
            assert(pCon);
            mpCon = pCon;
        }

        virtual void Update() = 0;
        virtual void Hide() = 0;
        virtual void Show() = 0;
        virtual void VtSizeDelta(int &ColDelta, int &RowDelta) = 0;
        bool Visible() { return mVisible; }
    protected:
        void SetColor(string & s);
        static Console* mpCon;

        int mFgColor, mBgColor, mColor1, mColor2;
        int mPixLight, mPixDark;
        bool mVisible;
};

#endif
