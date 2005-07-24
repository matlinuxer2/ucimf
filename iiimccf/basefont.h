#ifndef BASEFONT_H
#define BASEFONT_H
#include <string>
using namespace std;
//full or half hz when output
enum CharTag {
    FULL,LEFT,RIGHT
};

class BaseFont {
    public:
        BaseFont(string& fn,int w,int h);
        virtual ~BaseFont();
        int Width() const {
            return mWidth;
        }
        int Height() const {
            return mHeight;
        }
        int WidthBytes() const {
            return mWidthBytes;
        }
        char *GetChar(char c);
        char *GetChar(char c1, char c2);

    protected:
        /** mmap address of font(or buf) */
        int mFd;
        char* mpBuf;
        char* mpNull;
        unsigned mBufSize;
        int mWidth;
        int mHeight;
        long mNumChars;
        unsigned mDataOffset;
        int mByteLen;  //font size in bytes(not include blankline)
        int mWidthBytes;
        void GetInfo();
};
#endif
