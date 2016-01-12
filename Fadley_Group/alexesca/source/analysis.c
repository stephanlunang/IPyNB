
/*******************************************************************************/
/**	analysis.c							written by alex kay, winter 1997 			**/
/**													for fadley group							**/
/**																									**/
/** This ifle should contain the analysis portion of the HP ESCA West    		**/
/** program.  It will allow the user to look at previous spectra and print		**/
/** them, but not much else... may have a background subtraction and curve		**/
/** fitting routine, but these are not very useful features for the simple		**/
/** XPS instrument that the ESCA West currently is.									**/


#include <cvirte.h>		/* Needed if linking in external compiler; harmless otherwise */
#include <ansi_c.h>
#include <userint.h>
#include <analysis.h>
#include <userint.h>    
#include "analyze.h"
#include "define.h"
#include "globals.h"
#include "alex_ini.h"
#include "analysih.h"
#include "math_lib.h"


#define UNITS_CHANNEL  0
#define UNITS_BINDING  1
#define UNITS_KINETIC  2


//panel handles
static int  pAnalysis;
static int  pViewer;
//menu handle         
static int mAnalysis;	              

char Message[256];

char pathname[MAX_PATHNAME_LEN];              

int ApplyToAll = FALSE;

int SmoothFlag = SMOOTHING_NONE;
int SmoothDegree = 0;
int SmoothPoints = 5;

int BackgroundFlag = BACKGROUND_NONE;
int Background_Shirley_Iterations= 4;

int FittingFlag = FITTING_NONE;

int index1, index2; 		//defines window to do fitting within...
double value1, value2;  //value to use at index1 and index2 in background calculations.

double Area, PeakFwhm, PeakPosition, PeakHeight;
double PeakPositionBE, PeakPositionKE;

int Units = UNITS_BINDING;


struct experimentStruct ThisExperiment ; 
static int *RegionMenuItem;
static int *LayerMenuItem;
static int CurrentRegionIndex= -1;	 
static int CurrentLayerIndex= -1;


struct data_type cData; //holds data type info for currently displayed region...
struct data_type cSmoothData;
struct data_type cWorkData; //holds "working" copy of cData... i.e. after smoothing, background subtraction etc.
struct data_type cBackData; //holds background curve for current cData...                        
                  

struct PlotStruct   Plot_cData;
struct PlotStruct   Plot_cWorkData;         
struct PlotStruct   Plot_cBackData;
struct PlotStruct   Plot_cSmoothData;


int Plot_Show_RawData =1;
int Plot_Show_Background=1;  
int Plot_Show_Difference=1;
int Plot_Show_Smooth= 1;


int main (int argc, char *argv[])
{
	
	int error;    
	
	if (InitCVIRTE (0, argv, 0) == 0)	/* Needed if linking in external compiler; harmless otherwise */
		return -1;	/* out of memory */
	

	if ( ( pAnalysis = LoadPanel (0, "analyze.uir", pANALYSIS) ) < 0 )
		return -1;
	if ( ( pViewer = LoadPanel (pAnalysis, "analyze.uir", pVIEWER) ) < 0 )
		return -1;
	SetPanelPos (pViewer, 20, 10);
	
		
		
	//don't refresh graph until told to...	
	SetCtrlAttribute (pViewer, pVIEWER_GRAPH, ATTR_REFRESH_GRAPH, 0);
	
	// initial values for the plot characteristics...
	Plot_cData.Style = VAL_SCATTER;
	Plot_cData.TracePointStyle = VAL_SMALL_X;
	Plot_cData.LineStyle = VAL_SOLID;
	Plot_cData.Color = VAL_BLUE;
	Plot_cData.BackgroundColor = VAL_WHITE;
	// draw some crap so that we can get an initial plot handle...
	Plot_cData.Handle = PlotPoint (pViewer, pVIEWER_GRAPH, 0.0, 0.0,
												Plot_cData.TracePointStyle, Plot_cData.Color);
	
	Plot_cWorkData.Style = VAL_THIN_LINE;
	Plot_cWorkData.TracePointStyle = VAL_NO_POINT;
	Plot_cWorkData.LineStyle = VAL_SOLID;
	Plot_cWorkData.Color = VAL_RED;
	Plot_cWorkData.BackgroundColor = VAL_WHITE;
	// draw some crap so that we can get an initial plot handle...
	Plot_cWorkData.Handle = PlotPoint (pViewer, pVIEWER_GRAPH, 0.0, 0.0,
														Plot_cWorkData.TracePointStyle, Plot_cWorkData.Color);
	
	
	Plot_cBackData.Style = VAL_THIN_LINE;
	Plot_cBackData.TracePointStyle = VAL_NO_POINT;
	Plot_cBackData.LineStyle = VAL_SOLID;
	Plot_cBackData.Color = VAL_GRAY;
	Plot_cBackData.BackgroundColor = VAL_WHITE;
	// draw some crap so that we can get an initial plot handle...
	Plot_cBackData.Handle = PlotPoint (pViewer, pVIEWER_GRAPH, 0.0, 0.0,
							Plot_cBackData.TracePointStyle, Plot_cBackData.Color);
	
	Plot_cSmoothData.Style = VAL_THIN_LINE;
	Plot_cSmoothData.TracePointStyle = VAL_NO_POINT;
	Plot_cSmoothData.LineStyle = VAL_SOLID;
	Plot_cSmoothData.Color = VAL_GRAY;
	Plot_cSmoothData.BackgroundColor = VAL_WHITE;
	// draw some crap so that we can get an initial plot handle...
	Plot_cSmoothData.Handle = PlotPoint (pViewer, pVIEWER_GRAPH, 0.0, 0.0,
							Plot_cSmoothData.TracePointStyle, Plot_cSmoothData.Color);
							
							
	
	SetCtrlVal (pAnalysis,pANALYSIS_SMOOTH_DEGREE, SmoothDegree);      
	SetCtrlVal (pAnalysis,pANALYSIS_SMOOTH_POINTS, SmoothPoints);
	SetCtrlAttribute (pAnalysis, pANALYSIS_SMOOTH_DEGREE, ATTR_DIMMED, 1);
	SetCtrlAttribute (pAnalysis, pANALYSIS_SMOOTH_POINTS, ATTR_DIMMED, 1);
	
	SetCtrlVal( pAnalysis, pANALYSIS_BACK_SHIRLEY_ITER, Background_Shirley_Iterations);
	SetCtrlAttribute (pAnalysis, pANALYSIS_BACK_SHIRLEY_ITER, ATTR_DIMMED, 1);            
	
	
	//initialize the cData.data array so that we can always realloc later...
	cData.data = calloc( 1, sizeof( double ) );
	
	

	error = analysis_load_an_experiment( );
	if ( error != 0 ) {
		DiscardPanel( pViewer );    
		DiscardPanel( pAnalysis);
		return -1;
	}
	
	//set units to BE ... called after analysis_load_an_experiment() so that mAnalysis is defined
	menu_plot_style (mAnalysis, mANALYSIS_PLOT_BINDING, NULL, NULL);  
	
	DisplayPanel( pViewer );
	DisplayPanel( pAnalysis );


	RunUserInterface ();
	return 0;
}





int analysis_load_an_experiment( void )
{
	////char pathname[MAX_PATHNAME_LEN];
	int status;
	
	int n_o_regions;
	char messageString[250];
	
	
	int regionIndex,i;
	
		// get pathname of experiment user wants to look at
	status = analysis_select_experiment(  pathname ) ;
	// if file was not choosen properly, don't do anything
	if ( status != 0 ) return -1;
	
	
	// attempt to read the experiment parameters into memory
	//   ..also allocates the region arrays
	read_exp_param_from_file( pathname);
	
	read_layer_param_from_file( pathname );
	
	CurrentLayerIndex	=0;
	CurrentRegionIndex =0;
	
	// read in the data for the first region 
	//read_region_data_from_file( pathname, 0 ); 
	read_region_curve_from_file( pathname, CurrentLayerIndex, CurrentRegionIndex , &cData	) ; 
	
	// build the regions sub menu...
	analysis_build_region_menu();
	analysis_build_layer_menu();                
	
	
	update_displayed_region( pathname , CurrentLayerIndex, CurrentRegionIndex );          
	
	// fill the experiment specific data ...
	sprintf( messageString, "Experiment Description:  %s ", ThisExperiment.Description );
	SetCtrlVal (pViewer, pVIEWER_E_DESCRIPTION, messageString ); 
	
	sprintf( messageString, "Instrument: %s",   ThisExperiment.Instrument );
	SetCtrlVal (pViewer, pVIEWER_INSTRUMENT, messageString ); 
	
	sprintf( messageString, "Begun on: %s at %s.", ThisExperiment.DateBegan ,  ThisExperiment.TimeBegan );
	SetCtrlVal (pViewer, pVIEWER_TIME_BEGIN, messageString ); 
	
	
	
	//update message on scan panel
	sprintf( messageString, "Currently displaying region 1 of %i",  ThisExperiment.n_o_regions );
	SetCtrlAttribute (pViewer, pVIEWER_REGION_CURRENT , ATTR_CTRL_VAL, messageString);
	
	sprintf( messageString, "Layer displayed: 1 of %i. ", ThisExperiment.Layers.Size);   
	SetCtrlVal (pViewer, pVIEWER_LAYER_CURRENT, messageString);
	

	return 0;
}



// fill regions menu item with each region name and undim "regions" sub-menu item
 int analysis_build_region_menu( void )
{
	int i;
	
	/*****Not sure how this works, but it does !! ***************/
	/* Used below for menu callback	.. it's an extern function	*/  
	void (*function)(int, int , void *,int) = select_region_from_menu;
	/* void (*function2)(int, int , void *,int) = select_standard_source_from_menu;   */ 
	/************************************************************/	
	
	/*  NOTE!! can use  InstallMenuCallback (, , , i);  to set callback data (i) that is 
	     sent to the menucallback... this allows us to match user request with our array
	     of configuration data
	*/
	
	mAnalysis = GetPanelMenuBar (pAnalysis); 	//get menu handle 
	EmptyMenu (mAnalysis, mANALYSIS_REGION);

	
	/* if there were some configs found in the light.ini file, use to build a menu */
	if( ThisExperiment.n_o_regions > 0 ){
	
		SetMenuBarAttribute (mAnalysis, mANALYSIS_REGION , ATTR_DIMMED, FALSE);
		
		
		for( i=0 ; i < ThisExperiment.n_o_regions; i++ )
		{
		///	printf(" making menu: item added: %s \n",			ThisExperiment.Regions[i].Description);
			RegionMenuItem[i] = NewMenuItem (mAnalysis, mANALYSIS_REGION , ThisExperiment.Regions[i].Description, -1, 0, function, 0);
		}
	 	
	 	// make the first region item checked by default...
	 	SetMenuBarAttribute ( mAnalysis, RegionMenuItem[0]  , ATTR_CHECKED, TRUE);     
	}
		
	/* ... but if there were no regions found (how whould this happen??), then build an "empty" menu */
	else if( ThisExperiment.n_o_regions == 0 )
	{
		SetMenuBarAttribute (mAnalysis, mANALYSIS_REGION , ATTR_DIMMED, TRUE);
	}

	return 0;
}



