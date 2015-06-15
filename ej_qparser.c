//
//  ej_qparser.cpp
//  QAnalizer
//
//  Created by Neo Angel on 13. 12. 11..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "ej_qparser.h"
#include "ej_qgraphics.h"
#include "ej_malloc.h"

#define HSBTYPE 0
#define RGBTYPE 1
#define CMYKTYPE 2
#define PANTONETYPE 3
#define TOYOTYPE 4
#define DICTYPE 5
#define TRUMATCHTYPE 6
#define FOCOLTONETYPE 7
#define PANTONEPROCESS 8
#define PANTONEPROSIM 9
#define PANTONEUNCOATED 10
#define PANTONEANPA 11
#define PANTONERES1 12
#define PANTONERES2 13
#define PANTONERES3 14


//typedef enum {
//    EJ_COLORTYPE_HSB,
//    EJ_COLORTYPE_RGB,
//    EJ_COLORTYPE_CMYK,
//    EJ_COLORTYPE_PANTONE,
//    EJ_COLORTYPE_TOYO,
//    EJ_COLORTYPE_TRUMATCH,
//    EJ_COLORTYPE_FOCOLTONE,
//    EJ_COLORTYPE_PANTONEPROCESS,
//    EJ_COLORTYPE_PANTONEPROSIM,
//    EJ_COLORTYPE_PANTONEUNCOATED,
//    EJ_COLORTYPE_PANTONEANPA,
//    EJ_COLORTYPE_PANTONERES1,
//    EJ_COLORTYPE_PANTONERES2,
//    EJ_COLORTYPE_PANTONERES3
//}EJColorType;
//

char *EJ_GraphicTypeName[5] = {"Line","Image","Text","Group","UserBox"};


char *EJ_ColorTypeName[] = {"HSB","RGB","CMYK","PANTONE","TOYO",
    "DIC","TRUMATCH","FOCOLTONE","PANTONEPROCESS","PANTONEPROSIM",
    "PANTONEUNCOATED","PANTONEANPA", "PANTONERES1", "PANTONERES2", "PANTONERES3"};


void analizeDetail(EJQInfo *qinfo);



BOOL isQuarkData(unsigned char *datab, unsigned char *language, unsigned int *qver)
{
    unsigned char qtestb[8] = {0x0, 0x0, 0x4d, 0x4d, 0x58, 0x50, 0x52, 0x61};
    *qver = CFSwapInt32(*(int *)(datab + 8));
    *language = ((unsigned char *)(datab))[7]; // 0x61: JAP,KOR   0x33:ENG(US)
    int i;
    for(i=0;i<7;i++) {
		if(qtestb[i] != datab[i]) {
			return NO;
		}
    }
    if(*qver == 0x003f003f){
		return YES;
    }
    return NO;
}

BOOL isQuarkFile(char *qpath ,unsigned char *language, unsigned int *qver)
{
	BOOL ret = NO;
    unsigned char curdata[16];
    FILE *orgfileh = fopen(qpath, "r");
    size_t len = fread(curdata, 16, 1, orgfileh);
    if(len >= 16) {
		if(isQuarkData(curdata, language, qver)) {
			ret = YES;
		}
    }
    fclose(orgfileh);
	return ret;
}

FILE *openQuarkFile(char *qpath ,EJQInfo *qinfo)
{
    unsigned char curdata[16];
    FILE *org_fileh = fopen(qpath, "rb");
    if (org_fileh == NULL) {
        printf("Cannot open file.");
        return NULL;
    }
    size_t len = fread(curdata, 1, 16, org_fileh);
    if(len >= 16) {
		if(isQuarkData(curdata, &qinfo->qdoc.language, &qinfo->qdoc.version)) {
            fseek(org_fileh, 0L, SEEK_END);
            qinfo->qdoc.filesize = ftell(org_fileh);
            rewind(org_fileh);
            return org_fileh;
		}
    }
    
    fclose(org_fileh);
	return NULL;
}

// ###


