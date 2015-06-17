//
//  ej_malloc.h
//  QAnalyzer
//
//  Created by Neo Angel on 13. 12. 16..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#ifndef __QAnalyzer__ej_malloc__
#define __QAnalyzer__ej_malloc__
#include "ej_list.h"

char *ejmalloc(unsigned long size,EJList *memlist);
void ejfree(char *ptr, EJList *memlist);
void ejfreeall(EJList *memlist);

#endif /* defined(__QAnalyzer__ej_malloc__) */
