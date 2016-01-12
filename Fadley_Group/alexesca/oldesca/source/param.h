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

#define  pPARAM                          1       /* callback function: Parameters_panel */
#define  pPARAM_CANCEL                   2       /* callback function: param_control_buttons */
#define  pPARAM_STARTEXP                 3       /* callback function: param_control_buttons */
#define  pPARAM_DESCRIPTION              4
#define  pPARAM_REGION_AVAIL_CUT         5       /* callback function: param_region */
#define  pPARAM_REGION_AVAIL_NEW         6       /* callback function: param_region */
#define  pPARAM_REGION_SELECT_ADD        7       /* callback function: param_region */
#define  pPARAM_SELECTED_REGIONS         8       /* callback function: parameters_region_selected_list */
#define  pPARAM_AVAILABLE_REGIONS        9       /* callback function: parameters_region_available_list */
#define  pPARAM_REGIONS_SELECTED_MS_2    10
#define  pPARAM_N_SELECTED               11
#define  pPARAM_N_AVAIL                  12
#define  pPARAM_WAITBETWEEN              13
#define  pPARAM_TEXTMSG                  14
#define  pPARAM_TIMER                    15      /* callback function: param_show_region_parameters */
#define  pPARAM_DECORATION               16
#define  pPARAM_DECORATION_12            17
#define  pPARAM_DECORATION_11            18

#define  pREGION                         2       /* callback function: region_panel */
#define  pREGION_DESCRIPTION             2       /* callback function: region_check_param */
#define  pREGION_BE_CENTER               3       /* callback function: region_check_param */
#define  pREGION_BE_WIDTH                4       /* callback function: region_check_param */
#define  pREGION_TIME_STEP               5       /* callback function: region_check_param */
#define  pREGION_SCANS                   6       /* callback function: region_check_param */
#define  pREGION_SCAN_MODE_DITHERED      7       /* callback function: region_check_param */
#define  pREGION_SCAN_MODE_FIXED         8       /* callback function: region_check_param */
#define  pREGION_STEP_MULT               9       /* callback function: region_check_param */
#define  pREGION_OK                      10      /* callback function: region_command_buttons */
#define  pREGION_DONE                    11      /* callback function: region_command_buttons */
#define  pREGION_KE_WIDTH                12
#define  pREGION_EXP_CHANNELS            13
#define  pREGION_KE_CENTER               14
#define  pREGION_BE_FINAL                15
#define  pREGION_BE_INIT                 16
#define  pREGION_BE_MAX                  17
#define  pREGION_BE_MIN                  18
#define  pREGION_KE_MIN                  19
#define  pREGION_BE_STEP                 20
#define  pREGION_KE_FINAL                21
#define  pREGION_KE_INIT                 22
#define  pREGION_KE_MAX                  23
#define  pREGION_TEXTMSG_2               24
#define  pREGION_BE_TITLE                25
#define  pREGION_KE_TITLE                26
#define  pREGION_KE_BOX                  27
#define  pREGION_PICTURE_9               28
#define  pREGION_PICTURE_10              29
#define  pREGION_TEXTMSG_10              30
#define  pREGION_BE_BOX                  31
#define  pREGION_OTHER_BOX               32
#define  pREGION_OTHER_TITLE             33
#define  pREGION_OTHER_BOX_2             34
#define  pREGION_RES_TITLE               35


     /* Menu Bars, Menus, and Menu Items: */

#define  mPARAM                          1
#define  mPARAM_PANEL                    2
#define  mPARAM_PANEL_EXIT               3       /* callback function: param_menu */
#define  mPARAM_HELP                     4
#define  mPARAM_HELP_THIS_PANEL          5       /* callback function: param_menu */
#define  mPARAM_HELP_GENERAL             6       /* callback function: param_menu */

#define  mREGION                         2
#define  mREGION_FILE                    2
#define  mREGION_FILE_OPENFILE           3       /* callback function: region_menu */
#define  mREGION_FILE_EXIT               4       /* callback function: region_menu */
#define  mREGION_DEFINED                 5       /* callback function: region_menu */
#define  mREGION_HELP                    6
#define  mREGION_HELP_PANEL              7       /* callback function: region_menu */
#define  mREGION_HELP_GENERAL            8       /* callback function: region_menu */


     /* Callback Prototypes: */ 

int  CVICALLBACK Parameters_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK parameters_region_available_list(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK parameters_region_selected_list(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK param_control_buttons(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK param_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK param_region(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK param_show_region_parameters(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK region_check_param(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK region_command_buttons(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK region_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK region_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
