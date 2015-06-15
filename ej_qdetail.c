//
//  ej_qdetail.c
//  QAnalizer
//
//  Created by Neo Angel on 13. 12. 22..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ej_qparser.h"
#include "ej_malloc.h"



int findBoxID(EJTextLinkDict **linkDictList, int count, char *linkID)
{
    int i;
    for (i=0; i<count; i++) {
        EJTextLinkDict *linkDict = linkDictList[i];
        if (strcmp(linkID, linkDict->linkKey) == 0) {
            return linkDict->linkIndex;
        };
    }
    return 0;
}


// same with obove. =.=
int boxIndexForKey(EJQInfo *qinfo, char *keystr)
{
    int i, icnt = qinfo->linkDictListCount;
    for (i=0; i<icnt; i++) {
        EJTextLinkDict *linkDict = qinfo->linkDictList[i];
        if ( strcmp(keystr, linkDict->linkKey) == 0) {
            return linkDict->linkIndex;
        }
    }
    return 0;
}

BOOL containsObject(void **list, int count, void *obj)
{
    int i;
    for (i=0; i<count; i++) {
        if(list[i] == obj)
            return YES;
    }
    return NO;
}

void sortLinkedBoxes(EJQInfo *qinfo)
{
    int i, icnt = qinfo->linkRootListCount;
    for(i=0;i<icnt;i++) {
        EJGraphicItem *graphic = qinfo->linkRootList[i];
        char *data = graphic->data;
        char *linkIDStr = graphic->linkID;
        
        int boxIndex = findBoxID(qinfo->linkDictList, qinfo->linkDictListCount, linkIDStr);
        unsigned char vstr = *((unsigned char *)(data + 0x86));
        EJDirection textDirection;
        if(vstr & 0x80) {
            textDirection = EJ_DIR_VERTICAL;
        }
        else {
            textDirection = EJ_DIR_HORIZONTAL;
        }
        graphic->textDirection = textDirection;
        while(boxIndex > 0) {
            boxIndex --;
            graphic = qinfo->linkBoxList[boxIndex];
            graphic->textDirection = textDirection;
            linkIDStr = graphic->linkID;
            boxIndex = findBoxID(qinfo->linkDictList, qinfo->linkDictListCount, linkIDStr);
        }
    }
}

char *fontNameWithID(EJQInfo *qinfo, int fontID)
{
    int i, icnt = qinfo->qdoc.fontListCount;
    for (i=0; i<icnt; i++) {
        EJFont *fontItem = qinfo->qdoc.fontList[i];
        if (fontItem->fontID == fontID) {
            return (char *)fontItem->fontName;
        }
    }
    return NULL;
}


EJColor *colorItemWithID(EJQInfo *qinfo, int colorIdx)
{
    int i, icnt = qinfo->qdoc.colorListCount;
    for (i=0; i<icnt; i++) {
        EJColor *colorItem = qinfo->qdoc.colorList[i];
        if (colorItem->colorID == colorIdx) {
            return colorItem;
        }
    }
    return NULL;
}

EJColor *colorItemFromOffset(EJQInfo *qinfo, void *offset)
{
    int colorIdx = (int) *((unsigned char *)(offset));
    return colorItemWithID(qinfo, colorIdx);
}

void setCharStyle(EJQInfo *qinfo, char *attr_data, EJCharStyle *charStyle)
{
    unsigned int fontId = (unsigned int)NSSwapBigShortToHost(*((unsigned short *)(attr_data + 2)));
    float tscale = floatValueFrom((unsigned char *)(attr_data + 0x0a));
    unsigned char scale_dir = *((unsigned char *)(attr_data + 0x0f)) & 0x80;
    charStyle->trapOption = *((unsigned char *)(attr_data + 0x0f)) & 0x07;
    float base = floatValueFrom((unsigned char *)(attr_data + 0x1c)); // KOR-
    char *fontName = fontNameWithID(qinfo, fontId);
    unsigned short attrMask = NSSwapBigShortToHost(*((unsigned short *)(attr_data + 4)));
    float fontSize = floatValueFrom((unsigned char *)(attr_data + 6));
    EJColor *colorItem = colorItemFromOffset(qinfo, (void *)(attr_data + 0x0e));
    float fcolorShade = floatValueFrom((unsigned char *)(attr_data + 0x10));
    charStyle->track = floatValueFrom((unsigned char *)(attr_data + 0x18)); // KOR-
    unsigned char special = *((unsigned char *)(attr_data + 0x23)); // KOR-
    charStyle->trapValue = floatValueFrom((unsigned char *)(attr_data + 0x24));
    
    charStyle->special = special;
    charStyle->attributeMask = attrMask;
    charStyle->scaleValue = tscale;
    charStyle->scaleDirection = scale_dir == 0 ? EJ_TEXTSCALE_HORIZONTAL : EJ_TEXTSCALE_VERTICAL;
    charStyle->baseline = base;
    charStyle->baselineOffset = -(base * fontSize);
    charStyle->fontColor = colorItem;
    charStyle->fontName = fontName;
    charStyle->fontSize = fontSize;
    charStyle->colorShade = fcolorShade;
}

