//
//  QAnalizer.h
//  QAnalizer
//
//  Created by Neo Angel on 13. 12. 24..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#ifndef QAnalizer_QAnalizer_h
#define QAnalizer_QAnalizer_h
#include "ejtypedefs.h"


typedef struct _EJRGBColor {
    unsigned short red;
    unsigned short green;
    unsigned short blue;
    unsigned short alpha;
} EJRGBColor;

typedef struct _EJCMYKColor {
    unsigned short cyan;
    unsigned short magenta;
    unsigned short yellow;
    unsigned short black;
} EJCMYKColor;

typedef struct _EJColor {
    unsigned short colorID;

    EJColorType colorType; // EJ_COLORTYPE_RGB 일 경우 rgbColor를, EJ_COLORTYPE_CMYK 일 경우에는 cmykColor를 사용
    unsigned char colorName[256];
    EJRGBColor rgbColor;
    EJCMYKColor cmykColor;    
    short trapIndex;
} EJColor;


typedef struct _EJFont {
    unsigned short fontID;
    unsigned char fontName[256];
    unsigned char familyName[256];
} EJFont;

typedef struct _EJDateTime {
    unsigned short year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
} EJDateTime;


typedef struct _EJHNJStyle {
    char hnjName[256];
    unsigned char isAuto; // Hyphen = NO
    int sword;    // HyphenationShortestWord
    int prefix; // HyphenationPrefix
    int suffix; // HyphenationSuffix
    BOOL sepCap; // HyphenateCapitalLetters = 1
    
    BOOL setWord; // BreakByWord = YES
    float minSpc; // MinSpacing
    float optSpc; // Spacing
    float maxSpc; // MaxSpacing
    
    float minChar; // MinChar
    float optChar; // OptChar
    float maxChar; // MaxChar
} EJHNJStyle;

typedef struct _EJTabElement{
    char type;
    char alignOn;
    char leader1;
    char leader2;
    float position;
    
}EJTabElement;


// 단락 상하단에 그려지는 분리 선에 관한 정보들
typedef struct _EJRuler {
    unsigned char lineStyle;
    float lineWidth;
    float shade; //  1.0 == 100%
    unsigned char length; //
    unsigned char unit; //  %
    float leftStart;
    float rightStart;
    float offset; // unit
    EJColor *color;  // colorID를 통해 받아온 컬러값
}EJRuler;

// 문자 스타일 
typedef struct _EJCharStyle {
    char    special;        // Special Process (ejtypedefs.h 참조)
                            // 조판중 특별한 기능을 위한 코드 값,
                            // 대체적으로 이 값이 있으면 다른 스타일 특성들은 의미가 없게됨
    float   track;
    float   fontSize;
    float   colorShade;
    EJColor *fontColor;
    char *fontName;
    EJTextScaleDirection scaleDirection;   // 원래 크기에서 변형된 방향 
    float   scaleValue;      // 원래 크기에서 변형된 크기 비율
    float   baseline;
    float   baselineOffset;
    unsigned short attributeMask; // CharStyle Attribute Mask
    unsigned short anchorID;  // This links to EJGraphic anchorID
    EJTrap trapOption;
    float trapValue;
    
}EJCharStyle;


// ParaStyle Attribute Mask
#define PA_RULE_BELOW           0x02
#define PA_RULE_ABOVE           0x04
#define PA_R_LINESPACE          0x20
#define PA_LOCKTOGRID           0x40


