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

#define  pABOUT                          1
#define  pABOUT_ABOUT_DONE               2       /* callback function: about_panel_done */
#define  pABOUT_TEXTMSG_2                3
#define  pABOUT_TEXTMSG_3                4
#define  pABOUT_TEXTMSG_4                5
#define  pABOUT_TEXTMSG                  6

#define  pHPESCA                         2       /* callback function: esca_panel */
#define  pHPESCA_POWER_SHUTDOWN          2       /* callback function: frontend_controls */
#define  pHPESCA_TIMER                   3       /* callback function: timer_shut_down */
#define  pHPESCA_DECORATION              4
#define  pHPESCA_MESSAGE                 5       /* callback function: frontend_controls */
#define  pHPESCA_POWER_MESSAGE           6       /* callback function: frontend_controls */
#define  pHPESCA_EXCITATION_MESSAGE      7       /* callback function: frontend_controls */

#define  pSHUTDOWN                       3
#define  pSHUTDOWN_CANCEL                2       /* callback function: shutdown */
#define  pSHUTDOWN_OK                    3       /* callback function: shutdown */
#define  pSHUTDOWN_PERIOD                4       /* callback function: shutdown */
#define  pSHUTDOWN_DISABLE               5       /* callback function: shutdown */
#define  pSHUTDOWN_ENABLE                6       /* callback function: shutdown */
#define  pSHUTDOWN_DECORATION            7
#define  pSHUTDOWN_TEXTMSG               8

#define  pSPLASH                         4
#define  pSPLASH_DECORATION              2
#define  pSPLASH_STARTMESSAGE            3
#define  pSPLASH_BACKGNDPICTURE          4
#define  pSPLASH_TEXTMSG_2               5
#define  pSPLASH_TEXTMSG_3               6

#define  pSTARTUP                        5       /* callback function: startup_panel */
#define  pSTARTUP_CHECK_3                2       /* callback function: start_up */
#define  pSTARTUP_CHECK_2                3       /* callback function: start_up */
#define  pSTARTUP_CHECK_1                4       /* callback function: start_up */
#define  pSTARTUP_MSG_1                  5
#define  pSTARTUP_MSG_2                  6
#define  pSTARTUP_MSG_3                  7
#define  pSTARTUP_TEXTMSG                8


     /* Menu Bars, Menus, and Menu Items: */

#define  mHPESCA                         1
#define  mHPESCA_FILE                    2
#define  mHPESCA_FILE_SEPARATOR_3        3
#define  mHPESCA_FILE_EXIT               4       /* callback function: esca_menu */
#define  mHPESCA_UTILITIES               5
#define  mHPESCA_UTILITIES_POWER         6       /* callback function: esca_menu */
#define  mHPESCA_UTILITIES_DET           7
#define  mHPESCA_UTILITIES_DET_SUBMENU   8
#define  mHPESCA_UTILITIES_DET_AUTO      9       /* callback function: esca_menu */
#define  mHPESCA_UTILITIES_DET_MONITOR   10      /* callback function: esca_menu */
#define  mHPESCA_UTILITIES_DET_SEPARATOR_ 11
#define  mHPESCA_UTILITIES_DET_SENS      12      /* callback function: esca_menu */
#define  mHPESCA_UTILITIES_DET_MANUAL    13      /* callback function: esca_menu */
#define  mHPESCA_UTILITIES_EXCITATION    14      /* callback function: esca_menu */
#define  mHPESCA_UTILITIES_SEPARATOR_2   15
#define  mHPESCA_UTILITIES_APERTURE      16      /* callback function: esca_menu */
#define  mHPESCA_UTILITIES_STEPPER       17      /* callback function: esca_menu */
#define  mHPESCA_UTILITIES_AUTOSHUTDOWN  18      /* callback function: esca_menu */
#define  mHPESCA_EXPERIMENT              19
#define  mHPESCA_EXPERIMENT_PARAM        20      /* callback function: esca_menu */
#define  mHPESCA_EXPERIMENT_REGIONS      21      /* callback function: esca_menu */
#define  mHPESCA_HELP                    22
#define  mHPESCA_HELP_GENERAL            23      /* callback function: esca_menu */
#define  mHPESCA_HELP_SEPARATOR          24
#define  mHPESCA_HELP_ABOUT              25      /* callback function: esca_menu */


     /* Callback Prototypes: */ 

int  CVICALLBACK about_panel_done(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK esca_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK esca_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK frontend_controls(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK shutdown(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK startup_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK start_up(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timer_shut_down(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
