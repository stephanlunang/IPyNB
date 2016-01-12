// param.c 				written by alex kay Fall/Winter 1996/97 for Fadleyu Group
//					
//	This file contains the functions pertaing to the parameter selection and
//  region definition parts of the HP-ESCA West interface program
//



#include <utility.h>
#include <userint.h>

#include "inifile.h"
#include <formatio.h>
#include "param.h"

#include "define.h"
#include "globals.h"

#include "paramh.h"		// need function prototypes
#include "detectoh.h"	// need "#define"s
#include "scanh.h"		// need function prototypes and "#define"s
#include "powerh.h"		// need function prototypes
#include "escah.h"		// need function prototypes
#include "alex_ini.h"	// need function prototypes

//panel handles
static int pParameters, pRegion, pWarning, mRegion;
static int *RegionMenuItem;       // for array of handles for programatically built part of region menu bar

int n_o_available_regions, n_o_selected_regions;
struct regionStruct *AvailableRgnArray;      /* will dynamically allocate n_o_available_regions worth of Region later with this */
struct regionStruct *SelectedRgnArray;    
struct regionStruct *TempRgnArray;

struct experimentStruct Expt;								// could make this a pointer to array as above to construct an experiemtn queue!
//struct experimentStruct experiment;

struct ListBoxStruct 
{

	int panelHandle;
	int control;
	int numVisibleItems;
	
	int listboxWidth; 
	int firstListboxItemTop;
	int leftListboxEdge;
	int rightListboxEdge;
	
	int textHeight;
	int textWidth;
	
	int numItems;
	int selectionIndex;
	
	int numDivisions;
	int *vertDivision;

} AvailableListBox, SelectedListBox ;





int region_define_go( void )
{
	struct regionStruct rgn;
	
	// if the panel is not already loaded ..
	if ( RegionPanelOpen == FALSE ) 
	{
		
		if( (pRegion = LoadPanel (0, "param.uir", pREGION)) < 0 )
			return -1;
	
		
		region_build_available_menu(); // use available regions to build menu bar
	
	
		// make sure region has no name displayed and default values are shown 
		SetCtrlVal (pRegion, pREGION_DESCRIPTION, 	"<none>"	);
		// set limits on controls....
		SetCtrlAttribute (pRegion , pREGION_TIME_STEP, ATTR_MIN_VALUE, ( (double)ACCUM_TIME_MIN ) );
		SetCtrlAttribute (pRegion , pREGION_TIME_STEP, ATTR_MAX_VALUE, ( (double)ACCUM_TIME_MAX ) ); 
	
					
		// set initial values 
		rgn = Initialize_new_region( );
		Adjust_region_parameters(  rgn );     
	
		// set the kinetic energy limits indicators
				
		// open the region panel 
		RegionPanelOpen = TRUE;
		DisplayPanel( pRegion );
	}
	
	// if the panel is already loaded ...
	else SetActivePanel( pRegion );

	return 0;
}



int CVICALLBACK region_command_buttons (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char Description[101];
	int Status;
	
	switch (event) 
	{
		case EVENT_COMMIT:

			switch( control ) 
			{
				case pREGION_OK:
				
					GetCtrlVal (pRegion, pREGION_DESCRIPTION, Description);
					Status = region_check_if_description_is_already_used( Description );
					if (Status == -2 ) ;
					if ( Status == -1 ) region_add_to_available (); 
					if ( Status >= 0 ) region_replace( Status );  // replace region with index=Status with currently displayed parameters
					
					break;
				case pREGION_DONE:
					region_close_up_shop ();
					break;
			}
			
			break;
	}
	return 0;
}









int region_check_if_description_is_already_used( char *Description )
{
	
	int Replace,i;
	
	//first check if description has been altered from the default <None>:
	if( CompareStrings (Description, 0, "<None>" ,0, 0) == 0 ) 
	{      
		MessagePopup ("DESCRIPTION ERROR", "The description <None> is not valid. \n Please revise.");
		return -2;
	       	
	}
	
	// check if name is already used
	for ( i = 0 ; i < n_o_available_regions ; i++ )  
	{
		// compare the requested string to the already defined strings... 
		if( CompareStrings (Description, 0, AvailableRgnArray[i].Description ,0, 0) == 0 ) 
		{
			
			Replace = ConfirmPopup ("Description Conflict", "Description is already used.   Replace with new definition?");
			if ( Replace ) return i;  // return index of existing region to replace...
			else return -2; // return indicated that region already exists and should not be replaced
		
		}
		
	}

	return -1;  //description is not used yet

}


int region_close_up_shop ( void )     
{
	
	//free allocated menu handles
	free(RegionMenuItem);  
	
	// just close the panel ... 
	RegionPanelOpen = FALSE;		/* global indicating panel is closed (not displayed) */
	DiscardPanel( pRegion) ;   /* we'll keep it in memory for fast access... probably doesn't matter much */
	return 0;
}


struct regionStruct Initialize_new_region( void )
{
	struct regionStruct rgn;

	//This would default us to the last defined region.... rgn =  AvailableRgnArray[ n_o_available_regions - 1 ];
	
	sprintf( rgn.Description , "<None>" );
	rgn.BEcenter  = 0.0;	 			// minimum possible 
	rgn.BEwidth = 0.0;   			// minimum possible
	rgn.StepMultiple = 1;	 		// minimum step is one channel width
	rgn.DwellTime = 1.000;  			// a second 
	rgn.NumberOfScans	= 1;
	rgn.ScanMode = SCAN_MODE_DITHERED;
	
	return rgn;
}


/* the region panel callback */
int CVICALLBACK region_panel (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event) 
	{
		case EVENT_CLOSE:
			region_close_up_shop();
			break;
	}
	return 0;
}



//int CVICALLBACK region_toggle_scan_mode (int panel, int control, int event,
//		void *callbackData, int eventData1, int eventData2)
//{
//	switch (event) 
//	{
//		case EVENT_COMMIT:
//			if ( control == pREGION_SCAN_MODE_FIXED ) 
//			{
//			  	SetCtrlVal (pRegion, pREGION_SCAN_MODE_DITHERED, 	FALSE		);   
//		  		SetCtrlVal (pRegion, pREGION_SCAN_MODE_FIXED	,	 	TRUE		);
//			
//			}
//			if ( control == pREGION_SCAN_MODE_DITHERED )
//			{
//				SetCtrlVal (pRegion, pREGION_SCAN_MODE_DITHERED, 	TRUE		);   
//		   	SetCtrlVal (pRegion, pREGION_SCAN_MODE_FIXED, 		FALSE		);     
//			
//			}
//
//			// this may mean other parameters need adjustment:
//			DisplayedRegion = Read_region_parameters( );
//			DisplayedRegion = Adjust_region_parameters( DisplayedRegion );
//			
//			break;
//	}
//	return 0;
//}