void setParaStyle(EJQInfo *qinfo, char *paraData, EJParaStyle *paraDict);

void loadParaStylesDetail(EJQInfo *qinfo, EJStyleItem **styleList, int count)
{
    int i;
    qinfo->qdoc.paraStyleList = (EJParaStyle **)malloc(sizeof(EJParaStyle *) * count);
    for(i=0;i<count;i++) {
        EJStyleItem *styleItem = styleList[i];
        char *styleName = (char *)styleItem->styleName;
        
        char *charData = (char *)styleItem->data;
        EJParaStyle *paraStyle = (EJParaStyle *)ejmalloc(sizeof(EJParaStyle), qinfo->memlist);
        EJCharStyle *charStyle = (EJCharStyle *)ejmalloc(sizeof(EJCharStyle), qinfo->memlist);
        
        setCharStyle(qinfo, charData, charStyle);
        
        char *paraData;
        if(qinfo->qdoc.language == Q_KOREAN) {
            paraData = charData + 0x28; //[NSData dataWithBytes:[charData bytes] + 0x28 length:0x100];
        }
        else {
            paraData = charData + 0x26;//[NSData dataWithBytes:paraData + 0x26 length:0x100];
        }
        setParaStyle(qinfo, paraData, paraStyle);
        paraStyle->charStyle = charStyle;
        paraStyle->styleName = styleName;
        qinfo->qdoc.paraStyleList[i] = paraStyle;
    }
    qinfo->qdoc.paraStyleListCount = count;
    return;
}

void setHNJ(EJQInfo *qinfo, EJParaStyle *paraDict);

typedef struct _TabElement{
    char type;
    char alignOn;
    char leader1;
    char leader2;
    char position[4];  // Fixed Value
    
}TabElement;

