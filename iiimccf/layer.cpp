Font font;

void Word::render()
{
  font.FT_Render_Char( FT_ULong charcode );
  font.Draw();
}
  
void String::Render()
{
 for( int i= 0; i < words.size(); i++ )
 {
   words[i].render();
 }

}

void Text::Render()
{
  for( int i=0; i < strings.size(); i++ )
  {
    strings[i].render();
  }
}
