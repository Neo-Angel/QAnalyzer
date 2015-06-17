//
//  ej_qgraphics.cpp
//  QAnalyzer
//
//  Created by Neo Angel on 13. 12. 15..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//
#include <stdlib.h>
#include <string.h>
#include "ej_qgraphics.h"
#include "ej_malloc.h"

#define LINE 0
#define ORTHLINE 1
#define SPREADBOX 2
#define TEXTBOX 3
#define USERBOX 4
#define RECTBOX 5
#define RRECTBOX 6
#define OVALBOX 7
#define POLYBOX 8
#define REGIONBOX 9
#define ARCBOX 10
#define GROUPBOX 11
#define PICBOX 12						/* (Everything from here on is really a picture) */
#define RRECTPICBOX 13
#define OVALPICBOX 14
#define POLYPICBOX 15
#define REGIONPICBOX 16				/* Reserved for future use */
#define SPLINEPICBOX 17				/* Reserved for future use */
#define NOBOX 32767					/* No box selected (for XTensions) */

/* Item shapes */
#define SH_LINE 0
#define SH_ORTHLINE 1
#define SH_RECT 2
#define SH_RRECT 3
#define SH_OVAL 4
#define SH_POLY 5
#define SH_BEZIER 6
/* Item content types */
#define CT_SPREAD 0
#define CT_GROUP 1
#define CT_LINE 2
#define CT_TEXT 3
#define CT_GRAPHIC 4
#define CT_PICT 5
#define CT_USER 6

unsigned char addBytes[6] = {00,00, 00, 00, 03, 02};



char *newDataInsertBytes(char *addBytes, int len, int idx, char *orgData, int *orglen, EJList *memlist)
{
    char *retData = (char *)ejmalloc(len + *orglen,memlist);
    memcpy(retData, orgData, idx);
    memcpy(retData+idx, addBytes, len);
    int left = *orglen - idx;
    memcpy(retData+idx+len, orgData+idx, left);
    if(retData) {
        ejfree(orgData, memlist);
        *orglen = *orglen + len;
    }
    return retData;
}

char *newDataWithoutBlend(char *orgData, int *orglen, unsigned int blendlen, EJList *memlist)
{
    if(blendlen) {
        char *retData = (char *)ejmalloc(*orglen - blendlen, memlist);
        unsigned resume = *orglen - 0x28 - blendlen;
        memcpy(retData, orgData, 0x28);
        memcpy(retData + 0x28, orgData + 0x28 + blendlen, resume);
        if(retData) {
            ejfree(orgData, memlist);
            *orglen = *orglen - blendlen;
        }
        return retData;
    }
    return orgData;
}


char *newDataWithoutPolygon(char *orgData, int *orglen, unsigned int polylen, EJList *memlist)
{
    if(polylen) {
        char *retData = (char *)ejmalloc(*orglen - polylen, memlist);
        unsigned resume = *orglen - 0x84 - polylen;
        memcpy(retData, orgData, 0x84);
        memcpy(retData + 0x84, orgData + 0x84 + polylen, resume);
        if(retData) {
            ejfree(orgData, memlist);
            *orglen = *orglen - polylen;
        }
        return retData;
    }
    return orgData;
}

char *newDataWithoutPath(char *orgData, int *orglen, unsigned int pathlen, EJList *memlist)
{
    if(pathlen) {
        char *retData = (char *)ejmalloc(*orglen - pathlen, memlist);
        unsigned resume = *orglen - 0x90 - pathlen;
        memcpy(retData, orgData, 0x90);
        if (resume) {
            memcpy(retData + 0x90, orgData + 0x90 + pathlen, resume);
        }
        if(retData) {
            ejfree(orgData,memlist);
            *orglen = *orglen - pathlen;
        }
        
        return retData;
    }
    return orgData;
}

char *newDataFrom(char *data, long length, EJList *memlist)
{
    char *retData = (char *)ejmalloc((int)length, memlist);
    memcpy(retData, data, length);
    return retData;
}