typedef struct _EJParaStyle {
    char *styleName;
    EJCharStyle *charStyle;  //적용되는 문자 스타일
    EJHNJStyle *hnjStyle;
    unsigned int align;         // 정열 옵션
    // Drop Cap (첫 글자 크게 쓰기) 관련 정보
    unsigned char dropCapLine;
    unsigned char dropCapChar;
    unsigned int hnjIdx;        // 적용할 hnj스타일의 인덱스 값
    float lineSpace;            // 행간 값
    float headIndent;           // 단락 앞(왼쪽) 들여쓰기
    float firstLineIndent;      // 첫행 들여쓰기
    float tailIndent;           // 단락 뒷(오른쪽) 들여쓰기
    float spaceBefore;          // 앞 단락과의 간격
    float spaceAfter;           // 다음 단락과의 간격
    
    unsigned char attributeMask; // ParaStyle Attribute Mask
    EJTabElement tabList[20];   // 탭 위치 리스트
    EJRuler ruleAbove;  // if attributeMask & PA_RULE_ABOVE
    EJRuler ruleBelow;  // if attributeMask & PA_RULE_BELOW
    
}EJParaStyle;


typedef struct _EJAttrRange {
    EJCharStyle *charStyle;
    EJParaStyle *paraStyle;
    unsigned short length;
}EJAttrRange;


typedef struct _EJArticle {
    EJAttrRange **attrRangeList;
    unsigned short attrRangeListCount;
    char *string;  // NULL 로 끝나는 문자열
}EJArticle;


typedef struct _EJGraphic {
    EJGraphicType type;
    
    NSRect bounds;  // 그래픽 박스의 좌표정보
    EJColor *color;
    float angle;
    BOOL hFlip;     // 좌우로 뒤집는지
    BOOL vFlip;     // 상하로 뒤집는지
    unsigned short anchorID;  // 이 그래픽이 앵커로 들어갔을 경우 자신의 앵커 ID
    BOOL hasBackgroundColor;
    EJTrap trapOption;
    float trapValue;
    int number;         // Graphic 객체 고유 ID
    
    // 그래픽 객체 박스 선에 대한 정보
    EJColor *lineColor;
    float lineWidth;        // 박스 라인에 대한 굵기
    unsigned char borderType; // 실선, 점선등 선의 모양에 대한 옵션
    short shape;            // 박스 테두리 모양새에 대한 옵션
    short roundType;        // 박스 테두리 라운딩에 대한 옵션
    float round;            // 박스 테두리 라운딩의 굴곡 정도 값
    
    
    // Area
    // 텍스트와 겹쳤을 때 영역 처리를 어떻게 할 것인지에 대한 정보들
    char areaType;
    NSRect areaMargin; // 텍스트와의 경계선에서 마진의 정도
    NSPoint **areaPointList;            // Polygon Area
    unsigned short areaPointListCount;
    NSRect areaBounds;

    // Image
    unsigned char *imagePath;
    EJDateTime modified_time;
    NSRect imageRect;  // graphic 박스 안에 보여질 이미지의 영역 정보
    NSSize imageScale;
    float imageAngle;
    EJColor *maskColor;  // 단색 모노 이미지의 경우 컬러를 지정해 마스킹을 할 수 있음
    float maskScale;
    
    // Blending
    // 그라데이션 관련 정보
    unsigned char blendType;
    EJColor *color2; // 2nd color for blending
    float shade1;       // 기본 칼라에 대한 shade 값
    float shade2;       // color2에 대한 shade 값
    float blendAngle;
    
    // LINE
    // if type is Line 
    NSPoint startPoint;
    NSPoint endPoint;
    unsigned char lineKind;  // 선의 시작 부분을 어떤 타입(모양)으로 할 것인지
    unsigned char lineEnd;  // 선은 끝 부분을 어떤 타입(모양:화살표, 점등)으로 할 것인지

    // TextBox
    EJDirection textDirection;      // 텍스트 방향 : 가로쓰기 or 세로쓰기
    EJArticle article;      // 텍스트 컨텐츠 정보 : 문자열 및 각 범위 별 속성 정보
    EJMargin margin;        // 박스와 텍스트 블럭간의 간격 정보
    unsigned char valign;  // 세로 정렬 옵션 : 상단 맞춤, 중앙 맞춤, 하단 맞춤
    unsigned char columns;  // 텍스트 박스 내 단(컬럼) 수
    float topmargin;    // 상단 마진
    float interWidth;   // 각 컬럼간의 간격
    unsigned short linkIndex;
    struct _EJGraphic *nextBox;  // 현 박스에서 텍스트가 넘쳤을 경우 다음 텍스트 박스 포인터
    struct _EJGraphic *prevBox;  // 이전 텍스트 박스 포인터
    
    //Polygon
    NSPoint **polygonPointList;     // 다각형에 대한 점(Point)들 리스트
    unsigned short polygonPointListCount;
    
    // Group
    struct _EJGraphic **groupItems;     // 이 그래픽 객체가 그릅일 경우 그룹에 속하는 그래픽 리스트
    unsigned short groupItemCount;
    
}EJGraphic;

