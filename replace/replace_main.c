//
//  main.c
//  replace
//
//  Created by Neo Angel on 14. 2. 22..
//  Copyright (c) 2014년 Neo Angel. All rights reserved.
//

#include <stdio.h>
#include "ej_replace_str.h"


int main(int argc, const char * argv[])
{
    if (argc == 1) {
        printf("replace <Quark File Path>\n");
    }

    char *qfilepath = (char *)argv[1];
    
    int count = 3;
    
    char *org_list[3];
    char *new_list[3];
    
    org_list[0] = "coamorous";
    org_list[1] = "MG_7556";
    org_list[2] = "copy";
    
    new_list[0] = "coamor123";
    new_list[1] = "MG_2014";
    new_list[2] = "move";
    
//    int ret = replacStrAtOrg(org_list, new_list, qfilepath, count);
    int ret = replacStrAtNew(org_list, new_list, qfilepath, count);
    if (ret >= 0) {
        printf("replace successfully.\n");
        printf("replace result : %d item(s) replaced\n",ret);
    }
    else {
        printf("error code : %d",ret);
    }

    return ret;
}