void setParaStyle(EJQInfo *qinfo, char *paraData, EJParaStyle *paraStyle)
{
    int i;
    unsigned char attrMask = *((unsigned char *)(paraData+ 2));
    unsigned char ruleAbove = attrMask & 0x04;
    unsigned char ruleBelow = attrMask & 0x02;
//    unsigned char lockToGrid = attrMask & 0x40;
//    unsigned char relativLS = attrMask & 0x20;
    paraStyle->attributeMask = attrMask;
    
    paraStyle->align = (unsigned int)*((unsigned char *)(paraData+ 5));
    paraStyle->dropCapLine = *((unsigned char *)(paraData+ 6));
    paraStyle->dropCapChar = *((unsigned char *)(paraData+ 7));
    paraStyle->hnjIdx = (unsigned int)*((unsigned char *)(paraData+ 12));
    paraStyle->lineSpace = floatValueFrom(((unsigned char *)(paraData+ 0x1a)));
    paraStyle->headIndent = floatValueFrom((unsigned char *)(paraData+ 0x0e));
    paraStyle->firstLineIndent = floatValueFrom((unsigned char *)(paraData+ 0x12));
    paraStyle->tailIndent = floatValueFrom((unsigned char *)(paraData+ 0x016));
    paraStyle->spaceBefore = floatValueFrom((unsigned char *)(paraData+ 0x01e));
    paraStyle->spaceAfter = floatValueFrom((unsigned char *)(paraData+ 0x022));
    TabElement *tabList = ((TabElement *)(paraData+ 0x05a));
    
    for(i=0;i<20;i++) {
        paraStyle->tabList[i].type = tabList[i].type;
        paraStyle->tabList[i].alignOn = tabList[i].alignOn;
        paraStyle->tabList[i].leader1 = tabList[i].leader1;
        paraStyle->tabList[i].leader2 = tabList[i].leader2;
        paraStyle->tabList[i].position = floatValueFrom((unsigned char *)tabList[i].position);
    }
    if(ruleAbove) {
        paraStyle->ruleAbove.lineStyle = *((unsigned char *)(paraData+ 0x2a));
        paraStyle->ruleAbove.lineWidth = floatValueFrom((unsigned char *)(paraData+ 0x26));
        unsigned char colorID = *((unsigned char *)(paraData+ 0x2b));
        paraStyle->ruleAbove.shade = floatValueFrom((unsigned char *)(paraData+ 0x2c)); //  1.0 == 100%
        paraStyle->ruleAbove.length = *((unsigned char *)(paraData+ 2)) & 0x01; // YES : Text
        paraStyle->ruleAbove.unit = *((unsigned char *)(paraData+ 3)) & 0x40; // YES : %
        paraStyle->ruleAbove.leftStart = floatValueFrom((unsigned char *)(paraData+ 0x30));
        paraStyle->ruleAbove.rightStart = floatValueFrom((unsigned char *)(paraData+ 0x34));
        paraStyle->ruleAbove.offset = floatValueFrom((unsigned char *)(paraData+ 0x38)); // unit
        paraStyle->ruleAbove.color = colorItemWithID(qinfo, colorID);
    }
    if(ruleBelow) {
        paraStyle->ruleBelow.lineStyle = *((unsigned char *)(paraData+ 0x40));
        paraStyle->ruleBelow.lineWidth = floatValueFrom((unsigned char *)(paraData+ 0x3c));
        unsigned char colorID = *((unsigned char *)(paraData+ 0x41));
        paraStyle->ruleBelow.shade = floatValueFrom((unsigned char *)(paraData+ 0x42)); //  1.0 == 100%
        paraStyle->ruleBelow.length = *((unsigned char *)(paraData+ 3)) & 0x80; // YES : Text
        paraStyle->ruleBelow.unit = *((unsigned char *)(paraData+ 3)) & 0x20; // YES : %
        paraStyle->ruleBelow.leftStart = floatValueFrom((unsigned char *)(paraData+ 0x46));
        paraStyle->ruleBelow.rightStart = floatValueFrom((unsigned char *)(paraData+ 0x4a));
        paraStyle->ruleBelow.offset =  floatValueFrom((unsigned char *)(paraData+ 0x4e)); // %
        paraStyle->ruleBelow.color = colorItemWithID(qinfo, colorID);
    }
    setHNJ(qinfo, paraStyle);
}

void setHNJ(EJQInfo *qinfo, EJParaStyle *paraStyle)
{
    EJHNJStyle *hnjStyle = (EJHNJStyle *)ejmalloc(sizeof(EJHNJStyle), qinfo->memlist);
    EJHNJItem *hnjItem = qinfo->hnjList[paraStyle->hnjIdx]; //
    strcpy(hnjStyle->hnjName , (char *)hnjItem->hnjName); //[hnjList objectAtIndex:hnjIdx];
    unsigned char *hnjData = hnjItem->data; // [hnjDict objectForKey:hnjName];
    
    hnjStyle->isAuto = *((unsigned char *)(hnjData+ 2)) & 0x01; // Hyphen = NO
    hnjStyle->sword = (unsigned int)*((unsigned char *)(hnjData+ 4));;    // HyphenationShortestWord
    hnjStyle->prefix = (unsigned int)*((unsigned char *)(hnjData+ 5)); // HyphenationPrefix
    hnjStyle->suffix = (unsigned int)*((unsigned char *)(hnjData+ 6)); // HyphenationSuffix
    hnjStyle->sepCap = (*((unsigned char *)(hnjData+ 3)) & 0x01) ? NO : YES; // HyphenateCapitalLetters = 1
    
    hnjStyle->setWord = (*((unsigned char *)(hnjData+ 12)) & 0x02); // BreakByWord = YES
    if(qinfo->qdoc.language == Q_KOREAN ) {
        hnjStyle->minSpc = floatValueFrom((unsigned char *)(hnjData+ 0x10)); // MinSpacing
        hnjStyle->optSpc = floatValueFrom((unsigned char *)(hnjData+ 0x24)); // Spacing
        hnjStyle->maxSpc = floatValueFrom((unsigned char *)(hnjData+ 0x38)); // MaxSpacing
    }
    else {
        hnjStyle->minSpc = floatValueFrom((unsigned char *)(hnjData+ 0x10)); // MinSpacing
        hnjStyle->optSpc = floatValueFrom((unsigned char *)(hnjData+ 0x18)); // Spacing
        hnjStyle->maxSpc = floatValueFrom((unsigned char *)(hnjData+ 0x20)); // MaxSpacing
    }
    
    if(qinfo->qdoc.language == Q_KOREAN /*encoding == 0x80000003*/) { // Korea
        hnjStyle->minChar = floatValueFrom((unsigned char *)(hnjData+ 0x20)); // MinChar
        hnjStyle->optChar = floatValueFrom((unsigned char *)(hnjData+ 0x34)); // OptChar
        hnjStyle->maxChar = floatValueFrom((unsigned char *)(hnjData+ 0x48)); // MaxChar
    }
    else {
        // engmode
        hnjStyle->minChar = floatValueFrom((unsigned char *)(hnjData+ 0x14)); // MinChar
        hnjStyle->optChar = floatValueFrom((unsigned char *)(hnjData+ 0x1c)); // OptChar
        hnjStyle->maxChar = floatValueFrom((unsigned char *)(hnjData+ 0x24)); // MaxChar
    }
    paraStyle->hnjStyle = hnjStyle;
}

