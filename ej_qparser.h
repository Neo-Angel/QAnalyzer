//
//  ej_qparser.h
//  QAnalyzer
//
//  Created by Neo Angel on 13. 12. 11..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#ifndef __QAnalyzer__ej_qparser__
#define __QAnalyzer__ej_qparser__

#include "ej_qutils.h"
#include "ej_list.h"
#include "QAnalyzer.h"


#define STYLE_LEN   (16 * 19 + 4)
#define HNJDATA_LEN_KR  0x54
#define HNJDATA_LEN_EN  0x30
#define MAX_HNJDATA_LEN     HNJDATA_LEN_KR

#define TEXTATTRDATA_LEN_KR     0x30
#define TEXTATTRDATA_LEN_EN     0x2e


typedef struct _EJAttribute {
    char paraAttr[4];
    char textAttr[4];
    unsigned int length;
}EJAttribute;

typedef struct _EJData {
    char *data;
    unsigned int length;
} EJData;

typedef struct _EJArray {
    void **data;
    unsigned int count;
}EJArray;

typedef struct _EJTextLinkDict {
    int linkIndex;
    char linkKey[20];
}EJTextLinkDict;

typedef struct _EJStyleItem {
    unsigned char styleName[256];
    unsigned char data[STYLE_LEN];
} EJStyleItem;

typedef struct _EJHNJItem {
    unsigned char hnjName[256];
    unsigned char data[MAX_HNJDATA_LEN];
} EJHNJItem;

typedef struct _EJPageItem {
    char data[0x3c];
    
} EJPageItem;


typedef struct _EJTextAttr {
    char attrData[4];
    short   attrLength;
}EJTextAttr;

typedef struct _EJParaAttr {
    char attrData[4];
    short   attrLength;
}EJParaAttr;

//  Graphic
///////////////////////////
typedef struct _EJGraphicItem {
//    char *gdata;
//    unsigned long dataSize;
    
    char type;
    char *blending;
    short roundType;
    short shape;
    int number;
    
    char *data;
    int datalen;
    
    short polygonSize;
    char *polygonData;
    int hasImage;
    char imageInfo[0x20];
    unsigned char imagePath[256];
    char areaType;
    NSRect areaMargin;
    
    char *areaData;
    unsigned short areaDataLength;
    
    char *linkID;
    unsigned short linkIndex;
    
//    char *textString;
//    short textStringLen;
    
    EJData stringData;
    EJArray *textAttribute;
    
    EJDirection textDirection;
    
    EJGraphic *graphic;
} EJGraphicItem;
///////////////////////////

typedef struct _EJSpreadItem {
    char head[4];
    char masterName[256];
    EJPageItem **pageList;
    unsigned short pageListCount;
    
    EJGraphicItem **graphicList;
    unsigned short graphicListCount;
}EJSpreadItem;


typedef struct _EJQInfo {
    unsigned short idxLen;
    unsigned short curDocKey;
    unsigned short incKey;
    unsigned int ejgid;
    unsigned int groupCharId;
    
    unsigned char header[0x200];
    
    EJStyleItem **styleList;
    unsigned short styleListCount;
    
    // 문서내에서 사용되어진 컬러 리스트
    char **colorDataList;
    unsigned short colorDataListCount;

    EJHNJItem **hnjList;
    unsigned short hnjListCount;
//=========================================================
    char **textAttrDataList;
    unsigned short textAttrDataLength;
    unsigned short textAttrDataListCount;
    
    char **paraAttrDataList;
    unsigned short paraAttrDataLength;
    unsigned short paraAttrDataListCount;
    
    EJCharStyle **charAttrList;
    unsigned short charAttrListCount;
    
    EJParaStyle **paraAttrList;
    unsigned short paraAttrListCount;
//=========================================================
    
    
    EJSpreadItem **spreadList;
    unsigned short spreadListCount;
    
    EJTextLinkDict **linkDictList;
    unsigned short linkDictListCount;
    
    EJGraphicItem **linkBoxList;
    unsigned short linkBoxListCount;
    
    EJGraphicItem **linkRootList;
    unsigned short linkRootListCount;
    
    char *block1;
    unsigned short block1Length;
    char *block2;
    unsigned short block2Length;

    EJList *memlist;
    
    EJQDocument qdoc;
    
} EJQInfo;


BOOL isQuarkFile(char *qpath ,unsigned char *language, unsigned int *qver);

void startAnalize();


#endif /* defined(__QAnalyzer__ej_qparser__) */