char *newAndAppendDataAndFreeOld(char *data1, long data1len, char *data2, long data2len, EJList *memlist)
{
    char *retData = (char *)ejmalloc((int)(data1len + data2len), memlist);
    if (data1) {
        memcpy(retData, data1, data1len);
        ejfree(data1,memlist);
    }
    memcpy(retData + data1len, data2, data2len);
    ejfree(data2,memlist);
    return retData;
}

char *takeAreaDataFromFile(EJQInfo *qinfo, FILE *orgh, long offset, unsigned int len , long *ret_len)
{
    char *retData = NULL;
    int nextblock;
    char *data;
    BOOL iseof = NO;
    int _idxLen = qinfo->idxLen;
    int cur_data_len = 0;
    unsigned int datalen = 0x100 - _idxLen; //0xfc;
    fseek(orgh, offset, SEEK_SET);
    while(!iseof) {
        long readlen;
        data = newAndReadDataOfLength(orgh, datalen, &readlen, qinfo->memlist);
        retData = newAndAppendDataAndFreeOld(retData, cur_data_len, data, readlen, qinfo->memlist);
        cur_data_len += readlen;
        if(readlen < datalen) {
            iseof = YES;
        }
        if(len > 0 && readlen >= len) {
            iseof = YES;
        }
        if(!iseof) {
            if(_idxLen == 4)
                nextblock = intValueFromNext(orgh);
            else
                nextblock = signedShortValueFromNext(orgh);
            if(nextblock < 0) {
                unsigned long long nextoff;
                unsigned short blockcnt;
                nextblock = -nextblock;
                nextoff = (nextblock-1) * 0x100;
                blockcnt = shortValueFromOffset(orgh, nextoff);
                datalen = blockcnt * 0x100 - 2 - _idxLen;
            }
            else if(nextblock > 0){
                unsigned long long nextoff;
                nextoff = (nextblock-1) * 0x100;
                fseek(orgh, nextoff, SEEK_SET);
                datalen = 0x100 - _idxLen;
            }
            else {
                iseof = YES;
            }
        }
    }
    if (ret_len) {
        *ret_len = cur_data_len;
    }
    return retData;
}

char *textAttributeData(EJQInfo *qinfo, FILE *orgfh, int textIdx , long *ret_len)
{
    
    unsigned long long offset = (textIdx-1) * 0x100;
    
    char *attrData = takeAreaDataFromFile(qinfo, orgfh, offset ,0, ret_len);
    return attrData;
}

char *textStringDataFrom(EJQInfo *qinfo, FILE *orgfh ,char *bytes, int *retDataLen)
{
//    const void *bytes = [textAttr bytes];
    unsigned int len = CFSwapInt32(*((int *)(bytes+4)));
    unsigned int i, icnt;
    char *retData = NULL;
    *retDataLen = 0;
    if(len > 10000000) {
		printf("Text length was over 10000000.");
        //		int ret = NSRunAlertPanel([self localString:@"Alert"], [self localString:@"Unrecognized data. Continue?"],[self localString: @"OK"],[self localString:@"Skip"], nil);
        
        //		if(!ret)
        return NULL;
		
    }
    if(qinfo->idxLen == 4) {
		icnt = len / 6;
		for(i=0;i<icnt;i++) {
			char *valueData;
			unsigned int addr = 4 + 4 + i * 6;
			unsigned int idx = CFSwapInt32(*((int *)(bytes + addr)));
			unsigned short textlen = CFSwapInt16(*((unsigned short *)(bytes + addr + 4)));
			unsigned long long offset = (idx-1) * 0x100;
            fseek(orgfh, offset, SEEK_SET);
            valueData = (char *)ejmalloc(textlen+1, qinfo->memlist);
            fread(valueData, 1, textlen, orgfh);
            valueData[textlen] = 0;
            retData = newAndAppendDataAndFreeOld(retData, *retDataLen, valueData, textlen+1, qinfo->memlist);
            *retDataLen += textlen;
		}
    }
    else {
		icnt = len / 4;
		for(i=0;i<icnt;i++) {
			char *valueData;
			unsigned int addr = 4 + 4 + i * 4;
			unsigned short idx = CFSwapInt16(*((unsigned short *)(bytes + addr)));
			unsigned short textlen = CFSwapInt16(*((unsigned short *)(bytes + addr + 2)));
			unsigned long long offset = (idx-1) * 0x100;
            fseek(orgfh, offset, SEEK_SET);
            valueData = (char *)ejmalloc(textlen+1, qinfo->memlist);
            fread(valueData, 1, textlen, orgfh);
            valueData[textlen] = 0;
            retData = newAndAppendDataAndFreeOld(retData, *retDataLen, valueData, textlen+1, qinfo->memlist);
            *retDataLen += textlen;
		}
    }
    
    return retData;
}