// this will check the validity of user input and adjust indicators and control limits
//    to match user request				
int CVICALLBACK region_check_param(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	// a region structure to work with 
	
	struct regionStruct DisplayedRegion;
	
	switch (event) 
	{
		case EVENT_COMMIT:
			
			
			if ( control == pREGION_SCAN_MODE_FIXED ) 
			{
			  	SetCtrlVal (pRegion, pREGION_SCAN_MODE_DITHERED, 	FALSE		);   
		  		SetCtrlVal (pRegion, pREGION_SCAN_MODE_FIXED	,	 	TRUE		);
			
			}
			if ( control == pREGION_SCAN_MODE_DITHERED )
			{
				SetCtrlVal (pRegion, pREGION_SCAN_MODE_DITHERED, 	TRUE		);   
		   	SetCtrlVal (pRegion, pREGION_SCAN_MODE_FIXED, 		FALSE		);     
			
			}
			
			
			DisplayedRegion = Read_region_parameters( );
			DisplayedRegion = Adjust_region_parameters( DisplayedRegion );
			break;
	}
	return 0;
}



// read in all the user adjustable controls on the region define panel
struct regionStruct Read_region_parameters( void )
{
	struct regionStruct rgn;
	int itemIndex;
	int on ;
	
	GetCtrlVal (pRegion, pREGION_DESCRIPTION, 	rgn.Description	); 
	///printf( " region description is = [%s].\n", rgn.Description );

	
	GetCtrlVal (pRegion, pREGION_BE_CENTER, 			&rgn.BEcenter		);
	GetCtrlVal (pRegion, pREGION_BE_WIDTH, 				&rgn.BEwidth		);
	
	GetCtrlVal (pRegion, pREGION_SCANS, 				&rgn.NumberOfScans	);
	// if fixed radio button is = 1 (on) then mode is fixed, else mode is dithered
	GetCtrlVal (pRegion, pREGION_SCAN_MODE_FIXED, 	&on	); 
	if( on ) 	rgn.ScanMode = SCAN_MODE_FIXED;
	else 		rgn.ScanMode = SCAN_MODE_DITHERED;
	// want time in integer milliseconds... user inputs time in seconds
	GetCtrlVal (pRegion, pREGION_TIME_STEP, 		&rgn.DwellTime);
	// find get the step size 
	GetCtrlVal (pRegion, pREGION_STEP_MULT, 	&rgn.StepMultiple);
	
	return rgn;
}




struct regionStruct Adjust_region_parameters(  struct regionStruct rgn )
{
	
	/* ??want to read from spec.ini file :*/
	
	
	/*************************************/
	double  max_BE, min_BE;
	
	int Total_time ;
	int itemIndex, i;

	// find max and min BE available... 
	min_BE =  EscaWest.ExcitationEnergy - power_PS_to_KE( MAX_HP_SUPPLY_VOLTAGE );
	max_BE =  EscaWest.ExcitationEnergy - power_PS_to_KE( MIN_HP_SUPPLY_VOLTAGE );
	// physically makes no sense to have BE < 0.0 
	//	   ( by our sign convention of poitive binding energies ) 
	if( min_BE < 0.0 ) min_BE = 0.0;

	
	// could check to see if name is used already, but I'll do that elsewhere...
   if(  rgn.ScanMode == SCAN_MODE_DITHERED  ){
   	//nothing needed... handled below
   }
   else if ( rgn.ScanMode == SCAN_MODE_FIXED ){
   	// force the rgnBEwidth to be one detector width wide:
   	rgn.BEwidth = EscaWest.DetectorWidth ;
   }
			 	

	// NOTE: this should be unneeded since the controls limits hould already have been set
	if ( rgn.DwellTime <  ACCUM_TIME_MIN )    rgn.DwellTime =   ACCUM_TIME_MIN ;
	if ( rgn.DwellTime >  ACCUM_TIME_MAX )    rgn.DwellTime =   ACCUM_TIME_MAX ;  
			
			
	if ( rgn.BEcenter <= min_BE ) rgn.BEcenter = min_BE + rgn.BEwidth/2.0;
	if ( rgn.BEcenter >= max_BE ) rgn.BEcenter = max_BE - rgn.BEwidth/2.0;         
	
	if ( rgn.BEwidth  >= max_BE-min_BE ) rgn.BEwidth =  max_BE-min_BE ;
	if ( rgn.BEcenter + rgn.BEwidth/2.0 >= max_BE )	rgn.BEwidth = 2.0 * ( max_BE - rgn.BEcenter );
	if ( rgn.BEcenter - rgn.BEwidth/2.0 <= min_BE )  rgn.BEwidth = 2.0 * ( rgn.BEcenter - min_BE );
	
	
///	// compute the number of channels that can fit inside the defined init/final window
/////	rgn.NumberOfChannels  =   (unsigned int) (  RoundRealToNearestInteger( (rgn.BEwidth )*  ((double)( N_O_DETECTOR_CHANNELS)) /  DetectorWidth  )) ;
/////	// the above is fine, but if we are not using the minimum step size, we must adjust:
/////	rgn.NumberOfChannels  =   ( ( double )rgn.NumberOfChannels ) / ((double )rgn.StepMultiple);
	// which tells us that the stepSize is...  minStepSize * StepMultiple... etc..
	rgn.BEstepSize =  (double) (EscaWest.DetectorWidth) / ( (double)N_O_DETECTOR_CHANNELS ) * (double)rgn.StepMultiple ;
	rgn.NumberOfDetectorBins =  (unsigned int) ((unsigned int) N_O_DETECTOR_CHANNELS/ (unsigned int) rgn.StepMultiple ) ; 
	rgn.NumberOfChannels = (unsigned int) ( RoundRealToNearestInteger( ( rgn.BEwidth ) 			\
									* (double) rgn.NumberOfDetectorBins / ( (double)EscaWest.DetectorWidth ) ));
	
	// needs further modification;
	///NO!!! the step multiple is already in there from the number of Det channels calculation!!
	///rgn.NumberOfChannels =	( ( double )rgn.NumberOfChannels ) / ((double )rgn.StepMultiple);    							
	// also we can compute the total time based upon dwell time... doesn't include PC overhead...
	if(  rgn.ScanMode == SCAN_MODE_DITHERED  )
	{
	   // number of channels times dwell time
		rgn.TotalTime = ((double) rgn.DwellTime) / 1000.0 		* ( rgn.BEwidth / EscaWest.DetectorWidth )						\
										* ( (double) N_O_DETECTOR_CHANNELS  ) /  ((double) rgn.StepMultiple ) * ( (double) rgn.NumberOfScans  ) ;
			
		// include each of the two the "dither wings"...
		rgn.TotalTime =  rgn.TotalTime *  2.0 * ( (double) rgn.DwellTime)/ 1000.0		\
													* ( (double) ( N_O_DETECTOR_CHANNELS - 1)   ) / ( (double) rgn.StepMultiple );  
			
		
	}
	else
	{
		rgn.TotalTime =  ( (double) rgn.DwellTime) / 1000.0 * ( rgn.BEwidth / EscaWest.DetectorWidth   )		\
												/ ( (double) rgn.StepMultiple   ) * ( (double) rgn.NumberOfScans    ) ;  
		
	}
	
	
	
	
	
