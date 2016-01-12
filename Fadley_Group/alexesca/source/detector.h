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

#define  pAUTO                           1       /* callback function: autoread_panel */
#define  pAUTO_INTERVAL                  2       /* callback function: auto_controls */
#define  pAUTO_UPDATE_STRING             3
#define  pAUTO_PSTS_LED                  4
#define  pAUTO_IRQ_LED                   5
#define  pAUTO_STIO_LED                  6
#define  pAUTO_HALT                      7       /* callback function: auto_controls */
#define  pAUTO_BEGIN                     8       /* callback function: auto_controls */
#define  pAUTO_STATUS_MSG                9
#define  pAUTO_TIMER                     10      /* callback function: auto_timer */
#define  pAUTO_STATUS_BOX                11

#define  pDETECTOR                       2       /* callback function: detector_panel */
#define  pDETECTOR_PCTL                  2       /* callback function: detector_utility */
#define  pDETECTOR_STOP                  3       /* callback function: detector_utility */
#define  pDETECTOR_START                 4       /* callback function: detector_utility */
#define  pDETECTOR_SET_TIMER             5       /* callback function: detector_utility */
#define  pDETECTOR_RESET                 6       /* callback function: detector_utility */
#define  pDETECTOR_READ                  7       /* callback function: detector_utility */
#define  pDETECTOR_CLEAR                 8       /* callback function: detector_utility */
#define  pDETECTOR_PSTS_LED              9
#define  pDETECTOR_IRQ_LED               10
#define  pDETECTOR_STIO_LED              11
#define  pDETECTOR_TIMER_VALUE           12
#define  pDETECTOR_STATUS_INTERVAL       13      /* callback function: status_interval_adjust */
#define  pDETECTOR_DATA                  14
#define  pDETECTOR_UPDATE_STRING         15
#define  pDETECTOR_RESET_MSG             16
#define  pDETECTOR_STATUS_MSG            17
#define  pDETECTOR_STATUS_TIMER          18      /* callback function: status_timer */
#define  pDETECTOR_PERIPH_STATUS_MSG     19
#define  pDETECTOR_IRQ_ST_MSG            20
#define  pDETECTOR_STIO_MSG              21
#define  pDETECTOR_SET_TIMER_MSG         22
#define  pDETECTOR_START_MSG             23
#define  pDETECTOR_STOP_MSG              24
#define  pDETECTOR_STROBE_PERIPH_MSG     25
#define  pDETECTOR_CLEAR_BUFFERS_MSG     26
#define  pDETECTOR_READ_MSG              27
#define  pDETECTOR_BUFFER_MSG            28
#define  pDETECTOR_COMMANDS_MSG          29
#define  pDETECTOR_STATUS_BOX            30
#define  pDETECTOR_COMMANDS_BOX          31
#define  pDETECTOR_BUFFER_BOX            32

#define  pDETSENS                        3
#define  pDETSENS_EXIT                   2       /* callback function: sensitivity_controls */
#define  pDETSENS_RESET                  3       /* callback function: sensitivity_controls */
#define  pDETSENS_HALT                   4       /* callback function: sensitivity_controls */
#define  pDETSENS_BEGIN                  5       /* callback function: sensitivity_controls */
#define  pDETSENS_GRAPH                  6
#define  pDETSENS_DECORATION             7

#define  pMONITOR                        4       /* callback function: monitor_panel */
#define  pMONITOR_GRAPH                  2
#define  pMONITOR_FIXSCALE               3       /* callback function: detmonitor_control */
#define  pMONITOR_AUTOSCALE              4       /* callback function: detmonitor_control */
#define  pMONITOR_DOWN                   5       /* callback function: detmonitor_control */
#define  pMONITOR_UP                     6       /* callback function: detmonitor_control */
#define  pMONITOR_TOTALCOUNTS            7


     /* Menu Bars, Menus, and Menu Items: */

#define  mAUTO                           1
#define  mAUTO_CONTROLS                  2
#define  mAUTO_CONTROLS_MANUAL           3       /* callback function: auto_menu */
#define  mAUTO_CONTROLS_MONITOR          4       /* callback function: auto_menu */
#define  mAUTO_CONTROLS_SEPARATOR        5
#define  mAUTO_CONTROLS_CLOSE            6       /* callback function: auto_menu */
#define  mAUTO_HELP                      7
#define  mAUTO_HELP_PANEL                8       /* callback function: auto_menu */
#define  mAUTO_HELP_GENERAL              9       /* callback function: auto_menu */

#define  mDETECTOR                       2
#define  mDETECTOR_CONTROLS              2
#define  mDETECTOR_CONTROLS_AUTO         3       /* callback function: detector_menu */
#define  mDETECTOR_CONTROLS_MONITOR      4       /* callback function: detector_menu */
#define  mDETECTOR_CONTROLS_SEPARATOR    5
#define  mDETECTOR_CONTROLS_CLOSE        6       /* callback function: detector_menu */
#define  mDETECTOR_HELP                  7
#define  mDETECTOR_HELP_PANEL            8       /* callback function: detector_menu */
#define  mDETECTOR_HELP_GENERAL          9       /* callback function: detector_menu */


     /* Callback Prototypes: */ 

int  CVICALLBACK autoread_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK auto_controls(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK auto_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK auto_timer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK detector_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK detector_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK detector_utility(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK detmonitor_control(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK monitor_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK sensitivity_controls(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK status_interval_adjust(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK status_timer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