// fill regions menu item with each region name and undim "regions" sub-menu item
int analysis_build_layer_menu( void )
{
	int i;
	
	/*****Not sure how this works, but it does !! ***************/
	/* Used below for menu callback	.. it's an extern function	*/  
	void (*function)(int, int , void *,int) = select_layer_from_menu;
	/* void (*function2)(int, int , void *,int) = select_standard_source_from_menu;   */ 
	/************************************************************/	
	
	/*  NOTE!! can use  InstallMenuCallback (, , , i);  to set callback data (i) that is 
	     sent to the menucallback... this allows us to match user request with our array
	     of configuration data
	*/
	
	mAnalysis = GetPanelMenuBar (pAnalysis); 	//get menu handle 
	EmptyMenu (mAnalysis, mANALYSIS_LAYER);

	
	/* if there were some configs found in the light.ini file, use to build a menu */
	if( ThisExperiment.Layers.Size > 0 ){
	
		SetMenuBarAttribute (mAnalysis, mANALYSIS_LAYER , ATTR_DIMMED, FALSE);
		
		
		for( i=0 ; i < ThisExperiment.Layers.Size; i++ )
		{
		///	printf(" making menu: item added: %s \n",			ThisExperiment.Regions[i].Description);
			sprintf( Message, "Layer %i" , i);
			LayerMenuItem[i] = NewMenuItem (mAnalysis, mANALYSIS_LAYER , Message, -1, 0, function, 0);
		}
	 	
	 	// make the first region item checked by default...
	 	SetMenuBarAttribute ( mAnalysis, LayerMenuItem[0]  , ATTR_CHECKED, TRUE);     
	}
		
	/* ... but if there were no regions found (how whould this happen??), then build an "empty" menu */
	else if( ThisExperiment.Layers.Size == 0 )
	{
		SetMenuBarAttribute (mAnalysis, mANALYSIS_LAYER , ATTR_DIMMED, TRUE);
	}

	return 0;
}





/* callback for the menu items built in light_build_menu() */
void  select_region_from_menu (int menuBar, int menuItem, void *callbackData, int panel)
{
	int i;
	  
	/* look to find out which menu item was pressed, then set display to proper value */
	for( i = 0 ; i< ThisExperiment.n_o_regions ; i++ )
	{
	  		if( menuItem == RegionMenuItem[i])
	  		{
				 
				 // put a check marl next to the one that is choosen
				 SetMenuBarAttribute ( menuBar, RegionMenuItem[i]  , ATTR_CHECKED, 1);      
				 // update displayed parameters,
				 //////read_region_data_from_file( pathname ,  i-1 );
				 update_displayed_region( pathname, CurrentLayerIndex, i );
				 
				 // read in that regions data points
				 ///printf( "picked region %i \n", i);
				 
				 // create and display the graph
				 
				 
				 
				 CurrentRegionIndex =  i;			// set global
				 //no need to do this...doesn't save noticable amount of time i = ThisExperiment.n_o_regions;  //force end of loop
			}
			else{
				// if not choosen, make sure that that RegionMenuItem[i] is unchecked...
				SetMenuBarAttribute ( menuBar, RegionMenuItem[i]  , ATTR_CHECKED, 0);
			}
			
	}	
}

void  select_layer_from_menu (int menuBar, int menuItem, void *callbackData, int panel)
{
	int i;
	  
	/* look to find out which menu item was pressed, then set display to proper value */
	for( i = 0 ; i< ThisExperiment.Layers.Size ; i++ )
	{
	  		if( menuItem == LayerMenuItem[i])
	  		{
				 
				 // put a check marl next to the one that is choosen
				 SetMenuBarAttribute ( menuBar, LayerMenuItem[i]  , ATTR_CHECKED, 1);      
				 // update displayed parameters,
				 //////read_region_data_from_file( pathname ,  i-1 );
				 update_displayed_region( pathname, i, CurrentRegionIndex );
				 
				 
				 CurrentRegionIndex =  i;			// set global
				 //no need to do this...doesn't save noticable amount of time i = ThisExperiment.n_o_regions;  //force end of loop
			}
			else{
				// if not choosen, make sure that that RegionMenuItem[i] is unchecked...
				SetMenuBarAttribute ( menuBar, LayerMenuItem[i]  , ATTR_CHECKED, 0);
			}
			
	}	
}



int update_displayed_region( char pathn[MAX_PATHNAME_LEN], int LayerIndex , int RegionIndex )       
{
	
	// first, get the new data
///	read_region_data_from_file( pathname , RegionIndex ); 
	read_region_curve_from_file( pathname, LayerIndex, RegionIndex , &cData	);
	
	//mathc cWorkData and cBackdata to cData:
	CopyDataType( &cData, &cWorkData, 1 );  // 1 = copy cData.data array into cWorkData.data...
	CopyDataType( &cData, &cBackData, 0 );  // 0 = copy zeros into cBackData.data...
	CopyDataType( &cData, &cSmoothData,1);
	
	
	
	
	// then update the displayed parameters  and
	// then update the graph with this data
	update_panel_data( ThisExperiment.Regions[RegionIndex] , &cData); 
	
	
	
	return 0;
}



int update_panel_data( struct regionStruct Reg, struct data_type *CurvePtr  )
{
	char messageString[256];
	int i;
	int graphHandle;
	
	
	sprintf( messageString, "Region displayed: %i of %i. ",
											CurrentRegionIndex+1, ThisExperiment.n_o_regions);   
	SetCtrlVal (pViewer, pVIEWER_REGION_CURRENT, messageString);
			  
	sprintf( messageString, "Layer displayed: %i of %i. ",
											CurrentLayerIndex+1, ThisExperiment.Layers.Size);   
	SetCtrlVal (pViewer, pVIEWER_LAYER_CURRENT, messageString);
	
	// fill the region specific information
	sprintf( messageString, "Region Description:  %s ",  Reg.Description);       
	SetCtrlVal (pViewer, pVIEWER_R_DESCRIPTION, messageString );  
	
	SetPanelAttribute (pViewer, ATTR_TITLE,  Reg.Description);      
	
	//SetCtrlVal (pViewer, pVIEWER_BE_STEP, Reg.StepMultiple * (double) DetectorWidth / ((double)(N_O_DETECTOR_CHANNELS))  );     
	SetCtrlVal (pViewer, pVIEWER_BE_STEP, Reg.BEstepSize );
	
	sprintf( messageString, "Dwell Time:  %-7.3f (sec)" , Reg.DwellTime);
	SetCtrlVal (pViewer, pVIEWER_REGION_DWELL, messageString );
	
	sprintf( messageString, "Scans:     %i" , Reg.NumberOfScans );
	SetCtrlVal (pViewer, pVIEWER_REGION_SCANS, messageString );
	
	if ( Reg.ScanMode == SCAN_MODE_FIXED ) sprintf( messageString, "Scan Mode was FIXED");
	else sprintf( messageString, "Scan Mode was DITHERED (Swept)");
	SetCtrlAttribute (pViewer, pVIEWER_REGION_SCAN_MODE_MSG, ATTR_CTRL_VAL, messageString);
	
	
	SetCtrlVal (pViewer, pVIEWER_BE_CENTER, CurvePtr->center_energy );
	SetCtrlVal (pViewer, pVIEWER_BE_WIDTH , CurvePtr->width  );     
	SetCtrlVal (pViewer, pVIEWER_CHANNELS, CurvePtr->size ); 
	SetCtrlVal (pViewer, pVIEWER_BE_STEP, CurvePtr->width/(double)(CurvePtr->size - 1 ) );     
	SetCtrlVal (pViewer, pVIEWER_EXCITATION, CurvePtr->excitation_energy );  
	
	// plot should have always had a previouly drawn plot at this point that needs deleting now...
//	DeleteGraphPlot (pViewer, pVIEWER_GRAPH, Plot_cData.Handle, VAL_DELAYED_DRAW);

	//fill the graph
	//... plot all data up to current and update cursors

//	Plot_cData.Handle= PlotY (pViewer, pVIEWER_GRAPH, CurvePtr->data , CurvePtr->size, VAL_DOUBLE, Plot_cData.Style,
//					Plot_cData.TracePointStyle, Plot_cData.LineStyle, 1, Plot_cData.Color);
	
	update_displayed_units( Units );
	
	DeleteGraphPlot (pViewer, pVIEWER_GRAPH, -1, VAL_DELAYED_DRAW);
	
	index1 = 0;
	index2 = cData.size -1;
	value1 = cData.data[index1];
	value2 = cData.data[index2];
	
	if(Plot_Show_RawData)
		Plot_A_DataType_Curve( &cData, 		&Plot_cData, 		0, cData.size-1, FALSE,	pViewer, pVIEWER_GRAPH );
	if( Plot_Show_Difference )
		Plot_A_DataType_Curve( &cWorkData,  &Plot_cWorkData, 	index1,index2,   TRUE,	pViewer, pVIEWER_GRAPH ); 
	if( Plot_Show_Background )	
		Plot_A_DataType_Curve( &cBackData,  &Plot_cBackData, 	index1,index2,	  FALSE,	pViewer, pVIEWER_GRAPH );
	if( Plot_Show_Smooth )
		Plot_A_DataType_Curve( &cSmoothData,&Plot_cSmoothData,index1,index2,	  FALSE,	pViewer, pVIEWER_GRAPH );    
	
	
	
	//set cursors...
	SetGraphCursorIndex (pViewer, pVIEWER_GRAPH, 1, Plot_cData.Handle , index1);
	SetGraphCursorIndex (pViewer, pVIEWER_GRAPH, 2, Plot_cData.Handle , index2);
	
	fit_control (pAnalysis, pANALYSIS_CALCULATE, EVENT_COMMIT, 0,0,0);	

	//update display
	display_window_values( index1, index2 ,  &cData ) ;   
	return 0;
}





int analysis_utility_close_up_shop( void )
{
	// just delete the panel... 
	HidePanel(pAnalysis);
	DiscardPanel (pViewer);           
	DiscardPanel (pAnalysis);
	// and free up the memory used
	free( ThisExperiment.Regions);
	free( RegionMenuItem );
	free( LayerMenuItem  );
	
	QuitUserInterface(0);
	return 0;
	
}




int analysis_select_experiment( char pathn[MAX_PATHNAME_LEN] )
{
	int  status;
	
	status = FileSelectPopup ("c:\\alexesca\\data", "*.xps", "*.xps",
					"Select the experiment you wish to view", VAL_LOAD_BUTTON, 0, 0, 1, 0, pathname);

	switch( status ){
		case VAL_NO_FILE_SELECTED:
			// do nothing
			return -1;
			break;
		
		case VAL_EXISTING_FILE_SELECTED:	
			// now we have the proper pathname ... return success(0)
			return 0;
			break;
		case VAL_NEW_FILE_SELECTED:
			return -1; 
			// this is bad... do nothing
			break;
		default:
			// any other case indicated an error
			printf( " Error in analysis_select_experiment() " );
		}
	
	return -1;
}