	// update the displayed values after our adjustments...
	SetCtrlVal (pRegion, pREGION_DESCRIPTION, 	rgn.Description						);   
	SetCtrlVal (pRegion, pREGION_BE_INIT, 			rgn.BEcenter - rgn.BEwidth/2.0	);
	SetCtrlVal (pRegion, pREGION_BE_FINAL, 		rgn.BEcenter + rgn.BEwidth/2.0	);
	SetCtrlVal (pRegion, pREGION_BE_CENTER, 		rgn.BEcenter							);
	SetCtrlVal (pRegion, pREGION_BE_WIDTH, 		rgn.BEwidth								);
	SetCtrlVal (pRegion, pREGION_STEP_MULT, 	rgn.StepMultiple						);  
	//GetIndexFromValue (pRegion, pREGION_BE_STEP_MULTIPLE, &itemIndex, rgn.StepMultiple);
	//SetCtrlIndex (pRegion, pREGION_BE_STEP_MULTIPLE, itemIndex );
	
	
	if ( rgn.ScanMode ==  SCAN_MODE_DITHERED )
	{
		   SetCtrlVal (pRegion, pREGION_SCAN_MODE_DITHERED, 	TRUE		);   
		   SetCtrlVal (pRegion, pREGION_SCAN_MODE_FIXED, 		FALSE		);     
	}
	else 
	{
		   SetCtrlVal (pRegion, pREGION_SCAN_MODE_DITHERED, 	FALSE		);   
		   SetCtrlVal (pRegion, pREGION_SCAN_MODE_FIXED	,	 	TRUE		);     
	}
	
	//time_in_seconds =  ((double ) (rgn.DwellTime))/1000.0;   // rgn.DwellTime is in milliseconds as an integer
	SetCtrlVal (pRegion, pREGION_TIME_STEP, rgn.DwellTime );
	
	SetCtrlVal (pRegion, pREGION_SCANS, 		rgn.NumberOfScans		); 
	
	
	
	// there are several indicators that should be properly filled...      
	SetCtrlVal (pRegion, pREGION_EXP_CHANNELS, 	rgn.NumberOfChannels	); 	  
	
	SetCtrlVal (pRegion, pREGION_BE_STEP, 		((double) rgn.StepMultiple) * (EscaWest.DetectorWidth/( (double) (N_O_DETECTOR_CHANNELS )))	);      
	SetCtrlVal (pRegion, pREGION_BE_MIN, 		min_BE);
	SetCtrlVal (pRegion, pREGION_BE_MAX, 	    max_BE);  
	
	SetCtrlVal (pRegion, pREGION_KE_INIT, 		EscaWest.ExcitationEnergy - ( rgn.BEcenter + rgn.BEwidth/2.0 )	);	 
	SetCtrlVal (pRegion, pREGION_KE_FINAL,	 	EscaWest.ExcitationEnergy - ( rgn.BEcenter - rgn.BEwidth/2.0 )	);	
	SetCtrlVal (pRegion, pREGION_KE_WIDTH, 	    rgn.BEwidth	);
	SetCtrlVal (pRegion, pREGION_KE_CENTER, 	EscaWest.ExcitationEnergy - rgn.BEcenter);
	SetCtrlVal (pRegion, pREGION_KE_MIN, 		EscaWest.ExcitationEnergy - max_BE	);	 
	SetCtrlVal (pRegion, pREGION_KE_MAX, 		EscaWest.ExcitationEnergy - min_BE	);	 



	return rgn;
}




/* accept current region definition, add to available regions list, but don't close panel*/
int  region_add_to_available ( void )
{
	char item_label[101];
	int ok, i, ring_index, on;
	double time_in_seconds;
	struct regionStruct new_rgn;  
	
	new_rgn = Read_region_parameters( );  // read in currently displayed values as the region
	new_rgn = Adjust_region_parameters( new_rgn );
	
	TempRgnArray = calloc( n_o_available_regions, sizeof( struct regionStruct ) );
	for ( i = 0 ; i < n_o_available_regions  ; i++ )    
	{
		TempRgnArray[i] = AvailableRgnArray[i];
	}
	free( AvailableRgnArray);   // we need to reallocate it at one region size larger...
	
	
	n_o_available_regions ++ ;
	AvailableRgnArray = calloc( n_o_available_regions , sizeof( struct regionStruct ) );    
 
	for ( i = 0 ; i < n_o_available_regions - 1 ; i++ )  
	{
		AvailableRgnArray[i] = TempRgnArray[i];
	}
	free(TempRgnArray);				// free the useless temp data */
	
	// fill the last region in the array with the new one
	AvailableRgnArray[n_o_available_regions-1] = new_rgn;
	
	/* update the available regions list */
	if( ParametersPanelOpen) param_fill_avail_regions();
	if( RegionPanelOpen) region_build_available_menu();
	
	
	return 0;
}



int region_replace( int RegionIndex )
{
	struct regionStruct replacement_rgn;
	
	
	replacement_rgn = Read_region_parameters( );  // read in currently displayed values as the region
	replacement_rgn = Adjust_region_parameters( replacement_rgn );
	
	AvailableRgnArray[RegionIndex] =  replacement_rgn;

	return 0;
}






int region_help( void )
{
	MessagePopup ("Oops...",
              "Help files are not ready... ask Alex for assistance.");
	return 0;
}





// fill regions menu item with each region name and undim "regions" sub-menu item
int region_build_available_menu( void )
{
	int i;
	
	/*****Not sure how this works, but it does !! ***************/
	/* Used below for menu callback	.. it's an extern function	*/  
	void (*function)(int, int , void *,int) = region_select_from_menu;
	/************************************************************/	
	
	/*  NOTE!! can use  InstallMenuCallback (, , , i);  to set callback data (i) that is 
	     sent to the menucallback... this allows us to match user request with our array
	     of configuration data
	*/
	
	mRegion = GetPanelMenuBar (pRegion); 	//get menu handle 
	EmptyMenu (mRegion, mREGION_DEFINED);

	// allocate memory to hold the region menu item handles...
	///if ( RegionMenuItem != NULL ) free( RegionMenuItem );
	RegionMenuItem =  calloc( n_o_available_regions , sizeof( int ) ); 
	
	/* if there were some configs found in the light.ini file, use to build a menu */
	if( n_o_available_regions > 0 ){
	
		SetMenuBarAttribute (mRegion, mREGION_DEFINED , ATTR_DIMMED, FALSE);
		
		
		for( i=0 ; i < n_o_available_regions ; i++ )
		{
			RegionMenuItem[i] = NewMenuItem (mRegion, mREGION_DEFINED , AvailableRgnArray[i].Description, -1, 0, function, 0);
		}
	 	
	 	// make the first region item checked by default...
	 	///SetMenuBarAttribute ( mRegion, RegionMenuItem[0]  , ATTR_CHECKED, TRUE);     
	}
		
	/* ... but if there were no regions found  build an "empty" menu */
	else if( n_o_available_regions == 0 )
	{
		SetMenuBarAttribute (mRegion, mREGION_DEFINED , ATTR_DIMMED, TRUE);
	}

	return 0;
}




