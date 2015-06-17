//
//  ej_qgraphics.h
//  QAnalyzer
//
//  Created by Neo Angel on 13. 12. 15..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#ifndef __QAnalyzer__ej_qgraphics__
#define __QAnalyzer__ej_qgraphics__

//#include <list>
//#include <map>
//#include <iostream>
#include "ej_qparser.h"
#include "ej_list.h"

/***************************************************************************\
 **
 ** constants for xtnewbox types
 **
 \***************************************************************************/


//typedef struct _EJTextLink {
//    std::list<EJTextLinkDict *> linkDictList;
//    std::list<EJGraphicItem *> linkBoxList;
//    std::list<EJGraphicItem *> linkRootList;
//}EJTextLink;
//
typedef struct _EJTextLink {
    EJList *linkDictList;
    EJList *linkBoxList;
    EJList *linkRootList;
}EJTextLink;

long getGraphics(EJQInfo *qinfo, EJGraphicItem **glist, FILE *fileh ,unsigned short gcnt, FILE *orgfh, EJTextLink *textLink, BOOL flag);

#endif /* defined(__QAnalyzer__ej_qgraphics__) */