typedef struct _EJPage {
    unsigned char pageType; // Left or Right
    unsigned short pageNumber;
    unsigned char masterIdx;  //  마스터의 적용을 받은 경우 마스터 페이지의 인덱스 정보를 가짐
    NSRect bounds;
    
}EJPage;



// 한 스프레드 안에는 다수의 페이지가 존재 함
// 스프레드 안의 그래픽 객체들은 페이지와는 상관없이 흩어저 있음
// 보통은 페이지의 영역정보(bounds)와 그래픽의 영역정보를 통해 페이지에 속한 그래픽 객체들을 가려냄
typedef struct _EJSpread {
    char masterName[256];  // 마스터 스프레드일 경우 마스터 명
    EJPage **pageList;  // 스프레드 안에 존재하는 페이지 리스트
    unsigned short pageListCount;  // 스프레드 안에 존재하는 페이지 수
    
    EJGraphic **graphicList;  // 스프레드 안에 존재하는 그래픽 객체 리스트
    unsigned short graphicListCount;  // 스프레드 안에 존재하는 그래픽 객체의 수
}EJSpread;



typedef struct _EJQDocument {
    unsigned short numberOfPages;  // 도큐먼트 페이지 수
    unsigned char language; // 문서를 작성한 어플리케이션의 언어 정보
    unsigned int version; // 문서를 작성한 어플리케이션 버전 표시자
    unsigned long filesize;
    NSSize pageSize;
    float lineStart; // 텍스트 시작점
    float increment;  // 텍스트 라인 증가치
    float autoLeading;  // ???
    int columns;  // 단 수 (기본값)
    float gutter;  // 단간 : 단과 단 사이의 간격 (기본값)
    EJDirection textDirection; // 가로쓰기 / 세로쓰기
    BOOL autoAppendPages;  // 텍스트가 넘치면 페이지가 자동으로 추가되도록 하는 옵션
    BOOL spreadPage;  // 문서가 양면인지 단면인지 여부
    
    unsigned char trapMask; // 그래픽 객체 내의 트랩 정보에 대한 기본설정값 (Trap Enviropemnt Option Bit Mask)
    float autoTrap;         // 자동 트랩값
    float unspecified;      // 불특정
    float overLimit;        // 오버프린트 한계
    
    // 문서내 페이지 마진의 기본(Default) 정보
    float marginTop;
    float marginBottom;
    float marginLeft;
    float marginRight;
    
    // 마스터와 본문의 스프레드(펼친면) 숫자 정보
    unsigned char  masterSpreadCount;
    unsigned short docSpreadCount;
    
    // 문서내에서 사용되어진 컬러 리스트
    EJColor **colorList;
    unsigned short colorListCount;
    
    // 문서내 사용되어진 서체 리스트
    EJFont **fontList;
    unsigned short fontListCount;

    // 단락 스타일 리스트
    EJParaStyle **paraStyleList;
    unsigned short paraStyleListCount;

    EJSpread **spreadList; // 펼친면 리스트 
    unsigned short spreadListCount; //현재는 docSpreadCount 와 같은값임. Master Spread를 읽지 않아서 본문 Spread만 존재함
    
    void *reserved1; // 메모리 관리를 위해 내부적으로 사용하는 포인터
    
}EJQDocument;


EJQDocument *createQDocument(char *qdocpath);
void releaseQDocument(EJQDocument *qdocument);

#endif
