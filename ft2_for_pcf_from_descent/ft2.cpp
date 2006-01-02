/*
 * 用 ft2 來 render taipei16.pcf
 * 由於我不知道如何將 big5 -> glyph index,
 * 所以無法針對 big5 來 render 字
 *
 */

#include <ft2build.h>
#include FT_FREETYPE_H


#include <iostream>

using namespace std;

void err_msg(const char *msg)
{
  cout << msg << endl;
  exit(-1);
}

int print_face_info(const FT_Face &face)
{
  cout << "num_glyphs: " << face->num_glyphs << endl;
  cout << "units_per_EM: " << face->units_per_EM << endl;
  if (face->face_flags!=FT_FACE_FLAG_SCALABLE)
    cout << "not scalable" << endl;
  else
    cout << "scalable" << endl;
  cout << "num_fixed_sizes: " << face->num_fixed_sizes << endl;
  cout << "size: x_ppem:" << face->size->metrics.x_ppem << endl;
  cout << "size: y_ppem:" << face->size->metrics.y_ppem << endl;
  cout << "size: x_scale:" << face->size->metrics.x_scale << endl;
  cout << "size: y_scale:" << face->size->metrics.y_scale << endl;
  cout << "size: ascender:" << face->size->metrics.ascender << endl;
  cout << "size: descender:" << face->size->metrics.descender << endl;
  cout << "size: height:" << face->size->metrics.height << endl;
  cout << "size: max_advance:" << face->size->metrics.max_advance << endl;
  cout << "num_charmaps: " << face->num_charmaps << endl;

  //cout << "family_name: " << face->family_name << endl;

  return 0;
}

void draw_bitmap(FT_Bitmap & bitmap,int pen_x, int pen_y,int r=0,int g=0,int b=0)
{
   for (int i=0 ; i < bitmap.rows ; ++i) //  row
   {

      for (int j=0 ; j < bitmap.pitch ; ++j) // unit is byte, draw a row
      {
         //if (out_of_range == true) continue;
         for (int k=7 ; k >= 0 ; --k)
         {

           //if (x >= 0 && x < max_width)
           {
             if ((bitmap.buffer[i*bitmap.pitch + j] >> k) & 0x01)
             {

               cout << "*";
               //cout << '\t' << x << "," << y << endl;
               //SVGALIB::SVGA::make_svga().set_pixel(x,y,r,g,b);
	     }
	     else
               cout << " ";

	   }
	 }
      }
      cout << endl;

   }
}

int main(int argc, char **argv)
{
  FT_Library library;
  int error;


  error=FT_Init_FreeType(&library);
  if (error)
  {
    err_msg("fail: FT_Init_FreeType");
  }

  FT_Face face;
  char *fp="./ukai.ttf";
  if (argc > 1)
    fp=argv[1];
  //const char fp[]="./gz/taipei16.pcf.gz";
  error=FT_New_Face(library,fp,0,&face);
  if (error==FT_Err_Unknown_File_Format)
  {
    err_msg("fail: unsupport file format");
  }
  else if (error)
       {
         err_msg("fail: FT_New_Face");
       }
  print_face_info(face);

  error=FT_Set_Pixel_Sizes(face,0,16);
  if (error)
  {
    err_msg("fail: FT_Set_Pixel_Sizes");
  }

  
  error=FT_Select_Charmap(face,FT_ENCODING_UNICODE);

  if (error)
  {
    err_msg("fail: FT_Select_CharMap");
  }
  
  //int charcode=45157;// 0xb065 送
  union Int
  {
    char ch[4];
    int num;
  };
  Int i;
  i.ch[1]=0x65;
  i.ch[0]=0xb0;
  i.ch[2]=0x00;
  i.ch[3]=0x00;

  FT_CharMap charmap;
  for (int i=0 ; i < face->num_charmaps; ++i)
  {
    charmap=face->charmaps[i];
    cout << "platform_id: " << charmap->platform_id << endl;
    cout << "encoding_id: " << charmap->encoding_id << endl;
  }
  error=FT_Set_Charmap(face,charmap);
  if(error)
  {
    err_msg("fail: FT_Set_CharMap");
  }

  if (face->charmap==NULL)
    cout << "no charmap is selected" << endl;
  else
    cout << "charmap is selected" << endl;
  
  FT_Set_Char_Size( face, 16*64, 16*64, 0, 0);
  
  //int charcode=41; // 0190 送
  int charcode=42938; // hex a7ba = dec 42938 宋, this big5 encoding
  int glyph_index=FT_Get_Char_Index(face,charcode);
  //cout << glyph_index << endl;
  if (glyph_index==0)
    glyph_index=16*7;
  error=FT_Load_Glyph(face,glyph_index,FT_LOAD_DEFAULT);
  if (error)
  {
    err_msg("fail: FT_Load_Glyph");
  }
  if (face->glyph->format==ft_glyph_format_bitmap)
  {
    cout << "ft_glyph_format_bitmap" << endl;
  }
  else{
    cout << "not ft_glyph_format_bitmap" << endl;
  }


  cout << face->glyph->bitmap_top << endl;
  cout << face->glyph->bitmap_left << endl;
  cout << face->glyph->bitmap.width << endl;
  cout << face->glyph->bitmap.rows << endl;


  draw_bitmap(face->glyph->bitmap,face->glyph->bitmap_top,face->glyph->bitmap_left);

}