EJTextAttr **separatedAttributesFrom(EJQInfo *qinfo, char *bytes, int *ret_count)
{
    unsigned int blockLen = CFSwapInt32(*((int *)(bytes+4)));
    unsigned attrAddr = 4 + 4 + blockLen;
    unsigned int attrLen = CFSwapInt32(*((unsigned int *)(bytes + attrAddr)));
    unsigned int i, icnt;
    icnt = attrLen / 6;
    EJTextAttr **retList = (EJTextAttr **)ejmalloc(sizeof(EJTextAttr *) * icnt, qinfo->memlist);
    attrAddr += 4;
    for(i=0;i<icnt;i++) {
        EJTextAttr *textAttr = (EJTextAttr *)ejmalloc(sizeof(EJTextAttr), qinfo->memlist);
        memcpy(textAttr->attrData, (bytes + attrAddr), 4);
        unsigned short len = CFSwapInt16(*((unsigned short *)(bytes + attrAddr + 4)));
        textAttr->attrLength = len;
        retList[i] = textAttr;
        attrAddr += 6;
    }
    *ret_count = icnt;
    return retList;
    
}


EJParaAttr **separatedParaAttributesFrom(EJQInfo *qinfo, char *bytes, int *ret_count)
{
    unsigned int blockLen = CFSwapInt32(*((int *)(bytes+4)));
    unsigned attrAddr = 4 + 4 + blockLen;
    unsigned int attrLen = CFSwapInt32(*((unsigned int *)(bytes + attrAddr)));
    unsigned int i, icnt;
    attrAddr +=(4 + attrLen);
    attrLen = CFSwapInt32(*((unsigned int *)(bytes + attrAddr)));
    attrAddr += 4;
    if(attrLen > blockLen) {
        //	NSLog(@"separatedParaAttributesFrom: error.");
        //	return nil;
    }
    icnt = attrLen / 6;
    EJParaAttr **retList = (EJParaAttr **)ejmalloc(sizeof(EJParaAttr *) * icnt, qinfo->memlist);
    for(i=0;i<icnt;i++) {
        EJParaAttr *paraAttr= (EJParaAttr *)ejmalloc(sizeof(EJParaAttr), qinfo->memlist);
        memcpy(paraAttr->attrData, (bytes + attrAddr), 4);
        unsigned short len = CFSwapInt16(*((unsigned short *)(bytes + attrAddr + 4)));
        paraAttr->attrLength = len;
        retList[i] = paraAttr;
        
        attrAddr += 6;
    }
    *ret_count = icnt;
    return retList;
    
}

