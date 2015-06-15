//
//  ejtypedefs.h
//  QAnalizer
//
//  Created by Neo Angel on 13. 12. 25..
//  Copyright (c) 2013년 Neo Angel. All rights reserved.
//

#ifndef QAnalizer_ejtypedefs_h
#define QAnalizer_ejtypedefs_h


extern char *EJ_GraphicTypeName[];  // 그래픽 타입의 문자열 이름 리스트 : {"Line","Image","Text","Group","UserBox"};

extern char *EJ_ColorTypeName[];  // 컬러 타입의 문자열 이름 리스트 : {"HSB","RGB","CMYK","PANTONE",...};


#define Q_ENGLISH     0x33
#define Q_KOREAN      0x61

// Trap Enviropemnt Option Bit Mask
#define TRAP_ENV_MASK_ABSOLUTE          0x80
#define TRAP_ENV_MASK_IGNOREWHITE       0x40
#define TRAP_ENV_MASK_PROCESS           0x20


#define AREA_BASIC                      0x00
#define AREA_NO                         0x02
#define AREA_AUTO                       0x04
#define AREA_MANUAL                     0x06


// Special Process : EJCharStyle->special 에서 사용
#define SC_RUBI_BEGIN                   0x44
#define SC_RUBI_END                     0x24
#define SC_GROUP_BEGIN                  0x48
#define SC_GROUP_END                    0x28
#define SC_ANCHOR_BEGIN                 0x01
#define SC_ANCHOR_END                   0x80

// CharStyle Attribute Mask : EJCharStyle->attributeMask 에서 사용
#define CA_BOLD                         0x0001
#define CA_ITALIC                       0x0002
#define CA_UNDERLINE                    0x0004
#define CA_OUTLINE                      0x0008
#define CA_SHADOW                       0x0010
#define CA_LIFTUP                       0x0020
#define CA_LIFTDOWN                     0x0040
#define CA_MAKEHALF                     0x0100
#define CA_CENTERLINE                   0x0200
#define CA_BIGK                         0x0400
#define CA_SMALLK                       0x0800
#define CA_WORDLINE                     0x1000


// EJGraphic->shape 에서 사용
#define SH_LINE                         0
#define SH_ORTHLINE                     1
#define SH_RECT                         2
#define SH_RRECT                        3
#define SH_OVAL                         4
#define SH_POLY                         5
#define SH_BEZIER                       6


// BOOL 타입 정의
#ifndef BOOL
#ifdef bool
typedef bool BOOL;
#define YES     true
#define NO      false
#else
typedef int  bool;
typedef int  BOOL;
#define YES 1
#define NO  0
#define true 1
#define false  0
#endif
#endif



// 그래픽의 타입 : EJGraphic->type 에서 사용
typedef enum {
    EJ_GTYPE_LINE,
    EJ_GTYPE_IMAGEBOX,
    EJ_GTYPE_TEXTBOX,
    EJ_GTYPE_GROUP,
    EJ_GTYPE_USERBOX
} EJGraphicType;

// Trap options : EJGraphic->trapOption
typedef enum {
    EJ_TRAP_DEFAULT,
    EJ_TRAP_OVERPRINT,
    EJ_TRAP_KNOCKOUT,
    EJ_TRAP_AUTO_PLUS,
    EJ_TRAP_AUTO_MINUS,
    EJ_TRAP_CUSTOM
} EJTrap;


// Color Space Type : EJColor->colorSpace 에서 사용
typedef enum {
    EJ_COLORTYPE_HSB,
    EJ_COLORTYPE_RGB,
    EJ_COLORTYPE_CMYK,
    EJ_COLORTYPE_PANTONE,
    EJ_COLORTYPE_TOYO,
    EJ_COLORTYPE_DIC,
    EJ_COLORTYPE_TRUMATCH,
    EJ_COLORTYPE_FOCOLTONE,
    EJ_COLORTYPE_PANTONEPROCESS,
    EJ_COLORTYPE_PANTONEPROSIM,
    EJ_COLORTYPE_PANTONEUNCOATED,
    EJ_COLORTYPE_PANTONEANPA,
    EJ_COLORTYPE_PANTONERES1,
    EJ_COLORTYPE_PANTONERES2,
    EJ_COLORTYPE_PANTONERES3
}EJColorType;


// 텍스트 흐름의 방향 타입 : EJQDocument->textDirection, EJGraphic->textDirection 에서 사용
typedef enum {
    EJ_DIR_NOTSPECIFIED,
    EJ_DIR_HORIZONTAL,
    EJ_DIR_VERTICAL
} EJDirection;

typedef enum {
    EJ_TEXTSCALE_HORIZONTAL,
    EJ_TEXTSCALE_VERTICAL
} EJTextScaleDirection;



// 코드 수정양을 줄이기 위해 코코아의 NS... 구조체를 그대로 사용
#ifndef NSPoint
    typedef struct _NSPoint {
        float x;
        float y;
    } NSPoint;

    typedef struct _NSSize {
        float width;		/* should never be negative */
        float height;		/* should never be negative */
    } NSSize;

    typedef struct _NSRect {
        NSPoint origin;
        NSSize size;
    } NSRect;
#endif



typedef struct _EJMargin {
    float left;
    float top;
    float right;
    float bottom;
}EJMargin;



#endif