void CVICALLBACK analysis_menu (int menuBar, int menuItem, void *callbackData, int panel)
{
	 switch( menuItem ){
	 	 case mANALYSIS_FILE_LOAD:
	 	   analysis_load_an_experiment();
	 	 	break;
	 	 	
	     case mANALYSIS_FILE_PRINT:
	      print_utility_go();
	 	 	break;
	 	 	
	 	 case mANALYSIS_FILE_EXIT:
	 	    analysis_utility_close_up_shop();
	 	 	break;
	 	 	
	 	
	 	 case mANALYSIS_HELP_PANEL:
	 	 
	 	 	break;
	 	 	
	 	 case mANALYSIS_HELP_GENERAL:
	 	 
	 	 	break;	
	 
	 }

}

void CVICALLBACK menu_plot_style (int menuBar, int menuItem, void *callbackData, int panel)
{
	int checked;
	
	switch( menuItem ){
	
		case mANALYSIS_PLOT_DISPLAY_RAW:
			GetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, &Plot_Show_RawData );
			Plot_Show_RawData = !Plot_Show_RawData;
			SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, Plot_Show_RawData );  
			
			SetPlotAttribute (pViewer, pVIEWER_GRAPH ,Plot_cData.Handle ,
													ATTR_TRACE_VISIBLE, Plot_Show_RawData);
													
			break;
			
		case mANALYSIS_PLOT_DISPLAY_SMOOTH:
			GetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, &Plot_Show_Smooth );
			Plot_Show_Smooth = !Plot_Show_Smooth;
			SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, Plot_Show_Smooth );  
			
			SetPlotAttribute (pViewer, pVIEWER_GRAPH ,Plot_cSmoothData.Handle ,
													ATTR_TRACE_VISIBLE, Plot_Show_Smooth);
			break;
	
		case mANALYSIS_PLOT_DISPLAY_BACK:
			GetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, &Plot_Show_Background );
			Plot_Show_Background = !Plot_Show_Background;
			SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, Plot_Show_Background );  
			
			SetPlotAttribute (pViewer, pVIEWER_GRAPH ,Plot_cBackData.Handle ,
													ATTR_TRACE_VISIBLE, Plot_Show_Background);
			break;
	
		case mANALYSIS_PLOT_DISPLAY_FINAL:
			GetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, &Plot_Show_Difference ); 
			Plot_Show_Difference = !Plot_Show_Difference;
			SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, Plot_Show_Difference );  
			
			SetPlotAttribute (pViewer, pVIEWER_GRAPH ,Plot_cWorkData.Handle ,
													ATTR_TRACE_VISIBLE, Plot_Show_Difference );
													
			
			break;
	
	
		//================================================================
	
		case mANALYSIS_PLOT_CHANNEL:
			// make sure menu item is checked
			SetMenuBarAttribute (menuBar, mANALYSIS_PLOT_CHANNEL, ATTR_CHECKED, TRUE);  
			// and uncheck the binding menu item and kinetic
			SetMenuBarAttribute (menuBar, mANALYSIS_PLOT_BINDING, ATTR_CHECKED, FALSE);
			SetMenuBarAttribute (menuBar, mANALYSIS_PLOT_KINETIC, ATTR_CHECKED, FALSE);  
	
			Units = UNITS_CHANNEL;
			update_displayed_units( Units );          
			break;
	
	
		case mANALYSIS_PLOT_KINETIC:
			// make sure menu item is checked
			SetMenuBarAttribute (menuBar, mANALYSIS_PLOT_KINETIC, ATTR_CHECKED, TRUE);  
			// and uncheck the binding menu item and channel
			SetMenuBarAttribute (menuBar, mANALYSIS_PLOT_BINDING, ATTR_CHECKED, FALSE);
			SetMenuBarAttribute (menuBar, mANALYSIS_PLOT_CHANNEL, ATTR_CHECKED, FALSE);  
			// and then fix the graph attributes
			Units = UNITS_KINETIC;
			update_displayed_units( Units );                
			break;
	
		case mANALYSIS_PLOT_BINDING:
			SetMenuBarAttribute (menuBar,mANALYSIS_PLOT_BINDING, ATTR_CHECKED, TRUE);  
			SetMenuBarAttribute (menuBar, mANALYSIS_PLOT_KINETIC, ATTR_CHECKED, FALSE);
			SetMenuBarAttribute (menuBar, mANALYSIS_PLOT_CHANNEL, ATTR_CHECKED, FALSE);        
			
			Units = UNITS_BINDING;
			update_displayed_units( Units );            
			break;		
	//==========================================================	
		
		
		
		
		
		
		case mANALYSIS_PLOT_FLIP:
			GetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, &checked );
			// see if menu item is checked... if it is then uncheck it and unflip the axis        
			if( checked )
			{
				  SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, FALSE);
				  SetCtrlAttribute (pViewer, pVIEWER_GRAPH , ATTR_XREVERSE, FALSE);     
			}	
			// otherwise check it and flip the axis
			else
			{	  
				  SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, TRUE); 
				  SetCtrlAttribute (pViewer, pVIEWER_GRAPH, ATTR_XREVERSE, TRUE); 
			}
			break;		
		
		case mANALYSIS_PLOT_ZOOM :
			
			break;

			
		
		case mANALYSIS_PLOT_STYLE_THIN_LINE :
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_LINE, ATTR_CHECKED, 1);  
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_CONNECTED, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_SCATTER, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_VERT_BAR, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_STEP, ATTR_CHECKED, 0);
			
			Plot_cData.Style = VAL_THIN_LINE;
			break;
		
		
		case mANALYSIS_PLOT_STYLE_CONNECTED :
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_LINE, ATTR_CHECKED, 0);  
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_CONNECTED, ATTR_CHECKED, 1);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_SCATTER, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_VERT_BAR, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_STEP, ATTR_CHECKED, 0);
			
		 	Plot_cData.Style = VAL_CONNECTED_POINTS;
			break;
			
		case mANALYSIS_PLOT_STYLE_SCATTER :
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_LINE, ATTR_CHECKED, 0);  
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_CONNECTED, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_SCATTER, ATTR_CHECKED, 1);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_VERT_BAR, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_STEP, ATTR_CHECKED, 0);
		
			Plot_cData.Style = VAL_SCATTER;
			break;
			
		case mANALYSIS_PLOT_STYLE_VERT_BAR :
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_LINE, ATTR_CHECKED, 0);  
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_CONNECTED, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_SCATTER, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_VERT_BAR, ATTR_CHECKED, 1);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_STEP, ATTR_CHECKED, 0);
		
			Plot_cData.Style = VAL_VERTICAL_BAR;
			break;
			
		case mANALYSIS_PLOT_STYLE_THIN_STEP :
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_LINE, ATTR_CHECKED, 0);  
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_CONNECTED, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_SCATTER, 		ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_VERT_BAR, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_STYLE_THIN_STEP, 	ATTR_CHECKED, 1);
		
			Plot_cData.Style = VAL_THIN_STEP;
			break;
			
		
		
		
		
		case mANALYSIS_PLOT_POINTS_S_DOT:
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_S_DOT,	ATTR_CHECKED, 1);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_CIRCLE, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_SQUARE, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_SMALL_X, ATTR_CHECKED, 0);
			
	 		Plot_cData.TracePointStyle = VAL_SIMPLE_DOT;
			
			break;
			
		case mANALYSIS_PLOT_POINTS_CIRCLE:
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_S_DOT,	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_CIRCLE, 	ATTR_CHECKED, 1);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_SQUARE, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_SMALL_X, ATTR_CHECKED, 0);
			
		 	Plot_cData.TracePointStyle = VAL_EMPTY_CIRCLE; 
			
			break;
			
		case mANALYSIS_PLOT_POINTS_SQUARE:
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_S_DOT,	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_CIRCLE, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_SQUARE, ATTR_CHECKED, 1);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_SMALL_X, ATTR_CHECKED, 0);
			
			Plot_cData.TracePointStyle = VAL_EMPTY_SQUARE;
			
	 		break;
	 		
	 	case mANALYSIS_PLOT_POINTS_SMALL_X :
	 		SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_S_DOT,	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_CIRCLE, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_SQUARE, ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_POINTS_SMALL_X, ATTR_CHECKED, 1);	
			
			Plot_cData.TracePointStyle =  VAL_SMALL_X;
			
	 		break;
	 
	 		
	 		
	 		
	 		
	 	case mANALYSIS_PLOT_LINES_S_THIN:
	 		SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_S_THIN,	ATTR_CHECKED, 1);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_DASH, 		ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_DOT, 		ATTR_CHECKED, 0);
			
	 		Plot_cData.LineStyle = VAL_SOLID;
	 		break;
	 
	 	case mANALYSIS_PLOT_LINES_DASH:
	 		SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_S_THIN,	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_DASH, 		ATTR_CHECKED, 1);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_DOT, 		ATTR_CHECKED, 0);
			
			Plot_cData.LineStyle = VAL_DASH;
	 		break;
	 		
	 	case mANALYSIS_PLOT_LINES_DOT:
	 		SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_S_THIN,	ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_DASH, 		ATTR_CHECKED, 0);
			SetMenuBarAttribute (mAnalysis, mANALYSIS_PLOT_LINES_DOT, 		ATTR_CHECKED, 1);
			
			Plot_cData.LineStyle =  VAL_DOT;
	 		break;
	 
	}
	 
	SetPlotAttribute (pViewer, pVIEWER_GRAPH, Plot_cData.Handle, ATTR_PLOT_STYLE, Plot_cData.Style);
	SetPlotAttribute (pViewer, pVIEWER_GRAPH, Plot_cData.Handle, ATTR_TRACE_POINT_STYLE, Plot_cData.TracePointStyle);  
	SetPlotAttribute (pViewer, pVIEWER_GRAPH, Plot_cData.Handle, ATTR_LINE_STYLE, Plot_cData.LineStyle);          
	RefreshGraph (pViewer, pVIEWER_GRAPH);
	
}