void getDocInfo(EJQInfo *qinfo)
{
    unsigned char *bytes = qinfo->header; //(unsigned char *)[data bytes];
    qinfo->qdoc.lineStart = floatValueFrom(bytes + 0x88);
    qinfo->qdoc.increment = floatValueFrom(bytes + 0x8c);
    qinfo->qdoc.autoLeading = floatValueFrom(bytes + 0x7c);
    qinfo->qdoc.pageSize = sizeFrom(bytes+188);
    qinfo->qdoc.autoAppendPages = NO;
    if(bytes[263] & 0x02) {
        qinfo->qdoc.autoAppendPages = YES;
    }
    qinfo->qdoc.spreadPage = NO;
    if(bytes[24] & 0x10) {
        qinfo->qdoc.spreadPage = YES;
    }
    qinfo->qdoc.textDirection = EJ_DIR_HORIZONTAL;
    if(bytes[25] & 0x80) { // H or V
        qinfo->qdoc.textDirection = EJ_DIR_VERTICAL;
    }
    qinfo->qdoc.columns = bytes[91]; // == *(unsigned short *)(bytes + 90)
    qinfo->qdoc.gutter = floatValueFrom(bytes+92);
    qinfo->qdoc.marginTop = floatValueFrom(bytes+74);
    qinfo->qdoc.marginBottom = floatValueFrom(bytes+78);
    qinfo->qdoc.marginLeft = floatValueFrom(bytes+82);
    qinfo->qdoc.marginRight = floatValueFrom(bytes+86);
    
    qinfo->qdoc.trapMask = bytes[0x13c];
    qinfo->qdoc.autoTrap = floatValueFrom(bytes+0x134);
    qinfo->qdoc.unspecified = floatValueFrom(bytes+0x138);
    qinfo->qdoc.overLimit = floatValueFrom(bytes+0x130);
    
}

void pageArrayFromDataArray(EJQInfo *qinfo, EJPageItem **pageItemList, EJPage **pageList, int count)
{
    int j;
    for(j=0;j<count;j++) {
        EJPageItem *pageItem = pageItemList[j];
        EJPage *page = (EJPage *)ejmalloc(sizeof(EJPage), qinfo->memlist);
        char *bytes = pageItem->data;
        
        page->pageType = *((unsigned char *)(bytes + 0x1a)) & 0x10 ? 2 : 1;
        page->pageNumber = (int)NSSwapBigShortToHost(*((unsigned short *)(bytes + 0x20)));
        page->masterIdx = (int)NSSwapBigShortToHost(*((unsigned short *)(bytes + 0x18)));
        page->bounds = pageRectFrom((unsigned char *)(bytes));
        pageList[j] = page;
    }
}

