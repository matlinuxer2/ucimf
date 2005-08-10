#include <stdint.h>
#include <iiimp.h>
#include <vector>
#include <ft2build.h> 
#include FT_FREETYPE_H 
using namespace std;


/* Basic Object */

class Layer { 
  public: 
    Layer();
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

inline Layer::Layer()
{
  X=0; 
  Y=0;
  Width=0; 
  Height=0;
  Color = 0;
}


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

class String 
{
  public:
    String(){};
    String( const vector<IIIMP_card16>& charcodes );
    ~String(){};

    int size();
    void push_char( const IIIMP_card16& );
    void push_string( String );
    IIIMP_card16 operator[]( const int& i);
    
    void info();
    void render(int pos_x, int pos_y, int font_width, int font_height, int color );

  private:
    vector<IIIMP_card16> CharCodes;
 
};

class Text : public Layer
{
  public:
    Text();
    Text( vector<String> strings );
    Text( int pox_x, int pos_y, int font_height, int font_width, int color );

    ~Text(){};
    
    void append( String str );
    void info();
    
    int fw();
    int fh();
    int fc();
    
    void fw( int );
    void fh( int );
    void fc( int );

    virtual void render();
  private:
    int FontWidth;
    int FontHeight;
    int FontColor;
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
