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

#define  pCONTROL                        1       /* callback function: control_panel */
#define  pCONTROL_CLOSE                  2       /* callback function: scan_panel_controls */
#define  pCONTROL_PAUSE                  3       /* callback function: scan_panel_controls */
#define  pCONTROL_SHOW_DETMONITOR        4       /* callback function: scan_panel_controls */
#define  pCONTROL_SHOW_DATAMONITOR       5       /* callback function: scan_panel_controls */
#define  pCONTROL_DWELL_TIME_2           6
#define  pCONTROL_BE_STEP_2              7
#define  pCONTROL_BE_FINAL_2             8
#define  pCONTROL_BE_INIT_2              9
#define  pCONTROL_EXCITATION_2           10
#define  pCONTROL_CURRENT_BE_OLD         11
#define  pCONTROL_TIME_REGION            12
#define  pCONTROL_TIME_EXP               13
#define  pCONTROL_EXCITATION             14
#define  pCONTROL_N_O_CHANNELS           15
#define  pCONTROL_TIME_P_CHANNEL         16
#define  pCONTROL_DWELL_TIME             17
#define  pCONTROL_BE_STEP                18
#define  pCONTROL_BE_FINAL               19
#define  pCONTROL_BE_INIT                20
#define  pCONTROL_CURRENT_BE             21
#define  pCONTROL_BE_SLIDE               22
#define  pCONTROL_REGION_BOX_MSG         23
#define  pCONTROL_DECORATION             24
#define  pCONTROL_SCAN_MSG               25
#define  pCONTROL_REGION_SCAN_MODE_MSG   26
#define  pCONTROL_REGION_SCAN_MODE_MSG2  27
#define  pCONTROL_DECORATION_3           28
#define  pCONTROL_TIME_BEGIN_MSG         29
#define  pCONTROL_LAYER_MSG              30
#define  pCONTROL_REGION_MSG             31
#define  pCONTROL_REGION_DESCRIPTION     32
#define  pCONTROL_MESSAGE                33
#define  pCONTROL_DECORATION_4           34

#define  pDISPLAY                        2       /* callback function: display_panel */
#define  pDISPLAY_GRAPH                  2

#define  pFINISHED                       3
#define  pFINISHED_OK                    2       /* callback function: finished_ok */
#define  pFINISHED_TOP_MESSAGE           3
#define  pFINISHED_DOGBERT               4
#define  pFINISHED_MESSAGE               5

#define  pLAYERWAIT                      4
#define  pLAYERWAIT_OK                   2
#define  pLAYERWAIT_HELP                 3
#define  pLAYERWAIT_DECORATION           4
#define  pLAYERWAIT_MESSAGE              5
#define  pLAYERWAIT_TEXTMSG              6

#define  pPAUSED                         5
#define  pPAUSED_REDO_REGION             2       /* callback function: pause_choice */
#define  pPAUSED_REDO_EXPERIMENT         3       /* callback function: pause_choice */
#define  pPAUSED_STOP_EXPERIMENT         4       /* callback function: pause_choice */
#define  pPAUSED_CONTINUE                5       /* callback function: pause_choice */
#define  pPAUSED_OK                      6       /* callback function: pause_done */
#define  pPAUSED_HELP                    7
#define  pPAUSED_DECORATION              8
#define  pPAUSED_TEXTMSG                 9

#define  pPOLARWAIT                      6
#define  pPOLARWAIT_OK                   2
#define  pPOLARWAIT_HELP                 3
#define  pPOLARWAIT_DECORATION           4
#define  pPOLARWAIT_TEXTMSG              5
#define  pPOLARWAIT_MESSAGE              6

#define  pRGNWAIT                        7
#define  pRGNWAIT_OK                     2
#define  pRGNWAIT_HELP                   3
#define  pRGNWAIT_ENABLE                 4
#define  pRGNWAIT_DISABLE                5
#define  pRGNWAIT_DECORATION             6
#define  pRGNWAIT_MESSAGE                7
#define  pRGNWAIT_DECORATION_2           8
#define  pRGNWAIT_TEXTMSG                9

