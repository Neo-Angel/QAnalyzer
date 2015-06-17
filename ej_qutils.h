//
//  ej_qutils.h
//  QAnalyzer
//
//  Created by Neo Angel on 13. 12. 11..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#ifndef __QAnalyzer__ej_qutils__
#define __QAnalyzer__ej_qutils__

//#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "ejtypedefs.h"
#include "ej_list.h"

uint16_t CFSwapInt16(uint16_t arg);
uint32_t CFSwapInt32(uint32_t arg);

uint16_t NSSwapBigShortToHost(uint16_t arg);
uint32_t NSSwapBigIntToHost(uint32_t arg);


int intValueFromOffset(FILE *file, long offset);
short signedShortValueFromOffset(FILE *file, long offset);
unsigned short shortValueFromOffset(FILE *file, long offset);
unsigned char charValueFromOffset(FILE *file, long offset);
unsigned char string256ValueFromOffset(FILE *file, long offset, char *strbuf);
int intValueFromNext(FILE *file);
short signedShortValueFromNext(FILE *file);
unsigned short shortValueFromNext(FILE *file);
unsigned char charValueFromNext(FILE *file);
unsigned char string256ValueFromNext(FILE *file, unsigned char *strbuf);
//char *newAndReadDataOfLength(FILE *file, long size);
//char *newAndReadDataOfLength(FILE *file, long size, long *readlen);
char *newAndReadDataOfLength(FILE *file, long size, long *readlen, EJList *memlist);


int intValueFrom(unsigned char *bstr);
float floatValueFrom(unsigned char *bstr);
unsigned short shortValueFrom(unsigned char *bstr);
NSSize sizeFrom(unsigned char *bstr);
NSPoint pointFrom(unsigned char *bstr);
NSRect rectFrom2(unsigned char *bstr);
NSRect rectFromPoint(NSPoint pt1 , NSPoint pt2);
NSRect rectFromPoint1(NSPoint pt1 , NSPoint pt2);
NSRect rectFrom(unsigned char *bstr);
NSRect pageRectFrom(unsigned char *bstr);
EJMargin marginFrom(unsigned char *bstr);


#endif /* defined(__QAnalyzer__ej_qutils__) */