NSPoint **polygonArrayFromData2(EJQInfo *qinfo, char *polyData, int length, unsigned short *count)
{
    int npoints = (length - 0x18) / 8;
    int j;
    NSPoint **pointList = (NSPoint **)ejmalloc(sizeof(NSPoint *)*npoints, qinfo->memlist);
    
    for(j=0;j<npoints;j++) {
        NSPoint *pt = (NSPoint *)ejmalloc(sizeof(NSPoint), qinfo->memlist);
        pt->y= floatValueFrom((unsigned char *)(polyData + 0x10 + j * 8));
        pt->x= floatValueFrom((unsigned char *)(polyData + 0x10 + j * 8 + 4));
        pointList[j] = pt;
    }
    *count = npoints;
    return pointList;
}

NSPoint **polygonArrayFromData1(EJQInfo *qinfo, char *polyData, int length, unsigned short *count)
{
    int npoints = (length - 0x1a) / 8;
    int j;
    NSPoint **pointList = (NSPoint **)ejmalloc(sizeof(NSPoint *)*npoints, qinfo->memlist);
    
    for(j=0;j<npoints;j++) {
        NSPoint *pt = (NSPoint *)ejmalloc(sizeof(NSPoint), qinfo->memlist);
        pt->y= floatValueFrom((unsigned char *)(polyData + 0x12 + j * 8));
        pt->x= floatValueFrom((unsigned char *)(polyData + 0x12 + j * 8 + 4));
        pointList[j] = pt;
    }
    *count = npoints;
    return pointList;
}

#define EJ_DAY_TIME (3600 * 24)

BOOL isLeapYear(short year)
{
    if (year % 4 == 0) {
        if (year % 100 == 0 && year % 400 != 0 ) {
            return NO;
        }
        return YES;
    }
    return NO;
}

short getYearDays(short year)
{
    if (isLeapYear(year)) {
        return 366;
    }
    return 365;
}

unsigned char month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

char getMonthDays(short year, short month)
{
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return month_days[month-1];
}

EJDateTime timeValueFrom(unsigned char *bstr)
{
    EJDateTime retValue;
    unsigned int timeInterval = intValueFrom(bstr);
    retValue.year = 1904;
    retValue.month = 01;
    retValue.day = 01;
    retValue.hour = 0;
    retValue.minute = 0;
    retValue.second = 0;
    
    int days = timeInterval / EJ_DAY_TIME + 1;
    int seconds = timeInterval % EJ_DAY_TIME;
    short cur_year = 1904;
    short year_days = getYearDays(cur_year);
    
    while (days > year_days) {
        days -= year_days;
        cur_year ++;
        year_days = getYearDays(cur_year);
    }
    
    unsigned char cur_month = 1;
    unsigned char month_days = getMonthDays(cur_year, cur_month);
    while (days > month_days) {
        days -= month_days;
        cur_month++;
        month_days = getMonthDays(cur_year, cur_month);
    }
    unsigned char cur_day = days;
    
    unsigned char hour = seconds / 3600;
    
    unsigned char minute = (seconds % 3600) / 60;
    
    unsigned char second = seconds % 60;
    
    retValue.year = cur_year;
    retValue.month = cur_month;
    retValue.day = cur_day;
    retValue.hour = hour;
    retValue.minute = minute;
    retValue.second = second;
    
    return retValue;
}



#define AREA_BASIC     0x00
#define AREA_NO        0x02
#define AREA_AUTO      0x04
#define AREA_MANUAL    0x06