#define  pTIMEDWAIT                      8
#define  pTIMEDWAIT_OK                   2
#define  pTIMEDWAIT_HELP                 3
#define  pTIMEDWAIT_DECORATION           4
#define  pTIMEDWAIT_TEXTMSG              5
#define  pTIMEDWAIT_MESSAGE              6
#define  pTIMEDWAIT_TEXTMSG_2            7
#define  pTIMEDWAIT_TIMETOGO             8

#define  pWARNING                        9
#define  pWARNING_OK                     2       /* callback function: warning_panel */
#define  pWARNING_HELP                   3
#define  pWARNING_DECORATION             4
#define  pWARNING_MESSAGE                5


     /* Menu Bars, Menus, and Menu Items: */

#define  mDISPLAY                        1
#define  mDISPLAY_PANEL                  2
#define  mDISPLAY_PANEL_SEPARATOR        3
#define  mDISPLAY_PANEL_CLOSE            4       /* callback function: display_menu */
#define  mDISPLAY_VIEW                   5
#define  mDISPLAY_VIEW_BINDING           6       /* callback function: display_menu */
#define  mDISPLAY_VIEW_KINETIC           7       /* callback function: display_menu */
#define  mDISPLAY_VIEW_CHANNEL           8       /* callback function: display_menu */
#define  mDISPLAY_VIEW_SEPARATOR_3       9
#define  mDISPLAY_VIEW_FLIP              10      /* callback function: display_menu */
#define  mDISPLAY_VIEW_SEPARATOR_2       11
#define  mDISPLAY_VIEW_STYLE             12
#define  mDISPLAY_VIEW_STYLE_SUBMENU     13
#define  mDISPLAY_VIEW_STYLE_THIN_LINE   14      /* callback function: display_menu */
#define  mDISPLAY_VIEW_STYLE_CONNECTED   15      /* callback function: display_menu */
#define  mDISPLAY_VIEW_STYLE_SCATTER     16      /* callback function: display_menu */
#define  mDISPLAY_VIEW_STYLE_VERTICAL_BAR 17     /* callback function: display_menu */
#define  mDISPLAY_VIEW_STYLE_THIN_STEP   18      /* callback function: display_menu */
#define  mDISPLAY_VIEW_POINT             19
#define  mDISPLAY_VIEW_POINT_SUBMENU     20
#define  mDISPLAY_VIEW_POINT_SIMPLE_DOT  21      /* callback function: display_menu */
#define  mDISPLAY_VIEW_POINT_SMALL_X     22      /* callback function: display_menu */
#define  mDISPLAY_VIEW_POINT_EMPTY_SQUARE 23     /* callback function: display_menu */
#define  mDISPLAY_VIEW_POINT_EMPTY_CIRCLE 24     /* callback function: display_menu */

#define  mSHOW                           2
#define  mSHOW_FILE                      2
#define  mSHOW_FILE_LOAD                 3       /* callback function: show_menu */
#define  mSHOW_FILE_PRINT                4       /* callback function: show_menu */
#define  mSHOW_FILE_SEPARATOR            5
#define  mSHOW_FILE_EXIT                 6       /* callback function: show_menu */
#define  mSHOW_REGIONS                   7
#define  mSHOW_REGIONS_AXIS              8       /* callback function: show_menu */
#define  mSHOW_HELP                      9
#define  mSHOW_HELP_PANEL                10      /* callback function: show_menu */
#define  mSHOW_HELP_GENERAL              11      /* callback function: show_menu */


     /* Callback Prototypes: */ 

int  CVICALLBACK control_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK display_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK display_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK finished_ok(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK pause_choice(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK pause_done(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK scan_panel_controls(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK show_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK warning_panel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
