/*                                                                                                  
 * dummy.cpp : Implementation of dummy input method
 *
 * Copyright (c) 2006  Chun-Yu Lee (Mat) <Matlinuxer2@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in 
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of UCIMF nor the names of its contributors
 *    may be used to endorse or promote products derived from this 
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE.
 */

#include "dummy.h"
#include <imf/widget.h>
#include <config.h>

using namespace std;

void DummyImf::refresh()
{
  Status* stts = Status::getInstance();
  LookupChoice* lkc = LookupChoice::getInstance();

  stts->set_imf_status(const_cast<char*>("UCIMF"),const_cast<char*>(VERSION),const_cast<char*>("") );

  lkc->clear();
  lkc->append_next( const_cast<char*>("Note::") );
  lkc->append_next( const_cast<char*>("  <Ctrl+Space>: toggle IM on/off") );
  lkc->append_next( const_cast<char*>("  <Ctrl+RightShift>: Switch IME forward") );
  lkc->append_next( const_cast<char*>("  <Ctrl+LeftShift>: Switch IME backward") );
  lkc->append_next( const_cast<char*>("  <F9>: Switch IMF") );
  lkc->append_next( const_cast<char*>("") );
  lkc->append_next( const_cast<char*>("Press <F9> to continue...") );
  lkc->render();
}

string DummyImf::process_input( const string& input)
{
  string result = input;
  return result;
}

void DummyImf::switch_im()
{
  return;
}
