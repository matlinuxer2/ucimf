/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <sys/stat.h>
#include <iconv.h>

#include "fbiterm.h"

typedef int (readfontfunc) __P ((FontPtr, FontFilePtr, int, int, int, int));
unsigned char *get_glyph ();

/* get encoding name */
char *
get_encoding_name (FontRec * fr)
{
  int i, encoding = -1;
  char *ename = NULL, *font = NULL, *dup, *p;

  for (i = 0; i < fr->info.nprops; i++)
    {
      if (!strcmp (NameForAtom (fr->info.props[i].name), "CHARSET_REGISTRY"))
	ename = NameForAtom (fr->info.props[i].value);
      else
	if (!strcmp
	    (NameForAtom (fr->info.props[i].name), "CHARSET_ENCODING"))
	encoding = atoi (NameForAtom (fr->info.props[i].value));
      else if (!strcmp (NameForAtom (fr->info.props[i].name), "FONT"))
	font = NameForAtom (fr->info.props[i].value);
    }

  if (ename == NULL && font != NULL)
    {
      dup = strdup (font);
      p = strtok (dup, "-");

      for (i = 0; i < 11; i++)
	{
	  /* check if the `font' string has 14 fields */
	  p = strtok (NULL, "-");
	  if (p == NULL)
	    return NULL;
	}

      ename = p;
      free (dup);
      if (encoding == -1)
	encoding = atoi (strtok (NULL, "-"));
    }
  return (ename);
}

typedef struct _CodesetName
{
  char *font_codesetname;
  char *iconv_codesetname;
}
CodesetName;

static CodesetName lookup_tbl[] = {
  {"jisx0201.1976", "ISO-2022-JP"},
  {"jisx0208.1983", "ISO-2022-JP"},
  {"jisx0208.1990", "ISO-2022-JP"},
  {"jisx0212.1990", "ISO-2022-JP"},
  {"ksc5601.1987", "ISO-2022-KR"},
  {NULL, NULL}
};

#define TMP_BUFSIZ 10
unsigned int
get_glyph_codepoint (TermFont * fs, int codepoint)
{
  char *FROM = "ISO-10646/UCS2";
  CodesetName *cn = lookup_tbl;
  iconv_t cd;
  char *inp, *outp;
  char in[TMP_BUFSIZ], out[TMP_BUFSIZ];
  size_t in_len, out_len;
  unsigned int ret, tmp;

  if (!strcasecmp (fs->encoding_name, "iso10646"))
    return (codepoint);

  while (cn->font_codesetname)
    {
      if (!strcasecmp (fs->encoding_name, cn->font_codesetname))
	{
	  if ((cd =
	       iconv_open (cn->iconv_codesetname, FROM)) == (iconv_t) - 1)
	    return (codepoint);

	  bzero (out, TMP_BUFSIZ);
	  bzero (in, TMP_BUFSIZ);
	  out_len = TMP_BUFSIZ;
	  outp = out;
	  in[0] = (char) (codepoint & 0x000000ff);
	  in[1] = (char) ((codepoint >> 8) & 0x000000ff);
	  inp = in;
	  in_len = 3;
	  iconv (cd, &inp, &in_len, &outp, &out_len);	/* ISO-10646 to font's codeset */
	  tmp = out[3] & 0x000000ff;
	  ret = (tmp << 8) | out[4];
	  iconv_close (cd);
	  return (ret);
	}
      cn++;
    }
  return (codepoint);
}

unsigned char *
get_glyph (TermFont * fs, int codepoint, unsigned int *height)
{
  unsigned short firstCol, lastCol, firstRow, lastRow;
  unsigned short col, row, numCols, numRows;
  unsigned int can_num, codep;
  int r, c, n;
  unsigned char *bits;
  xCharInfo *metric;

  BitmapFontPtr bp = (BitmapFontPtr) (fs->frec->fontPrivate);
  CharInfoPtr cp = (CharInfoPtr) (bp->metrics);
  CharInfoPtr dp = (CharInfoPtr) (bp->pDefault);
  CharInfoPtr **ep = (CharInfoPtr **) (bp->encoding);

  firstCol = fs->frec->info.firstCol;
  lastCol = fs->frec->info.lastCol;
  firstRow = fs->frec->info.firstRow;
  lastRow = fs->frec->info.lastRow;
  numCols = lastCol - firstCol + 1;
  numRows = lastRow - firstRow + 1;

  /* set default glyph */
  bits = dp->bits;
  metric = &(dp->metrics);
  *height = (metric->ascent + metric->descent);

  can_num = codep = get_glyph_codepoint (fs, codepoint);

  if (can_num > 0)
    {
      row = (codep >> 8) & 0x000000ff;
      r = row - firstRow;

      col = codep & 0x000000ff;
      c = col - firstCol;

      can_num = (r < 0 ? 0 : r) * numCols + c;

      if (r < numRows && c < numCols && can_num != 0)
	{
	  n = ACCESSENCODING (ep, can_num) - cp;
	  if (n > 0)
	    {
	      bits = cp[n].bits;
	      metric = &(cp[n].metrics);
	      *height = (metric->ascent + metric->descent);
	    }
	  return bits;
	}
    }

  return bits;
}

