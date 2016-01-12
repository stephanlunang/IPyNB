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

#define  pSTEPPER                        1       /* callback function: stepper_panel */
#define  pSTEPPER_GO                     2       /* callback function: stepper_manual_control */
#define  pSTEPPER_SPEED                  3       /* callback function: stepper_manual_control */
#define  pSTEPPER_DIRECTION              4       /* callback function: stepper_manual_control */
#define  pSTEPPER_LIMIT_2_LED            5
#define  pSTEPPER_LIMIT_1_LED            6
#define  pSTEPPER_JOG_CCW_LED            7
#define  pSTEPPER_JOG_CW_LED             8
#define  pSTEPPER_HANDLE_LED             9
#define  pSTEPPER_ENABLE_PC              10      /* callback function: stepper_manual_control */
#define  pSTEPPER_ENABLE_REMOTE          11      /* callback function: stepper_manual_control */
#define  pSTEPPER_STATUS_MSG             12
#define  pSTEPPER_STATUS_BOX             13
#define  pSTEPPER_DECORATION             14
#define  pSTEPPER_CONTROL_MSG            15
#define  pSTEPPER_CONTROL_BOX            16
#define  pSTEPPER_TIMER                  17      /* callback function: stepper_timer */


     /* Menu Bars, Menus, and Menu Items: */

#define  mSTEPPER                        1
#define  mSTEPPER_UTILITY                2
#define  mSTEPPER_UTILITY_SEPARATOR      3
#define  mSTEPPER_UTILITY_EXIT           4       /* callback function: stepper_menu */
#define  mSTEPPER_HELP                   5
#define  mSTEPPER_HELP_PANEL             6       /* callback function: stepper_menu */
#define  mSTEPPER_HELP_GENERAL           7       /* callback function: stepper_menu */


     /* Callback Prototypes: */ 

int  CVICALLBACK stepper_manual_control(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK stepper_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK stepper_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stepper_timer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