void graphicArrayFromDataArray(EJQInfo *qinfo, EJGraphicItem **graphicItemList, EJGraphic **graphicList, int count)
{
    EJList *groupArray = ejCreateList();
    int i, idx;
    for(i=0;i<count;i++) {
        idx = i;  // count - i - 1;
        EJGraphicItem *gitem = graphicItemList[idx];
        EJGraphic *graphic = gitem->graphic;
        char *bytes = gitem->data;
        graphic->color = colorItemFromOffset(qinfo, (void *)(bytes + 0x01));
        graphic->angle = floatValueFrom((unsigned char *)(bytes + 0x0c));
        graphic->hFlip = *((unsigned char *)(bytes + 0x20)) & 0x01;
        graphic->vFlip= *((unsigned char *)(bytes + 0x21)) & 0x80;
        graphic->anchorID = NSSwapBigShortToHost(*((unsigned short *)(bytes + 0x1e)));
        graphic->bounds = rectFrom((unsigned char *)(bytes + 0x28));
        graphic->type = gitem->type;
        graphic->shape = gitem->shape;
        graphic->roundType = gitem->roundType;
        graphic->lineWidth = floatValueFrom((unsigned char *)(bytes + 0x38));
        graphic->hasBackgroundColor = *((unsigned char *)(bytes + 0x0a)) & 0x80 ? NO : YES;
        graphic->trapOption = *((unsigned char *)(bytes + 0x0b));
        graphic->trapValue = floatValueFrom((unsigned char *)(bytes + 0x18));
        
        graphic->number = gitem->number;
        graphic->imagePath = gitem->imagePath;
        graphic->areaType = gitem ->areaType;
        
        if (graphic->type == EJ_GTYPE_TEXTBOX) {
            char *cArticle = gitem->stringData.data;
            graphic->textDirection = gitem->textDirection;
            char *linkID = gitem->linkID;
            if(cArticle) {
                EJArray *attrDataList = gitem->textAttribute;
                int j, jcnt = attrDataList->count;
                graphic->article.attrRangeList = (EJAttrRange **)ejmalloc(sizeof(EJAttrRange *)*jcnt, qinfo->memlist);
                graphic->article.attrRangeListCount = jcnt;
                for (j=0; j<jcnt; j++) {
                    EJAttribute *attrDataDict = (EJAttribute *)attrDataList->data[j];
                    int orgLen = attrDataDict->length;
                    unsigned short attrIdx = NSSwapBigShortToHost(*((unsigned short *)attrDataDict->textAttr));
                    unsigned short paraIdx = NSSwapBigShortToHost(*((unsigned short *)(attrDataDict->paraAttr)));
                    EJAttrRange *attrRange = (EJAttrRange *)ejmalloc(sizeof(EJAttrRange), qinfo->memlist);
                    attrRange->charStyle = qinfo->charAttrList[attrIdx];
                    attrRange->paraStyle = qinfo->paraAttrList[paraIdx];
                    attrRange->length = orgLen;
                    graphic->article.attrRangeList[j] = attrRange;
                }
                graphic->article.string = cArticle;
            }
            
            graphic->margin = marginFrom((unsigned char *)(bytes + 0x56));
            graphic->columns = *((unsigned char *)(bytes + 0x6e));
            graphic->valign = *((unsigned char *)(bytes + 0x6f));
            graphic->topmargin = floatValueFrom((unsigned char *)(bytes + 0x74));
            if (graphic->textDirection == EJ_DIR_NOTSPECIFIED) {
                if(linkID) {
                    int boxIndex = boxIndexForKey(qinfo, linkID); // [[linkBoxDict objectForKey:linkIDStr] intValue];
                    if(boxIndex > 0 && !containsObject((void **)qinfo->linkBoxList, qinfo->linkBoxListCount, gitem) ){
                        unsigned char vstr = *((unsigned char *)(bytes + 0x86));
                        if(vstr & 0x80) {
                            graphic->textDirection = EJ_DIR_VERTICAL;//[gtextbox setObject:@"SMVerticalBox" forKey:@"Class"];
                        }
                        else {
                            graphic->textDirection = EJ_DIR_HORIZONTAL;///[gtextbox setObject:@"SMGTextBox" forKey:@"Class"];
                        }
                    }
                }
                else {
                    unsigned char vstr = *((unsigned char *)(bytes + 0x86));
                    if(vstr & 0x80) {
                        graphic->textDirection = EJ_DIR_VERTICAL;//[gtextbox setObject:@"SMVerticalBox" forKey:@"Class"];
                    }
                    else {
                        graphic->textDirection = EJ_DIR_HORIZONTAL;///[gtextbox setObject:@"SMGTextBox" forKey:@"Class"];
                    }
                }
            }
            graphic->interWidth = floatValueFrom((unsigned char *)(bytes + 0x52));

            {
                if(linkID) {
                    int boxIndex = boxIndexForKey(qinfo, linkID);
                    if(boxIndex > 0) {
                        boxIndex --;
                        EJGraphicItem *nextgitem = qinfo->linkBoxList[boxIndex];
                        graphic->nextBox = nextgitem->graphic;
                        nextgitem->graphic->prevBox = graphic;
                    }
                }
            }
        }
        if(graphic->hasBackgroundColor) {
            graphic->shade1 = floatValueFrom((unsigned char *)(bytes + 0x02));
            
            char *blendData = gitem->blending; 
            if(blendData) {
                graphic->shade2 = floatValueFrom((unsigned char *)(blendData + 0x12));
                graphic->color2 = colorItemFromOffset(qinfo, (void *)(blendData + 0x10));
                graphic->blendType = *((unsigned char *)(blendData + 0x0b));
                graphic->blendAngle =  floatValueFrom((unsigned char *)(blendData + 0x16));
            }
        }
        if(graphic->lineWidth >= 0) {
            if (graphic->type == EJ_GTYPE_LINE) {
                graphic->lineKind = *((unsigned char *)(bytes + 0x3c));  // lkind
                graphic->lineEnd = *((unsigned char *)(bytes + 0x3d));  // lend
                graphic->startPoint.y = floatValueFrom((unsigned char *)(bytes + 0x28));
                graphic->startPoint.x = floatValueFrom((unsigned char *)(bytes + 0x28 + 4));
                graphic->endPoint.y = floatValueFrom((unsigned char *)(bytes + 0x28 + 8));
                graphic->endPoint.x = floatValueFrom((unsigned char *)(bytes + 0x28 + 12));
            }
            else {
                graphic->borderType = *((unsigned char *)(bytes + 0x41)) & 0x0f;
                graphic->lineColor = colorItemFromOffset(qinfo, (void *)(bytes + 0x40));
            }
        }
        if(strlen((const char *)graphic->imagePath) > 0) {
            graphic->imageAngle = floatValueFrom((unsigned char *)(bytes + 0x5e));
            
            short maskColorIdx = NSSwapBigShortToHost(*((unsigned short *)(bytes + 0x58)));
            if(maskColorIdx > 0) { // ### 090905  if aColor nil skip
                graphic->maskColor = colorItemWithID(qinfo,maskColorIdx);
                if(graphic->maskColor) {
                    graphic->maskScale = floatValueFrom((unsigned char *)(bytes + 0x5a));
                }
            }
            graphic->imageRect.size.height = NSSwapBigShortToHost(*((unsigned short *)(bytes + 0x54)));
            graphic->imageRect.size.width = NSSwapBigShortToHost(*((unsigned short *)(bytes + 0x56)));
            graphic->imageRect.origin = pointFrom((unsigned char *)(bytes + 0x66));
            graphic->imageScale = sizeFrom((unsigned char *)(bytes + 0x6e));
            graphic->modified_time = timeValueFrom((unsigned char *)(gitem->imageInfo+6));
        }
        if(graphic->areaType) {
            graphic->areaMargin = gitem->areaMargin;
            if(graphic->areaType == AREA_AUTO) {
                char *areaPolyData = gitem->areaData; //[gdict objectForKey:@"AreaData"];
            
                graphic->areaPointList = polygonArrayFromData2(qinfo, gitem->areaData, gitem->areaDataLength, &graphic->areaPointListCount);
                
                graphic->areaBounds = rectFrom((unsigned char *)areaPolyData);
            }
            else if(graphic->areaType == AREA_MANUAL) {
                char *areaPolyData = gitem->areaData; //[gdict objectForKey:@"AreaData"];
                graphic->areaPointList = polygonArrayFromData1(qinfo, gitem->areaData, gitem->areaDataLength, &graphic->areaPointListCount);
                graphic->areaBounds = rectFrom((unsigned char *)areaPolyData + 2);
            }
        }
        if(graphic->shape == SH_RECT || graphic->shape == SH_RRECT) {
            if(graphic->shape == SH_RRECT) {
                graphic->round = floatValueFrom((unsigned char *)(bytes + 0x24));
            }
        }
        else if(graphic->shape == SH_POLY) {
            char *polygonData = gitem->polygonData;
            graphic->polygonPointList = polygonArrayFromData1(qinfo, polygonData, gitem->polygonSize, &graphic->polygonPointListCount);
        }
        
        if(graphic->type == EJ_GTYPE_GROUP) {
            ejAddValue(groupArray, gitem);
        }
        
        graphicList[i] = graphic;
    }
    int gcount = groupArray->count;
    EJNode *node = groupArray->start;
    for(i=0;i<gcount;i++) {
        EJGraphicItem *group = node->value;
        char *bytes = group->data;
        int groupbytes = (unsigned int)NSSwapBigShortToHost(*((unsigned short *)(bytes + 0x48)));
        int g, gcnt = groupbytes / 4;
        EJGraphic *graphic = group->graphic;
        graphic->groupItems = NULL;
        graphic->groupItemCount = 0;
        if(gcnt > 0) {
            graphic->groupItems = (EJGraphic **)ejmalloc(sizeof(EJGraphic *)*gcnt, qinfo->memlist);
            
            for(g=0;g<gcnt;g++) {
                int gidx = NSSwapBigIntToHost(*((unsigned int *)(bytes + 0x4a + g * 4)));
                EJGraphic *groupedItem = graphicList[gidx]; // [count-gidx-1];
                graphic->groupItems[g] = groupedItem;
            }
            graphic->groupItemCount = gcnt;
        }
         node = node->next;
    }
}