/* load font */
TermFont *
load_font (char *input_filename)
{
  FontRec *font = NULL;
  TermFont *pFs = NULL;
  FontFilePtr input = NULL;
  readfontfunc *readfont = NULL;

  /* allocate TermFont structure area */
  pFs = (TermFont *) malloc (sizeof (TermFont));
  if (pFs == NULL)
    return (TermFont *) NULL;
  memset (pFs, 0x0, sizeof (TermFont));

  /* allocate FontRec area */
  font = (FontRec *) malloc (sizeof (FontRec));
  if (font == NULL)
    return (TermFont *) NULL;
  memset (font, 0x0, sizeof (FontRec));

  /* open font file */
  if (strlen (input_filename) > 0)
    {
      /* choose suitable function */
      readfont = strstr (input_filename, ".bdf") ||
	strstr (input_filename, ".BDF") ? bdfReadFont : pcfReadFont;

      input = FontFileOpen (input_filename);
      if (input == NULL)
	return (TermFont *) NULL;

      pFs->filename = malloc (strlen (input_filename) + 1);
      if (pFs->filename == NULL)
	{
	  FontFileClose (input);
	  return (TermFont *) NULL;
	}

      memset (pFs->filename, 0x0, strlen (input_filename) + 1);
      strcpy (pFs->filename, input_filename);
    }

  /* read font data */
  if ((readfont) != NULL)
    if ((readfont) (font, input, MSBFirst, MSBFirst, 4, 4) != Successful)
      {
	fprintf (stderr, "font file, %s, corrupt\n", input_filename);
	FontFileClose (input);
	return (TermFont *) NULL;
      }

  /* set global variable */
#ifdef _DEBUG
  fprintf (stdout, "ascent  [%d]\n", font->info.maxbounds.ascent);
  fprintf (stdout, "descent [%d]\n", font->info.maxbounds.descent);
#endif
  pFs->cell_width = font->info.maxbounds.characterWidth;
  pFs->cell_height =
    font->info.maxbounds.ascent + font->info.maxbounds.descent;

#ifdef STRICT_DEF
  /* check terminal font */
  if (!font->info.terminalFont)
    {
      fprintf (stderr, "not terminal font\n", input_filename);
      FontFileClose (input);
      return (TermFont *) NULL;
    }
#endif

  pFs->frec = font;
  /* check width and height of char */
  if (font->info.maxbounds.characterWidth > 32 ||
      font->info.maxbounds.ascent + font->info.maxbounds.descent > 32)
    {
      fprintf (stderr, "font %s too big size\n", input_filename);
      FontFileClose (input);
      return (TermFont *) NULL;
    }

  /* get encoding name */
  {
    char *p;
    if ((p = get_encoding_name (font)) == NULL)
      p = "none";

    pFs->encoding_name = malloc (strlen (p) + 1);
    memset (pFs->encoding_name, 0x0, strlen (p) + 1);
    strcpy (pFs->encoding_name, p);
#ifdef _DEBUG
    printf ("encoding name is [%s]\n", pFs->encoding_name);
#endif
  }
  return pFs;
}

/* Load X font */
int
InitFont (char *ascfontfile, char *mbfontfile)
{
  struct stat sbuf;
  int rc = 0;
  char *ascfname = ascfontfile;
  char *mbfname = mbfontfile;

  /* check mbfont first */
  if (stat (mbfname, &sbuf) < 0)
    {
      if (stat (DefaultMB, &sbuf) < 0)
	{
	  if (stat (DefaultAsc, &sbuf) < 0)
	    {
	      if (stat (DefaultFont, &sbuf) < 0)
		{
		  fprintf (stderr, "font file not found.\n");
		  return -1;
		}
	      else
		{
		  mbfname = DefaultFont;
		  rc |= ITERM_REPLACEFONT;
		}
	    }
	  else
	    {
	      mbfname = DefaultAsc;
	      rc |= ITERM_REPLACEFONT;
	    }
	}
      else
	mbfname = DefaultMB;
    }

  pIterm->mb_font = (TermFont *) load_font (mbfname);
  if (pIterm->mb_font == NULL)
    {
      fprintf (stderr, "failed to load ascii font file.\n");
      return -1;
    }

  /* check ascii font */
  if (stat (ascfname, &sbuf) < 0)
    {
      if (stat (DefaultAsc, &sbuf) < 0)
	{
	  if (stat (DefaultFont, &sbuf) < 0)
	    {
	      fprintf (stderr, "ascii font file not found.\n");
	      return -1;
	    }
	  else
	    ascfname = DefaultFont;
	}
      else
	ascfname = DefaultAsc;
    }

  if (!strcmp (ascfname, mbfname))
    pIterm->asc_font = pIterm->mb_font;
  else
    pIterm->asc_font = (TermFont *) load_font (ascfname);
  if (pIterm->asc_font == NULL)
    {
      fprintf (stderr, "failed to load ascii font file.\n");
      return -1;
    }

  /* check ascii font range */
  if (pIterm->asc_font->frec->info.firstRow > 0)
    {
      fprintf (stderr, "not a suitable font as terminal.\n");
      return -1;
    }

#ifdef _DEBUG
  fprintf (stdout, "%s was selected as multibyte font.\r\n",
	   pIterm->mb_font->filename);
  fprintf (stdout, "MB  column[0x%x]-[0x%x]  row[0x%x]-[0x%x]\r\n",
	   pIterm->mb_font->frec->info.firstCol,
	   pIterm->mb_font->frec->info.lastCol,
	   pIterm->mb_font->frec->info.firstRow,
	   pIterm->mb_font->frec->info.lastRow);
  fprintf (stdout, "%-18s column[0x%02x]-[0x%02x]  row[0x%02x]-[0x%02x] %s\n",
	   pIterm->asc_font->encoding_name,
	   pIterm->asc_font->frec->info.firstCol,
	   pIterm->asc_font->frec->info.lastCol,
	   pIterm->asc_font->frec->info.firstRow,
	   pIterm->asc_font->frec->info.lastRow, pIterm->asc_font->filename);
#endif

  pIterm->cell_width =
    min (pIterm->asc_font->cell_width, pIterm->mb_font->cell_width);
  pIterm->cell_height =
    max (pIterm->asc_font->cell_height, pIterm->mb_font->cell_height);
  return rc;
}