//uses the cData (current data) to set the appropriate x-axis scale for the
//   requested units
void update_displayed_units( int units )
{
	// update the graph attributes... just want to plot the array taht was sent to it.. no gain, no offset...
	switch( units )
	{
	
		case UNITS_BINDING:
			// update the graph attributes... just want to plot the array taht was sent to it.. no gain, no offset...
			SetCtrlAttribute (pViewer, pVIEWER_GRAPH , ATTR_XAXIS_OFFSET, cData.center_energy - 0.5*cData.width);
			SetCtrlAttribute (pViewer, pVIEWER_GRAPH , ATTR_XAXIS_GAIN, cData.width/cData.size);   
			SetCtrlAttribute (pViewer, pVIEWER_GRAPH , ATTR_XNAME, "Binding Energy (eV)");
			SetCtrlAttribute (pViewer, pVIEWER_GRAPH, ATTR_XPRECISION, 2); 
			break;
			
		case UNITS_KINETIC:
					  	// Multiply binding energy by -1.0 ...
			SetCtrlAttribute (pViewer,pVIEWER_GRAPH , ATTR_XAXIS_GAIN, -cData.width/cData.size); 
						// ...and add the excitation energy to this to get kinetic ...
			SetCtrlAttribute (pViewer, pVIEWER_GRAPH , ATTR_XAXIS_OFFSET, cData.excitation_energy - (cData.center_energy - 0.5*cData.width)); 
						//.. then update the axis label.
			SetCtrlAttribute (pViewer,pVIEWER_GRAPH , ATTR_XNAME, "Kinetic Energy (eV)");
			SetCtrlAttribute (pViewer, pVIEWER_GRAPH, ATTR_XPRECISION, 2); 
			break;
			
		case UNITS_CHANNEL:
			SetCtrlAttribute (pViewer,pVIEWER_GRAPH , ATTR_XAXIS_GAIN, 1.0); 
			SetCtrlAttribute (pViewer, pVIEWER_GRAPH , ATTR_XAXIS_OFFSET, 0.0); 
			SetCtrlAttribute (pViewer,pVIEWER_GRAPH , ATTR_XNAME, "Channels");
			SetCtrlAttribute (pViewer, pVIEWER_GRAPH, ATTR_XPRECISION, 0); 
			break;
			
	}

}






//==========================================================================      
// This function reads the data under [Experiemental Parameters] 
// in the passed filehandle.  Note the the filehandle is assumed to 
// belong to a valid open file: no checking is done.  If any one of
// the search for pieces of information is not found, a return error
// code is returned (-1).  If everything is ok, it returns 0;
// 
int read_exp_param_from_file( char pathn[MAX_PATHNAME_LEN] )
{
	int i, ReturnStatus = 0;
	char tempString[256];   // 256 is more than enough room for this...
	unsigned long tempUnsignedLong  = 0;
	double tempDouble = 0;
	int error;

	FILE *file; 
	long filePos;
	
///	printf( " pathname passed was : %s \n" , pathname );
	
	// open the choosen file
	SetWaitCursor (1);
	file = fopen (pathname, "r");	/* open file in read only mode	*/      	 
	
	// find the correct position in file...
	error = ini_find_section( file, " Experiment Parameters " , '[', ']', ';' , &filePos );        
	
	// read the experiment description      
	error = ini_read_string_after_tag_value( file, filePos, '[', '=', ';' , 
								"Experiment Description", ThisExperiment.Description , 100);
	
	
	// get number of regions
	error = ini_read_unsigned_long_after_tag_value( file, filePos, '[', '=', ';' , 
								"Number_of_Regions_in_Experiment", &tempUnsignedLong );  
	ThisExperiment.n_o_regions = ( unsigned int) tempUnsignedLong;
	
	// read the instrument
	error = ini_read_string_after_tag_value( file, filePos, '[', '=', ';' , 
								"Instrument", ThisExperiment.Instrument , 100);
	
	// read the date begun
	error = ini_read_string_after_tag_value( file, filePos, '[', '=', ';' , 
								"Date", ThisExperiment.DateBegan , 10);
	
	// read the time begun
	error = ini_read_string_after_tag_value( file, filePos, '[', '=', ';' , 
								"Time", ThisExperiment.TimeBegan , 8);
	
	
	// allocate memory for these regions... use ThisExperiment.Regions pointer to point to the allocated memory...
	ThisExperiment.Regions =  calloc( ThisExperiment.n_o_regions , sizeof( struct regionStruct ) );    
	// for each region ...
	for ( i = 0; i < ThisExperiment.n_o_regions ; i++ )
	{
		// find location in file where current region parameters are stored...
		sprintf( tempString, " Region %i Parameters " ,  i+1 );
		error = ini_find_section( file, tempString , '[', ']', ';' , &filePos );     
		
		// read the Description ...
		error = ini_read_string_after_tag_value( file, filePos, '[', '=', ';' , 
							"Region_Description", ThisExperiment.Regions[i].Description , 100);

		// read the center BE
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' ,
							"BEcenter(eV)", &ThisExperiment.Regions[i].BEcenter);

		 // read the BE width
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"BEwidth(eV)", &ThisExperiment.Regions[i].BEwidth);
		
		// read the BE step
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"BEstep(eV)", &ThisExperiment.Regions[i].BEstepSize);
		
		// read the Dwell Time
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Dwell_Time(sec)", &ThisExperiment.Regions[i].DwellTime);
							
		// read the total time per data channel
//		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
//							"Time_per_Channel(sec)", &ThisExperiment.Regions[i].TimePerChannel);
		
		
		// read the number of scans
		error = ini_read_unsigned_long_after_tag_value( file, filePos, '[', '=', ';' , 
								"N_o_scans", &tempUnsignedLong );  
		ThisExperiment.Regions[i].NumberOfScans = ( unsigned int) tempUnsignedLong;
		
		
		// read the scan mode
		error = ini_read_unsigned_long_after_tag_value( file, filePos, '[', '=', ';' , 
								"Scan_mode", &tempUnsignedLong );  
		ThisExperiment.Regions[i].ScanMode = ( unsigned int) tempUnsignedLong;
		
///		// read the excitation energy
///		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
///							"Excitation_Energy", &ThisExperiment.Regions[i].ExcitationEnergy);

		
		// read the number of channels (data points)
		error = ini_read_unsigned_long_after_tag_value( file, filePos, '[', '=', ';' , 
								"N_o_Channels", &tempUnsignedLong );  
		ThisExperiment.Regions[i].NumberOfChannels = ( unsigned int) tempUnsignedLong;

	}
	
	// allocate memory to hold the region menu item handles...
	RegionMenuItem =  calloc( ThisExperiment.n_o_regions , sizeof( int ) );  

	// close the file...
	error = fclose (file);
	if( error == EOF &&  errno < 0 )
		printf( " error closing file\n");
	SetWaitCursor (0);    
	return 0;


	return ReturnStatus;
}






//==========================================================================      
int read_layer_param_from_file( char pathn[MAX_PATHNAME_LEN] )
{
	int i, ReturnStatus = 0;
	char tempString[256];   // 256 is more than enough room for this...
	unsigned long tempUnsignedLong  = 0;
	double tempDouble = 0;
	int error;

	FILE *file; 
	long filePos;
	
///	printf( " pathname passed was : %s \n" , pathname );
	
	// open the choosen file
	SetWaitCursor (1);
	file = fopen (pathname, "r");	/* open file in read only mode	*/      	 
	
	// find the correct position in file...
	error = ini_find_section( file, " Layer Parameters " , '[', ']', ';' , &filePos );        
	
	// read the experiment description      
	error = ini_read_string_after_tag_value( file, filePos, '[', '=', ';' , 
								"Layer_Type", ThisExperiment.Layers.Description , 100);
	
	error = ini_read_int_after_tag_value( file, filePos, '[', '=', ';' , 
							"Size", &ThisExperiment.Layers.Size);
	
	if( ThisExperiment.Layers.Size  < 1 ) 
		ThisExperiment.Layers.Size = 1;
							
	//the rest of the parameters are layer type specific.
	if( strcmp (ThisExperiment.Layers.Description ,  "XPS" ) == 0 )
	{
		//printf( "found XPS layers\n");
		ThisExperiment.Layers.Type =  LAYER_TYPE_NONE; 
		
	}
		
	else if (strcmp (ThisExperiment.Layers.Description ,  "Manual" ) == 0 )
	{
		//printf( "found Manual layers\n");
		
		ThisExperiment.Layers.Type =  LAYER_TYPE_MANUAL;
		
		ThisExperiment.Layers.Manual.Size = ThisExperiment.Layers.Size;                
	}
		
	else if (strcmp (ThisExperiment.Layers.Description ,  "Polar" ) == 0 )
	{
		//printf( "found Polar layers\n");
		
		ThisExperiment.Layers.Type =  LAYER_TYPE_POLAR;             
		
		ThisExperiment.Layers.Polar.Size = ThisExperiment.Layers.Size;
		
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Initial", &ThisExperiment.Layers.Polar.Initial);
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Final", &ThisExperiment.Layers.Polar.Final);
							
		if( ThisExperiment.Layers.Polar.Size > 1 )
			ThisExperiment.Layers.Polar.Step=
					(ThisExperiment.Layers.Polar.Final - ThisExperiment.Layers.Polar.Initial) /
												(double)(ThisExperiment.Layers.Polar.Size -1) ;
		else
			ThisExperiment.Layers.Polar.Step= 0.0;
					

	}
		
	else if (strcmp (ThisExperiment.Layers.Description ,  "Timed" ) == 0 )
	{
		//printf( "found Timed layers\n");
		
		ThisExperiment.Layers.Type =  LAYER_TYPE_TIMED;             
		
		ThisExperiment.Layers.Timed.Size = ThisExperiment.Layers.Size;
		
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Time", &ThisExperiment.Layers.Timed.Time );
		
	}
	
	
	LayerMenuItem  =  calloc( ThisExperiment.Layers.Size , sizeof( int ) );
	
	
// close the file...
	error = fclose (file);
	if( error == EOF &&  errno < 0 )
		printf( " error closing file\n");
	SetWaitCursor (0);    
	return 0;


	return ReturnStatus;
}



//==========================================================================
// we automatically do this for all the regions when the exp. parameters are read in.
int read_region_param_from_file(char pathn[MAX_PATHNAME_LEN], int RgnIdx )
{
	int ReturnStatus = 0;
	
	// locate the proper place in file: [Region # Parameters]  
	
	// begin reading in each parameter for that file into the region structure
	
			// if a param is not found, ReturnSataus = -1
	
	return ReturnStatus;
}