void getDocumentPages(EJQInfo *qinfo)
{
    int total_page_cnt = 0;
    int masterSpreadCount = qinfo->qdoc.masterSpreadCount;
    int i, icnt;
    icnt = qinfo->spreadListCount - masterSpreadCount;
    if(icnt <= 0) {
        printf("No Document Pages.");
        return;
    }
    qinfo->qdoc.spreadList = (EJSpread **)ejmalloc(sizeof(EJSpread *) * icnt, qinfo->memlist);
    qinfo->qdoc.spreadListCount = icnt;
    for(i=0;i<icnt;i++) {
        EJSpread *spread = (EJSpread *)ejmalloc(sizeof(EJSpread), qinfo->memlist);
        EJSpreadItem *spreadItem = qinfo->spreadList[masterSpreadCount+i];
        EJPageItem **pageItemList = spreadItem->pageList;
        EJGraphicItem **graphicItemList = spreadItem->graphicList;
        
        strncpy(spread->masterName, spreadItem->masterName, 255);
        
        spread->pageListCount = spreadItem->pageListCount;
        spread->pageList = (EJPage **)ejmalloc(sizeof(EJPage *) * spread->pageListCount, qinfo->memlist);
        pageArrayFromDataArray(qinfo, pageItemList, spread->pageList,spread->pageListCount);
        total_page_cnt += spread->pageListCount;
        
        spread->graphicListCount = spreadItem->graphicListCount;
        spread->graphicList = (EJGraphic**)ejmalloc(sizeof(EJGraphic *) * spread->graphicListCount, qinfo->memlist);
        graphicArrayFromDataArray(qinfo, graphicItemList, spread->graphicList, spread->graphicListCount);
        
        qinfo->qdoc.spreadList[i] = spread;
    }
    qinfo->qdoc.numberOfPages = total_page_cnt;
}

