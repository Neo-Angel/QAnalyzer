//
//  ej_list.c
//  QAnalyzer
//
//  Created by Neo Angel on 13. 12. 22..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "ej_list.h"

EJList *ejCreateList()
{
    EJList *retlist = (EJList *)malloc(sizeof(EJList));
    retlist->count = 0;
    retlist->start = NULL;
    retlist->end = NULL;
    return retlist;
}

void ejAddValue(EJList *list, void *value)
{
    EJNode *node = (EJNode *)malloc(sizeof(EJNode));
    node->value = value;
    node->prev = list->end;
    node->next = NULL;
    if (list->start == NULL) {
        list->start = node;
    }
    if (list->end) {
        list->end->next = node;
    }
    list->end = node;
    list->count++;
}

void ejRemoveValue(EJList *list, void *value)
{
    EJNode *_prevNode;
    EJNode *__prevNode;
    EJNode *node = list->end;
    int i = 0;
    while (node) {
        if (node->value == value) {
            EJNode *prev_node = node->prev;
            EJNode *next_node = node->next;
            if (prev_node) {
                prev_node->next = next_node;
            }
            if (next_node) {
                next_node->prev = prev_node;
            }
            free(node);
            list->count--;
            break;
        }
        i++;
        __prevNode = _prevNode;
        _prevNode = node;
        node = node->prev;
    }
}

void ejReleaseList(EJList *list)
{
    EJNode *node = list->start;
    while (node) {
        EJNode *next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