// fills the passed data structure associated with specified region
// numberwith values from file... */
int read_region_curve_from_file( char pathn[MAX_PATHNAME_LEN], int LyrIdx,
										int RgnIdx ,  data_type_pointer CurvePtr	)
{
	int ReturnStatus = 0;
	char tempString[100];
	int i ,error;

	FILE *file ;
	long filePos ; 
	
	int num_read;
	double junk;
	
	double BEinit, BEstep;

	SetWaitCursor (1);
	file = fopen (pathname, "r");	/* open file in read only mode	*/      
	if( file == NULL && errno < 0 ){
		printf( "error opening file to read region data \n");
		return -1;
	}
	
	 
	// locate the proper place in file: [Region # Data]
	sprintf( tempString, " L= %i R= %i ", LyrIdx+1, RgnIdx+ 1);
	error = ini_find_section( file, tempString , '[', ']', ';' , &filePos );
	
	//read curve parameters:
	error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"BEcenter", &CurvePtr->center_energy);
	error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"BEwidth", &CurvePtr->width);		  /*assumed negative!*/
							
	error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Excitation", &CurvePtr->excitation_energy);	
							
	error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Excitation", &CurvePtr->time_p_channel);
							
	error = ini_read_int_after_tag_value( file, filePos, '[', '=', ';' , 
							"Size", &CurvePtr->size);				
	 
	

	// *re*allocate enough memory for the BE and Counts
	CurvePtr->data	=  realloc ( CurvePtr->data, CurvePtr->size * sizeof(double));
	

	// NOTE: 
	//   our convention requires (assumes) a negative width and 
	//   intensities (counts) to be tabulated as { I(BEmax) ... I(BEmin }

	// begin reading on next line the sets of  double (BE),	and unsigned long (counts) 
	BEinit = CurvePtr->center_energy - 0.5*CurvePtr->width ;
	BEstep = CurvePtr->width/ (double)( CurvePtr->size - 1 );
	
	for( i = 0 ; i < CurvePtr->size ; i++ )
	{
		num_read = fscanf (file, "%Lf%Lf", &junk, &CurvePtr->data[i] );    
		if( num_read != 2 ) {
			printf( "error in fscanf... did not read 2 values.\n");
			break;
		}
	}
	
	// close the file...
	error = fclose (file);
	if( error == EOF &&  errno < 0)
		printf( "  error closing file \n");
	SetWaitCursor (0);    

	return ReturnStatus ; 
}
















#define PRINT_WHAT_ALL			0
#define PRINT_WHAT_BOTH 		1
#define PRINT_WHAT_PARAM 		2
#define PRINT_WHAT_SPECTRUM 	3

#define PRINT_WHICH_ALL			0
#define PRINT_WHICH_CURRENT	1




int PrintPanelOpen = FALSE;
int pPrint;
int PrintWhat, PrintWhich;

int print_utility_go( void )
{
	
	// if everything is ok, load the panel
	if( PrintPanelOpen == FALSE ){
	
		PrintPanelOpen = TRUE ;
		if( ( pPrint = LoadPanel (0, "analyze.uir", pPRINT) ) < 0 )
			return -1;
		
		//initialize panel controls/indicators
		SetCtrlAttribute (pPrint, pPRINT_REGIONS_CURRENT   , ATTR_CTRL_VAL, ON);
		SetCtrlAttribute (pPrint, pPRINT_REGIONS_ALL  , ATTR_CTRL_VAL, OFF); 
		PrintWhich =  PRINT_WHICH_CURRENT;
		
		
		SetCtrlAttribute (pPrint, pPRINT_INCLUDE_PARAM , ATTR_CTRL_VAL, OFF);
		SetCtrlAttribute (pPrint, pPRINT_INCLUDE_SPECTRUM , ATTR_CTRL_VAL, OFF); 
		SetCtrlAttribute (pPrint, pPRINT_INCLUDE_BOTH , ATTR_CTRL_VAL, OFF); 
		SetCtrlAttribute (pPrint, pPRINT_INCLUDE_ALL , ATTR_CTRL_VAL, ON);
		PrintWhat = PRINT_WHAT_ALL;       
		
		
		SetCtrlAttribute (pPrint, pPRINT_OPTIONS_LANDSCAPE  , ATTR_CTRL_VAL, OFF);
		SetCtrlAttribute (pPrint, pPRINT_OPTIONS_PORTRAIT  , ATTR_CTRL_VAL, ON); 
		SetCtrlAttribute (pPrint, pPRINT_OPTIONS_COPIES   , ATTR_CTRL_VAL,  1 );    
		SetPrintAttribute (ATTR_ORIENTATION, VAL_PORTRAIT);
		SetPrintAttribute (ATTR_NUMCOPIES, 1);
		
		
		SetCtrlAttribute (pPrint, pPRINT_COLOR_BW ,			ATTR_CTRL_VAL, OFF);
		SetCtrlAttribute (pPrint, pPRINT_COLOR_GREYSCALE , ATTR_CTRL_VAL, ON); 
		SetCtrlAttribute (pPrint, pPRINT_COLOR_COLOR , 		ATTR_CTRL_VAL, OFF); 
		SetPrintAttribute (ATTR_COLOR_MODE, VAL_GRAYSCALE);
		
		
		InstallPopup ( pPrint );
	}
	// or bring it to the foreground
	else  { 
		
		SetActivePanel (pPrint);
	}



	return 0;
}



int print_utility_done( void )
{
	if( PrintPanelOpen == TRUE )
	{
		PrintPanelOpen = FALSE;
		RemovePopup (0);
	}
	return 0;
}


int CVICALLBACK print_panel (int panel, int event, void *callbackData,int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			print_utility_done();
			break;
	}
	return 0;
}






int CVICALLBACK print_command (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event) 
	{
		case EVENT_COMMIT:

			switch (control)
			{
				case pPRINT_CANCEL: 
					print_utility_done();
					break;
				
				case pPRINT_PRINT:
					// whatever experiment is currently displayed is printed...
					print_send_to_printer();
					// after requesting a print job, get rid of teh popup
					// ... the printing takes so long, the user may push the 
					//        print request button repeatedly thinking it didn't work.
					print_utility_done();    
					break;
			
			}
			break;
	}
	return 0;
}


int print_send_to_printer( void )
{
	int orientation, PrintStatus = 0;
	
	// read the current settings
	
	// get the orientation...use to define paper size and offset
	GetPrintAttribute (ATTR_ORIENTATION, &orientation);
	switch( orientation )
	{
		case VAL_PORTRAIT:
			SetPrintAttribute (ATTR_PAPER_HEIGHT, 	2200	);		  //1250 commented values work ok for graph only printing
			SetPrintAttribute (ATTR_PAPER_WIDTH , 	1500	);		  //1900
			SetPrintAttribute (ATTR_XOFFSET, 		50		);		  //30
			SetPrintAttribute (ATTR_YOFFSET, 		50		);		  //30
			break;
		
		
		case VAL_LANDSCAPE:
			SetPrintAttribute (ATTR_PAPER_HEIGHT, 	2500	);		  //2500
			SetPrintAttribute (ATTR_PAPER_WIDTH , 	1900	);		  //1200
			SetPrintAttribute (ATTR_XOFFSET, 		50		);		  //30
			SetPrintAttribute (ATTR_YOFFSET, 		50		);		  //30
			break;
	}
	
	SetPrintAttribute (ATTR_XRESOLUTION, VAL_USE_PRINTER_DEFAULT);
	SetPrintAttribute (ATTR_YRESOLUTION, VAL_USE_PRINTER_DEFAULT);

	
	
	// for each region selected, sent to printer
	//PrintStatus = PrintCtrl (pViewer, pVIEWER_GRAPH, "", 1, 0);
	PrintStatus = PrintPanel (pViewer, "", 1, VAL_FULL_PANEL, 0);
	/*	
		The result of this function is a bit-field.


			 VAL_TOO_MANY_COPIES              (1<<0)
			 VAL_NO_MULTIPLE_COPIES           (1<<1)
			 VAL_NO_DUPLEX                    (1<<2)
			 VAL_NO_LANDSCAPE                 (1<<3)
			 VAL_CANT_FORCE_MONO              (1<<4)
			 VAL_NO_SUCH_XRESOLUTION          (1<<5)
			 VAL_NO_MULTIPLE_XRESOLUTIONS     (1<<6)
			 VAL_NO_SUCH_YRESOLUTION          (1<<7)
			 VAL_NO_MULTIPLE_YRESOLUTIONS     (1<<8)
			 VAL_NO_SEPARATE_YRESOLUTION      (1<<9)
			 VAL_USER_CANCEL                  (1<<10)


			Valid Range:      0  success
			                 xx  print report (as shown above)
			                 -1  The Interface Manager could not be
			                 ... other neg numbers ...
	*/			                 
	switch( PrintStatus )
	{
		case 0:
			// no error
			break;
		
		
	/*	case VAL_TOO_MANY_COPIES:
			break;
			
		case VAL_NO_MULTIPLE_COPIES:
			break;
			
		case VAL_NO_LANDSCAPE:
			break;	  
	*/
		default:
			printf( "Error code returned from print request: Error = %i\n", PrintStatus );	
		//		MessagePopup ("Print Error",
        //      "There was an error in the print command sent. Printing may have been canceled");			
			  break;
	}
	
			
	return 0;
}