int fileExists(char *filename)
{
    
    struct stat stFileInfo;
    bool result;
    int intStat;
    
    intStat = stat(filename, &stFileInfo);
    
    if (intStat == 0)
    {
        result = true;
    }
    else
    {
        result = false;
    }
    
    return result;
}

FILE *makeWorkFileFrom(FILE *org_fileh, EJQInfo *qinfo ,char *tmp_filepath)
{
    unsigned short idxLen;
    fseek(org_fileh, 0x2fc, SEEK_SET);
    unsigned char testData[4];
    fread(testData, 1, 4, org_fileh);
    if((testData[0] == 0 &&  testData[1] == 0 && testData[2] == 0 ) || (testData[0] == 0xff &&  testData[1] == 0xff && testData[2] == 0xff )) {
        idxLen = 4;
    }
    else
        idxLen = 2;
    qinfo->idxLen = idxLen;
    fseek(org_fileh, 0, SEEK_SET);

    strcpy(tmp_filepath, "/tmp/__qtmpdata__.dat");
    remove(tmp_filepath);
    int fidx = 0;
    while (fileExists(tmp_filepath)) {
        sprintf(tmp_filepath,"/tmp/__qtmpdata%04d__.dat",fidx++);
        remove(tmp_filepath);
        if (fidx > 1000) {
            printf("Too many temp files.\n");
            return NULL;
        }
    }
    
    FILE *work_fileh = fopen(tmp_filepath, "w+");
    if (!work_fileh) {
        return NULL;
    }
    
    int nextblock;
    unsigned int datalen = 0x100 - idxLen; //0xfc;
    char data[0x200];
    fread(data, 1, 0x200, org_fileh);
    fwrite(data, 1, 0x200, work_fileh);
    BOOL iseof = NO;
    while (!iseof) {
        char *block_data = (char *)malloc(datalen+1);
        size_t len = fread(block_data, 1, datalen, org_fileh);
        fwrite(block_data, 1, len, work_fileh);
        if (block_data == (char *)0xb0000000210) {
            printf("block_data == 0xb0000000210\n");
        }
        free(block_data);
        if(len < datalen) {
            iseof = YES;
            break;
        }
        if(idxLen == 4)
            nextblock = intValueFromNext(org_fileh);
        else
            nextblock = signedShortValueFromNext(org_fileh);
        if(nextblock < 0) {
            unsigned long nextoff;
            unsigned short blockcnt;
            nextblock = -nextblock;
            nextoff = (nextblock-1) * 0x100;
            blockcnt = shortValueFromOffset(org_fileh,nextoff);
            datalen = blockcnt * 0x100 - 2 - idxLen;
        }
        else if(nextblock > 0){
            unsigned long nextoff;
            nextoff = (nextblock-1) * 0x100;
            fseek(org_fileh, nextoff, SEEK_SET);
            datalen = 0x100 - idxLen; //0xfc;
        }
        else {
//            NSLog(@"Next offset is zero.");
            unsigned long curpos = ftell(org_fileh);  // or ftell
            unsigned long left = qinfo->qdoc.filesize - curpos;
            block_data = (char *)malloc(left);
            fread(block_data, 1, left, org_fileh);
            fwrite(block_data, 1, left, work_fileh);
            if (block_data == (char *)0xb0000000210) {
                printf("block_data == 0xb0000000210\n");
            }
            free(block_data);
            iseof = YES;
            break;
        }
    }
    fclose(work_fileh);
    work_fileh = fopen(tmp_filepath, "r");
    return work_fileh;
}


// Insignificant from here
char encodeValues[4] ={0,   -1,   +2,   +1};
char decodeValues[4] ={0,  3,   2,   -3};
char decodeValues2[4] ={0,  1,   6,   7};
char decodeValues3[4] ={-4, -1, -2, +1};
char decodeValues4[4] ={0, 3, 2, -3};
int decodeValues16[16] = {0, 0x20, 0, 0xa0, 0x80, 0xa0, 0x80, 0x20 ,0, 0x20, 0, - 0x60, - 0x80, - 0x60, - 0x80, - 0xe0};