/* callback for the menu items built in light_build_menu() */
void  region_select_from_menu (int menuBar, int menuItem, void *callbackData, int panel)
{
	int i;
	  
	/* look to find out which menu item was pressed, then set display to proper value */
	for( i = 0 ; i< n_o_available_regions ; i++ )
	{
	  		if( menuItem == RegionMenuItem[i])
	  		{
				 // update displayed parameters in region ,
				 Adjust_region_parameters(  AvailableRgnArray[i]  ); 
					
			}
			
	}
	
}


void CVICALLBACK region_menu (int menuBar, int menuItem, void *callbackData, int panel)
{
	switch( menuItem )
	{
		case mREGION_FILE:
			
			break;
			
		case mREGION_FILE_OPENFILE:
			
			break;
			
		case mREGION_FILE_EXIT :
		
			break;
			
		case mREGION_HELP_PANEL:
		
			break;
			
		case mREGION_HELP_GENERAL:
		
			break;
	}

}






























int region_ini_read ( void )
{

	FILE *file; 
	long filePos;
	
	char directory[MAX_PATHNAME_LEN];
	char *pathname;
	int i, error;
	char string[64];																													 
	
	SetWaitCursor (1);				/*  turn on the hourglass	*/        
	
	// find out what the execuable directory is...
	if(  (error = GetProjectDir (directory)) != 0 ) return -1;;
	// the *.ini files are supposed to be stored here...
	pathname = strcat (directory, "\\regions.ini");
	
	//open the file...
	file = fopen (pathname, "r");	/* open file in read only mode	*/    
	
	// read in the number of saved regions...
	error = ini_find_section( file, "General" , '[', ']', ';' , &filePos );    
	
	error = ini_read_int_after_tag_value( file, filePos, '[', '=', ';' , 
								"Number_of_Defined_Regions", &n_o_available_regions );
//	printf( " num_reg = %i \n ",  n_o_available_regions );
	
	// allocate memory for these regions: 
	AvailableRgnArray = calloc( n_o_available_regions, sizeof(struct regionStruct) );   
	
	
	//read in each of the saved regions...
	for (i=0; i < n_o_available_regions ; i++) 
	{ 
		// build the token ... ie [Region ##]	
		sprintf( string, "Region %i" , i+1) ;
		//locate the token in the file...
		error = ini_find_section( file, string , '[', ']', ';' , &filePos );
		
			// find the description
			error = ini_read_string_after_tag_value( file, filePos, '[', '=', ';' , 
								"Description",
								AvailableRgnArray[i].Description , 256);
			// find the center binding energy  (eV)
			error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
								"Binding_Energy_Center",
								&AvailableRgnArray[i].BEcenter );
			// find the binding energy width (eV)
			error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
								"Binding_Energy_Width",
								&AvailableRgnArray[i].BEwidth );
			// find the step multiple
			error = ini_read_unsigned_long_after_tag_value( file, filePos, '[', '=', ';' , 
								"Step_Multiple",
								&AvailableRgnArray[i].StepMultiple );  
			// find the number of scans
			error = ini_read_unsigned_long_after_tag_value( file, filePos, '[', '=', ';' , 
								"Number_of_Scans",
								&AvailableRgnArray[i].NumberOfScans );  
			// find the dwell time  ( integer millisec )
			error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
								"Dwell_Time",
								&AvailableRgnArray[i].DwellTime);  
			// find the scan mode ( 0 = dithered, 1= fixed )
			error = ini_read_unsigned_long_after_tag_value( file, filePos, '[', '=', ';' , 
								"Scan_Mode",
								&AvailableRgnArray[i].ScanMode );  
			
		
	/*	printf(" %i)Des = %s****  center = %f, width = %f , dwell = %i \n",
							i,
							AvailableRgnArray[i].Description,
							AvailableRgnArray[i].BEcenter,
							AvailableRgnArray[i].BEwidth,
							AvailableRgnArray[i].DwellTime );
																*/
		
		
		// fill the rest of the region info based upon data read in:       
		
		// the StepSize
		AvailableRgnArray[i].BEstepSize =  (double) (EscaWest.DetectorWidth) / ( (double)N_O_DETECTOR_CHANNELS ) * (double)AvailableRgnArray[i].StepMultiple ;
		// NumberOfDetectorBins:
		AvailableRgnArray[i].NumberOfDetectorBins  															\
								=  (unsigned int) ((unsigned int) N_O_DETECTOR_CHANNELS/ (unsigned int) AvailableRgnArray[i].StepMultiple ) ;
	
		// find how many step widths ( detector channel width * Region.StepMultiple )  wide this region is...
		AvailableRgnArray[i].NumberOfChannels =  																							\
									(unsigned int) ( RoundRealToNearestInteger(  AvailableRgnArray[i].BEwidth *((double)N_O_DETECTOR_CHANNELS) 	 \
									/ (  (double)EscaWest.DetectorWidth * (double) AvailableRgnArray[i].StepMultiple)  ));

		// The TotalTime
		if(  AvailableRgnArray[i].ScanMode == SCAN_MODE_DITHERED  )
		{
		   // number of channels times dwell time
			AvailableRgnArray[i].TotalTime = ((double) AvailableRgnArray[i].DwellTime) / 1000.0 				\
													* ( AvailableRgnArray[i].BEwidth / EscaWest.DetectorWidth )						\
													* ( (double) N_O_DETECTOR_CHANNELS          ) 							\
													/ ( (double) AvailableRgnArray[i].StepMultiple )						\
													* ( (double) AvailableRgnArray[i].NumberOfScans  ) ;
			
			// include each of the two the "dither wings"...
			AvailableRgnArray[i].TotalTime =  AvailableRgnArray[i].TotalTime   										\
													+  2.0 * ( (double) AvailableRgnArray[i].DwellTime)/ 1000.0			\
													* ( (double) ( N_O_DETECTOR_CHANNELS - 1)   ) 							\
													/ ( (double) AvailableRgnArray[i].StepMultiple );  
		
		}
		else
		{
			AvailableRgnArray[i].TotalTime =  ( (double) AvailableRgnArray[i].DwellTime) / 1000.0 			\
												* ( AvailableRgnArray[i].BEwidth / EscaWest.DetectorWidth   )					\
												/ ( (double) AvailableRgnArray[i].StepMultiple   )						\
												* ( (double) AvailableRgnArray[i].NumberOfScans    ) ;  
		
		}
	}
	
	// close the file
	fclose(file);
	SetWaitCursor (0);				/*  turn off the hourglass	*/       
	return 0;
}



