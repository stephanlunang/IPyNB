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

#define  pANALYSIS                       1
#define  pANALYSIS_CANCEL                2       /* callback function: quit_analysis */
#define  pANALYSIS_CALCULATE             3       /* callback function: fit_control */
#define  pANALYSIS_BACK_SHIRLEY          4       /* callback function: fit_control */
#define  pANALYSIS_FIT_NONE              5       /* callback function: fit_control */
#define  pANALYSIS_SMOOTH_OFF            6       /* callback function: fit_control */
#define  pANALYSIS_FIT_SPLINE            7       /* callback function: fit_control */
#define  pANALYSIS_SMOOTH_ON             8       /* callback function: fit_control */
#define  pANALYSIS_BACK_NONE             9       /* callback function: fit_control */
#define  pANALYSIS_BACK_LINEAR           10      /* callback function: fit_control */
#define  pANALYSIS_BACK_SHIRLEY_ITER     11      /* callback function: fit_control */
#define  pANALYSIS_SMOOTH_POINTS         12      /* callback function: fit_control */
#define  pANALYSIS_SMOOTH_DEGREE         13      /* callback function: fit_control */
#define  pANALYSIS_TEXTMSG               14
#define  pANALYSIS_TEXTMSG_2             15
#define  pANALYSIS_DECORATION_2          16
#define  pANALYSIS_FWHM_LABEL            17
#define  pANALYSIS_DECORATION_3          18
#define  pANALYSIS_AREA_CNTS             19
#define  pANALYSIS_AREA_CNTS_PSEC        20
#define  pANALYSIS_AREA_CNTS_EV_PSEC     21
#define  pANALYSIS_AREA_CNTS_EV          22
#define  pANALYSIS_FWHM_EV               23
#define  pANALYSIS_WIND_INDEX1           24
#define  pANALYSIS_WIND_INDEX2           25
#define  pANALYSIS_DECORATION_4          26
#define  pANALYSIS_WIND_INDEX1_BE        27
#define  pANALYSIS_WIND_INDEX2_BE        28
#define  pANALYSIS_WIND_INDEX2_KE        29
#define  pANALYSIS_WIND_INDEX1_KE        30
#define  pANALYSIS_DECORATION            31
#define  pANALYSIS_DECORATION_5          32
#define  pANALYSIS_TEXTMSG_3             33
#define  pANALYSIS_PEAK_POSIITON_LABEL   34
#define  pANALYSIS_PEAK_HEIGHT_LABEL     35
#define  pANALYSIS_PEAK_HEIGHT           36
#define  pANALYSIS_AREA_LABEL            37
#define  pANALYSIS_WINDOW_LABEL          38
#define  pANALYSIS_PEAK                  39
#define  pANALYSIS_PEAK_POSITION_BE      40
#define  pANALYSIS_PEAK_POSITION_KE      41
#define  pANALYSIS_PEAK_POSITION_CH      42
#define  pANALYSIS_FWHM_CH               43

#define  pPRINT                          2       /* callback function: print_panel */
#define  pPRINT_REGIONS_CURRENT          2       /* callback function: print_options */
#define  pPRINT_COLOR_BW                 3       /* callback function: print_options */
#define  pPRINT_COLOR_GREYSCALE          4       /* callback function: print_options */
#define  pPRINT_COLOR_COLOR              5       /* callback function: print_options */
#define  pPRINT_OPTIONS_LANDSCAPE        6       /* callback function: print_options */
#define  pPRINT_OPTIONS_PORTRAIT         7       /* callback function: print_options */
#define  pPRINT_INCLUDE_RAWDATA          8       /* callback function: print_options */
#define  pPRINT_INCLUDE_PARAM            9       /* callback function: print_options */
#define  pPRINT_INCLUDE_SPECTRUM         10      /* callback function: print_options */
#define  pPRINT_INCLUDE_BOTH             11      /* callback function: print_options */
#define  pPRINT_INCLUDE_ALL              12      /* callback function: print_options */
#define  pPRINT_REGIONS_ALL              13      /* callback function: print_options */
#define  pPRINT_CANCEL                   14      /* callback function: print_command */
#define  pPRINT_PRINT                    15      /* callback function: print_command */
#define  pPRINT_OPTIONS_COPIES           16      /* callback function: print_options */
#define  pPRINT_DECORATION               17
#define  pPRINT_DECORATION_2             18
#define  pPRINT_DECORATION_3             19
#define  pPRINT_TEXTMSG_2                20
#define  pPRINT_TEXTMSG_3                21
#define  pPRINT_TEXTMSG                  22

