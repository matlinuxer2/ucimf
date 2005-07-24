class Color
{
  char r,g,b;
}

class Pixel
{
  int x,y;
  char r,g,b;
}

class Layer
{
  public:
    Layer( int pos_x, int pos_y, int width, int height, Color color );
    ~Layer();
   
    int x();
    int y();
    int w();
    int h();
    Color color();

    void x( int new_pos_x ); 
    void y( int new_pos_y );
    void w( int new_width );
    void h( int new_height );
    void color( Color new_color );
    
    virtual void render();
  
 private:
    int pos_x, pos_y;
    int width, height;
    char r, g, b;
};


/*
 * Geometry Object Layer
 */

class Point : Layer
{
  public:
    Point( int pos_x, int pos_y );
    ~Point();
}

class Line : Layer
{
  public:
    Line( int pos_x, int pos_y, int pos_xx, int pos_yy );
    ~Line();
}

class Rectangle: Layer
{
  public:
    Rectangle(int pos_x, int pos_y, int pos_xx, int pos_yy);
    ~Rectangle();
    
    virtual void render();	// boarder only
    virtual void render2();	// full filled
    
}

/*
 * Character Object Layer
 */
typedef long FT_ULong 

class Word : Layer
{
  public:
    Word( int pos_x, int pos_y, int height, FT_encoding encoding, FT_ULong charcode );
    Word( FT_encoding encoding, FT_ULong charcode );
    ~Word();
  private:
    int font_width;
    int font_height;

    FT_Face face;
    FT_Slot slot;
    FT_Glyph glyph;
}

class String : Layer
{
  public:
    String( int pos_x, int pos_y, int height, vector<Word>& words );
    String( int pos_x, int pos_y, int height, FT_encoding encoding, vector<FT_ULong> charcodes );
    ~String();
    
    virtual void render();
  private:
    vector<Word> words;

};

class Text : Layer
{
  public:
    Text( int pos_x, int pos_y, int height, vector<String>& strings );
    Text( int pos_x, int pos_y, int height, int width, int rows, FT_encoding encoding, vector<FT_ULong> charcodes );
    ~Text();
    
    virtual render();
  private:
    vector<String> strings;
};

/* 
 * Other Bitmap Object
 */
class BitmapLayer : Layer
{
  BitmapLayer( int pos_x, int pos_y, int width, int height );
  
  virtual void render();
};