char decodeByte(unsigned char ch, int addr)
{
    unsigned  char retch;
    if(addr % 2) {
        retch = ((ch /4) +1) * 4 + decodeValues3[ch % 4];
    }
    else {
        retch = (ch / 4) * -4 + decodeValues4[ch % 4];
        int idx = retch >> 4;
        int addValue = decodeValues16[idx];
        retch += addValue;
        //retch = [self decodeByte1:ch];
    }
    return retch;
}
// to here

void readDocumentInfo(FILE *fileh, unsigned char *bytedata, EJQInfo *qinfo)
{
    fseek(fileh, 0, SEEK_SET);
    fread(bytedata, 1, 0x200, fileh);
    if (qinfo->qdoc.version == 0x203F203F) {  // Insignificant coding area
        int i;
        unsigned char subch = 0;
        for(i=0;i<0x1ec;i++) {
            int addr = 0x14 + i;
            unsigned char ch = bytedata[addr];
            
            ch = ch - subch ++;
            if( addr ==  0x41) {
                ch = decodeByte(ch, 0);
            }
            if( addr == 0x110 || addr == 0x111) {
                ch = decodeByte(ch, 0);
            }
            if( addr == 0x112 || addr == 0x113) {
                ch = decodeByte(ch, 0);
            }
            if( addr == 0x75) {
                ch = decodeByte(ch, 1);
            }
            //	    ch = [self decodeByte:ch addr:addr];
            bytedata[addr] = ch;
        }
    }
}


void readHeaderInfo(FILE *fileh, unsigned char *header, EJQInfo *qinfo)
{
    readDocumentInfo(fileh, header, qinfo);
    qinfo->qdoc.docSpreadCount = shortValueFrom(header + 0x40);
    qinfo->qdoc.masterSpreadCount = header[0x75];
    qinfo->curDocKey = shortValueFrom(header + 0x110);
    qinfo->incKey = shortValueFrom(header + 0x112);
}

void postProcessLinkText(EJQInfo *qinfo, EJTextLink *textLink)
{
    unsigned long i, icnt;
    icnt = textLink->linkDictList->count;
    qinfo->linkDictList = (EJTextLinkDict **)ejmalloc(sizeof(EJTextLinkDict *) * icnt, qinfo->memlist);
    EJNode *node = textLink->linkDictList->start;
    for (i=0; i<icnt; i++) {
        EJTextLinkDict *dictItem = (EJTextLinkDict *)node->value;
        qinfo->linkDictList[i] = dictItem;
        node = (EJNode *)node->next;
    }
    qinfo->linkDictListCount = icnt;
    
    icnt = textLink->linkBoxList->count;
    qinfo->linkBoxList = (EJGraphicItem **)ejmalloc(sizeof(EJGraphicItem *) * icnt, qinfo->memlist);
    node = textLink->linkBoxList->start;
    for (i=0; i<icnt; i++) {
        EJGraphicItem *boxItem = (EJGraphicItem *)node->value;
        qinfo->linkBoxList[i] = boxItem;
        node = (EJNode *)node->next;
    }
    qinfo->linkBoxListCount = icnt;
    
    
    icnt = textLink->linkRootList->count;
    qinfo->linkRootList = (EJGraphicItem **)ejmalloc(sizeof(EJGraphicItem *) * icnt, qinfo->memlist);
    node = textLink->linkRootList->start;
    for (i=0; i<icnt; i++) {
        EJGraphicItem *boxItem = (EJGraphicItem *)node->value;
        qinfo->linkRootList[i] = boxItem;
        node = (EJNode *)node->next;
    }
    qinfo->linkRootListCount = icnt;

}


