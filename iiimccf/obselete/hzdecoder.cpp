#include "hzdecoder.h"

HzDecoder::HzDecoder() {}

HzDecoder:: ~HzDecoder() {}

bool HzDecoder::IsCode1(char c) {
    return c >= 0xa1 && c <= 0xfa;
}

bool HzDecoder::IsCode2(char c) {
    return c >= 40 && c <= 0xff;
}

unsigned int HzDecoder::Index(char c1, char c2) {
    return (c1 - 0xa1) * (0xff - 0x40) + c2 - 0x40;
}