int CVICALLBACK print_options (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int copies;        
	
	switch (event) 
	{
		case EVENT_COMMIT:

			switch( control )
			{
				case pPRINT_REGIONS_CURRENT :
					SetCtrlAttribute (pPrint, pPRINT_REGIONS_CURRENT   , ATTR_CTRL_VAL, ON);
					SetCtrlAttribute (pPrint, pPRINT_REGIONS_ALL  , ATTR_CTRL_VAL, OFF);
					PrintWhich = PRINT_WHICH_CURRENT;
					break;
			
				case	pPRINT_REGIONS_ALL   :
					SetCtrlAttribute (pPrint, pPRINT_REGIONS_CURRENT   , ATTR_CTRL_VAL, OFF);
					SetCtrlAttribute (pPrint, pPRINT_REGIONS_ALL  , ATTR_CTRL_VAL, ON); 
					PrintWhich = PRINT_WHICH_ALL; 
					break;
					
					
					
				case pPRINT_INCLUDE_RAWDATA :
					// nothing to do .. probably shouldn't allow this option
					break;

				case pPRINT_INCLUDE_PARAM :
					// make sure it is on and turn off both, all, and spectrum
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_PARAM , ATTR_CTRL_VAL, ON);
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_SPECTRUM , ATTR_CTRL_VAL, OFF); 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_BOTH , ATTR_CTRL_VAL, OFF); 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_ALL , ATTR_CTRL_VAL, OFF); 
					PrintWhat = PRINT_WHAT_PARAM;
					break;
					
				case pPRINT_INCLUDE_SPECTRUM :
					// make sure it is on and turn off both, all, and param 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_PARAM , ATTR_CTRL_VAL, OFF);
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_SPECTRUM , ATTR_CTRL_VAL, ON); 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_BOTH , ATTR_CTRL_VAL, OFF); 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_ALL , ATTR_CTRL_VAL, OFF); 
					PrintWhat = PRINT_WHAT_SPECTRUM;    
					break;
					
				case pPRINT_INCLUDE_BOTH :
					// make sure it is on and turn off param, all, and spectrum 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_PARAM , ATTR_CTRL_VAL, OFF);
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_SPECTRUM , ATTR_CTRL_VAL, OFF); 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_BOTH , ATTR_CTRL_VAL, ON); 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_ALL , ATTR_CTRL_VAL, OFF);
					PrintWhat = PRINT_WHAT_BOTH;    
					break;
			
				case	pPRINT_INCLUDE_ALL :
					// make sure it is on and turn off both, param , and spectrum 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_PARAM , ATTR_CTRL_VAL, OFF);
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_SPECTRUM , ATTR_CTRL_VAL, OFF); 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_BOTH , ATTR_CTRL_VAL, OFF); 
					SetCtrlAttribute (pPrint, pPRINT_INCLUDE_ALL , ATTR_CTRL_VAL, ON);
					PrintWhat = PRINT_WHAT_ALL;    
					break;
			
			
			
			
			
				case	pPRINT_OPTIONS_COPIES :
					
					GetCtrlVal (pPrint,pPRINT_OPTIONS_COPIES  , &copies);
					SetPrintAttribute (ATTR_NUMCOPIES, copies);
					break;
				case	pPRINT_OPTIONS_PORTRAIT :
					SetCtrlAttribute (pPrint, pPRINT_OPTIONS_LANDSCAPE  , ATTR_CTRL_VAL, OFF);
					SetCtrlAttribute (pPrint, pPRINT_OPTIONS_PORTRAIT  , ATTR_CTRL_VAL, ON);
					SetPrintAttribute (ATTR_ORIENTATION, VAL_PORTRAIT);
					break;
				case	pPRINT_OPTIONS_LANDSCAPE :
					SetCtrlAttribute (pPrint, pPRINT_OPTIONS_LANDSCAPE  , ATTR_CTRL_VAL, ON);
					SetCtrlAttribute (pPrint, pPRINT_OPTIONS_PORTRAIT  , ATTR_CTRL_VAL, OFF); 
					SetPrintAttribute (ATTR_ORIENTATION, VAL_LANDSCAPE);
					break;

			
				case pPRINT_COLOR_BW:
					SetCtrlAttribute (pPrint, pPRINT_COLOR_BW ,			ATTR_CTRL_VAL, ON);
					SetCtrlAttribute (pPrint, pPRINT_COLOR_GREYSCALE , ATTR_CTRL_VAL, OFF); 
					SetCtrlAttribute (pPrint, pPRINT_COLOR_COLOR , 		ATTR_CTRL_VAL, OFF); 
					SetPrintAttribute (ATTR_COLOR_MODE, VAL_BW);
					break;
					
				case pPRINT_COLOR_GREYSCALE:
					SetCtrlAttribute (pPrint, pPRINT_COLOR_BW ,			ATTR_CTRL_VAL, OFF);
					SetCtrlAttribute (pPrint, pPRINT_COLOR_GREYSCALE , ATTR_CTRL_VAL, ON); 
					SetCtrlAttribute (pPrint, pPRINT_COLOR_COLOR , 		ATTR_CTRL_VAL, OFF); 
					SetPrintAttribute (ATTR_COLOR_MODE, VAL_GRAYSCALE);
					break;
					
				case pPRINT_COLOR_COLOR:
					SetCtrlAttribute (pPrint, pPRINT_COLOR_BW ,			ATTR_CTRL_VAL, OFF);
					SetCtrlAttribute (pPrint, pPRINT_COLOR_GREYSCALE , ATTR_CTRL_VAL, OFF); 
					SetCtrlAttribute (pPrint, pPRINT_COLOR_COLOR , 		ATTR_CTRL_VAL, ON); 
					SetPrintAttribute (ATTR_COLOR_MODE, VAL_COLOR);
					break;
			
			
			
			
			
			
			
			}
			break;
	}
	return 0;
}






//void build_datatype_from_region( struct regionStruct Reg,  struct data_type *DataType )
//{
//	
//	DataType->width					= 	Reg.BEwidth;
//	DataType->center_energy			=	Reg.BEcenter;
//	DataType->excitation_energy 	=  Reg.ExcitationEnergy ;
//	DataType->time_p_channel 		=  Reg.TimePerChannel;
//	
//	DataType->size  = Reg.NumberOfChannels;
//	DataType->data = realloc( DataType->data, DataType->size * sizeof(double) ); 
//	DataType->data = Reg.TotalCounts;
//
//
//}






int CVICALLBACK fit_control (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int i;
	//double swap;
	
	if(panel == pViewer && control == pVIEWER_GRAPH && event == EVENT_COMMIT)
	{
			double swap;
			int plotHandle;
			
			GetGraphCursorIndex (pViewer, pVIEWER_GRAPH, 1, &plotHandle , &index1);
			GetGraphCursorIndex (pViewer, pVIEWER_GRAPH, 2, &plotHandle , &index2);
			
			
			
			if( index2 < index1 )
			{
				swap = index1;
				index1 = index2;
				index2 = swap;
			}
			if( index1 < 0 ) index1 = 0;
			if( index2 > cData.size -1 ) index2 = cData.size - 1;
			//redraw the cursors at these indices... sometimes the index does not match actual
			//   position for some reason
			SetGraphCursorIndex (pViewer, pVIEWER_GRAPH, 1, Plot_cData.Handle , index1);
			SetGraphCursorIndex (pViewer, pVIEWER_GRAPH, 2, Plot_cData.Handle , index2);
			
			value1 = cData.data[index1];
			value2 = cData.data[index2];
			
			display_window_values( index1, index2 ,  &cData ) ;        
	}
	if( panel == pViewer && control == pVIEWER_GRAPH && event == EVENT_RIGHT_DOUBLE_CLICK)
	{
			double swap;
			index1 = 0;
			index2 = cData.size - 1;
			//redraw the cursors at these indices... sometimes the index does not match actual
			//   position for some reason
			SetGraphCursorIndex (pViewer, pVIEWER_GRAPH, 1, Plot_cData.Handle , index1);
			SetGraphCursorIndex (pViewer, pVIEWER_GRAPH, 2, Plot_cData.Handle , index2);
			
			value1 = cData.data[index1];
			value2 = cData.data[index2];
			
			display_window_values( index1, index2 ,  &cData ) ;        
	}
	if ( panel == pAnalysis  &&  event == EVENT_COMMIT )
	{
	
		switch( control )
		{
			
			
			
			case pANALYSIS_SMOOTH_ON:
				if( SmoothFlag !=	SMOOTHING_SLIDING_WINDOW)
				{
					SmoothFlag = SMOOTHING_SLIDING_WINDOW;
					SetCtrlVal (pAnalysis, pANALYSIS_SMOOTH_OFF	, 0);
					
					SetCtrlAttribute (pAnalysis, pANALYSIS_SMOOTH_DEGREE, ATTR_DIMMED, 0);
					SetCtrlAttribute (pAnalysis, pANALYSIS_SMOOTH_POINTS, ATTR_DIMMED, 0);
					
				}
				SetCtrlVal (pAnalysis, pANALYSIS_SMOOTH_ON	, 1);                
				break;
				
			case pANALYSIS_SMOOTH_OFF:
				if( SmoothFlag != SMOOTHING_NONE )
				{
					SmoothFlag = SMOOTHING_NONE;
					SetCtrlVal (pAnalysis, pANALYSIS_SMOOTH_ON	, 0);
					
					SetCtrlAttribute (pAnalysis, pANALYSIS_SMOOTH_DEGREE, ATTR_DIMMED, 1);
					SetCtrlAttribute (pAnalysis, pANALYSIS_SMOOTH_POINTS, ATTR_DIMMED, 1);
				}
				SetCtrlVal (pAnalysis, pANALYSIS_SMOOTH_OFF	, 1);     
				break;
				
			case pANALYSIS_SMOOTH_DEGREE:
				GetCtrlVal (panel,control, &SmoothDegree);      
				break;
				
			case pANALYSIS_SMOOTH_POINTS:
				GetCtrlVal (panel,control, &SmoothPoints);           
				break;	
				
			case pANALYSIS_BACK_NONE:
				if( BackgroundFlag != BACKGROUND_NONE )
				{
					BackgroundFlag = BACKGROUND_NONE;
				
					SetCtrlVal (pAnalysis, pANALYSIS_BACK_LINEAR		, 0);
					SetCtrlVal (pAnalysis, pANALYSIS_BACK_SHIRLEY	, 0);
					
				}
				SetCtrlVal (pAnalysis, pANALYSIS_BACK_NONE		, 1);
				SetCtrlAttribute (pAnalysis, pANALYSIS_BACK_SHIRLEY_ITER, ATTR_DIMMED, 1);      
				
				break;
				
			case pANALYSIS_BACK_LINEAR:
				if( BackgroundFlag != BACKGROUND_LINEAR )
				{
					BackgroundFlag = BACKGROUND_LINEAR;
				
					SetCtrlVal (pAnalysis, pANALYSIS_BACK_NONE		, 0);
					SetCtrlVal (pAnalysis, pANALYSIS_BACK_SHIRLEY	, 0); 
				}
				SetCtrlVal (pAnalysis, pANALYSIS_BACK_LINEAR		, 1);
				SetCtrlAttribute (pAnalysis, pANALYSIS_BACK_SHIRLEY_ITER, ATTR_DIMMED, 1);          
				break;
				
			case pANALYSIS_BACK_SHIRLEY:
				if (BackgroundFlag != BACKGROUND_SHIRLEY )
				{
					BackgroundFlag = BACKGROUND_SHIRLEY;
				
					SetCtrlVal (pAnalysis, pANALYSIS_BACK_NONE		, 0);
					SetCtrlVal (pAnalysis, pANALYSIS_BACK_LINEAR		, 0);
				
				}
				SetCtrlVal (pAnalysis, pANALYSIS_BACK_SHIRLEY	, 1);
				SetCtrlAttribute (pAnalysis, pANALYSIS_BACK_SHIRLEY_ITER, ATTR_DIMMED, 0);          
				break;
				
			case pANALYSIS_BACK_SHIRLEY_ITER: 
				GetCtrlVal( pAnalysis,  pANALYSIS_BACK_SHIRLEY_ITER, &Background_Shirley_Iterations);       
				break;
				
			
			case pANALYSIS_FIT_NONE:
				if (FittingFlag != FITTING_NONE )
				{
					FittingFlag = FITTING_NONE;
					SetCtrlVal (pAnalysis, pANALYSIS_FIT_SPLINE		, 0);
				}
				SetCtrlVal (pAnalysis, pANALYSIS_FIT_NONE			, 1);          
				    
				break;
				
			case pANALYSIS_FIT_SPLINE:
				if (FittingFlag != FITTING_SPLINE )
				{
					FittingFlag = FITTING_SPLINE;
					SetCtrlVal (pAnalysis, pANALYSIS_FIT_NONE		, 0);
				}
				SetCtrlVal (pAnalysis, pANALYSIS_FIT_SPLINE			, 1);          
				    
				break;
			
			case pANALYSIS_CALCULATE:
				break;
				
			case pANALYSIS_FIT_APPLY:
				GetCtrlVal( pAnalysis,  pANALYSIS_FIT_APPLY, &ApplyToAll);
				break;
			
				
		}
		
			
		//set working data to equal raw data....       
		for( i=0 ; i< cData.size ; i++ ) {
			cWorkData.data[i] = cData.data[i];
			cSmoothData.data[i] = cData.data[i];
			cBackData.data[i]   = 0.0;
		}
		// if need smooth... do it
		if( SmoothFlag == SMOOTHING_SLIDING_WINDOW )
		{ 
			//smooth by sliding window approach...
			Smooth( &cSmoothData, SmoothPoints, SmoothDegree ) ;
		}
		
		if( Plot_Show_Smooth )
			Plot_A_DataType_Curve( &cSmoothData,  &Plot_cSmoothData, index1, index2, FALSE, pViewer, pVIEWER_GRAPH );       
		
		
		//if need backgorund.. calculate it
		if( BackgroundFlag == BACKGROUND_SHIRLEY )
			ShirleyBackground( &cData, &cBackData ,index1,index2, value1, value2, Background_Shirley_Iterations);
		else if( BackgroundFlag == BACKGROUND_LINEAR )
			LinearBackground( &cData, &cBackData ,index1,index2, value1, value2)  ;	  
		else /*( BackgroundFlag == BACKGROUND_NONE )*/  
			NoBackground( &cData, &cBackData ,index1,index2, value1, value2)  ;
		if( Plot_Show_Background && BackgroundFlag != BACKGROUND_NONE )
			Plot_A_DataType_Curve( &cBackData,  &Plot_cBackData, index1, index2, FALSE,pViewer, pVIEWER_GRAPH );   
		
		
		//subtract this background from the cWorkData (include smoothed function if needed):
		if( SmoothFlag == SMOOTHING_NONE ) 
			for( i = 0 ; i< cWorkData.size ; i++ )  cWorkData.data[i] -= cBackData.data[i];
		else /*SmoothFlag == SMOOTHING_SLIDING_WINDOW */
			for( i = 0 ; i< cWorkData.size ; i++ )  cWorkData.data[i] = cSmoothData.data[i] - cBackData.data[i];
		
		if( Plot_Show_Difference )
			Plot_A_DataType_Curve( &cWorkData,  &Plot_cWorkData,index1, index2, TRUE, pViewer, pVIEWER_GRAPH );    	
		
		//get curve characteristics...
		GetArea( &Area,  &cWorkData, index1, index2 ); 
		
			//area(s)
			//FWHM
			//other??
			
		display_fit_results( Area, NULL,  &cWorkData ) ;
		
		
		if( FittingFlag == FITTING_SPLINE )
		{
			double left, right;
		
			//send the working curve to the GetPeak function:
			GetPeak( &PeakPosition, &PeakHeight, &PeakFwhm, &left, &right, index1,index2, &cWorkData ) ;
			// the Position and Fwhm are in channels... convert to eV:
			PeakPositionBE= 	IndexToBinding( &cWorkData, PeakPosition ) ;  
			PeakPositionKE= 	BindingToKinetic( PeakPositionBE, cWorkData.excitation_energy );                              
			
			
			sprintf( Message, "CH: %8.2f ", PeakPosition );
			SetCtrlVal( pAnalysis, pANALYSIS_PEAK_POSITION_CH, Message );
			sprintf( Message, "BE: %8.2f (eV)", PeakPositionBE );
			SetCtrlVal( pAnalysis, pANALYSIS_PEAK_POSITION_BE, Message );
			sprintf( Message, "KE: %8.2f (eV)", PeakPositionKE );
			SetCtrlVal( pAnalysis, pANALYSIS_PEAK_POSITION_KE, Message );
			
			sprintf( Message, "%8.2f (counts)", PeakHeight );
			SetCtrlVal( pAnalysis, pANALYSIS_PEAK_HEIGHT, 	Message );
			
			
			sprintf( Message, "CH:    %8.2f", fabs(PeakFwhm ));             
			SetCtrlVal( pAnalysis, pANALYSIS_FWHM_CH, 		Message );     
			PeakFwhm= 	fabs((cWorkData.width/(double)(cWorkData.size - 1) )* PeakFwhm );    
			sprintf( Message, "BE/KE: %8.2f (eV)", PeakFwhm );             
			SetCtrlVal( pAnalysis, pANALYSIS_FWHM_EV, 		Message );      
		}
		
		else 
		{
			SetCtrlVal( pAnalysis, pANALYSIS_PEAK_POSITION_BE, 	"Unknown" );
			SetCtrlVal( pAnalysis, pANALYSIS_PEAK_POSITION_KE, 	"Unknown" );
			SetCtrlVal( pAnalysis, pANALYSIS_PEAK_POSITION_CH, 	"Unknown" );   
			SetCtrlVal( pAnalysis, pANALYSIS_PEAK_HEIGHT, 		"Unknown"  );         
			SetCtrlVal( pAnalysis, pANALYSIS_FWHM_EV, 			"Unknown" );   
			SetCtrlVal( pAnalysis, pANALYSIS_FWHM_CH, 			"Unknown" );        
		}
		
		
	}
	return 0;
}







