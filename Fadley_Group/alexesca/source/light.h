/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 1997. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  pEXCITE                         1
#define  pEXCITE_CANCEL                  2       /* callback function: excite_command */
#define  pEXCITE_OK                      3       /* callback function: excite_command */
#define  pEXCITE_ENERGY                  4       /* callback function: excite_command */
#define  pEXCITE_DECORATION              5
#define  pEXCITE_TEXTMSG                 6


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK excite_command(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
