//
//  ej_replace_str.c
//  QAnalyzer
//
//  Created by Neo Angelon 14. 2. 21..
//  Copyright (c) 2014년 Neo Angel. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int findandreplace(char *find, char *replace, char *content, unsigned long size)
{
    int ret = 0;
    unsigned long find_len = strlen(find);
    unsigned long loc = 0;
    while (loc + find_len < size) {
        int res = memcmp(content+loc, find, find_len);
        while (res !=0 && loc + find_len < size) {
            loc ++;
            res = memcmp(content+loc, find, find_len);
        }
        if (res == 0) {
            memcpy(content+loc, replace, find_len);
            loc = loc + find_len;
            ret ++;
        }
    }
    return ret;
}


int replacStrAtOrg(char **strList1, char **strList2, char *path, int count)
{
    int ret = 0;
    unsigned long filesize;
    char *content;
    FILE *org_fileh = fopen(path, "r");
    if (org_fileh == NULL)
        return -1;
    fseek(org_fileh, 0L, SEEK_END);
    filesize = ftell(org_fileh);
    rewind(org_fileh);
    content = malloc(filesize);
    fread(content, 1, filesize, org_fileh);
    fclose(org_fileh);
    
    int i;
    for (i=0; i<count; i++) {
        char *str_org = strList1[i];
        char *str_new = strList2[i];
        if (strlen(str_org) == 0 || strlen(str_new) == 0) {
            ret = -2;
            break;
        }
        if (strlen(str_org) != strlen(str_new)) {
            ret = -3;  // different string length
            break;
        }
        int times = findandreplace(str_org, str_new, content, filesize);
        if (times < 0) {
            ret = -4;
            break;
        }
        printf("> replace '%s' %d times.\n",str_org, times);
        ret += times;
    }
    
    if (ret > 0) {
        FILE *new_fileh = fopen(path, "w+");
        if (new_fileh == NULL) {
            free(content);
            return -5;
        }
        size_t nbytes = fwrite(content, 1, filesize, new_fileh);
        if (nbytes != filesize) {
            free(content);
            return -6;
        }
        fclose(new_fileh);
    }
    free(content);

    return ret;
}

int replacStrAtNew(char **strList1, char **strList2, char *path, int count)
{
    int ret = 0;
    unsigned long filesize;
    char *content;
    FILE *org_fileh = fopen(path, "r");
    if (org_fileh == NULL)
        return -1;
    fseek(org_fileh, 0L, SEEK_END);
    filesize = ftell(org_fileh);
    rewind(org_fileh);
    content = malloc(filesize);
    fread(content, 1, filesize, org_fileh);
    fclose(org_fileh);
    
    int i;
    for (i=0; i<count; i++) {
        char *str_org = strList1[i];
        char *str_new = strList2[i];
        if (strlen(str_org) == 0 || strlen(str_new) == 0) {
            ret = -2;
            break;
        }
        if (strlen(str_org) != strlen(str_new)) {
            ret = -3;  // different string length
            break;
        }
        int times = findandreplace(str_org, str_new, content, filesize);
        if (times < 0) {
            ret = -4;
            break;
        }
        printf("> replace '%s' %d times.\n",str_org, times);
        ret += times;
    }
    
    if (ret > 0) {
        char *newpath = malloc(strlen(path) + 10);
        sprintf(newpath, "%s_new",path);
        
        FILE *new_fileh = fopen(newpath, "w+");
        if (new_fileh == NULL) {
            free(content);
            return -5;
        }
        
        size_t nbytes = fwrite(content, 1, filesize, new_fileh);
        if (nbytes != filesize) {
            free(content);
            return -6;
        }
        fclose(new_fileh);
        free(newpath);
    }
    free(content);
    
    return ret;
}


