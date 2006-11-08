/*
 * UCIMF - Unicode Console InputMethod Framework
 * Copyright (C) <2006>  Chun-Yu Lee (Mat) <Matlinuxer2@gmail.com> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <iiimcf.h>
#include "imf.h"
#include <string>
using std::string;


class IIIMCCF : public Imf {
  public:
    static Imf* getInstance();

    string process_input( const string& input );
    void switch_lang();
    void switch_im();
    void switch_im_per_lang();

  protected:
    IIIMF_status dispatch_event(IIIMCF_context context, IIIMCF_event event);
    IIIMF_status iiimccf_preedit(IIIMCF_context context, IIIMCF_event event);
    IIIMF_status iiimccf_lookup_choice(IIIMCF_context context, IIIMCF_event event);
    IIIMF_status iiimccf_commit(IIIMCF_context context, IIIMCF_event event);
    IIIMF_status iiimccf_status(IIIMCF_context context, IIIMCF_event event);
    
  protected:
    IIIMCCF();
    ~IIIMCCF();
    static Imf* _instance;
    
    // IIIMCF Object
    IIIMCF_handle 	handle;
    IIIMCF_context 	context;
    
    /* committed text */
    string cmt_buf;

    /* for input methods */
    int cur_ims_id; // current imput method id 
    int cur_pos; // current preedit position 
    int cur_idx; // current lookupchoice index 
    int num_of_ims; // number of ims

    IIIMCF_input_method *pims; // set of input methods.
    
};

