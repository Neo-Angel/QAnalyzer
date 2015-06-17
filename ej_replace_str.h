//
//  ej_replace_str.h
//  QAnalyzer
//
//  Created by Neo Angel on 14. 2. 21..
//  Copyright (c) 2014년 Neo Angel. All rights reserved.
//

#ifndef QAnalyzer_ej_replace_str_h
#define QAnalyzer_ej_replace_str_h


//      Example
/////////////////////////////////////

//    char *qfilepath = "test_qfile";
//
//    int count = 3;
//
//    char *org_list[3];
//    char *new_list[3];
//
//    org_list[0] = "coamorous";
//    org_list[1] = "MG_7556";
//    org_list[2] = "copy";
//
//    new_list[0] = "coamor123";
//    new_list[1] = "MG_2014";
//    new_list[2] = "move";
//
//    int ret = replacStrAtOrg(org_list, new_list, qfilepath, count);
//
//    printf("replace result : %d item(s) replaced\n",ret);

int replacStrAtOrg(char **strList1, char **strList2, char *path, int count); // 원본파일을 수정함
int replacStrAtNew(char **strList1, char **strList2, char *path, int count); // '_new' 를 붙여 복사본에 저장함



#endif