//assumes the *.data pointer has been allocated previously...
int CopyDataType( struct data_type *Source, struct data_type *Target, int CopyArrayFlag )
{

	Target->center_energy = Source->center_energy;
	Target->width = Source->width; 
	Target->time_p_channel = Source->time_p_channel;
	Target->size = Source->size;                
	Target->excitation_energy = Source->excitation_energy;                
	Target->center_energy = Source->center_energy; 
	
	free( Target->data );
	
	Target->data = calloc( Target->size, sizeof(double) );
	
	if(  CopyArrayFlag )
	{
		int i=0;
		
		for( i = 0; i<Target->size ; i++ )
			  Target->data[i] = Source->data[i];
			  
	}
	
	return 0;
}



//assumes the Source.data is allocated and filled with data.
//assumes the Plot.Handle is already assigned to a plot that 
//   must be deleted...
int Plot_A_DataType_Curve( struct data_type *Source,  struct PlotStruct
										*Plot, int WindowIndex1, int WindowIndex2, int UseRightYaxis, int panel, int control )
{

	int  i;
	double *Counts, *Window;
	int swap;
	
	
	if( WindowIndex1 > WindowIndex2 ) 
	{
		swap = WindowIndex1;
		WindowIndex1 = WindowIndex2;
		WindowIndex2 = swap;
	}
	if( WindowIndex1 < 0 ) WindowIndex1 = 0;
	if( WindowIndex2 > Source->size -1 ) WindowIndex2 = Source->size -1;
	
	/* build x-axis channel array from window indicies */
	Window = calloc( WindowIndex2 - WindowIndex1 + 1, sizeof(double) );
	for( i = 0; i<=WindowIndex2 -WindowIndex1 ; i++ )
	{
		Window[i] = (double) i+WindowIndex1;
//		printf( "%i    %i\n", i, Window[i] );
	}
	
	if( UseRightYaxis ) SetCtrlAttribute (panel, control, ATTR_ACTIVE_YAXIS, VAL_RIGHT_YAXIS);
																					
	/* build y-axis count array from Source curve data */    
	Counts = calloc( WindowIndex2 - WindowIndex1 + 1, sizeof(double) );
	for( i = 0; i<=WindowIndex2 -WindowIndex1 ; i++ )
	{
		Counts[i] = Source->data[i + WindowIndex1];
	}
		
	
	// plot should have always had a previouly drawn plot at this point that needs deleting now...
	DeleteGraphPlot (panel, control, Plot->Handle, VAL_DELAYED_DRAW);

	//fill the graph
	//... plot all data up to current and update cursors
	
//	Plot->Handle= PlotY (panel, control, Source->data , Source->size, VAL_DOUBLE, Plot->Style,
//											Plot->TracePointStyle, Plot->LineStyle, 1, Plot->Color);
											
											
	Plot->Handle= PlotXY (	panel, 
									control, 
									Window, 
									Counts , 
									WindowIndex2 - WindowIndex1 + 1, 
									VAL_DOUBLE, 
									VAL_DOUBLE, 
									Plot->Style,
        							Plot->TracePointStyle,
        							Plot->LineStyle,
        							1,
        							Plot->Color);
										
	
	RefreshGraph ( panel, control );
	
	if( UseRightYaxis ) SetCtrlAttribute (panel, control, ATTR_ACTIVE_YAXIS, VAL_LEFT_YAXIS);
	free( Window );
	free( Counts );

	return 0;
}	





void display_fit_results( double area, double fwhm,  struct data_type *data )
{

	if( Area != NULL )
	{
		sprintf( Message, "%.2f (C)", area );
		SetCtrlVal (pAnalysis, pANALYSIS_AREA_CNTS, Message);
		
		sprintf( Message, "%.2f (C/sec)", area/ data->time_p_channel );     
		SetCtrlVal (pAnalysis, pANALYSIS_AREA_CNTS_PSEC, Message);
		
		sprintf( Message, "%.2f (C-eV)", area*fabs(data->width/data->size)  );     
		SetCtrlVal (pAnalysis, pANALYSIS_AREA_CNTS_EV, Message); 
		
		sprintf( Message, "%.2f (C-eV/sec)", area*fabs(data->width/data->size) / data->time_p_channel );     
		SetCtrlVal (pAnalysis, pANALYSIS_AREA_CNTS_EV_PSEC, Message);   
	
	
	}
	
	if( fwhm != NULL )
	{
		sprintf( Message, "%.2f (eV)", fwhm* (data->width/data->size)  );
		SetCtrlVal (pAnalysis, pANALYSIS_FWHM_EV, Message);
	
	}
	
}



void display_window_values( int index1, int index2,  struct data_type *dataPtr )
{

	sprintf( Message, "index1: %i", index1 );
	SetCtrlVal( pAnalysis, pANALYSIS_WIND_INDEX1, Message );
	sprintf( Message, "index2: %i", index2 );
	SetCtrlVal( pAnalysis, pANALYSIS_WIND_INDEX2, Message );
			
	sprintf( Message,  "BE_1: %6.2f eV", IndexToBinding( dataPtr , index1 ));
	SetCtrlVal( pAnalysis, pANALYSIS_WIND_INDEX1_BE, Message );
	sprintf( Message,  "BE_2: %6.2f eV", IndexToBinding( dataPtr , index2 ));
	SetCtrlVal( pAnalysis, pANALYSIS_WIND_INDEX2_BE, Message );
			
	sprintf( Message,  "KE_1: %6.2f eV", IndexToKinetic( dataPtr , index1 ));
	SetCtrlVal( pAnalysis, pANALYSIS_WIND_INDEX1_KE, Message );
	sprintf( Message,  "KE_2: %6.2f eV", IndexToKinetic( dataPtr , index2 ));
	SetCtrlVal( pAnalysis, pANALYSIS_WIND_INDEX2_KE, Message );

}










