//
//  ej_list.h
//  QAnalizer
//
//  Created by Neo Angel on 13. 12. 22..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#ifndef QAnalizer_ej_list_h
#define QAnalizer_ej_list_h



typedef struct _EJNode {
    void *next;
    void *prev;
    void *value;
    
}EJNode;


typedef struct _EJList {
    EJNode *start;
    EJNode *end;
    int count;
}EJList;


EJList *ejCreateList();
void ejAddValue(EJList *list, void *value);
void ejRemoveValue(EJList *list, void *value);
void ejReleaseList(EJList *);

#endif