BOOL readQDcoument(char *qdocpath, EJQInfo *qinfo, BOOL flag)
{
//    BOOL ret = YES;
    FILE *org_fileh = openQuarkFile(qdocpath, qinfo);
    if (!org_fileh) {
        return NO;
    }
    char tmp_filepath[30];
    FILE *work_fileh = makeWorkFileFrom(org_fileh, qinfo, tmp_filepath);
    if (work_fileh == NULL) {
        fclose(org_fileh);
        return NO;
    }
    
    int i;
    long cnt, len;
    int spreadCount ;
    unsigned long long curoffset;
    unsigned long long workingoffset;
    
    // Start Read
    readHeaderInfo(org_fileh, qinfo->header, qinfo);
    
    if(qinfo->qdoc.version == 0x203F203F ) {
        fclose(org_fileh);
        fclose(work_fileh);
        return NO;
    }
    spreadCount = qinfo->qdoc.docSpreadCount + qinfo->qdoc.masterSpreadCount;
    curoffset = 0x200;
    // ??????????????
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
    // ?????????????
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
    // Generic Color
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
    
    // Dictionary
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
    // Fonts
    len = intValueFromOffset(work_fileh, curoffset); // [fileh intValueFromOffset:curoffset]
    curoffset = ftell(work_fileh) + len;
    cnt = shortValueFromNext(work_fileh);
    
    qinfo->qdoc.fontList = NULL;
    if (cnt) {
        qinfo->qdoc.fontList = (EJFont **)ejmalloc(sizeof(EJFont *) * cnt, qinfo->memlist);
    }
    qinfo->qdoc.fontListCount = cnt;
    
    for(i=0;i<cnt;i++) {
        EJFont *fontItem = (EJFont *)ejmalloc(sizeof(EJFont), qinfo->memlist);
        fontItem->fontID = shortValueFromNext(work_fileh);
        string256ValueFromNext(work_fileh, fontItem->fontName);
        string256ValueFromNext(work_fileh, fontItem->familyName);
        qinfo->qdoc.fontList[i] = fontItem;
    }
    
    len = intValueFromOffset(work_fileh, curoffset);  // Font Attributes ... Later~
    if(qinfo->qdoc.version != 0x003A003A ) {
        curoffset = curoffset + 4 + len;
        // Color
        len = intValueFromOffset(work_fileh, curoffset);
    }
    
    // Color List
    cnt = (int)charValueFromOffset(work_fileh, curoffset+5);
    qinfo->qdoc.colorList = NULL;
    if (cnt) {
        qinfo->qdoc.colorList = (EJColor **)ejmalloc(sizeof(EJColor *) * cnt, qinfo->memlist);
    }
    qinfo->qdoc.colorListCount = cnt;
    qinfo->colorDataListCount = cnt;
    qinfo->colorDataList = (char **)ejmalloc(sizeof(char *)*cnt, qinfo->memlist);
    
    workingoffset = curoffset + 0x26;
#define MAXSHORT 65535
    
    for(i=0;i<cnt;i++) {
        unsigned char stlen;
        EJColor *colorItem = (EJColor *)ejmalloc(sizeof(EJColor), qinfo->memlist);
        
        fseek(work_fileh, workingoffset, SEEK_SET);
        colorItem->colorID = charValueFromNext(work_fileh);
        colorItem->colorType = charValueFromOffset(work_fileh, workingoffset + 0x23);
//        if(colorItem->colorSpace == EJColorSpace_RGB) {  // == 1
            fseek(work_fileh, workingoffset+2, SEEK_SET);
            colorItem->rgbColor.red = shortValueFromNext(work_fileh);
            colorItem->rgbColor.green = shortValueFromNext(work_fileh);
            colorItem->rgbColor.blue = shortValueFromNext(work_fileh);
            colorItem->rgbColor.alpha = 0xffff;
//         }
//         else
//        {
            fseek(work_fileh, workingoffset+8, SEEK_SET);
            colorItem->cmykColor.cyan = shortValueFromNext(work_fileh);
            colorItem->cmykColor.magenta = shortValueFromNext(work_fileh);
            colorItem->cmykColor.yellow = shortValueFromNext(work_fileh);
            colorItem->cmykColor.black = shortValueFromNext(work_fileh);
//        }
        colorItem->trapIndex = signedShortValueFromOffset(work_fileh, workingoffset+0x30);
        fseek(work_fileh, workingoffset+0x32, SEEK_SET);
        
        stlen = string256ValueFromNext(work_fileh, colorItem->colorName);
        stlen = (stlen / 2 + 1) * 2 ;
        
        fseek(work_fileh, workingoffset, SEEK_SET);
        
        char *color_data =  ejmalloc(0x32, qinfo->memlist);
        qinfo->colorDataList[i] = color_data;
        fread(qinfo->colorDataList[i], 1, 0x32, work_fileh);
        
        workingoffset += 0x32 + stlen;
        qinfo->qdoc.colorList[i] = colorItem;
    }
    curoffset += (len + 4);
    // Color Trap Info Block
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
    if (len) {
        qinfo->block1 = ejmalloc(len, qinfo->memlist);
        qinfo->block1Length = len;
        fread(qinfo->block1, 1, len, work_fileh);
    }
    else {
        qinfo->block1 = NULL;
        qinfo->block1Length = 0;
    }
    
    // Style
    len = intValueFromOffset(work_fileh, curoffset);
    workingoffset = ftell(work_fileh);
    curoffset = workingoffset + len;
    
    EJList *styleList = ejCreateList();
    
    while(curoffset > workingoffset) {
        EJStyleItem *styleItem = (EJStyleItem *)ejmalloc(sizeof(EJStyleItem), qinfo->memlist);
        unsigned char stlen;
        fseek(work_fileh, workingoffset, SEEK_SET);
        
        if(qinfo->qdoc.language == Q_KOREAN) {
            fread(styleItem->data, 1, STYLE_LEN, work_fileh);
        }
        else {
            fread(styleItem->data, 1, STYLE_LEN-2, work_fileh);
        }
        stlen = string256ValueFromNext(work_fileh, styleItem->styleName);
        stlen = (stlen / 2 + 1) * 2 ;
        if(qinfo->qdoc.language == Q_KOREAN)
            workingoffset += STYLE_LEN + stlen;
        else
            workingoffset += STYLE_LEN - 2 + stlen;
        ejAddValue(styleList, styleItem);
    }
    unsigned long listcnt = styleList->count;
    qinfo->styleList = NULL;
    if (listcnt) {
        qinfo->styleList = (EJStyleItem **)ejmalloc(sizeof(EJStyleItem *) * listcnt, qinfo->memlist);
    }
    EJNode *node = styleList->start;
    for (i=0; i<listcnt; i++) {
        EJStyleItem *styleItem = (EJStyleItem *)node->value;
        qinfo->styleList[i] = styleItem;
    }
    qinfo->styleListCount = listcnt;
    ejReleaseList(styleList);
    
    // Hypen & Justification
    len = intValueFromOffset(work_fileh, curoffset);
    workingoffset = ftell(work_fileh);
    curoffset = workingoffset + len;
    
    EJList *hnjList = ejCreateList();
    
    while(curoffset > workingoffset) {
        unsigned char stlen;
        
        EJHNJItem *hnjItem =  (EJHNJItem *)ejmalloc(sizeof(EJHNJItem), qinfo->memlist);
        fseek(work_fileh, workingoffset, SEEK_SET);
        if(qinfo->qdoc.language == Q_KOREAN) {
            fread(hnjItem->data, 1, HNJDATA_LEN_KR, work_fileh);
        }
        else {
            fread(hnjItem->data, 1, HNJDATA_LEN_EN, work_fileh);
        }
        stlen = string256ValueFromNext(work_fileh, hnjItem->hnjName);
        stlen = (stlen / 2 + 1) * 2 ;
        if(qinfo->qdoc.language == Q_KOREAN)
            workingoffset += HNJDATA_LEN_KR + stlen;
        else
            workingoffset += HNJDATA_LEN_EN + stlen;
        ejAddValue(hnjList, hnjItem);
    }
    listcnt = hnjList->count;
    qinfo->hnjList = NULL;
    if (listcnt) {
        qinfo->hnjList = (EJHNJItem **)ejmalloc(sizeof(EJHNJItem *) * listcnt, qinfo->memlist);
    }
    node = hnjList->start;
    for (i=0; i<listcnt; i++) {
        EJHNJItem *hnjItem = (EJHNJItem *)node->value;
        qinfo->hnjList[i] = hnjItem;
    }
    qinfo->hnjListCount = listcnt;
    ejReleaseList(hnjList);
    
    // Font Set
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;

    // ?????????????  KOR-
    if(qinfo->qdoc.language == Q_KOREAN) {
        len = intValueFromOffset(work_fileh, curoffset);
        curoffset = ftell(work_fileh) + len;
    }
    
    // Text Trap Info Block
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
//    if (len) {
//        qinfo->block2 = ejmalloc(len, qinfo->memlist);
//        qinfo->block2Length = len;
//        fread(qinfo->block2, 1, len, work_fileh);
//    }
//    else {
//        qinfo->block2 = NULL;
//        qinfo->block2Length = 0;
//    }
    
    // Text Attributes
    if(qinfo->qdoc.language == Q_KOREAN) {
        cnt = len / TEXTATTRDATA_LEN_KR;
    }
    else {
        cnt = len / TEXTATTRDATA_LEN_EN;
    }
    qinfo->textAttrDataList = NULL;
    if (cnt) {
        qinfo->textAttrDataList = (char **)ejmalloc(sizeof(char *) * cnt, qinfo->memlist);
    }
    qinfo->textAttrDataListCount = cnt;
    for(i=0;i<cnt;i++) {
        char *textAttrData = (char *)ejmalloc(TEXTATTRDATA_LEN_KR, qinfo->memlist);
        if(qinfo->qdoc.language == Q_KOREAN) {
            fread(textAttrData, 1, TEXTATTRDATA_LEN_KR, work_fileh);
        }
        else {
            fread(textAttrData, 1, TEXTATTRDATA_LEN_EN, work_fileh);
        }
        qinfo->textAttrDataList[i] = textAttrData;
    }
    if(qinfo->qdoc.language == Q_KOREAN) {
        qinfo->textAttrDataLength = TEXTATTRDATA_LEN_KR;
    }
    else {
        qinfo->textAttrDataLength = TEXTATTRDATA_LEN_EN;
    }
    qinfo->textAttrDataListCount = cnt;
    
    // Paragraph Attributes
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
    cnt = len / 0x100;
    qinfo->paraAttrDataList = NULL;
    if (cnt) {
        qinfo->paraAttrDataList = (char **)ejmalloc(sizeof(char *) * cnt, qinfo->memlist);
    }
    qinfo->paraAttrDataListCount = cnt;
    qinfo->paraAttrDataLength = 0x100;
    
    for(i=0;i<cnt;i++) {
        char *paraAttrData = (char *)ejmalloc(0x100, qinfo->memlist);
        fread(paraAttrData, 1, 0x100, work_fileh);
        qinfo->paraAttrDataList[i] = paraAttrData;
    }
    // ????????????? (Short Cut Key)
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
    // ?????????????
    len = intValueFromOffset(work_fileh, curoffset);
    curoffset = ftell(work_fileh) + len;
   
    // ====================================================
    // Spread
    qinfo->spreadList = (EJSpreadItem **)ejmalloc(sizeof(EJSpreadItem *) * spreadCount, qinfo->memlist);
    qinfo->spreadListCount = spreadCount;
    qinfo->ejgid = 0;
    EJTextLink textLink;
    textLink.linkDictList = ejCreateList();
    textLink.linkBoxList = ejCreateList();
    textLink.linkRootList = ejCreateList();
    int total_page_cnt = 0;
    for(i=0;i<spreadCount;i++) {
        // Spread (Pages)
        EJSpreadItem *aSpread =  (EJSpreadItem *)ejmalloc(sizeof(EJSpreadItem), qinfo->memlist);
        long j, jcnt;
        long pgcnt;
        int stlen;
        int type;
        workingoffset = curoffset;
        len = shortValueFromOffset(work_fileh, curoffset);
        curoffset = ftell(work_fileh) + len;
        pgcnt = (len - 4) / 0x3c;
        // Read Spread
        fread(aSpread->head, 1, 4, work_fileh);
        
        // Read Pages
#ifdef SM_DEBUG
        NSLog(@"Spread Offset: 0x%x Count : %d, Length : 0x%x",(unsigned int)workingoffset ,i, len);
#endif
        aSpread->pageList = (EJPageItem **)ejmalloc(sizeof(EJPageItem *)*pgcnt, qinfo->memlist);
        for(j=0;j<pgcnt;j++) {
            EJPageItem *page = (EJPageItem *)ejmalloc(sizeof(EJPageItem), qinfo->memlist);
            fread(page->data, 1, 0x3c, work_fileh);
            aSpread->pageList[j] = page;
        }
        aSpread->pageListCount = pgcnt;
        total_page_cnt += pgcnt;
        
        jcnt = (pgcnt * 2)+ 2;
        for(j=0;j<jcnt;j++) {
            int xlen = intValueFromOffset(work_fileh, curoffset);
            curoffset += 4 + xlen;
            
        }
        aSpread->masterName[0] = 0;
        type = intValueFromOffset(work_fileh, curoffset);
        if(type == 1) {
            curoffset += 7;
        }
        else {
            curoffset += 4;
            stlen = string256ValueFromOffset(work_fileh, curoffset, aSpread->masterName);
            if(stlen) {
                curoffset += stlen + 1 + 2;
            }
            else {
                curoffset += 6;
            }
        }
        cnt = shortValueFromOffset(work_fileh, curoffset);
        curoffset += 2;
        aSpread->graphicList = NULL;
        if(cnt) {
            aSpread->graphicList = (EJGraphicItem **)ejmalloc(sizeof(EJGraphicItem *) * cnt, qinfo->memlist);
            len = getGraphics(qinfo, aSpread->graphicList, work_fileh , cnt, org_fileh, &textLink, flag);
            curoffset += len;
        }
        aSpread->graphicListCount = cnt;
        curoffset += 4;
        qinfo->spreadList[i] = aSpread;
    }
    postProcessLinkText(qinfo, &textLink);
    ejReleaseList(textLink.linkDictList);
    ejReleaseList(textLink.linkBoxList);
    ejReleaseList(textLink.linkRootList);
    
    fclose(org_fileh);
    fclose(work_fileh);
    
    remove(tmp_filepath);
    return YES;
}