int CVICALLBACK analysis_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	int i;
	
	
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			analysis_utility_close_up_shop();      
			break;
		
		
		case EVENT_KEYPRESS:
			
			switch( eventData1 )
			{
				
				case VAL_DOWN_ARROW_VKEY:
					// change layer to next
					if ( CurrentLayerIndex >0 && CurrentLayerIndex <= ThisExperiment.Layers.Size-1 )
					{
						CurrentLayerIndex--;
	  
						/* update which reigon is checked */
						for( i = 0 ; i< ThisExperiment.Layers.Size ; i++ )
						{
						  		if( CurrentLayerIndex == i)
						  		{
									 // put a check marl next to the one that is choosen
									 SetMenuBarAttribute ( mAnalysis, LayerMenuItem[i]  , ATTR_CHECKED, 1);      
									 // update displayed parameters,
									 update_displayed_region( pathname, CurrentLayerIndex, CurrentRegionIndex);
				 					 
								}
								else{
									// if not choosen, make sure that that RegionMenuItem[i] is unchecked...
									SetMenuBarAttribute ( mAnalysis, LayerMenuItem[i]  , ATTR_CHECKED, 0);
								}
			
						}
						
					}
					break;
				case VAL_UP_ARROW_VKEY:
					// change layer to previous
					if ( CurrentLayerIndex >=0 && CurrentLayerIndex < ThisExperiment.Layers.Size-1 )
					{
						CurrentLayerIndex++;
	  
						/* update which reigon is checked */
						for( i = 0 ; i< ThisExperiment.Layers.Size ; i++ )
						{
						  		if( CurrentLayerIndex == i)
						  		{
									 // put a check marl next to the one that is choosen
									 SetMenuBarAttribute ( mAnalysis, LayerMenuItem[i]  , ATTR_CHECKED, 1);      
									 // update displayed parameters,
									 update_displayed_region( pathname, CurrentLayerIndex ,CurrentRegionIndex );
				 					 
								}
								else{
									// if not choosen, make sure that that RegionMenuItem[i] is unchecked...
									SetMenuBarAttribute ( mAnalysis, LayerMenuItem[i]  , ATTR_CHECKED, 0);
								}
			
						}
						
					}
					break;
					
				case VAL_HOME_VKEY:
					// change layer to first
					break;   
				case VAL_END_VKEY:
					//change layer to last
					break;
					
				case VAL_LEFT_ARROW_VKEY:
					//go to previous region
					
					if ( CurrentRegionIndex > 0 && CurrentRegionIndex <= ThisExperiment.n_o_regions-1 )
					{
						CurrentRegionIndex--;
	  
						/* update which reigon is checked */
						for( i = 0 ; i< ThisExperiment.n_o_regions ; i++ )
						{
						  		if( CurrentRegionIndex == i)
						  		{
									 // put a check marl next to the one that is choosen
									 SetMenuBarAttribute ( mAnalysis, RegionMenuItem[i]  , ATTR_CHECKED, 1);      
									 // update displayed parameters,
									 update_displayed_region( pathname, CurrentLayerIndex, i );
				 					 
								}
								else{
									// if not choosen, make sure that that RegionMenuItem[i] is unchecked...
									SetMenuBarAttribute ( mAnalysis, RegionMenuItem[i]  , ATTR_CHECKED, 0);
								}
			
						}
						
					}
					break;
				case VAL_RIGHT_ARROW_VKEY:
					//go to next region
					if ( CurrentRegionIndex >= 0 && CurrentRegionIndex < ThisExperiment.n_o_regions-1 )        
					{
						CurrentRegionIndex++;
	  
						/* update which reigon is checked */
						for( i = 0 ; i< ThisExperiment.n_o_regions ; i++ )
						{
						  		if( CurrentRegionIndex == i)
						  		{
									 // put a check marl next to the one that is choosen
									 SetMenuBarAttribute ( mAnalysis, RegionMenuItem[i]  , ATTR_CHECKED, 1);      
									 // update displayed parameters,
									 update_displayed_region( pathname, CurrentLayerIndex, i );
				 					 
								}
								else{
									// if not choosen, make sure that that RegionMenuItem[i] is unchecked...
									SetMenuBarAttribute ( mAnalysis, RegionMenuItem[i]  , ATTR_CHECKED, 0);
								}
			
						}
						
					}
					
					break;
			}
			break;
	}
	return 0;
}


















/*

//===================================================================
//===================================================================
// makes a "header" in the file with all the experiment info ...
/////int scan_save_experiment_info_in_file( struct experimentStruct Exper  )	 old version
int scan_save_experiment_info_in_file( struct experimentStruct *ExpPtr  )     
{

	int i;
	
	FILE *file_handle;
	
	//char *Date;
	//char *Time;
	
	//Date = DateStr ();
	//Time = TimeStr ();

	SetWaitCursor (1);
	
	file_handle = fopen (ExpPtr -> Pathname, "a");	// open file in append mode	      	
	
	fprintf (file_handle,"EXPERIMENT FILE\n");
	fprintf (file_handle,"Version = 2\n"); 
	fprintf (file_handle,"\n");                      
	// here we save the needed parameters to know how the expt was performed
	fprintf (file_handle,"[ Experiment Parameters ]\n");     
	
	fprintf (file_handle, "Experiment Description= %s \n", ExpPtr -> Description );
	fprintf (file_handle, "Number_of_Regions_in_Experiment= %i \n\n", ExpPtr -> n_o_regions );
	
	fprintf (file_handle,"Original_File_name= %s \n", ExpPtr -> Pathname);
	fprintf (file_handle, "Instrument= ESCA-West\nDate= %s\nTime=  %s\n\n", ExpPtr -> DateBegan, ExpPtr -> TimeBegan );  
	
//===LAYERS===========================
	 // here we save the needed parameters to know how the expt was performed
	fprintf (file_handle,"[ Layer Parameters ]\n");
	
	switch( ExpPtr->Layers.Type )
	{
		case LAYER_TYPE_NONE:
			fprintf (file_handle, "Layer_Type= XPS\n" );
			fprintf (file_handle, "Size= 1 \n" );    
			break;
			
		case LAYER_TYPE_MANUAL:
			fprintf (file_handle, "Layer_Type= Manual\n"); 
			fprintf (file_handle, "Size= %i \n" , ExpPtr->Layers.Manual.Size);               
			break;
			
		case LAYER_TYPE_POLAR:
			fprintf (file_handle, "Layer_Type= Polar\n"); 
			fprintf (file_handle, "Size= %i \n" , ExpPtr->Layers.Polar.Size );
			fprintf (file_handle, "Initial= %f \n" , ExpPtr->Layers.Polar.Initial );
			fprintf (file_handle, "Final= %f \n" , ExpPtr->Layers.Polar.Final );
			break;
			
		case LAYER_TYPE_TIMED:
			fprintf (file_handle, "Layer_Type= Timed\n"); 
			fprintf (file_handle, "Size= %i \n" , ExpPtr->Layers.Timed.Size );
			fprintf (file_handle, "Time= %f \n" , ExpPtr->Layers.Timed.Time );
			break;
			
	}
	fprintf ( file_handle,"\n" );
//-----------------------------------------------------------------------






//REGIONS:	
	for ( i=0 ; i < ExpPtr -> n_o_regions ; i++ )
	{
		
		fprintf (file_handle, "[ Region %i Parameters ]\n",  i+1);  
	
		fprintf (file_handle, "Region_Description = %s\n", ExpPtr -> Regions[i].Description); 
		fprintf (file_handle, "BEcenter(eV) = %f\n"	,	ExpPtr -> Regions[i].BEcenter);
		fprintf (file_handle, "BEwidth(eV) = %f\n"		,	ExpPtr -> Regions[i].BEwidth);
		fprintf (file_handle, "BEstep(eV) = %f\n"		,	(double) ExpPtr -> Regions[i].StepMultiple* ((double)EscaWest.DetectorWidth) / ((double)N_O_DETECTOR_CHANNELS)  ) ;     
		fprintf (file_handle, "Dwell_Time(sec) = %f\n",	ExpPtr -> Regions[i].DwellTime );          
		fprintf (file_handle, "N_o_scans = %i  \n"		,	ExpPtr -> Regions[i].NumberOfScans);
		fprintf (file_handle, "N_o_Channels = %i  \n"		,	ExpPtr -> Regions[i].NumberOfChannels);  
		
		if( ExpPtr -> Regions[i].ScanMode == SCAN_MODE_DITHERED )		fprintf (file_handle, "Scan_mode = 0  (DITHERED)\n\n");        
		else	 fprintf (file_handle, "Scan_mode = 1  (FIXED)\n\n");   
	}
	
	
	fclose (file_handle);					// close the file      	
	SetWaitCursor (0);	
	
	return 0;
}



// saves data from an experiment...
// assumes filename passed is valid... will append to end of file....
int scan_save_curve_in_file( char pathname[MAX_PATHNAME_LEN],  data_type_pointer Curve , int lIndex, int rIndex)
{
	int i;
	FILE *file_handle;
	
	double firstBE;
	double stepBE;
	
	
	SetWaitCursor (1);        
	file_handle = fopen (pathname, "a");	// open file in append mode	     	

	//****here we save the needed parameters to describe the accumulated data ****
	
	// depend upon regionIndex being correct: (a global variable)
	fprintf (file_handle, "[ L= %i R= %i ]\n"			, lIndex + 1 , rIndex+1  ); 
	//fprintf (file_handle, "N_o_data_points= %i\n"	,number_of_data_points );
	
	//**** then we save the data from this expt ************************************
	fprintf(file_handle, "BEcenter= %-8.3f\n", 	Curve->center_energy );
	fprintf(file_handle, "BEwidth= %-8.3f\n", 	-fabs(Curve->width)  );				  //always negative?
	fprintf(file_handle, "Excitation= %-8.3f\n", Curve->excitation_energy ); 
	
	fprintf(file_handle, "Time_p_channel= %-8.3f\n", Curve->time_p_channel);
	fprintf(file_handle, "Size= %i\n", 				Curve->size);							  //num points
	
	//===========================================================================
	//NOTE: 
	//			yes, tabulating the BE points is wasting disk space, but
	//       	in the interest of having cut&pasteable ASCI text for export to
	//       	other programs, this seems reasonable 
	
	
	//BY CONVENTION, 
	// a negative width means the array tabulation is	 {BEmax...BEmin}
	// a positive                                       {BEmin...BEmax}
	//
	// we will ALWAYS SAVE the data in {BEmax...BEmin} format and therefore 
	//   will always record the width as negative...
	
	if( Curve->width < 0.0 ) // array format is already ok
	{
		firstBE = Curve->center_energy - 0.5*Curve->width;					//get maxBE
		stepBE  =  Curve->width / (double) (Curve->size - 1 );		//negative 
		
		for ( i=0; i < Curve->size ; i++ ) 
			fprintf(file_handle, "%-8.3f %12.8e \n", firstBE + stepBE*i , Curve->data[i] );
	}
	else  // positive width, currently have {BEmin...BEmax} tabulation, want {BEmax...BEmin}
	{
		firstBE = Curve->center_energy - 0.5*Curve->width;				// get minBE 
		stepBE  = Curve->width / (double) (Curve->size - 1 );	// positive  
		
		for ( i = Curve->size - 1;  i >= 0 ;  i-- ) 
			fprintf(file_handle, "%-8.3f %12.8e \n", firstBE + stepBE*i , Curve->data[i] );
	}
	fprintf(file_handle, "\n" );
	//==========================================================================
	
	
 		
	fclose (file_handle);					// close the file	
	SetWaitCursor (0);								

	return 0;
	
}

*/