int region_ini_write ( void )
{

	FILE *file; 
	//long filePos;
	
	char string[64];
	int i, error;
	
	char directory[MAX_PATHNAME_LEN];
	char *pathname;
	
	SetWaitCursor (1);	/*  turn on the hourglass	*/        
	
	// find out what the execuable directory is...
	if(  (error = GetProjectDir (directory)) != 0 ) return -1;;
	// the *.ini files are supposed to be stored here...
	pathname = strcat (directory, "\\regions.ini");
	
	//open the file in write mode ...
	file = fopen (pathname, "w");	
	
	
	// record the number of regions...
	fprintf (file, "[General]\n" );
	fprintf (file,  "Number_of_Defined_Regions = %i\n", n_o_available_regions);
	fprintf (file, "\n");
	
	// write the information message (semicolon indicates a comment)...
	fprintf( file, ";  What is stored and how: \n");
	fprintf( file, "; \n");  
	fprintf( file, "; Description is 256 char maximum \n");  
	fprintf( file, "; Binding_Energy_Center  in eV \n");  
	fprintf( file, "; Binding_Energy_Width  in eV \n");  
	fprintf( file, "; Step_Multiple is number of detector channels wide the data channels are \n");  
	fprintf( file, "; Number_of_Scans is just that \n");  
	fprintf( file, "; Dwell_Time is in seconds \n");  
	fprintf( file, "; Scan_Mode:  0 == DITHERED, 1 == FIXED \n");  
	fprintf( file, "; \n");
	fprintf (file, "\n"); 
	
	// now record each of the regions...
		//read in each of the saved regions...
	for (i=0; i < n_o_available_regions ; i++) 
	{ 
		// build the token ... ie [Region ##]	
		sprintf( string, "[Region %i]" , i+1) ;
		fprintf( file, "%s\n", string );
		
		
		// find the description
		fprintf( file, "Description = %s\n", AvailableRgnArray[i].Description );
		// find the center binding energy  (eV)
		fprintf( file, "Binding_Energy_Center = %-8.4f\n", AvailableRgnArray[i].BEcenter );
		// find the binding energy width (eV)
		fprintf( file,"Binding_Energy_Width  = %-8.4f\n",  AvailableRgnArray[i].BEwidth );
		// find the step multiple
		fprintf( file,"Step_Multiple = %-i\n", AvailableRgnArray[i].StepMultiple );  
		// find the number of scans
		fprintf( file,"Number_of_Scans = %-i\n", AvailableRgnArray[i].NumberOfScans );  
		// find the dwell time  ( integer millisec )
		fprintf( file,"Dwell_Time = %-8.4f\n", AvailableRgnArray[i].DwellTime);  
		// find the scan mode ( 0 = dithered, 1= fixed )
		fprintf( file, "Scan_Mode = %-i\n", AvailableRgnArray[i].ScanMode ); 
		
		fprintf (file, "\n"); 
	}
	
	// close the file
	fclose(file);
	SetWaitCursor (0);	/*  turn off the hourglass	*/       
	return 0;
}



int region_ini_append_single_region ( void )
{

	FILE *file;
	//long filePos;
	
	SetWaitCursor (1);	/*  turn on the hourglass	*/    

// close the file
	fclose(file);
	SetWaitCursor (0);	/*  turn off the hourglass	*/       
	return 0;
}





























int parameters_go( void )
{
	
	int num_checked, i;
	
	
	// if the panel is not already loaded ..
	if ( ParametersPanelOpen == FALSE )
	{
		ParametersPanelOpen = TRUE;
		// load the panel
		if ( (pParameters = LoadPanel (0, "param.uir", pPARAM) ) < 0)
			return -1;
		
//		avail_build_list_box ( );	// draws the vertical separation bars...
		
		
		// fix current displayed items before displaying... looks bad to have things flashing and changing after displaying
		param_fill_avail_regions();
		
		//fill the selected array with any selected regions (should be none first time through...
		ClearListCtrl (pParameters, pPARAM_SELECTED_REGIONS);
		for( i = 0 ; i < n_o_selected_regions ; i++ ){
			 InsertListItem (pParameters, pPARAM_SELECTED_REGIONS, -1, SelectedRgnArray[i].Description, i);
		}
		
		SetCtrlVal (pParameters, pPARAM_N_AVAIL, n_o_available_regions);   
		SetCtrlVal (pParameters, pPARAM_N_SELECTED, n_o_selected_regions);   		
		// now display the panel
		DisplayPanel( pParameters );
	}
	
	// if the panel is already loaded ...
	else SetActivePanel( pParameters );

	GetNumCheckedItems (pParameters, pPARAM_AVAILABLE_REGIONS, &num_checked);
	// if at least one item is checked then we allow user to push the "select checked regions" button"... 
	if( num_checked > 0 )SetCtrlAttribute (pParameters, pPARAM_REGION_SELECT_ADD, 	ATTR_DIMMED, FALSE); 
	else  SetCtrlAttribute (pParameters, pPARAM_REGION_SELECT_ADD, 	ATTR_DIMMED, TRUE);
	
	return 0;
}




/*--------------------------------------------------------------------------------------*/ 
/*----     Callback function for the "Parameters" panel = PARAMS 				----*/       
/*----																				----*/
int Parameters_panel (int panel, int event, void *callbackData, int eventData1, int eventData2)
{   

	switch (event) {
		case EVENT_CLOSE:
			// discard panel and save region definitions
			param_close_up_shop();
			break;
	}
	return 0;
}



/* fill available region list on the parameter panel */
/*    uses regions read in froim region.ini          */
int param_fill_avail_regions( void )
{
	int i,j;		/* will be used as the value of the label/value pir*/
//	char info_string[500];
	char info_string[128];
	char buf[32];
	char abbrev_description[35];
	
	int vLine;   // position of first vertical line

	
	ClearListCtrl (pParameters, pPARAM_AVAILABLE_REGIONS);         
	
	//update the number of available regions indicator:
	
	SetCtrlVal (pParameters, pPARAM_N_AVAIL, n_o_available_regions);

	/* n_o_available regions is number of items in the list */
	for ( i=0 ; i < n_o_available_regions ; i++) 
	{        
		
		vLine = 180;
		
		for( j = 0 ; j<30 ; j++ )
	   {
	   	//first check that descipttion is >35 char???!!!
	   	abbrev_description[j] = AvailableRgnArray[i].Description[j];
	   	
	   	if( AvailableRgnArray[i].Description[j] == NULL )
	   	{
	   	
	   		for(  ; j < 35 ; j++ )
	   		{
	   		 AvailableRgnArray[i].Description[j] = ' ';
	   		}
	   		AvailableRgnArray[i].Description[35] = '\0';    
	   		
	   		break;
	   	}
	   	
	   	if( j == 29 ){
			   abbrev_description[j]   = ' ' ;      	
			   abbrev_description[j+1] = '.' ; 
			   abbrev_description[j+2] = '.' ;    
			   abbrev_description[j+3] = '.' ; 
			   abbrev_description[j+4] = '\0';
			   break;
	   	}
	   	
	   }
	   
		sprintf( info_string, "%-60s" , abbrev_description);
		InsertListItem (pParameters, pPARAM_AVAILABLE_REGIONS, -1 , info_string , i);   				
	}

	return 0;
}