void loadAttrList(EJQInfo *qinfo)
{
    int i, icnt = qinfo->textAttrDataListCount;
    qinfo->charAttrList = (EJCharStyle **)ejmalloc(sizeof(EJCharStyle *) * icnt, qinfo->memlist);
    for (i=0; i<icnt; i++) {
        char *data = qinfo->textAttrDataList[i];
        EJCharStyle *charStyle = (EJCharStyle *)ejmalloc(sizeof(EJCharStyle), qinfo->memlist);
        setCharStyle(qinfo, data, charStyle);
        qinfo->charAttrList[i] = charStyle;
    }
    qinfo->charAttrListCount = icnt;

    icnt = qinfo->paraAttrDataListCount;
    qinfo->paraAttrList = (EJParaStyle **)ejmalloc(sizeof(EJParaStyle *) * icnt, qinfo->memlist);
    for (i=0; i<icnt; i++) {
        char *data = qinfo->paraAttrDataList[i];
        EJParaStyle *paraStyle = (EJParaStyle *)ejmalloc(sizeof(EJParaStyle), qinfo->memlist);
        setParaStyle(qinfo, data, paraStyle);
        qinfo->paraAttrList[i] = paraStyle;
    }
    qinfo->paraAttrListCount = icnt;
}

void analizeDetail(EJQInfo *qinfo)
{
    sortLinkedBoxes(qinfo);
    loadAttrList(qinfo);
    loadParaStylesDetail(qinfo, qinfo->styleList, qinfo->styleListCount);
    getDocInfo(qinfo);
    getDocumentPages(qinfo);
    
}
