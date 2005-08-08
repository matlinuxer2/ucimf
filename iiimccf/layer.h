#include <stdint.h>
#include <iiimp.h>
#include <vector>
#include <ft2build.h> 
#include FT_FREETYPE_H 
using namespace std;


/* Basic Object */

class Layer { 
  public: 
    Layer(){};
    ~Layer(){};
    int x();
    int y();
    int w();
    int h();
    int c();

    void x( int new_x );
    void y( int new_y );
    void w( int new_width );
    void h( int new_height );
    void c( int new_color );
  
  private:
    int X, Y;
    int Width, Height;
    int Color;
};


/*
 * Geometry Object Layer
 */

class Rectangle: public Layer
{
  public:
    Rectangle(){};
    Rectangle(int pos_x, int pos_y, int pos_xx, int pos_yy, int color );
    ~Rectangle(){ };
    
    virtual void render();	
};


/*
 * Character Object Layer
 */

class String : public Layer
{
  public:
    //String(){};
    String( vector<IIIMP_card16> charcodes );
    String( int font_width, int font_height, vector<IIIMP_card16> charcodes );
    ~String(){};

    int fw();
    int fh();

    void fw( int new_font_width );
    void fh( int new_font_height ); 
    
    void render(int pos_x, int pos_y);
    void render();

  private:
    int FontWidth;
    int FontHeight;
    vector<FT_ULong> CharCodes;
 
};

class Text : public Layer
{
  public:
    Text(){};
    Text( vector<String> strings );
    Text( int pos_x, int pos_y, vector<String> strings );
    ~Text(){};
    
    void append( String str );
    
    int fw();
    int fh();

    void fw( int );
    void fh( int );
    
    virtual void render();
  private:
    vector<String> Strings;
};



/* 
 * Other Bitmap Object
 */

class BitmapLayer : public Layer
{
  public:
    BitmapLayer( int pos_x, int pos_y, int width, int height );
    virtual void render();
};