EJArray *mergeAttributes(EJQInfo *qinfo, EJTextAttr **textattrs, int textcnt, EJParaAttr **paraattrs, int paracnt)
{
    int maxcnt = textcnt + paracnt; //textcnt > paracnt ? textcnt : paracnt;
    EJArray *retArray = (EJArray *)ejmalloc(sizeof(EJArray), qinfo->memlist);
    retArray->data = (void **)ejmalloc(sizeof(EJAttribute *)*(maxcnt+1), qinfo->memlist);
    retArray->count = maxcnt;
    int array_cnt = 0;
    
    int i = 0, icnt, j = 0, jcnt, curt = 0, curp = 0, lastp = 0;
    icnt = textcnt;
    jcnt = paracnt;
    while(i < icnt || j < jcnt) {
        EJTextAttr *tdict;
        EJParaAttr *pdict;
        char *tdata;
        char *pdata;
        int newlen;
        if(curp > curt) {
            int tlen;
            tdict = textattrs[i++];
            tlen = tdict->attrLength;
            tdata = tdict->attrData;
            curt += tlen;
        }
        else if(curp < curt){
            int plen;
           pdict = paraattrs[j++];
            plen = pdict->attrLength;
            pdata = pdict->attrData;
            curp += plen;
        }
        else {
            int tlen, plen;
            if(i < icnt) {
                tdict = textattrs[i++];
                tlen = tdict->attrLength;
                tdata = tdict->attrData;
                curt += tlen;
            }
            if(j < jcnt) {
                pdict = paraattrs[j++];
                plen = pdict->attrLength;
                pdata = pdict->attrData;
                curp += plen;
            }
        }
        if(curp > curt) {
            EJAttribute *newdict = (EJAttribute *)ejmalloc(sizeof(EJAttribute), qinfo->memlist);
            // merge Data
            memcpy(newdict->paraAttr, pdata, 4);
            memcpy(newdict->textAttr, tdata, 4);
            
            newlen = curt - lastp;
            newdict->length = newlen;
            retArray->data[array_cnt++] = newdict;
            lastp = curt;
        }
        else {
            EJAttribute *newdict = (EJAttribute *)ejmalloc(sizeof(EJAttribute), qinfo->memlist);
            //newAndAppendDataAndFreeOld
            memcpy(newdict->paraAttr, pdata, 4);
            memcpy(newdict->textAttr, tdata, 4);
            
            newlen = curp - lastp;
            newdict->length = newlen;
            retArray->data[array_cnt++] = newdict;
            lastp = curp;
        }
    }
    if (maxcnt != array_cnt) {
        if (maxcnt > array_cnt) {
            retArray->count = array_cnt;
        }
        else {
            printf("Error: 'maxcnt(%d) != array_cnt(%d)'.", maxcnt, array_cnt);
        }
    }
    return retArray;
}


