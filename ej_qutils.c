//
//  ej_qutils.cpp
//  QAnalizer
//
//  Created by Neo Angel on 13. 12. 11..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#include <stdlib.h>
#include "ej_qutils.h"
#include "ej_malloc.h"


// NSSwapBigIntToHost
uint32_t CFSwapInt32(uint32_t arg)
{
#if __BIG_ENDIAN__
    return arg;
#else
    
    uint32_t result;
    result = ((arg & 0xFF) << 24) | ((arg & 0xFF00) << 8) | ((arg >> 8) & 0xFF00) | ((arg >> 24) & 0xFF);
    return result;
#endif
}

// NSSwapBigShortToHost
uint16_t CFSwapInt16(uint16_t arg)
{
#if __BIG_ENDIAN__
    return arg;
#else
    uint16_t result;
    result = (uint16_t)(((arg << 8) & 0xFF00) | ((arg >> 8) & 0xFF));
    return result;
#endif
}

uint16_t NSSwapBigShortToHost(uint16_t arg)
{
    return CFSwapInt16(arg);
}

uint32_t NSSwapBigIntToHost(uint32_t arg)
{
    return CFSwapInt32(arg);
}


int intValueFromOffset(FILE *file, long offset)
{
    int retValue;
    fseek(file, offset, SEEK_SET);
    fread(&retValue, 1, 4, file);
    retValue = CFSwapInt32(retValue);
    return retValue;
}


short signedShortValueFromOffset(FILE *file, long offset)
{
    short retValue;
    fseek(file, offset, SEEK_SET);
    fread(&retValue, 1, 2, file);
    retValue = CFSwapInt16(retValue);
    return retValue;
}

unsigned short shortValueFromOffset(FILE *file, long offset)
{
    unsigned short retValue;
    fseek(file, offset, SEEK_SET);
    fread(&retValue, 1, 2, file);
    retValue = CFSwapInt16(retValue);
    return retValue;
}

unsigned char charValueFromOffset(FILE *file, long offset)
{
    unsigned char retValue;
    fseek(file, offset, SEEK_SET);
    fread(&retValue, 1, 1, file);
    return retValue;
}

unsigned char string256ValueFromOffset(FILE *file, long offset, char *strbuf)
{
    unsigned char stlen = charValueFromOffset(file, offset);
    fread(strbuf, 1, stlen, file);
    return stlen;
}

int intValueFromNext(FILE *file)
{
    int retValue;
    fread(&retValue, 1, 4, file);
    retValue = CFSwapInt32(retValue);
    return retValue;
}

short signedShortValueFromNext(FILE *file)
{
    short retValue;
    fread(&retValue, 1, 2, file);
    retValue = CFSwapInt16(retValue);
    return retValue;
}

unsigned short shortValueFromNext(FILE *file)
{
    unsigned short retValue;
    fread(&retValue, 2, 1, file);
    retValue = CFSwapInt16(retValue);
    return retValue;
}

unsigned char charValueFromNext(FILE *file)
{
    unsigned char retValue;
    fread(&retValue, 1, 1, file);
    return retValue;
}

unsigned char string256ValueFromNext(FILE *file, unsigned char *strbuf)
{
    unsigned char stlen = charValueFromNext(file);
    fread(strbuf, 1, stlen, file);
    return stlen;
}

char *newAndReadDataOfLength(FILE *file, long size, long *readlen, EJList *memlist)
{
    char *data = (char *)ejmalloc(size, memlist);
    *readlen = fread(data, 1, size, file);
    return data;
}
//
//char *newAndReadDataOfLength(FILE *file, long size)
//{
//    char *data = (char *)malloc(size);
//    fread(data, 1, size, file);
//    return data;
//}

int intValueFrom(unsigned char *bstr)
{
    int retValue;
    retValue = CFSwapInt32(*((int *)bstr));
    return retValue;
}

float floatValueFrom(unsigned char *bstr)
{
    short shortVal = (short)(bstr[0]) * 256 + bstr[1];
    float floatVal;
    floatVal = shortVal + ((float)(bstr[2]) * 256.0 + bstr[3]) / 65536.0;
    
    return floatVal;
}

unsigned short shortValueFrom(unsigned char *bstr)
{
    unsigned short retValue;
    retValue = CFSwapInt16(*((unsigned short *)bstr));
    return retValue;
}

NSSize sizeFrom(unsigned char *bstr)
{
    NSSize retsize;
    retsize.width = floatValueFrom(bstr);
    retsize.height = floatValueFrom(bstr+4);
    return retsize;
}

NSPoint pointFrom(unsigned char *bstr)
{
    NSPoint retpoint;
    retpoint.x = floatValueFrom(bstr);
    retpoint.y = floatValueFrom(bstr+4);
    return retpoint;
}


NSRect rectFrom2(unsigned char *bstr)
{
    NSRect retRect;
    retRect.origin.y = floatValueFrom(bstr);
    retRect.origin.x = floatValueFrom(bstr+4);
    retRect.size.height = floatValueFrom(bstr+8);
    retRect.size.width = floatValueFrom(bstr+12);
    return retRect;
}

EJMargin marginFrom(unsigned char *bstr)
{
    EJMargin margin;
    margin.left = floatValueFrom(bstr);
    margin.top = floatValueFrom(bstr+4);
    margin.right = floatValueFrom(bstr+8);
    margin.bottom = floatValueFrom(bstr+12);
    return margin;
}

NSRect rectFromPoint(NSPoint pt1 , NSPoint pt2)
{
    NSRect retRect;
    if(pt1.x > pt2.x) {
        retRect.origin.x = pt2.x;
        retRect.size.width = pt1.x - pt2.x;
    }
    else {
        retRect.origin.x = pt1.x;
        retRect.size.width = pt2.x - pt1.x;
    }
    
    if(pt1.y > pt2.y) {
        retRect.origin.y = pt2.y;
        retRect.size.height = pt1.y - pt2.y;
    }
    else {
        retRect.origin.y = pt1.y;
        retRect.size.height = pt2.y - pt1.y;
    }
    return retRect;
}

NSRect rectFromPoint1(NSPoint pt1 , NSPoint pt2)
{
    NSRect retRect;
    retRect = rectFromPoint(pt1 ,pt2);
    retRect.size.width -= 2.0; //?????
    retRect.size.height -= 2.0;
    return retRect;
}

NSRect rectFrom(unsigned char *bstr)
{
    NSPoint pt1, pt2;
    NSRect retRect;
    pt1.y = floatValueFrom(bstr);
    pt1.x = floatValueFrom(bstr+4);
    pt2.y = floatValueFrom(bstr+8);
    pt2.x = floatValueFrom(bstr+12);
    retRect = rectFromPoint1(pt1 ,pt2);
    return retRect;
}

NSRect pageRectFrom(unsigned char *bstr)
{
    NSPoint pt1, pt2;
    NSRect retRect;
    pt1.y = floatValueFrom(bstr);
    pt1.x = floatValueFrom(bstr+4);
    pt2.y = floatValueFrom(bstr+8);
    pt2.x = floatValueFrom(bstr+12);
    retRect = rectFromPoint(pt1 ,pt2);
    return retRect;
}


//NSRect cvtRectToM(NSRect aRect)
//{
//    NSRect retRect = aRect;
//    retRect.origin.y =  aRect.origin.y - distV;
//    retRect.origin.x =  aRect.origin.x - distH;
//    return retRect;
//}