#define  pVIEWER                         3
#define  pVIEWER_CHANNELS                2
#define  pVIEWER_BE_STEP                 3
#define  pVIEWER_BE_WIDTH                4
#define  pVIEWER_EXCITATION              5
#define  pVIEWER_BE_CENTER               6
#define  pVIEWER_GRAPH                   7       /* callback function: fit_control */
#define  pVIEWER_TEXTMSG_2               8
#define  pVIEWER_TIME_BEGIN              9
#define  pVIEWER_PATHNAME                10
#define  pVIEWER_E_DESCRIPTION           11
#define  pVIEWER_R_DESCRIPTION           12
#define  pVIEWER_DECORATION_2            13
#define  pVIEWER_DECORATION_3            14
#define  pVIEWER_CURVE_BOX_MSG           15
#define  pVIEWER_DECORATION              16
#define  pVIEWER_INSTRUMENT              17
#define  pVIEWER_REGION_SCAN_MODE_MSG    18
#define  pVIEWER_REGION_SCANS            19
#define  pVIEWER_REGION_DWELL            20
#define  pVIEWER_REGION_CURRENT          21
#define  pVIEWER_DECORATION_4            22


     /* Menu Bars, Menus, and Menu Items: */

#define  mANALYSIS                       1
#define  mANALYSIS_FILE                  2
#define  mANALYSIS_FILE_LOAD             3       /* callback function: analysis_menu */
#define  mANALYSIS_FILE_PRINT            4       /* callback function: analysis_menu */
#define  mANALYSIS_FILE_SEPARATOR        5
#define  mANALYSIS_FILE_EXIT             6       /* callback function: analysis_menu */
#define  mANALYSIS_REGION                7
#define  mANALYSIS_LAYER                 8
#define  mANALYSIS_PLOT                  9
#define  mANALYSIS_PLOT_DISPLAY_RAW      10      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_DISPLAY_SMOOTH   11      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_DISPLAY_BACK     12      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_DISPLAY_FINAL    13      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_SEPARATOR_3      14
#define  mANALYSIS_PLOT_BINDING          15      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_KINETIC          16      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_CHANNEL          17      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_SEPARATOR_4      18
#define  mANALYSIS_PLOT_FLIP             19      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_ZOOM             20      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_SEPARATOR_2      21
#define  mANALYSIS_PLOT_STYLE            22
#define  mANALYSIS_PLOT_STYLE_SUBMENU    23
#define  mANALYSIS_PLOT_STYLE_THIN_LINE  24      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_STYLE_CONNECTED  25      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_STYLE_SCATTER    26      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_STYLE_VERT_BAR   27      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_STYLE_THIN_STEP  28      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_POINTS           29
#define  mANALYSIS_PLOT_POINTS_SUBMENU   30
#define  mANALYSIS_PLOT_POINTS_S_DOT     31      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_POINTS_CIRCLE    32      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_POINTS_SQUARE    33      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_POINTS_SMALL_X   34      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_LINES            35
#define  mANALYSIS_PLOT_LINES_SUBMENU    36
#define  mANALYSIS_PLOT_LINES_S_THIN     37      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_LINES_DASH       38      /* callback function: menu_plot_style */
#define  mANALYSIS_PLOT_LINES_DOT        39      /* callback function: menu_plot_style */
#define  mANALYSIS_HELP                  40
#define  mANALYSIS_HELP_PANEL            41      /* callback function: analysis_menu */
#define  mANALYSIS_HELP_GENERAL          42      /* callback function: analysis_menu */


     /* Callback Prototypes: */ 

void CVICALLBACK analysis_menu(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK fit_control(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK menu_plot_style(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK print_command(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK print_options(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK print_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quit_analysis(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