/* callback for  region selection control buttons....ADD and NEW */
int CVICALLBACK param_region (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{

	int  item_index, checked, selected_index ;
	int item_value, label_length, i;
	

	switch (event) {
		case EVENT_COMMIT:
			switch(control) 
			{
			
				// user requested that regions be added to selected list from the available list
				case pPARAM_REGION_SELECT_ADD:
					
					// clear the "selected" list 
					ClearListCtrl (panel, pPARAM_SELECTED_REGIONS);
					
					// allocate the memory for each selected region
					GetNumCheckedItems (panel, pPARAM_AVAILABLE_REGIONS, &n_o_selected_regions );
					SelectedRgnArray = calloc( n_o_selected_regions, sizeof(struct regionStruct) );  
				
				
					/* if the label/item is checked in the "available" list, copy to the "selected" list*/
					GetNumListItems (panel, pPARAM_AVAILABLE_REGIONS , &n_o_available_regions);
					
					selected_index = 0;
					
					for( item_index = 0 ; item_index < n_o_available_regions ; item_index++)
					{
						
						IsListItemChecked (panel, pPARAM_AVAILABLE_REGIONS ,item_index, &checked );
						
						if( checked )
						{
							
							/* get the list value for current item (equal to array index)*/
							GetValueFromIndex (panel, pPARAM_AVAILABLE_REGIONS , item_index, &item_value);
							// assign this available region to a selected region 
							SelectedRgnArray[selected_index] = AvailableRgnArray[item_value]; 
							
							/* get the label length, allocate memory, get label */
							//GetLabelLengthFromIndex (panel, pPARAM_AVAILABLE_REGIONS , item_index, &label_length);
							//item_label = calloc( label_length + 1, sizeof(char) );   /* need label_length +1 to hold null terminator */
							//GetLabelFromIndex (panel, pPARAM_AVAILABLE_REGIONS , item_index, item_label);
							
							/* then create one of these in the "selected" list */
							//InsertListItem (panel, pPARAM_SELECTED_REGIONS, -1 , item_label, item_value);
							InsertListItem (panel, pPARAM_SELECTED_REGIONS, -1 , SelectedRgnArray[selected_index].Description , item_value);  
							selected_index++ ;
						
							
						}
					
						
					}
					// need to fill a structure of type experimentStruct: (global for now)
					Expt.n_o_regions = n_o_selected_regions ;
					Expt.Regions  		=  SelectedRgnArray ;			// ???? Does this work ????? what if SelectedRgnArray is "free()"ed ??
					
					Expt.TotalTime = 0.0;        
					
					//was for( i = 1 ; i <= Expt.n_o_regions ; i++ )
					for( i = 0 ; i < Expt.n_o_regions ; i++ )   
					{
						Expt.TotalTime = Expt.TotalTime + Expt.Regions[i].TotalTime;                      
					}
					
					// update the number of selected regions...
					SetCtrlVal (pParameters, pPARAM_N_SELECTED, n_o_selected_regions);
					
					//calculate the time for this experiment:
					//Expt = experiment_calculate_times ( Expt );
					
					
					
					
					break;
				
				
				case pPARAM_REGION_AVAIL_NEW:
					// open and configure the region definition panel
					region_define_go(  );  
					break;
				
				case pPARAM_REGION_AVAIL_CUT: 
				
					// ask user if they really want to delete the checked regions...
					
					// count the number of checked regions
					GetNumCheckedItems (panel, pPARAM_AVAILABLE_REGIONS, &n_o_selected_regions );
					if( n_o_selected_regions == 0 ) return 0;
					
					// allocate a temp array of regions to hold new smaller set of regions  
					TempRgnArray = calloc( n_o_available_regions - n_o_selected_regions , sizeof(struct regionStruct) );  
					
					
					// write only the non-checked regions to the temp one
					selected_index = 0;
					for( item_index = 0 ; item_index < n_o_available_regions ; item_index++)
					{
						
						IsListItemChecked (panel, pPARAM_AVAILABLE_REGIONS ,item_index, &checked );
						
						if( !checked )
						{
							/* get the list value for current item (equal to array index)*/
							GetValueFromIndex (panel, pPARAM_AVAILABLE_REGIONS , item_index, &item_value);
							// assign this available region to a selected region 
							TempRgnArray[selected_index] = AvailableRgnArray[item_value];
							selected_index++ ;     
						}
					}
					
					// update the new number of available regions
					n_o_available_regions =  n_o_available_regions - n_o_selected_regions;
					
					// reallocate the old one at the smaller size
					free( AvailableRgnArray );
					//was: AvailableRgnArray = calloc( n_o_available_regions + 1 , sizeof(struct regionStruct) );
					AvailableRgnArray = calloc( n_o_available_regions  , sizeof(struct regionStruct) );
			
					// copy temp array to new array ... can I just say AvailableRgnArray = TempRgnArray??? haven't tried it...
					
					//was: for( item_index = 1 ; item_index <= n_o_available_regions  ; item_index++ )
					for( item_index = 0 ; item_index < n_o_available_regions  ; item_index++ ) 
					{
						AvailableRgnArray[item_index] = TempRgnArray[item_index];	
					}
					//free temp array
					free(TempRgnArray);
					
					// update the dispalyed available regions...
					/*if( ParametersPanelOpen*/ param_fill_avail_regions( );
					
					break;
				
				default:
					printf( "something went wrong in param_region() \n" );
					break;
			
			}
			
			break;
	}
	return 0;
}
/* callback for the available regions list */
int CVICALLBACK parameters_region_available_list (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	int num_checked;
	int item_index, item_value,i;
	
	int RightButtonDown;
	char Message[500];
	
	switch (event) 
	{
		case EVENT_COMMIT:
			
			GetNumCheckedItems (panel, control, &num_checked);
			
			// if at least one item is checked then we allow user to push the "select checked regions" button"... 
			if( num_checked > 0 )SetCtrlAttribute (pParameters, pPARAM_REGION_SELECT_ADD, 	ATTR_DIMMED, FALSE); 
			else  SetCtrlAttribute (pParameters, pPARAM_REGION_SELECT_ADD, 	ATTR_DIMMED, TRUE);
			
			break;
			
		
		
		case EVENT_LEFT_DOUBLE_CLICK:
			
			// add the double clicked item to the selected region list
			GetCtrlIndex (panel, control, &item_index);
			GetValueFromIndex (panel, control, item_index, &item_value);
			//if double clicked, don't check the item
			CheckListItem (panel,control , item_index, 0);
			
			if( n_o_selected_regions == 0 )  
			{
				n_o_selected_regions++;  
				//allocate space for one region
				SelectedRgnArray = calloc( n_o_selected_regions, sizeof(struct regionStruct) ); 
				
			}
			else
			{
				//add the region cooresponding to the index  to the selected regions
				// reallocate the memory to one region larger...
				n_o_selected_regions++;
				SelectedRgnArray= realloc (SelectedRgnArray , n_o_selected_regions * sizeof(struct regionStruct));
			
			}
			
			// fill the last array element with the requested region
			SelectedRgnArray[n_o_selected_regions-1] = AvailableRgnArray[item_value];
			//add item to end of selected list box
			InsertListItem (panel, pPARAM_SELECTED_REGIONS, -1 , SelectedRgnArray[n_o_selected_regions-1].Description , n_o_selected_regions-1);   
			
			
			
			
			//BELOW SHOULD BE A FUNCTION CALLED compile_experiement_from_request()
	
			// need to fill a structure of type experimentStruct: (global for now)
			Expt.n_o_regions = n_o_selected_regions ;
			Expt.Regions  		=  SelectedRgnArray ;			// ???? Does this work ????? what if SelectedRgnArray is "free()"ed ??
			
			for( i = 0 ; i < Expt.n_o_regions ; i++ )   
			{
				Expt.TotalTime = Expt.TotalTime + Expt.Regions[i].TotalTime;                      
			}
					
			// update the number of selected regions...
			SetCtrlVal (pParameters, pPARAM_N_SELECTED, n_o_selected_regions);
					
			//calculate the time for this experiment:
			//Expt = experiment_calculate_times ( Expt );
			
			break;
			
		}																				 
	
	return 0;
}


/* callback for the selected regions list */
int CVICALLBACK parameters_region_selected_list (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int num_checked;
	int item_index, item_value,i,j;

	switch (event) 
	{
		case EVENT_COMMIT:
			/*
			GetNumCheckedItems (panel, control, &num_checked);
			// if at least one item is checked... 
			if( num_checked > 0 )SetCtrlAttribute (pParameters, pPARAM_REGION_SELECT_ADD, 	ATTR_DIMMED, FALSE); 
			else  SetCtrlAttribute (pParameters, pPARAM_REGION_SELECT_ADD, 	ATTR_DIMMED, TRUE);
			
			// if only one region is checked, "edit region" button is available for the "selected" regions
			///if( num_checked == 1 )  SetCtrlAttribute (pParameters, pPARAM_REGION_SELECT_EDIT, 	ATTR_DIMMED, FALSE);      
			///else  SetCtrlAttribute (pParameters, pPARAM_REGION_SELECT_EDIT, 	ATTR_DIMMED, TRUE);
			*/
			break;
		
		case EVENT_RIGHT_CLICK:
			//want to popup with the clicked on region parameters??
			break;
			
		case EVENT_LEFT_DOUBLE_CLICK:
			//get rid of the double clicked region...
			
			// count the click on region
				// add the double clicked item to the selected region list
			GetCtrlIndex (panel, control, &item_index);
			if( item_index == -1 ) return 0;		//no items in the list box... nothing to do...
			
			GetValueFromIndex (panel, control, item_index, &item_value);
		
			// allocate a temp array of regions to hold new smaller set of regions  
			TempRgnArray = calloc( n_o_selected_regions  , sizeof(struct regionStruct) );  
			//copy the current selected elements to the temp array
			for( i = 0 ; i < n_o_selected_regions ; i++)
			{
				/* get the list value for current item (equal to array index)*/
				GetValueFromIndex (panel, pPARAM_AVAILABLE_REGIONS , i, &item_value);
				// assign this available region to a selected region 
				TempRgnArray[item_value] = SelectedRgnArray[item_value];
			}
			
			
			//decrement the number of selected regions 
			n_o_selected_regions--; 
			
			// reallocate the old one at the smaller size
			free( SelectedRgnArray );
			SelectedRgnArray = calloc( n_o_selected_regions  , sizeof(struct regionStruct) );
			
			
			// copy temp array to new array ... 
			j = 0	;	
			// for each of the temp region ( n_o_selected_regions+1 of them now )
			for( i = 0 ; i< n_o_selected_regions+1  ; i++ ) 
			{
				//if not at the item user double left clicked
				if ( i != item_index ){
					//include region in updated selected region list
					SelectedRgnArray[j] = TempRgnArray[i];
					j++;
				}
					
			}
			
			//free temp array
			free(TempRgnArray);
					
			
			// update the displayed selected regions...
			if( ParametersPanelOpen )
			{
				ClearListCtrl (panel, control);
				for( i = 0 ; i < n_o_selected_regions ; i++ ){
					 InsertListItem (panel, pPARAM_SELECTED_REGIONS, -1, SelectedRgnArray[i].Description, i);
				}
				
				//BELOW SHOULD BE A FUNCTION CALLED compile_experiement_from_request()
	
				// need to fill a structure of type experimentStruct: (global for now)
				Expt.n_o_regions = n_o_selected_regions ;
				Expt.Regions  		=  SelectedRgnArray ;			// ???? Does this work ????? what if SelectedRgnArray is "free()"ed ??
			
				for( i = 0 ; i < Expt.n_o_regions ; i++ )   
				{
					Expt.TotalTime = Expt.TotalTime + Expt.Regions[i].TotalTime;                      
				}
					
				// update the number of selected regions...
				SetCtrlVal (pParameters, pPARAM_N_SELECTED, n_o_selected_regions);
				
			}
			break;
			
			
	}
	return 0;
}




/* close panel if open and if there is an unsaved config, ask user what to do */           
int param_close_up_shop(void)
{
	if( ParametersPanelOpen) {
		ParametersPanelOpen = FALSE;	
		DiscardPanel( pParameters) ;
		
		// free the allocated region arrays
		////free( AvailableRgnArray );			//causes problems when writing to file
		////free( SelectedRgnArray  );			// may casue probelms with Expt.Regions ptr which was set = to this ptr!
		
	}
	
	// save region definitions in file
	region_ini_write();
	
	////free( AvailableRgnArray );			//causes problems when writing to file       
	////free( SelectedRgnArray  );	// may casue probelms with Expt.Regions ptr which was set = to this ptr!               
	return 0;
}


// when user wants to begin scan, make sure experiment is valid, then start the experiment
int CVICALLBACK param_control_buttons (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	char Description[256];
	char pathname[MAX_PATHNAME_LEN];
	int FileStatus;
	int m;
	
	switch (event) {
		case EVENT_COMMIT:
			switch ( control ) 
			{
				// if user pushed the start button, get ready to run the experiment   
				case pPARAM_STARTEXP :
				
					if( esca_check_if_utilities_are_open() == TRUE )
					{
						if( ConfirmPopup ("WARNING",
              					"All utilities (except the detector monitor) must be closed before proceeding. Do you want me to close them for you?")
							) esca_close_all_utilities();
						else return 0;
						
					}
					
					
					// if there is at least one region choosen...
					if ( Expt.n_o_regions > 0 )
					{
					
						GetCtrlVal (pParameters ,pPARAM_DESCRIPTION, 	Expt.Description	);
						GetCtrlVal (pParameters ,pPARAM_WAITBETWEEN, 	&Expt.PauseBetweenRegions	);   
					
						// get a filename to save the experiment in
						FileStatus = param_get_a_filename( Expt.Pathname );  
						// if a filename was choosen, proceed...
						if( FileStatus == 0 )
						{
							// close the extra panels
							if( ParametersPanelOpen )
							{
								ParametersPanelOpen = FALSE;
								DiscardPanel( pParameters );
							}
							if( RegionPanelOpen )
							{
								RegionPanelOpen = FALSE;   
								DiscardPanel ( pRegion );
							}
						
							// compute the total time of the experiment (ideal time... no computational lag)
							Expt.TotalTime = 0;
							//was: for( m=1 ; m < Expt.n_o_regions ; m++ )     
							for( m=0 ; m < Expt.n_o_regions ; m++ )
							{
								Expt.Regions[m].PercentDone = 0.0;		 ///??? should this be done when the region is loaded???
								Expt.TotalTime += Expt.Regions[m].TotalTime;
							}
						
							scan_start_experiment( &Expt );
							//scan_start_experiment( SelectedRgnArray, n_o_selected_regions,
							//	 								 Description, pathname );
						}
						// ... otherwise do nothing.
						else return 0;
					}
					
					// if user has not choosen a region, ask them to...
					else  MessagePopup ("INVALID EXPERIMENT",  "Please select at least one region.");
					break;
					
			   	// if user pushed the cancel button... close the panel   
				case pPARAM_CANCEL :
					param_close_up_shop();
					break;
					
				
			}
			
			break;
	}
	return 0;
}



// returns 0 if successful in getting filename,  -1 if not
int param_get_a_filename( char Pathname[MAX_PATHNAME_LEN] )
{
	int status, returnValue, Response;
	
	
	//make a date based suggested for the pathname...
	//GetDir (Dir);
	//sprintf( pathname," %s\\ 
	
	
	
	// get a filename for the expermeint to be saved in 
	status = FileSelectPopup ("c:\\alexesca\\data", "*.xps", ".xps", "Select Experiment Storage File",
					VAL_OK_BUTTON, 0, 0, 1, 1,Pathname );
	switch( status )
	{
		case  VAL_NO_FILE_SELECTED:
			// if no name is selected, return an error
			returnValue = -1;					
			break;
						
		case  VAL_EXISTING_FILE_SELECTED:
			
			Response = ConfirmPopup ("WARNING",
                         "This file already exists.  Do you want me to replace it?");
	 		if (Response == FALSE) returnValue = -1;
	 		else if (Response == TRUE) {
	 			// erase the file now...
	 			remove (Pathname);
	 			returnValue = 0;
	 		}
			break;
						 
		case  VAL_NEW_FILE_SELECTED:
			returnValue = 0; 
			break;
						
		default:
			printf( "Error in file selection. FileSelectPopup() returned status = %i \n", status);
			break;
			
	}
				
	return returnValue;
}


void CVICALLBACK param_menu (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	switch( menuItem )
	{
		 case( mPARAM_PANEL_EXIT ):
		    // user wants to close this panel
		    param_close_up_shop();
		 	break;
		 	
		 
		case( mPARAM_HELP_THIS_PANEL ):
		    // user wants help with this panel
		    param_help();
		 	break;
		 	
		case( mPARAM_HELP_GENERAL ):
		    // user wants help with the program
		    //esca_general_help();
		 	break;
	
	   default:
	   		printf( "Error in param_menu(): could not match case = %i \n" , menuItem);
	   		break;
	}

}


int param_help( void )
{
	MessagePopup ("Oops...",
              "Help files are not ready... ask Alex for assistance.");
	return 0;
}






// assumes that  Expt.Regions[i].ScanMode is properly set !
struct experimentStruct experiment_calculate_times( struct experimentStruct Expt )
{
	//fills each Expt.Regions[i].TotalTime		  (seconds)
	//fills the Expt.TotalTime						  (seconds)
	
	//note: DwellTime is an integer number of milliseconds     
	int i;
	
	Expt.TotalTime = 0.0;
	//was for( i = 1 ; i <= Expt.n_o_regions ; i++ )   
	for( i = 0 ; i < Expt.n_o_regions ; i++ )
	{
		if( Expt.Regions[i].ScanMode == SCAN_MODE_DITHERED )						 
		{									
			// number of channels times dwell time
			Expt.Regions[i].TotalTime = ((double) Expt.Regions[i].DwellTime) *1000.0 						\
													* ( Expt.Regions[i].BEwidth / EscaWest.DetectorWidth )					\
													* ( (double) N_O_DETECTOR_CHANNELS          ) 					\
													/ ( (double) Expt.Regions[i].StepMultiple )					\
													* ( (double) Expt.Regions[i].NumberOfScans  ) ;
			
			// include each of the two the "dither wings"...
			Expt.Regions[i].TotalTime =  Expt.Regions[i].TotalTime   											\
													+  2.0 * ( (double) Expt.Regions[i].DwellTime) *1000.0		\
													* ( (double) ( N_O_DETECTOR_CHANNELS - 1)   ) 					\
													/ ( (double) Expt.Regions[i].StepMultiple );  
			
		}
		else if ( Expt.Regions[i].ScanMode == SCAN_MODE_FIXED ) 
		{
			Expt.Regions[i].TotalTime =  ( (double) Expt.Regions[i].DwellTime) * 1000.0 					\
												* ( Expt.Regions[i].BEwidth / EscaWest.DetectorWidth   )						\
												/ ( (double) Expt.Regions[i].StepMultiple   )						\
												* ( (double) Expt.Regions[i].NumberOfScans    ) ;  
		
		}
		
		Expt.TotalTime = Expt.TotalTime + Expt.Regions[i].TotalTime;
	}
	
	return Expt;

}

// string must be 14 chars long DDDD:HH:MM:SS + NULL
int seconds_to_DDDD_HH_MM_SS_string( double seconds,  char *string )
{
	unsigned int DDDD;
	unsigned int HH;
	unsigned int MM;
	unsigned int SS;
	
	//printf( "Passed: %f,  " , seconds );
	
	
	DDDD = 	TruncateRealNumber ( seconds / 86400.0 ); 
	seconds = seconds - 86400.0*DDDD;
	
	HH =  TruncateRealNumber ( seconds / 3600.0 );
	seconds = seconds - 3600.0*HH;
	
	MM =  TruncateRealNumber ( seconds / 60 );
	
	SS = TruncateRealNumber ( seconds ) - 60*MM;
	
	//printf( " and found DD= %i, HH= %i, MM= %i, SS= %i (sum is = %i)  \n", DDDD,HH,MM,SS,   DDDD*86400 + HH*3600 + MM*60 + SS  );
	
	
	sprintf( string, "%-4.4i:%-2.2i:%-2.2i:%-2.2i", DDDD,HH,MM,SS );

	return 0;
}









// this callback is associated with the timer on the parameters panel...
// we will use the clock ticks to monitor the mouse position... if the 
// mouse stops moving and is over the available region list box, we check which
// item it is over and show that region;s parameters in a little text box beside the
// item...
int CVICALLBACK param_show_region_parameters (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
		
			

			// get the mouse state
			/////GetGlobalMouseState (&panel, &x, &y, &left, &right, key);
			// is it the same as before?
			
			// is it in the list box??
			
			// which item is it over?
			
			// display that items parameters...
			break;
	}
	return 0;
}






