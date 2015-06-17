//
//  ej_malloc.cpp
//  QAnalyzer
//
//  Created by Neo Angel on 13. 12. 16..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "ej_malloc.h"


char *ejmalloc(unsigned long size, EJList *memlist)
{
    if (size == 0) {
        return NULL;
    }
    char *retptr = (char *)malloc(size);
    if (retptr) {
        bzero((void *)retptr, (unsigned long)size);
        ejAddValue(memlist, retptr);
    }
    return retptr;
}


void ejfree(char *ptr,EJList *memlist)
{
    ejRemoveValue(memlist, ptr);
    free(ptr);
}

void ejfreeall(EJList *memlist)
{
    EJNode *node = memlist->start;    
    while (node) {
        free(node->value);
        node = node->next;
    }
//    ejReleaseList(memlist);
}
