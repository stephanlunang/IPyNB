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

#define  pAPERTURE                       1       /* callback function: aperture_panel */
#define  pAPERTURE_CLOSED_LED            2
#define  pAPERTURE_CHANGING_LED          3
#define  pAPERTURE_OPEN_LED              4
#define  pAPERTURE_CONTROL_RINGSLIDE     5       /* callback function: aperture_control */
#define  pAPERTURE_STATUS_BOX            6
#define  pAPERTURE_STATUS_MSG            7
#define  pAPERTURE_TEXTMSG_2             8
#define  pAPERTURE_TIMER                 9       /* callback function: aperture_timer */
#define  pAPERTURE_TEXTMSG_3             10


     /* Menu Bars, Menus, and Menu Items: */

#define  mAPERTURE                       1
#define  mAPERTURE_UTILITY               2
#define  mAPERTURE_UTILITY_SEPARATOR     3
#define  mAPERTURE_UTILITY_EXIT          4       /* callback function: aperture_menu */
#define  mAPERTURE_HELP                  5
#define  mAPERTURE_HELP_PANEL            6       /* callback function: aperture_menu */
#define  mAPERTURE_HELP_GENERAL          7       /* callback function: aperture_menu */


     /* Callback Prototypes: */ 

int  CVICALLBACK aperture_control(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK aperture_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK aperture_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK aperture_timer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