EJQDocument *createQDocument(char *qdocpath)
{
    EJQInfo *qinfo = (EJQInfo *)malloc(sizeof(EJQInfo));
    qinfo->groupCharId = 0;
    qinfo->memlist = ejCreateList();
    qinfo->qdoc.reserved1 = qinfo;
    BOOL ret = readQDcoument(qdocpath, qinfo, NO);
    if (ret == NO) {
        if (qinfo == (EJQInfo *)0xb0000000210) {
            printf("qinfo == 0xb0000000210\n");
        }
        free(qinfo);
        return NULL;
    }
    analizeDetail(qinfo);
    return &qinfo->qdoc;
}

EJQDocument *createQDocument2(char *qdocpath)
{
    EJQInfo *qinfo = (EJQInfo *)malloc(sizeof(EJQInfo));
    qinfo->groupCharId = 0;
    qinfo->memlist = ejCreateList();
    qinfo->qdoc.reserved1 = qinfo;
    BOOL ret = readQDcoument(qdocpath, qinfo, YES);
    if (ret == NO) {
        if (qinfo == (EJQInfo *)0xb0000000210) {
            printf("qinfo == 0xb0000000210\n");
        }
        free(qinfo);
        return NULL;
    }
    analizeDetail(qinfo);
    return &qinfo->qdoc;
}

void releaseQDocument(EJQDocument *qdoc)
{
    EJQInfo *qinfo = (EJQInfo *)qdoc->reserved1;
    ejfreeall(qinfo->memlist);
    ejReleaseList(qinfo->memlist);
    if (qinfo == (EJQInfo *)0xb0000000210) {
        printf("qinfo == 0xb0000000210\n");
    }
    free(qinfo);
    
}

