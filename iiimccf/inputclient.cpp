// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          inputclient.cpp  -  description
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

#include "inputclient.h"

Console* InputClient::mpCon = NULL;
InputClient::InputClient()
: mVisible(false),
mpServer(0) {
    assert(mpCon);
}

InputClient::~InputClient() {}

void InputClient::SetColor(string & s) {
    unsigned int ParmNo, Pos;
    unsigned int NextPos;
    int iVal;
    string sVal;
    // debug << "inputcolor define " << s << endl;
    ParmNo = 0;
    Pos = 0;
    while ( Pos < s.length() ) {
        NextPos = s.find(',', Pos);
        if (NextPos == string::npos)
            NextPos = s.length();
        sVal = s.substr(Pos,NextPos - Pos);
        // debug << atoi(sVal.c_str()) << endl;
        iVal = atoi(sVal.c_str());
        iVal %= 16;
        switch (ParmNo) {
            case 0:
                mFgColor = iVal;
                break;
            case 1:
                mBgColor = iVal;
                break;
            case 2:
                mColor1 = iVal;
                break;
            case 3:
                mColor2 = iVal;
                break;
            case 4:
                mPixLight = iVal;
                break;
            case 5:
                mPixDark = iVal;
                break;
            default:
                break;
        }
        ParmNo++;
        Pos = NextPos + 1;
    }
    // debug << mFgColor << mBgColor << mColor1 << mColor2 << endl;
    return;
}

