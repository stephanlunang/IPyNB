/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2011. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  pPOWER                          1       /* callback function: power_panel */
#define  pPOWER_DONE                     2       /* callback function: power_done */
#define  pPOWER_SET_SUPPLY               3       /* callback function: power_set_supply */
#define  pPOWER_PS                       4       /* callback function: power_param */
#define  pPOWER_KE                       5       /* callback function: power_param */
#define  pPOWER_BE                       6       /* callback function: power_param */
#define  pPOWER_PORT5T                   7
#define  pPOWER_PORT5                    8
#define  pPOWER_PORT4                    9
#define  pPOWER_PORT3                    10
#define  pPOWER_DIGIT_4                  11
#define  pPOWER_DIGIT_3                  12
#define  pPOWER_DIGIT_2                  13
#define  pPOWER_DIGIT_1                  14
#define  pPOWER_UPDATE_STRING            15
#define  pPOWER_TEXTMSG                  16
#define  pPOWER_TEXTMSG_3                17
#define  pPOWER_TEXTMSG_2                18
#define  pPOWER_TEXTMSG_5                19
#define  pPOWER_TEXTMSG_4                20
#define  pPOWER_BOX_1                    21
#define  pPOWER_BOX_2                    22
#define  pPOWER_BOX_3                    23


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK power_done(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK power_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK power_param(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK power_set_supply(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
