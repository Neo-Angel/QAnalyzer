//
//  main.c
//  QATestC
//
//  Created by Neo Angel on 13. 12. 21..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "QAnalyzer.h"
EJQDocument *createQDocument2(char *qdocpath);

int main(int argc, const char * argv[])
{
    // insert code here...
    if (argc == 1) {
        printf("Usage: QATestCPP <QuarkPath>\n");
        return 0;
    }
    EJQDocument *qdoc = createQDocument2((char *)argv[1]);
    if (qdoc) {
        printf("side of void * = %ld\n",sizeof(void *));
        
        printf("===============\n");
        printf("Document Info\n");
        printf("===============\n");
        printf("Number Of Pages :   %d\n",qdoc->numberOfPages);
        printf("Version :           %d\n",qdoc->version);
        printf("Font List :         %d\n",qdoc->fontListCount);
        int i, icnt = qdoc->fontListCount;
        for (i=0; i<icnt; i++) {
            printf("\t%s : %s\n",qdoc->fontList[i]->fontName, qdoc->fontList[i]->familyName);
        }
        printf("Color List :        %d\n",qdoc->colorListCount);
        icnt = qdoc->colorListCount;
        for (i=0; i<icnt; i++) {
            EJColorType color_type = qdoc->colorList[i]->colorType;
            printf("\t%s : %s\n",qdoc->colorList[i]->colorName, EJ_ColorTypeName[color_type]);
        }
        printf("Style List :        %d\n",qdoc->paraStyleListCount);
        icnt = qdoc->paraStyleListCount;
        for (i=0; i<icnt; i++) {
            char *stylename = qdoc->paraStyleList[i]->styleName;
            printf("\t%s[%zd]\n",stylename,strlen(stylename));
        }
        printf("Spread List :        %d\n",qdoc->spreadListCount);
        icnt = qdoc->spreadListCount;
        for (i=0; i<icnt; i++) {
            printf("-------------------------\n");
            
            EJSpread *spread = qdoc->spreadList[i];
            printf("\tPage Count :\t%d\n",spread->pageListCount);
            printf("\tGraphic Count :\t%d\n",spread->graphicListCount);
            int j, jcnt = spread->graphicListCount;
            for (j=0; j<jcnt; j++) {
                EJGraphic *graphic = spread->graphicList[j];
                printf("\t\t%d. %s[%d]\n",j, EJ_GraphicTypeName[graphic->type],graphic->number );
                if (graphic->type == EJ_GTYPE_TEXTBOX) {
                    printf("\t\t    Text - %s\n",graphic->article.string );
                }
                else if (graphic->type == EJ_GTYPE_IMAGEBOX) {
                    printf("\t\t    Path - %s\n",graphic->imagePath );
                    printf("\t\t    Modified - %04d.%02d.%02d - %02d:%02d:%02d\n",
                           graphic->modified_time.year,
                           graphic->modified_time.month,
                           graphic->modified_time.day,
                           graphic->modified_time.hour,
                           graphic->modified_time.minute,
                           graphic->modified_time.second);
                }
                else if (graphic->type == EJ_GTYPE_GROUP) {
                    printf("\t\t    Count - %d\n",graphic->groupItemCount );
                    int k;
                    printf("\t\t    Items - ");
                    for (k=0; k<graphic->groupItemCount; k++) {
                        EJGraphic *gitem = graphic->groupItems[k];
                        printf("%s[%d] ",EJ_GraphicTypeName[gitem->type],gitem->number);
                        
                    }
                    printf("\n");
                }

            }
        }
        
//        printf("HNJ List :          %d\n",qinfo->hnjListCount);
//        printf("Text Attr List :    %d\n",qinfo->textAttrDataListCount);
//        printf("Para List :         %d\n",qinfo->paraAttrDataListCount);
        printf("===========================================================================\n");
        releaseQDocument(qdoc);
    }
    else {
        printf("Cannot open file.\n");
        return -1;
    }
    return 0;
}