long getGraphics(EJQInfo *qinfo, EJGraphicItem **glist, FILE *fileh ,unsigned short gcnt, FILE *orgfh, EJTextLink *textLink, BOOL flag)
{
    long startPtr, seekPtr = ftell(fileh);
    long preSeekPtr = 0L;
    unsigned char itemType;
    unsigned char itemShape;
//    NSData *data;
    int i;
    int exception = (qinfo->qdoc.version == 0x003A003A) ? 6 : 0;
    startPtr = seekPtr;
    for(i=0;i<gcnt;i++) {
        unsigned char itemRound = 0;
        EJGraphicItem *gitem = (EJGraphicItem *)ejmalloc(sizeof(EJGraphicItem), qinfo->memlist);
        gitem->graphic = (EJGraphic *)ejmalloc(sizeof(EJGraphic), qinfo->memlist);
        
        gitem->textDirection = EJ_DIR_NOTSPECIFIED;

        int blending;
        unsigned int blendlen = 0;
        unsigned char area;
        if(qinfo->qdoc.version == 0x003A003A ) {
            unsigned char curKey = qinfo->curDocKey;
            unsigned char gtype = charValueFromOffset(fileh,seekPtr);
            gtype = curKey ^ gtype;
            switch(gtype) {
                case LINE:
                case ORTHLINE:
                    itemType = 2;
                    itemShape = 0;
                    break;
                    
                case TEXTBOX:
                case RECTBOX:
                    itemType = 3;
                    itemShape = 2;
                    break;
                case RRECTBOX:
                    itemType = 3;
                    itemShape = 3;
                    break;
                case OVALBOX:
                    itemType = 3;
                    itemShape = 4;
                    break;
                case POLYBOX:
                    itemType = 3;
                    itemShape = 5;
                    break;
                    
                case PICBOX:
                    itemType = 5;
                    itemShape = 2;
                    break;
                case RRECTPICBOX:
                    itemType = 5;
                    itemShape = 3;
                    break;
                case OVALPICBOX:
                    itemType = 5;
                    itemShape = 4;
                    break;
                case POLYPICBOX:
                    itemType = 5;
                    itemShape = 5;
                    break;
                    
                case GROUPBOX:
                    itemType = 1;
                    itemShape = 2;
                    break;
                    
                default:
                    itemType = 5;
                    itemShape = 2;
            }
            addBytes[4] = itemType;
            addBytes[5] = itemShape;
        }
        else {
            fseek(fileh, seekPtr + 0x21, SEEK_SET);
            itemRound = charValueFromNext(fileh) & 0x60;
            itemType = charValueFromNext(fileh);
            itemShape = charValueFromNext(fileh);
        }
        area = charValueFromOffset(fileh, seekPtr + 0x0a) & 0x06;
        blending = intValueFromOffset(fileh, seekPtr + 0x18);
        gitem->blending = NULL;
        if(blending != 0) {
            blendlen = intValueFromOffset(fileh, seekPtr + 0x28 - exception);
            gitem->blending = (char *)ejmalloc(blendlen, qinfo->memlist);
            fread(gitem->blending, 1, blendlen, fileh);
            blendlen += 4;
        }
        gitem->roundType = itemRound;
        gitem->shape = itemShape;
        gitem->number = qinfo->ejgid++;
        if(( itemType == CT_LINE) && (itemShape == 0 || itemShape == 1) ) { // Line
            int lineDataLen = 0x3e - exception + blendlen;
            char *data = (char *)ejmalloc(lineDataLen, qinfo->memlist);
            fseek(fileh, seekPtr, SEEK_SET);
            fread(data, 1, lineDataLen, fileh);
            int datalen = lineDataLen;
            if(exception) 
                data = newDataInsertBytes((char *)addBytes, exception, 0x1e, data, &datalen, qinfo->memlist);
            data = newDataWithoutBlend(data, &datalen, blendlen, qinfo->memlist);
            gitem->data = data;
            gitem->datalen = datalen;
           if (data == NULL) {
                printf("data is NULL.");
            }
            gitem->type = EJ_GTYPE_LINE;
            preSeekPtr = seekPtr;
            seekPtr += lineDataLen;
        }
        else if(itemType == CT_USER /* && itemShape == 2 */) {
            int glen = 0x4e + blendlen - exception;
            fseek(fileh, seekPtr, SEEK_SET);
            char *data = (char *)ejmalloc(glen, qinfo->memlist);
            fread(data, 1, glen, fileh);
            int datalen = glen;
            if(exception)
                data = newDataInsertBytes((char *)addBytes, exception, 0x1e, data, &datalen, qinfo->memlist);
            data = newDataWithoutBlend(data, &datalen, blendlen, qinfo->memlist);
            gitem->data = data;
            gitem->datalen = datalen;
            if (data == NULL) {
                printf("data is NULL.");
            }
            gitem->type = EJ_GTYPE_USERBOX;
            preSeekPtr = seekPtr;
            seekPtr += glen;
        }
        else if((itemType == 0 || itemType == CT_PICT)&& (itemShape == 2 || itemShape == 3 || itemShape ==4 || itemShape == 5)) { // Image Box
            unsigned int hasAdd = 0;
            unsigned int addLen = 0;
            unsigned int areaLen = 0;
            gitem->hasImage = 0;
            int polysize = 0;
            int glen = 0x94 + blendlen - exception;
            if(itemShape == 5) {
                unsigned short npolys;
                unsigned long long polyPtr = seekPtr + glen + 2;
                fseek(fileh, polyPtr, SEEK_SET);
                npolys = shortValueFromNext(fileh);
                char *polyData = (char *)ejmalloc(npolys, qinfo->memlist);
                fread(polyData, 1, npolys, fileh);
                polysize = 4 + npolys;
                gitem->polygonSize = npolys;
                gitem->polygonData = polyData;
            }
            fseek(fileh, seekPtr, SEEK_SET);
            char *data = (char *)ejmalloc(glen, qinfo->memlist);
            fread(data, 1, glen, fileh);
            int datalen = glen;
            if(exception)
                data = newDataInsertBytes((char *)addBytes, exception, 0x1e, data, &datalen, qinfo->memlist);
            data = newDataWithoutBlend(data, &datalen, blendlen, qinfo->memlist);
            gitem->data = data;
            gitem->datalen = datalen;
            if (data == NULL) {
                printf("data is NULL.");
            }
            gitem->type = EJ_GTYPE_IMAGEBOX;
            if(exception) {
                gitem->hasImage = CFSwapInt32(*((unsigned int *)(data + 0x50)));
            }
            else {
                gitem->hasImage = CFSwapInt32(*((unsigned int *)(data + 0x4c)));
            }
            gitem->imagePath[0] = 0;
            if(gitem->hasImage || hasAdd) {
                int imgInfoLen = intValueFromOffset(fileh, seekPtr + glen + polysize);
                glen += imgInfoLen + 4;
                fread(gitem->imageInfo, 1, 0x20, fileh);
                string256ValueFromNext(fileh, gitem->imagePath);
            }
            if(area != AREA_NO) { // Area Procedure
                NSRect areaMargin;
                unsigned int extArea;
                char *areaMarginData = NULL;
                float areaValue = floatValueFrom((unsigned char *)(data + 0x42));
                extArea = CFSwapInt32(*((unsigned int *)(data + 0x46)));
                areaMargin.origin.x = areaValue;  // Left
                areaMargin.origin.y = areaValue; // Top
                areaMargin.size.width = areaValue; // Right
                areaMargin.size.height = areaValue; // Bottom
                if(extArea) {
                    areaLen = intValueFromOffset(fileh, seekPtr + glen + polysize + addLen);
                    areaMarginData = (char *)ejmalloc(areaLen, qinfo->memlist);
                    fread(areaMarginData, 1, areaLen, fileh);
                    areaLen += 4;
                }
                if(area == AREA_BASIC) {
                    if(areaMarginData) {
                        gitem->areaMargin = rectFrom2((unsigned char *)areaMarginData);
                    }
                    gitem->areaType = 0;
                }
                else if(area == AREA_MANUAL) {
                    unsigned int addedAreaLen = intValueFromOffset(fileh, seekPtr + glen + polysize + addLen + areaLen);
                    long readlen;
                    char *areaPolyData = newAndReadDataOfLength(fileh, addedAreaLen, &readlen, qinfo->memlist);
                    gitem->areaType = 6;
                    gitem->areaMargin = areaMargin;
                    gitem->areaData = areaPolyData;
                    gitem->areaDataLength = addedAreaLen;
                    areaLen += addedAreaLen + 4;
                }
                else if(area == AREA_AUTO) {
                    unsigned int areaBlock;
                    areaBlock = CFSwapInt32(*((unsigned int *)(data + 0x90)));
                    if(areaBlock) {
                        unsigned long long polyAreaOffset = (areaBlock-1) * 0x100;
                        unsigned short polyAreaLen = shortValueFromOffset(orgfh, polyAreaOffset + 0x22) * 8;
                        char *areaPolyData = takeAreaDataFromFile(qinfo, orgfh, polyAreaOffset, polyAreaLen + 0x34,0);
                        char *newAreaPolyData = newDataFrom(areaPolyData+0x24, polyAreaLen + 0x10, qinfo->memlist);
                        ejfree(areaPolyData, qinfo->memlist);
//                        areaPolyData = [NSData dataWithBytes:[areaPolyData bytes] + 0x24 length:polyAreaLen + 0x10];
                        
                        gitem->areaType = 4;
                        gitem->areaMargin = areaMargin;
                        gitem->areaData = newAreaPolyData;
                        gitem->areaDataLength = polyAreaLen + 0x10;
                    }
                }
            }
            //  hasAdd = [fileh intValueFromOffset:seekPtr + glen + polysize + areaLen];
            hasAdd = CFSwapInt32(*((unsigned int *)(data + 0x8c)));
            if(hasAdd) {
                addLen = intValueFromOffset(fileh, seekPtr + glen + polysize + areaLen);
                addLen += 4;
            }
            preSeekPtr = seekPtr;
            seekPtr += glen + polysize + addLen + areaLen;
//            ejfree(data, qinfo->memlist);
        } // Image Box
        else if(itemType == CT_TEXT && (itemShape == 2 || itemShape == 3 || itemShape == 4 || itemShape == 5)) {// Text
            unsigned int  textIdx;
            int linkCnt;
            int prevBox;
            int polysize = 0;
            int glen = blendlen;
            int pathChk = 0;
            int blockLen = 0;
            int txtPathLen = 0;
            unsigned int areaLen = 0;
            short textIdxPlus;
            textIdxPlus = signedShortValueFromOffset(fileh, seekPtr+6);
            if(textIdxPlus < 0) {
                textIdxPlus = textIdxPlus * -1 - 1;
            }
            textIdx = shortValueFromOffset(fileh ,seekPtr+8);
            textIdx = textIdx ^ qinfo->curDocKey;
            textIdx += (textIdxPlus * 0x10000);
            int linkID = intValueFromOffset(fileh, seekPtr+ 0x14);  //  Not Used. Why???
            linkCnt = intValueFromOffset(fileh, seekPtr+ blendlen+ 0x78 - exception);
            prevBox = intValueFromNext(fileh);
            
            if(linkCnt || prevBox) {
                EJTextLinkDict *linkDict = (EJTextLinkDict *)ejmalloc(sizeof(EJTextLinkDict), qinfo->memlist);
                sprintf(linkDict->linkKey, "%08x_0x%08x", linkCnt, prevBox);
                linkDict->linkIndex = linkCnt;
                gitem->linkID = linkDict->linkKey;
                gitem->linkIndex = linkCnt;
                
                ejAddValue(textLink->linkDictList, linkDict);
                if(prevBox) {
                    ejAddValue(textLink->linkBoxList, gitem);
                }
                else {
                    ejAddValue(textLink->linkRootList, gitem);
                }
            }
            if(prevBox) {
                blockLen = 0x84 - exception;
            }
            else if(textIdx) {
                long textAttr_len;
                char *textAttr = textAttributeData(qinfo, orgfh, textIdx, &textAttr_len);
                int stringData_len;
                char *stringData = textStringDataFrom(qinfo, orgfh, textAttr, &stringData_len);
                if(stringData) {
                    int attr_count;
                    EJTextAttr **attrList = separatedAttributesFrom(qinfo, textAttr, &attr_count);
                    int para_count;
                    EJParaAttr **paraList = separatedParaAttributesFrom(qinfo, textAttr, &para_count);
                    
                    if(attrList && paraList) {
                        EJArray *textAttrList = mergeAttributes(qinfo, attrList, attr_count, paraList, para_count);
                        gitem->textAttribute = textAttrList;
                        gitem->stringData.data = stringData;
                        gitem->stringData.length = stringData_len;
                    }
                }
                blockLen = 0x90 - exception;
                // Plygon starts at 0x84
            }
            else {
                blockLen = 0x9c - exception;
            }
            if(itemShape == 5) {
                char *polyData;
                unsigned short polyDataLength;
                unsigned long long polyPtr = seekPtr + blendlen + 0x84 + 2 - exception;
                fseek(fileh, polyPtr, SEEK_SET);
                polyDataLength = shortValueFromNext(fileh);
                polyData = (char *)ejmalloc(polyDataLength, qinfo->memlist);
                fread(polyData, 1, polyDataLength, fileh);
                polysize = 4 + polyDataLength;
                gitem->polygonSize = polyDataLength;
                gitem->polygonData = polyData;
            }
            
            if(blockLen == (0x90  - exception) || blockLen == (0x9c - exception)) {
                pathChk = intValueFromOffset(fileh, seekPtr + blendlen + polysize + 0x88 - exception);
                if(pathChk) {
                    txtPathLen = intValueFromOffset(fileh, seekPtr + blendlen + polysize + 0x90 - exception);
                    txtPathLen += 4;
                }
            }
            glen += (blockLen + txtPathLen + polysize);
            fseek(fileh, seekPtr, SEEK_SET);
            char *data = (char *)ejmalloc(glen, qinfo->memlist);
            fread(data, 1, glen, fileh);
            int datalen = glen;
            if(exception) 
                data = newDataInsertBytes((char *)addBytes, exception, 0x1e, data, &datalen, qinfo->memlist);
            data = newDataWithoutBlend(data, &datalen, blendlen, qinfo->memlist);
            data = newDataWithoutPolygon(data, &datalen, polysize, qinfo->memlist);
            data = newDataWithoutPath(data, &datalen, polysize, qinfo->memlist);
            gitem->data = data;
            gitem->datalen = datalen;
            if (data == NULL) {
                printf("data is NULL.");
            }
            gitem->type = EJ_GTYPE_TEXTBOX;
            if(area != AREA_NO) { // Area Procedure
                unsigned int extArea;
                char *areaMarginData = NULL;
                float areaValue = floatValueFrom((unsigned char *)(data + 0x42));
                extArea = CFSwapInt32(*((unsigned int *)(data + 0x46)));
                if(extArea) {
                    areaLen = intValueFromOffset(fileh ,seekPtr + glen);
                    areaMarginData = (char *)ejmalloc(areaLen, qinfo->memlist);
                    fread(areaMarginData, areaLen, 1, fileh);
                    areaLen += 4;
                }
                if(area == AREA_BASIC) {
                    if(areaMarginData) {
                        gitem->areaMargin = rectFrom2((unsigned char *)areaMarginData);
                    }
                    else {
                        gitem->areaMargin.origin.x = areaValue;
                        gitem->areaMargin.origin.y = areaValue;
                        gitem->areaMargin.size.width = areaValue;
                        gitem->areaMargin.size.height = areaValue;
                    }
                    gitem->areaType = 0;
                }
            }
            preSeekPtr = seekPtr;
            seekPtr += glen + areaLen;
        }
        else if(itemType == CT_GROUP) {// Group
            unsigned long long lengthPtr = seekPtr + 0x46 + blendlen - exception;
            int extlen = intValueFromOffset(fileh,lengthPtr);
            int glen = 0x4a + blendlen + extlen - exception;
            fseek(fileh, seekPtr, SEEK_SET);
            char *data = (char *)ejmalloc(glen, qinfo->memlist);
            fread(data, glen, 1, fileh);
            int datalen = glen;
            if(exception) {
                data = newDataInsertBytes((char *)addBytes, exception, 0x1e, data, &datalen, qinfo->memlist);
            }
            data = newDataWithoutBlend(data, &datalen, blendlen, qinfo->memlist);
            gitem->data = data;
            gitem->datalen = datalen;
            
            if (data == NULL) {
                printf("data is NULL.");
            }
            gitem->type = EJ_GTYPE_GROUP;
            
            preSeekPtr = seekPtr;
            seekPtr += glen;
        }
        else {
            printf("Invalid Graphic....%x %x %d : %x %x\n",(unsigned int)preSeekPtr, (unsigned int)seekPtr, i, itemType, itemShape);
        }
//        gitem->dataSize = seekPtr - preSeekPtr;
//        if (flag) {
//            gitem->gdata = ejmalloc(gitem->dataSize, qinfo->memlist);
//            fread(gitem->gdata, 1, gitem->dataSize, fileh);
//        }
        glist[i] = gitem;
//        [glist addObject:gdict];
        qinfo->curDocKey += qinfo->incKey;
    }
    return (seekPtr-startPtr);
}


