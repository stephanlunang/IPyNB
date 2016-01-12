// scan.c	
//
// written by Alex Kay for Fadley Group 				Fall/Winter 1996-97

#define PAUSE_SCAN		0
#define PAUSE_STEP 		1
#define PAUSE_REGION 	2
#define PAUSE_ABORT 		3

#define CONTINUE										1
#define CONTINUE_KILL_CURRENT_REGION			2
#define CONTINUE_RESTART_CURRENT_REGION		3
#define CONTINUE_RESTART_CURRENT_EXPERIMENT	4
#define CONTINUE_KILL_EXPERIMENT					5



#include <utility.h>
#include <userint.h>
#include <ansi_c.h> 

#include "define.h"		// needed for "#define"s
#include "globals.h"	// needed for definition of global variables

#include "escah.h"
#include "scanh.h"		 // needed for function prototyped
#include "powerh.h"		 // needed for function prototypes
#include "detectoh.h"	 // needed for function prototypes and "#define"s
#include "scan.h"		 // needed for control callback function prototypes


int ShowDataMonitor = TRUE;
int ShowDetectorMonitor = FALSE;


//panel handles
static int pScan, pDataMonitor, pPaused, pScanFinished;     

int Paused, Continue = CONTINUE;
int panel, control;

int regionIndex;



struct pauseStruct{

	int flag;
	int regionIndex;
	int scanIndex;
	int stepIndex;
	
};


struct data_type cDataCurve; /*holds currently accumulating data curve*/
struct PlotStruct Plot;      




// accepts an experimentStruct ...
///int scan_start_experiment( struct experimentStruct E )
int scan_start_experiment( struct experimentStruct *Expt )  
{																																		
	
	// at the begining of an experiment...
	Paused = FALSE;
	Continue = CONTINUE; 
	
	//NOTE THE USE OF THE -> (stucture pointer operator) to access the members of the sturcture !
	//   we have passed a pointer to the experiment structure so that memory is not wasted
	// reproducing the experiment parameters and data...
	Expt->DateBegan = DateStr ();
	Expt->TimeBegan = TimeStr ();
	
	//disable the auto shutdown...
	shutdown_enable(FALSE);
	// dim the menu items on the main panel... don't want user to start something strange happening...
	esca_dim_menu(TRUE);
	set_front_end_message( "EXPERIMENT IN PROGRESS...", VAL_BLUE );
	// load the scan panel
	if( (pScan = LoadPanel (0, "scan.uir", pCONTROL) ) < 0 )
		return -1;
	SetPanelAttribute (pScan, ATTR_CLOSE_ITEM_VISIBLE, FALSE);

	if( (pDataMonitor  = LoadPanel (0, "scan.uir", pDISPLAY) ) < 0 )
		return -1;
	//SetPanelAttribute (pDataMonitor, ATTR_CLOSE_ITEM_VISIBLE, FALSE);

	// fix up the panel??
	SetCtrlAttribute (pScan, pCONTROL_PAUSE, 	ATTR_VISIBLE, TRUE);
	SetCtrlAttribute (pScan, pCONTROL_CLOSE, 	ATTR_VISIBLE, FALSE);
		
	//scan_save_experiment_info_in_file( E.Pathname, E.Regions, E.n_o_regions,E.Description );
	scan_save_experiment_info_in_file( Expt );
	//update Message on scan panel
		
	sprintf( Message, "Current Experiment was begun at %s on %s.", Expt->TimeBegan ,  Expt->DateBegan );
	SetCtrlAttribute (pScan, pCONTROL_TIME_BEGIN_MSG , ATTR_CTRL_VAL, Message);
	
	
	sprintf( Message, " Region is 0.0%% complete." );
	SetCtrlAttribute (pScan, pCONTROL_TIME_REGION, ATTR_LABEL_TEXT, Message);     
	SetCtrlVal (pScan, pCONTROL_TIME_REGION,  0.0);   
	
	sprintf( Message, " Exp't is 0.0%% complete." );
	SetCtrlAttribute (pScan, pCONTROL_TIME_EXP, ATTR_LABEL_TEXT, Message);     
	SetCtrlVal (pScan, pCONTROL_TIME_EXP,  0.0);   
	
	//initialize data curve...the scanning functions assume that the array needs to be reallocated (not allocated)
	cDataCurve.size = 1;
	cDataCurve.data = calloc(cDataCurve.size, sizeof(double) );
	// this initialized the plot attrimbutes and assigns a plot handle ... make the delete/draw graph work more nicely...
	display_initialize_plot(  );          
	
	
	//find out if the detector monitor is already displayed... if it is fill the "show det monitor button"
	if( DetMonitorPanelOpen ) {
		ShowDetectorMonitor = TRUE;
	}
	else {
		ShowDetectorMonitor = FALSE;
	}
	ShowDataMonitor = TRUE;

	
	// display panel
	DisplayPanel( pScan ) ;
	DisplayPanel( pDataMonitor);

	 
	///// LOOP THROUGH EACH OF THE REGIONS:        
	//was: for ( regionIndex = 1 ; regionIndex <= ( Expt->n_o_regions) ; regionIndex++ )
	for ( regionIndex = 0 ; regionIndex < ( Expt->n_o_regions) ; regionIndex++ )
	{
		
		// During the region scans, a pause request may have been generated
		//   ...if the user choose to kill the region, repeat the region, kill the exp,
		//        or restart the experiment, we can deal with it here.
		// The variable Continue should now indicate how to procede...
		if ( Continue == CONTINUE ){ 
			/* do nothing*/ ;
		}
		else if ( Continue == CONTINUE_RESTART_CURRENT_REGION ) {	
			--regionIndex;
			Continue = CONTINUE;
		}
		else if ( Continue == CONTINUE_RESTART_CURRENT_EXPERIMENT ) {
			regionIndex = 0;
			//we may have already saved data, so delete the file and start over...
			DeleteFile (Expt->Pathname);	 
			//scan_save_experiment_info_in_file( E.Pathname, E.Regions, E.n_o_regions,E.Description );
			scan_save_experiment_info_in_file( Expt );
			Continue = CONTINUE; 
			//need to erase/adjust the data file(s) for data that has been saved ...no just allow to write over it.
		}
		else if ( Continue ==  CONTINUE_KILL_EXPERIMENT ){
			
			//Continue = CONTINUE;
			regionIndex = Expt->n_o_regions ; //past last region...
			// erase the file we were using?
			if( regionIndex > 1 )
			{
				if( ConfirmPopup ("Experiment Has Been Terminated...", "Keep the data so far accumulated?") );
				else DeleteFile (Expt->Pathname);	     
			}
			else DeleteFile (Expt->Pathname);	  
			
			// return from this function... we are done
			continue;
		}	
		
		
		//update Message on scan panel
		sprintf( Message, "Scaning region %i of %i", regionIndex+1,  Expt->n_o_regions );
		SetCtrlAttribute (pScan, pCONTROL_REGION_MSG , ATTR_CTRL_VAL, Message);
		
		SetCtrlVal( pScan, pCONTROL_EXCITATION, EscaWest.ExcitationEnergy );
		SetPanelAttribute (pScan, ATTR_TITLE,  Expt-> Regions[regionIndex].Description);
		
		// if we are going to perform a fixed mode scan:			
		if ( Expt->Regions[regionIndex].ScanMode == SCAN_MODE_FIXED )
		{
		 	//make sure graph cursors are transparent (not visible)
		 	SetCursorAttribute (pDataMonitor, pDISPLAY_GRAPH, 1, ATTR_CURSOR_COLOR, VAL_TRANSPARENT);
		 	SetCursorAttribute (pDataMonitor, pDISPLAY_GRAPH, 2, ATTR_CURSOR_COLOR, VAL_TRANSPARENT);  
		 	SetCursorAttribute (pDataMonitor, pDISPLAY_GRAPH, 3, ATTR_CURSOR_COLOR, VAL_TRANSPARENT);  
		 	
		 	scan_fixed( Expt , regionIndex,  &cDataCurve); 
		}
		// if we are going to perform a dithered mode scan:
		else /*if ( Expt->Regions[regionIndex].ScanMode == SCAN_MODE_DITHERED  )*/
		{
			//make sure graph cursors are proper colors:
			SetCursorAttribute (pDataMonitor, pDISPLAY_GRAPH, 1, ATTR_CURSOR_COLOR, VAL_RED);
			SetCursorAttribute (pDataMonitor, pDISPLAY_GRAPH, 2, ATTR_CURSOR_COLOR, VAL_DK_MAGENTA);
			SetCursorAttribute (pDataMonitor, pDISPLAY_GRAPH, 3, ATTR_CURSOR_COLOR, VAL_RED);
			
			scan_dithered( Expt , regionIndex,  &cDataCurve);  
		}
		
		//save the data curve for the just finished region...if we are continuing...
		if( Continue == CONTINUE )
			scan_save_curve_in_file( Expt->Pathname,  &cDataCurve );
	
		if( Expt->PauseBetweenRegions && (regionIndex!= Expt->n_o_regions-1)  )
			if( !( ConfirmPopup ("User Requested Pause Between Regions...",
              "Yes:Continue Pausing Between Regions. No= Cancel Future Pauses.") ))
             		Expt->PauseBetweenRegions == FALSE;
             
              
	}  //END REGION LOOP
	
	// zero power supply ...
	power_set_voltage( 0.0 ); 
	
	// re-enable the auto shutdown... 
	shutdown_enable(TRUE);
	esca_dim_menu(FALSE);    
	
	// experiment is now complete... record time/date and notify user...
	Expt-> DateFinished = DateStr ();
	Expt-> TimeFinished = TimeStr ();
	
	if( Continue == CONTINUE )
	{
		sprintf( Message , "The experiment named \"%s\" was finished at %s on %s.\nThe results were saved in file %s.\n", \
							Expt-> Description, Expt-> TimeFinished, Expt-> DateFinished,  Expt-> Pathname);
							
		pScanFinished = LoadPanel (0, "scan.uir", pFINISHED);
		SetCtrlVal (pScanFinished, pFINISHED_MESSAGE, Message);
		InstallPopup( pScanFinished );
	}
		
	// we are done.. allow user to discard panel or look at data??
	SetCtrlAttribute (pScan, pCONTROL_PAUSE, ATTR_VISIBLE, FALSE);
	SetCtrlAttribute (pScan, pCONTROL_CLOSE, ATTR_VISIBLE, TRUE);  
	
	set_front_end_message( "Ready...", VAL_BLUE );           
	free( cDataCurve.data );    
	return 0;  
	
	
}


// this just closes the "you're exp. is finished" popup...
int CVICALLBACK finished_ok (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			RemovePopup(0);
			DiscardPanel( pScanFinished );
			break;
	}
	return 0;
}




int scan_close_up_shop( void )
{

	
	//printf("scan_close_up_shop() does nothing \n");

	return 0;
}










// performs a fixed mode scan on the (regionIndex)th region of the passed experiment structure
//  fixed scans are across ONE detector width. Mostly this is useful in calibrating
//  the instrument ( i.e setting the ReferenceEnergy and the DetectorWidth )
int scan_fixed( struct experimentStruct *Expt , int regionIndex, struct data_type *cDataPtr )
{
   
	int panel, control;  	
	double DetectorChannelCounts[N_O_DETECTOR_CHANNELS]={0};	   // array where detector data is held
	double			DetectorChannelCountsAdjusted[N_O_DETECTOR_CHANNELS]={0};		
	static int scan = 0;   
	unsigned int NumberOfPowerSupplySteps ;
	double BEinit, BEfinal;
	int i,j;
	//char Message[150];
	unsigned char STIOstatus, IRQstatus, PSTSstatus;
	
	//it looks real messy to keep righting lines like:
	//		  Expt->Regions[regionIndex].TotalCounts[ step + i - Expt->Regions[regionIndex].NumberOfDetectorBins  ]
	// so lets initialize a few variables that are easier to type:
	unsigned int NumDetBins  =  Expt->Regions[regionIndex].NumberOfDetectorBins;
	unsigned int NumChannels =  Expt->Regions[regionIndex].NumberOfChannels;
	unsigned int StepMult	 =  Expt->Regions[regionIndex].StepMultiple;
	double 		 DwellTime 	 =  Expt->Regions[regionIndex].DwellTime ;
	double 	    BEcenter	 =  Expt->Regions[regionIndex].BEcenter;
	double		 BEwidth     =  Expt->Regions[regionIndex].BEwidth ;
	
	
	///  parameters assumed to be properly set:
	///			Regions[i].NumberOfDetectorBins
	///			Regions[i].NumberOfChannels
	///         etc.
	
	
	/* initialize the cDataCurve*/
	cDataPtr->center_energy =  		Expt->Regions[regionIndex].BEcenter;
	cDataPtr->width = 					-fabs(Expt->Regions[regionIndex].BEwidth );
	cDataPtr->excitation_energy =  	EscaWest.ExcitationEnergy;
	cDataPtr->size = 					Expt->Regions[regionIndex].NumberOfChannels;   
	cDataPtr->time_p_channel =  		Expt->Regions[regionIndex].DwellTime * Expt->Regions[regionIndex].StepMultiple * Expt->Regions[regionIndex].NumberOfScans;
	cDataPtr->data = calloc( cDataPtr->size, sizeof(double) );
	
	// allocate an array for this much data
	Expt->Regions[regionIndex].TotalCounts = calloc( NumChannels , sizeof( double ) );   
		
	// allocate an array of binding energies across the region...
	//    ... note there is no real need to do this... we could just calculate each value as
	//    ... needed...   no real obvious advantage to either approach... EXCEPT! we need an array to graph on x-axis!
	Expt->Regions[regionIndex].BindingEnergies = calloc( NumChannels  , sizeof( double) );  
	
	// Set the initial BindingEnergy...
	Expt->Regions[regionIndex].BindingEnergies[0] =  BEcenter - 0.5*(double) EscaWest.DetectorWidth ;  
	// plus a small  StepMult dependent peice ???!!! No, taken care of below
	Expt->Regions[regionIndex].BindingEnergies[0] +=  0.5*((double)EscaWest.DetectorWidth)	/ ((double)N_O_DETECTOR_CHANNELS) * ((double)StepMult) ;
	
	
	// ... and fill the values of the remaining BindingEnergies...
	for ( i = 0 ; i < NumChannels ; i++ )
	{
		// just add multiples of the detector channel width 
		Expt->Regions[regionIndex].BindingEnergies[i] = 	Expt->Regions[regionIndex].BindingEnergies[0] 					\
						+ ((double)i) * ((double)StepMult)* ((double)EscaWest.DetectorWidth)	/ ((double)N_O_DETECTOR_CHANNELS)   ; 
		
	}
		
	
	
	// might as well set the power supply here to allow for a bit of settling while the rest of the crap 
	//   is set up...
	power_set_voltage(    power_BE_to_PS( BEcenter /*dith:CurrentEnergy*/ )   );     
	// update the displayed current energy
	SetCtrlVal (pScan, pCONTROL_CURRENT_BE, BEcenter /*dith:CurrentEnergy*/);
	// wait for supply to settle??
	Delay ( HP_SUPPLY_SETTLE_TIME/ 1000.0 );


	
	
	
	
	//initialize the display screen values and graph
	sprintf( Message, "Region: %s" , Expt->Regions[regionIndex].Description );
	SetCtrlAttribute (pScan, pCONTROL_REGION_DESCRIPTION, ATTR_CTRL_VAL, Message);
	
	SetCtrlVal (pScan, pCONTROL_BE_INIT		, 	Expt->Regions[regionIndex].BindingEnergies[0] 					);
	SetCtrlVal (pScan, pCONTROL_BE_FINAL	, 	Expt->Regions[regionIndex].BindingEnergies[NumChannels-1 ]	);     
	SetCtrlVal (pScan, pCONTROL_BE_STEP		, 	(double) EscaWest.DetectorWidth / ((double)NumDetBins)   				);     
	SetCtrlVal (pScan, pCONTROL_DWELL_TIME	, 	DwellTime									);
	SetCtrlVal (pScan, pCONTROL_TIME_P_CHANNEL	, 	cDataPtr->time_p_channel );
	SetCtrlVal (pScan, pCONTROL_N_O_CHANNELS, Expt->Regions[regionIndex].NumberOfChannels						);      
	sprintf( Message, "Scan Mode is FIXED");
	SetCtrlAttribute (pScan, pCONTROL_REGION_SCAN_MODE_MSG, ATTR_CTRL_VAL, Message);
	
	// update % of region completed ...
	Expt->Regions[regionIndex].PercentDone = 100.0* (double)(  ((double)scan) / ((double)Expt->Regions[regionIndex].NumberOfScans)  );
	sprintf( Message, " Region is %-4.1f%% complete." , Expt->Regions[regionIndex].PercentDone );      
	SetCtrlAttribute (pScan, pCONTROL_TIME_REGION, ATTR_LABEL_TEXT, Message);     
	SetCtrlVal (pScan, pCONTROL_TIME_REGION,  Expt->Regions[regionIndex].PercentDone);
		
	// update the % of the total experiment completed ...
	get_percent_expt_done( Expt );  
	sprintf( Message, " Exp't is %-4.1f%% complete." , Expt->PercentDone );      
	SetCtrlAttribute (pScan, pCONTROL_TIME_EXP, ATTR_LABEL_TEXT, Message);     
	SetCtrlVal (pScan, pCONTROL_TIME_EXP,  Expt->PercentDone);
	
	
	// set x-axis range of the graph...we are plotting in channels, with variable 
	//   gain and offset to control BE vs. KE displays...
	SetAxisScalingMode (	pDataMonitor, pDISPLAY_GRAPH, VAL_XAXIS, VAL_MANUAL, 0 ,  cDataPtr->size -1 );
	
	
// WE WANT TO START THE FIRST ACCUMULATION HERE:
	
	//set display
	sprintf( Message, "Performing scan 1 of %i",  Expt->Regions[regionIndex].NumberOfScans );
	SetCtrlAttribute (pScan, pCONTROL_SCAN_MSG, ATTR_CTRL_VAL, Message);
		
	// this reset may not be needed, but doesn't hurt anything    
	det_reset_control();	   
	// clear the detector buffers
	det_clear_buffers();
	// set the detector timer 
	det_send_timer_command( DwellTime);		// time is in seconds
	det_strobe_PCTL(); 					 	// strobe command onto board 
	
		
	// set the detector timer 
	det_send_timer_command( DwellTime );    // time is in seconds
	det_strobe_PCTL(); 							// strobe command onto board 
	// start first accumulation
	det_send_start_command() ;  
	det_strobe_PCTL();

	
//REPEAT THE SCAN OVER THE REGION AS MANY TIMES AS REQUESTED:
	// Note: if NumberOfScans = 1, then this is all skipped and data is
	//       read in following this loop... otherwise this loop is
	//       entered and the last scan's data is read in following the loop.
	for ( scan = 1  ; scan < Expt->Regions[regionIndex].NumberOfScans ; scan++ )
	{
		
		// wait for accum to finish... check for STIO to go low 
		if(ScanTesting) Delay( 0.90 *  DwellTime   );
		
		det_check_status_lines( &STIOstatus, &IRQstatus, &PSTSstatus );    
		while ( IRQstatus == FALSE )    //until accum is done
		{     
			Delay( 0.001 );
			det_check_status_lines( &STIOstatus, &IRQstatus, &PSTSstatus );     
				
			// check for a pause request
			GetUserEvent (0, &panel, &control);
			// Pause has been set to TRUE if user pressed the Pause button...
			if ( Paused == TRUE ) 
				scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
			// the variable Continue should now indicate how to procede...
			if ( Continue != CONTINUE ) return Continue;
		}
		// Pause has been set to TRUE if user pressed the Pause button...
		if ( Paused == TRUE ) 
			scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
		// the variable Continue should now indicate how to procede...
		if ( Continue != CONTINUE ) return Continue;
		
		
		sprintf( Message, "Performing scan %i of %i", scan+1,  Expt->Regions[regionIndex].NumberOfScans );
		SetCtrlAttribute (pScan, pCONTROL_SCAN_MSG, ATTR_CTRL_VAL, Message);
		
		
		// set the detector timer 
		det_send_timer_command( DwellTime );    // time is in milliseconds
		det_strobe_PCTL(); 							// strobe command onto board 
		// start first accumulation
		det_send_start_command() ;  
		det_strobe_PCTL();
		
		// this start has swapped the buffers... the previous accum data is ready to be read:
		// read in the detector channel buffer counts... data is put in the DetectorChannelCounts array...
			//                 ...(this is the PREVIOUS accumulation data!)
		det_read_buffer( DetectorChannelCounts );
		// correct for sensitivity...
		det_sensitivity_correction( DetectorChannelCounts );
		   
		//det_sensitivity_correction( DetectorChannelCounts );	to do this, need to change counts to doubles everywhere!
		
		
		// add the detector chanel counts to the true data... recall that the data bin (or channel) width
		//    may be an integer multiple of the detector channel width, so arrays add in a specific manner
		//    depending upon the StepMultiple
		for ( i = 0 ; i <  NumDetBins ; i++)
		{  
			// add the appropriate channels of the larger array...
			for( j = 0 ; j < StepMult ; j++ )
			{
				cDataPtr->data[i] = cDataPtr->data[i] + ( double )DetectorChannelCounts[ (StepMult) *i + j ];
			}
			
		}
		
		// update % of region completed ...
		Expt->Regions[regionIndex].PercentDone = 100.0* (double)(  ((double)scan) / ((double)Expt->Regions[regionIndex].NumberOfScans)  ); 
		sprintf( Message, " Region is %-4.1f%% complete." , Expt->Regions[regionIndex].PercentDone );      
		SetCtrlAttribute (pScan, pCONTROL_TIME_REGION, ATTR_LABEL_TEXT, Message);     
		SetCtrlVal (pScan, pCONTROL_TIME_REGION,  Expt->Regions[regionIndex].PercentDone);
			
		// and update the % of the experiment completed...
		get_percent_expt_done( Expt );  
		sprintf( Message, " Exp't is %-4.1f%% complete." , Expt->PercentDone );      
		SetCtrlAttribute (pScan, pCONTROL_TIME_EXP, ATTR_LABEL_TEXT, Message);     
		SetCtrlVal (pScan, pCONTROL_TIME_EXP,  Expt->PercentDone);
		
		
		//plot the current data:
		// but first delete the previous plot... there is always a previous plot, even if
		// this is the first plot of data... remember I plotted a sigle point earlier, just to
		// get a plot handle...
		// delayed delete of data so that graph stay clean... delete takes place when next plot is drawn
		DeleteGraphPlot (	pDataMonitor, pDISPLAY_GRAPH, Plot.Handle, VAL_DELAYED_DRAW);
		//... plot all data up to current and update cursors
		Plot.Handle = PlotY (	pDataMonitor,
									pDISPLAY_GRAPH,
									cDataPtr->data, 
									cDataPtr->size,
									VAL_DOUBLE,
									Plot.Style,
									Plot.TracePointStyle,
									Plot.LineStyle,
									1,
									Plot.Color);	
				
	} // end of scan loop
		
	// NOW WE WAIT FOR THER **LAST** ACCUMULATION TO FINISH SO THAT
	// WE CAN SWAP THE BUFFERS, READ THE LAST DATA IN, AND ADD TO THE TOTAL COUNTS
	
	// wait for accum to finish... check for IRQ to go high... service request 
	det_check_status_lines( &STIOstatus, &IRQstatus, &PSTSstatus );    
	while( IRQstatus == FALSE )
	{     
		Delay( 0.001);
		det_check_status_lines( &STIOstatus, &IRQstatus, &PSTSstatus );
			
		// check for a pause request
		GetUserEvent (0, &panel, &control);
		// Pause has been set to TRUE if user pressed the Pause button...
		if ( Paused == TRUE ) 
			scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
		// the variable Continue should now indicat how to procede...
		if ( Continue != CONTINUE ) return Continue;
	}
	// Pause has been set to TRUE if user pressed the Pause button...
	if ( Paused == TRUE ) 
		scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
	// the variable Continue should now indicat how to procede...
	if ( Continue != CONTINUE ) return Continue;
		
		
		
		
	//   we need to start an accum to swap that buffer into the readable buffer
	det_send_timer_command( 0 );     // time is in milliseconds
	det_strobe_PCTL(); 				  	// strobe command onto board 
		
	det_send_start_command() ;  
	det_strobe_PCTL();
		
	det_send_stop_command() ; 
	det_strobe_PCTL();
		
	// read in the data from the buffer 
	det_read_buffer( DetectorChannelCounts );
		
		
	// add the detector channel counts to the true data... recall that the data bin (or channel) width
	//    may be an integer multiple of the detector channel width, so arrays add in a specific manner
	//    depending upon the StepMultiple
	for ( i = 0 ; i <  NumDetBins ; i++)
	{  
		// add the appropriate channels of the larger array...
		for( j = 0 ; j < StepMult ; j++ )
		{
			cDataPtr->data[i] = cDataPtr->data[i] + (double )DetectorChannelCounts[ (StepMult) *i + j ];
		}
		
	}	
	
	
	//if(  scan != Expt->Regions[regionIndex].NumberOfScans -1 ) 
	DeleteGraphPlot (pDataMonitor,pDISPLAY_GRAPH, Plot.Handle, VAL_DELAYED_DRAW);
	
	//... plot all data up to current and update cursors
	Plot.Handle = PlotY (	pDataMonitor,
									pDISPLAY_GRAPH,
									cDataPtr->data, 
									cDataPtr->size,
									VAL_DOUBLE,
									Plot.Style,
									Plot.TracePointStyle,
									Plot.LineStyle,
									1,
									Plot.Color);	
	
	// update % of region completed ...
	Expt->Regions[regionIndex].PercentDone = 100.0* (double)(  ((double)scan) / ((double)Expt->Regions[regionIndex].NumberOfScans)  );  
	sprintf( Message, " Region is %-4.1f%% complete." , Expt->Regions[regionIndex].PercentDone );      
	SetCtrlAttribute (pScan, pCONTROL_TIME_REGION, ATTR_LABEL_TEXT, Message);     
	SetCtrlVal (pScan, pCONTROL_TIME_REGION,  Expt->Regions[regionIndex].PercentDone);
	// update the % experiement done ...
	get_percent_expt_done( Expt );
	sprintf( Message, " Exp't is %-4.1f%% complete." , Expt->PercentDone );    
	SetCtrlAttribute (pScan, pCONTROL_TIME_EXP, ATTR_LABEL_TEXT, Message);     
	SetCtrlVal (pScan, pCONTROL_TIME_EXP,  Expt->PercentDone);
	
	
	return 0;
}




















int scan_dithered( struct experimentStruct *Expt , int regionIndex, struct data_type *cDataPtr )
{
   //static int NeedsInitialization = TRUE; // flag that we just began region            
   
   //static int RegionPaused = FALSE;	 
	static int scan = 0;
	static int step = 0;		  // loop increments
	
	int panel, control;  	

	double DetectorChannelCounts[N_O_DETECTOR_CHANNELS]={0};	   // array where detector data is held

	unsigned int NumberOfPowerSupplySteps ;
	double InitialEnergy, CurrentEnergy;
	double *DetectorBinCounts;				// bins are the combined channels... can't have more bins than channels... N*bins (approx=) channels , N=1,2,3,4....

	
	double BEinit, BEfinal;
	double Offset;							// used to correct the initial binding energy... needed if detector bins < detector channels
	double ChannelOffset;
	
	int i,j, NumberOfGraphPoints;
	
	unsigned char STIOstatus, IRQstatus, PSTSstatus;
	
	//it looks real messy to keep righting lines like:
	//		  Expt->Regions[regionIndex].TotalCounts[ step + i - Expt->Regions[regionIndex].NumberOfDetectorBins  ]
	// so lets initialize a few variables that are easier to type:
	unsigned int NumDetBins  =  Expt->Regions[regionIndex].NumberOfDetectorBins;
	unsigned int NumChannels =  Expt->Regions[regionIndex].NumberOfChannels;
	unsigned int StepMult	 =  Expt->Regions[regionIndex].StepMultiple;
	double		 DwellTime 	 =  Expt->Regions[regionIndex].DwellTime ;
	double 	    BEcenter	 =  Expt->Regions[regionIndex].BEcenter;
	double		 BEwidth     =  Expt->Regions[regionIndex].BEwidth ;
	
	int OldPlot;
	
	
	
	
	DeleteGraphPlot ( pDataMonitor, pDISPLAY_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	// begin the initalization routine at beginning of the region...
	
	/*========== initialize the cDataCurve========================*/
	// NOTE: we will accumulate in BEmin ... BEmax direction...
	//   therefore by my convention, need positive width
	cDataPtr->center_energy =  		Expt->Regions[regionIndex].BEcenter;
	cDataPtr->width = 					fabs(Expt->Regions[regionIndex].BEwidth ); 
	cDataPtr->excitation_energy =  	EscaWest.ExcitationEnergy;
	cDataPtr->size = 					Expt->Regions[regionIndex].NumberOfChannels;   
	cDataPtr->time_p_channel =  		Expt->Regions[regionIndex].DwellTime * 				
													Expt->Regions[regionIndex].NumberOfScans *	
														(double)N_O_DETECTOR_CHANNELS /
															Expt->Regions[regionIndex].StepMultiple ;
	free( cDataPtr->data );  //assume data has been allocated previously....
	cDataPtr->data = calloc( cDataPtr->size, sizeof(double) );


	graph_control( PLOT_BINDING, cDataCurve );     
	
	//the user specified a center and a width... we will do as good as possible...
	BEinit  = BEcenter - BEwidth/2.0;
	BEfinal = BEinit+ ((double)NumChannels -1) * ((double)StepMult)* ((double)EscaWest.DetectorWidth)	/ ((double)N_O_DETECTOR_CHANNELS)   ; BEcenter + BEwidth/2.0;      
	// ... but we cannot accomodate the exact width... shift init and final so that center is still ok
	BEinit  += (BEfinal + BEinit)/2.0 - BEcenter ;
	BEfinal += (BEfinal + BEinit)/2.0 - BEcenter ; 
	
	cDataPtr->width = fabs( BEfinal - BEinit ); /*positove because data array is to be {BEmin..BEmax}*/
	

	///  parameters assumed to be properly set:
	///			Regions[i].NumberOfDetectorBins
	///			Regions[i].NumberOfChannels
	///         etc.
		
	
	//allocate an array to hold the detector bin counts :
	DetectorBinCounts = calloc( NumDetBins ,  sizeof( double ) );   
		
	// calculate the initial binding energy to actually sit at ... 
	//    Need to start a half detector width before the region 
	//          and end a half detector width behind it.
	NumberOfPowerSupplySteps =  NumChannels + NumDetBins -2; 
	
	///initial energy is a half detector width behind the first binding energy in the region... 
	/// ..  this leaves the LAST detector channel centered over the INITIAL BE...
	///     ... HOWEVER, we want the last detector *BIN* to be centerd over the initial BE
	///     ... move over by half a channel width for each detector CAHNNEL > 1 in the BIN
	///           (remember a BIN is some interger multiple 1 to N_O_DETECTOR_CHANNELS of the 
	///					detector channel... max resolution is with the bin width = detector channel width
	
	Offset = 0.5*((double)EscaWest.DetectorWidth) - 0.5*( (double)(StepMult - 1))*((double)EscaWest.DetectorWidth)/((double)N_O_DETECTOR_CHANNELS);
	ChannelOffset =  0.5 * N_O_DETECTOR_CHANNELS / (double)(StepMult );
	
	InitialEnergy = BEinit - Offset ;
	

	// this reset may not be needed, but doesn't hurt anything    
	det_reset_control();	   
	// clear the detector buffers
	det_clear_buffers();
	// set the detector timer 
	det_send_timer_command( DwellTime);		// time is in milliseconds
	det_strobe_PCTL(); 					 	// strobe command onto board 
	
	
	
	//initialize the display screen values and graph
	sprintf( Message, "Region: %s" , Expt->Regions[regionIndex].Description );
	SetCtrlAttribute (pScan, pCONTROL_REGION_DESCRIPTION, ATTR_CTRL_VAL, Message);
	
	SetCtrlVal (pScan, pCONTROL_BE_INIT		, 	BEinit 	);
	SetCtrlVal (pScan, pCONTROL_BE_FINAL	,  BEfinal  ); 
	SetCtrlVal (pScan, pCONTROL_BE_STEP		, 	(double) EscaWest.DetectorWidth / ((double)NumDetBins)   				);     
	SetCtrlVal (pScan, pCONTROL_DWELL_TIME	, 	DwellTime				);
	SetCtrlVal (pScan, pCONTROL_TIME_P_CHANNEL	, 	cDataPtr->time_p_channel );    
	SetCtrlVal (pScan, pCONTROL_N_O_CHANNELS, Expt->Regions[regionIndex].NumberOfChannels						);      
	sprintf( Message, "Scan Mode is DITHERED (SWEPT)");
	SetCtrlAttribute (pScan, pCONTROL_REGION_SCAN_MODE_MSG, ATTR_CTRL_VAL, Message);
	
	// update % of region completed ...
	Expt->Regions[regionIndex].PercentDone = 100.0* ( (double) (NumberOfPowerSupplySteps*scan + step) ) / ((double)(NumberOfPowerSupplySteps*Expt->Regions[regionIndex].NumberOfScans));
	sprintf( Message, " Region is %-4.1f%% complete." , Expt->Regions[regionIndex].PercentDone );      
	SetCtrlAttribute (pScan, pCONTROL_TIME_REGION, ATTR_LABEL_TEXT, Message);     
	SetCtrlVal (pScan, pCONTROL_TIME_REGION,  Expt->Regions[regionIndex].PercentDone);
		
	// update the % of the total experiment completed ...
	get_percent_expt_done( Expt );  
	sprintf( Message, " Exp't is %-4.1f%% complete." , Expt->PercentDone );      
	SetCtrlAttribute (pScan, pCONTROL_TIME_EXP, ATTR_LABEL_TEXT, Message);     
	SetCtrlVal (pScan, pCONTROL_TIME_EXP,  Expt->PercentDone);
	
	
	// set x-axis range of the graph...we are plotting in channels, with variable 
	//   gain and offset to control BE vs. KE displays...
	SetAxisScalingMode (	pDataMonitor, pDISPLAY_GRAPH, VAL_XAXIS, VAL_MANUAL, 0 ,  cDataPtr->size -1 );
	

	
	//REPEAT THE SCAN OVER THE REGION AS MANY TIMES AS REQUESTED:
	for ( scan = 0  ; scan < Expt->Regions[regionIndex].NumberOfScans ; scan++ )
	{
		// check for a pause request
		GetUserEvent (0, &panel, &control);
		// Pause has been set to TRUE if user pressed the Pause button...
		if ( Paused == TRUE ) 
			scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
		// the variable Continue should now indicat how to procede...
		if ( Continue != CONTINUE ) return Continue;
		
		
		//insert current data plot as dim grey points...(to view last full scan (all zeroes first time)
		Plot.Handle = PlotY (	pDataMonitor,pDISPLAY_GRAPH,
										cDataPtr->data, cDataPtr->size,
										VAL_DOUBLE, Plot.Style,  Plot.TracePointStyle,
										Plot.LineStyle, 1, VAL_LT_GRAY);
		
		sprintf( Message, "Performing scan %i of %i", scan+1,  Expt->Regions[regionIndex].NumberOfScans );
		SetCtrlAttribute (pScan, pCONTROL_SCAN_MSG, ATTR_CTRL_VAL, Message);
		
		CurrentEnergy = InitialEnergy;
		// set initial energy on power supply
		power_set_voltage(    power_BE_to_PS( CurrentEnergy )   );   
		// update the displayed current energy
		SetCtrlVal (pScan, pCONTROL_CURRENT_BE, CurrentEnergy);
		// update the cursor positions
		scan_adjust_cursors(	0 - ChannelOffset, //current position is det half det width behind init point
									ChannelOffset ,	  //half detector width..."wings"
									0 ,
									cDataPtr->size -1); 
									
		// wait for supply to settle??
		Delay ( HP_SUPPLY_SETTLE_TIME/ 1000.0 );
		
		
		// set the detector timer 
		det_send_timer_command( DwellTime );    // time is in milliseconds
		det_strobe_PCTL(); 							// strobe command onto board 
		// start first accumulation
		det_send_start_command() ;  
		det_strobe_PCTL();
		
		//DITHER (STEP) THROUGH THE REGION:
		// step = 0 was effectively done above...     
		for( step = 1 ; step <= NumberOfPowerSupplySteps  ; step++ )     
		{														 
			// check for a pause request
			GetUserEvent (0, &panel, &control);
			// Pause has been set to TRUE if user pressed the Pause button...
			if ( Paused == TRUE ) 
				scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
			// the variable Continue should now indicat how to procede...
			if ( Continue != CONTINUE ) return Continue;
			
		
			// wait for accum to finish... check for STIO to go low 
			det_check_status_lines( &STIOstatus, &IRQstatus, &PSTSstatus );    
			//while( STIOstatus == TRUE )   //while accumulating
			while ( IRQstatus == FALSE )    //until accum is done
			{     
				Delay( 0.001 );
				det_check_status_lines( &STIOstatus, &IRQstatus, &PSTSstatus );     
				
				// check for a pause request
				GetUserEvent (0, &panel, &control);
				// Pause has been set to TRUE if user pressed the Pause button...
				if ( Paused == TRUE ) 
					scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
				// the variable Continue should now indicat how to procede...
				if ( Continue != CONTINUE ) return Continue;
			}
			// Pause has been set to TRUE if user pressed the Pause button...
			if ( Paused == TRUE ) 
				scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
			// the variable Continue should now indicat how to procede...
			if ( Continue != CONTINUE ) return Continue;
			
			
			// set the new voltage now... we must start the next accumulation
			//     before we can get the just completed region's data from the buffer board ...
			CurrentEnergy =  InitialEnergy +  ((double)step) * ((double)EscaWest.DetectorWidth)	/ ((double)NumDetBins) ; 

			power_set_voltage(  power_BE_to_PS( CurrentEnergy) );  
			// wait for supply to settle??
			Delay ( 0.001 * HP_SUPPLY_SETTLE_TIME );
			
			// start next accumulaiton... automatically swaps previous data into read buffer
			det_send_start_command() ;  
			det_strobe_PCTL();
				
		
			// read in the detector channel buffer counts... data is put in the DetectorChannelCounts array...
			//                 ...(this is the PREVIOUS accumulation data!)
			det_read_buffer( DetectorChannelCounts );
	
			det_send_timer_command( DwellTime);    // time is in milliseconds	///??needed
			det_strobe_PCTL(); 							   	// strobe command onto board 
			
			// put the data from the detector CHANNELS into the data BINS
			for ( i = 0 ; i <  NumDetBins ; i++)
			{  
				 DetectorBinCounts[i] = 0.0;
				 // add the appropriate channels of the larger array...
				 for( j = 0 ; j < StepMult ; j++ )
				 {
				 	DetectorBinCounts[i] = DetectorBinCounts[i] + (long unsigned )DetectorChannelCounts[ (StepMult) *i + j ];
				 }
			}
			
			
				
			// add new data to old data ... remember this includes the NumDetBins-1  region channels/bins 
			//		of junk data on either end of the data set (the "dither wings")
			 	
			// i is counting through the detector bins we wish to add to the old data
			//  ... it is only used if bin cooresponds to a region channel/bin...                
			for ( i = 0 ; i < NumDetBins ; i++)
			{			
				// if it is not a data point from the "dither wing" at the front...
				//       for step = 1 of this loop (first step) we are looking at the
				//       data set for which only the LAST detector bin should contribute
				//       and it should contribute to the FIRST region channel/bin
				if( step + i > NumDetBins -1 )
				{        
					// if it is not a data point from the "dither wing" at the back 
					if ( (step + i) <= NumChannels + NumDetBins - 1 )   ////was  if ( (step + i) < NumChannels + NumDetBins - 1 )               
					{	
						// note: for the first point (step=0) , only the last detector channel ( i = NumDetBins-1 )
						//       contibutes and it goes into TotalCounts[0]
//						Expt->Regions[regionIndex].TotalCounts[ step+i-NumDetBins] += (double)  DetectorBinCounts[i];
						cDataPtr->data[ step+i-NumDetBins] += (double)  DetectorBinCounts[i];   
					}
				}
				
			}			
			
			//in the above, could this work?
			//for ( i = NumDetBins - step ; i < NumChannels + NumDetBins - step - 1 ; i++ ){
			//			  Expt->Regions[regionIndex].TotalCounts[ step+i-NumDetBins] += (unsigned int)  DetectorBinCounts[i];     
			//}
				

			//add the last COMPLETE point from the ditthering... only once step > NumDetBins...
			if( step > NumDetBins )
				PlotPoint (		pDataMonitor, pDISPLAY_GRAPH,
									step - NumDetBins,								//x-point (channel)
									cDataPtr->data[step - NumDetBins],			//y-point
									Plot.TracePointStyle, Plot.Color);
				
		
			// now that the data has been updated, show user the new energy region being scaned...
			// update the cursors to surround the position of the detector within the region being scanned
			scan_adjust_cursors( step - ChannelOffset,
										ChannelOffset,			 //send the current channel number
										0 ,			 								 // and the min/max channel
										cDataPtr->size -1  );  
										
			
			// update the displayed current energy
			SetCtrlVal (pScan, pCONTROL_CURRENT_BE,
					InitialEnergy+((double)(step*StepMult)*((double)EscaWest.DetectorWidth)/((double)N_O_DETECTOR_CHANNELS)) );
			
			
			// update % of region completed ...
			Expt->Regions[regionIndex].PercentDone = 100.0* ( (double) (NumberOfPowerSupplySteps*scan + step) ) / ((double)(NumberOfPowerSupplySteps*Expt->Regions[regionIndex].NumberOfScans));
			if ( Expt->Regions[regionIndex].PercentDone  < 0.0   ) Expt->Regions[regionIndex].PercentDone = 0.0;
			if ( Expt->Regions[regionIndex].PercentDone  > 100.0 ) Expt->Regions[regionIndex].PercentDone = 100.0;
			sprintf( Message, " Region is %-4.1f%% complete." , Expt->Regions[regionIndex].PercentDone );      
			SetCtrlAttribute (pScan, pCONTROL_TIME_REGION, ATTR_LABEL_TEXT, Message);     
			SetCtrlVal (pScan, pCONTROL_TIME_REGION,  Expt->Regions[regionIndex].PercentDone);
			
			// and update the % of the experiment completed...
			get_percent_expt_done( Expt );  
			sprintf( Message, " Exp't is %-4.1f%% complete." , Expt->PercentDone );      
			SetCtrlAttribute (pScan, pCONTROL_TIME_EXP, ATTR_LABEL_TEXT, Message);     
			SetCtrlVal (pScan, pCONTROL_TIME_EXP,  Expt->PercentDone);
			
		} // end of for( step, ,  )
		
		// once all the steps have been done, we still have the last buffer worth of data to read
		//  .... this will add the first detector BIN (composed of between 1 to N_O_DETECTOR_CHANNELS
		//       detector CHANNELS) to the last region binding energy counts...
		
		// wait for accum to finish... check for STIO to go low 
		//Delay( 0.90 * ((double) DwellTime ) / 1000.0 );
		det_check_status_lines( &STIOstatus, &IRQstatus, &PSTSstatus );    
		//while( STIOstatus == TRUE )
		while( IRQstatus == FALSE )
		{     
			Delay( 0.001);
			det_check_status_lines( &STIOstatus, &IRQstatus, &PSTSstatus );
			
			// check for a pause request
			GetUserEvent (0, &panel, &control);
			// Pause has been set to TRUE if user pressed the Pause button...
			if ( Paused == TRUE ) 
				scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
			// the variable Continue should now indicat how to procede...
			if ( Continue != CONTINUE ) return Continue;
		}
		// Pause has been set to TRUE if user pressed the Pause button...
		if ( Paused == TRUE ) 
			scan_pause_go( PAUSE_SCAN );	  //pause the scan and ask user how to procede   
		// the variable Continue should now indicat how to procede...
		if ( Continue != CONTINUE ) return Continue;
		
		
		
		//   we need to start an accum to swap that buffer into the readable buffer
		det_send_timer_command( 0 );     // time is in milliseconds
		det_strobe_PCTL(); 				  	// strobe command onto board 
		
		det_send_start_command() ;  
		det_strobe_PCTL();
		
		det_send_stop_command() ; 
		det_strobe_PCTL();
		
		// read in the data from the buffer 
		det_read_buffer( DetectorChannelCounts );
		
			
		// we could get all the DetectorBinCounts[i], but we only need DetectorBinCounts[0]:
		DetectorBinCounts[0] = 0;
		for( j = 0 ; j < StepMult ; j++ ){
			 	DetectorBinCounts[0] = DetectorBinCounts[0] + DetectorChannelCounts[ j ];
		}
		
		// add this data to the totals... note that only detector channel zero data is used and is added to the
		//    last channel of the region counts
//		Expt->Regions[regionIndex].TotalCounts[NumChannels -1]  +=  (unsigned int) DetectorBinCounts[0];  
		cDataPtr->data[NumChannels -1]  +=  DetectorBinCounts[0];   
		
		DeleteGraphPlot (pDataMonitor,  pDISPLAY_GRAPH, -1, VAL_DELAYED_DRAW);




		// update % of region completed ...
		Expt->Regions[regionIndex].PercentDone = 100.0* ( (double) (NumberOfPowerSupplySteps*scan + step) ) / ((double)(NumberOfPowerSupplySteps*Expt->Regions[regionIndex].NumberOfScans));
		if ( Expt->Regions[regionIndex].PercentDone  < 0.0   ) Expt->Regions[regionIndex].PercentDone = 0.0;
		if ( Expt->Regions[regionIndex].PercentDone  > 100.0 ) Expt->Regions[regionIndex].PercentDone = 100.0;
		sprintf( Message, " Region is %-4.1f%% complete." , Expt->Regions[regionIndex].PercentDone );      
		SetCtrlAttribute (pScan, pCONTROL_TIME_REGION, ATTR_LABEL_TEXT, Message);     
		SetCtrlVal (pScan, pCONTROL_TIME_REGION,  Expt->Regions[regionIndex].PercentDone);
		// update the % experiemtn done ...
		get_percent_expt_done( Expt );
		sprintf( Message, " Exp't is %-4.1f%% complete." , Expt->PercentDone );    
	  	SetCtrlAttribute (pScan, pCONTROL_TIME_EXP, ATTR_LABEL_TEXT, Message);     
		SetCtrlVal (pScan, pCONTROL_TIME_EXP,  Expt->PercentDone);
		
		
	
	}// end of for( scan ,  ,  )
	
	//finish with a plot of the final data...
	scan_adjust_cursors( NumberOfPowerSupplySteps , /* + 1 - ChannelOffset, omot to insure cursor is move to far left*/
										ChannelOffset,			 //send the current channel number
										0 ,			 								 // and the min/max channel
										cDataPtr->size -1  ); 
										
	Plot.Handle = PlotY (	pDataMonitor,
										pDISPLAY_GRAPH,
										cDataPtr->data, 
										cDataPtr->size,
										VAL_DOUBLE,
										Plot.Style,
										Plot.TracePointStyle,
										Plot.LineStyle,
										1,
										Plot.Color);
	
	
	
	// free the allocated memory regardless of status
	free( DetectorBinCounts );

	return 0;
}





void get_percent_expt_done( struct experimentStruct *ExPtr )
{
	int i;
	
	ExPtr->PercentDone = 0.0;
	
	// assume that the Regions[i].PercentDone are correct and simply
	//was: for( i = 1 ; i <= ( ExPtr->n_o_regions ) ; i++ )      
	for( i = 0 ; i < ( ExPtr->n_o_regions ) ; i++ )
	{
		//printf( "%i) region total time = %-6.2f, region percent done =%-6.2f \n",
		//						i, ExPtr->Regions[i].TotalTime,  ExPtr->Regions[i].PercentDone);
		
		ExPtr->PercentDone += ExPtr->Regions[i].TotalTime * ExPtr->Regions[i].PercentDone;
	}
	
	//don't need to multiply by 100.0 because the Regions[i].PercentDone already included it
	ExPtr->PercentDone =  ExPtr->PercentDone/ExPtr->TotalTime;  
	// no return needed, we altered the passed by reference value...

}








// cursors indicate the current position of the detector in the region being scaned
//  set center cursor to "current_center" and the outer cursors to "current_center" +- "HalfWidth"
//   checks that cursors are not set to > "last" or < "first"
int scan_adjust_cursors( double current_center , double HalfWidth  , double first, double last)
{
	double yVal;
	
	
	// First read a y axis value off the current graph. 
	//  This gives a valid y value for the functions below .
	GetAxisScalingMode (pDataMonitor, pDISPLAY_GRAPH, VAL_LEFT_YAXIS, 0, &yVal, 0);
	
	
	// adjust the cursor that indicates the low BE side of the detector    
	if (  current_center - HalfWidth > first )
	{
		if (  current_center - HalfWidth < last )      
			SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 1, current_center - HalfWidth 	, yVal);
		else 																										
			SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 1, last 								, yVal);   
	}
	// but if it is off the edge of the graph, put cursor at the edge of the graph
	else 	SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 1, first 								, yVal);      
	
	
	
	// adjust the cursor that indicates the center BE of the detector        
	if (  current_center  > last )
			SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 2, 	last				, yVal);  
	else if (  current_center  < first )   
			SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 2, 	first				, yVal);     
	else  SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 2,  	current_center	, yVal);           	    


	
	// adjust the cursor that indicates the high BE side of the detector   
	if (  current_center + HalfWidth < last )
	{
		if (  current_center + HalfWidth > first )       
				SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 3, current_center + HalfWidth	, yVal);
		else	SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 3, first								, yVal);             	
	}
	else 		SetGraphCursor (pDataMonitor, pDISPLAY_GRAPH, 3, last									, yVal);  

	
	return 0;
}








// makes a "header" in the file with all the experiment info ...
/////int scan_save_experiment_info_in_file( struct experimentStruct Exper  )	 old version
int scan_save_experiment_info_in_file( struct experimentStruct *ExpPtr  )     
{

	int LocalRegionIndex;
	
	FILE *file_handle;
	
	//char *Date;
	//char *Time;
	
	//Date = DateStr ();
	//Time = TimeStr ();

	SetWaitCursor (1);
	
	file_handle = fopen (ExpPtr -> Pathname, "a");	// open file in append mode	      	

	// here we save the needed parameters to know how the expt was performed
	fprintf (file_handle,"[ Experiment Parameters ]\n");     
	
	fprintf (file_handle, "Experiment Description= %s \n", ExpPtr -> Description );
	fprintf (file_handle, "Number_of_Regions_in_Experiment= %i \n\n", ExpPtr -> n_o_regions );
	
	fprintf (file_handle,"Original_File_name= %s \n", ExpPtr -> Pathname);
	fprintf (file_handle, "Instrument= ESCA-West\nDate= %s\nTime=  %s\n\n", ExpPtr -> DateBegan, ExpPtr -> TimeBegan );  
	
	
	//was: for ( LocalRegionIndex=1 ; LocalRegionIndex <= ExpPtr -> n_o_regions ; LocalRegionIndex++ )        
	for ( LocalRegionIndex=0 ; LocalRegionIndex < ExpPtr -> n_o_regions ; LocalRegionIndex++ )
	{
		
		fprintf (file_handle, "[ Region %i Parameters ]\n",  LocalRegionIndex+1);  
	
		fprintf (file_handle, "Region_Description = %s\n", ExpPtr -> Regions[LocalRegionIndex].Description); 
		fprintf (file_handle, "BEcenter(eV) = %f\n"	,	ExpPtr -> Regions[LocalRegionIndex].BEcenter);
		fprintf (file_handle, "BEwidth(eV) = %f\n"		,	ExpPtr -> Regions[LocalRegionIndex].BEwidth);
		fprintf (file_handle, "BEstep(eV) = %f\n"		,	(double) ExpPtr -> Regions[LocalRegionIndex].StepMultiple* ((double)EscaWest.DetectorWidth) / ((double)N_O_DETECTOR_CHANNELS)  ) ;     
		fprintf (file_handle, "Dwell_Time(sec) = %f\n",	ExpPtr -> Regions[LocalRegionIndex].DwellTime );          
		fprintf (file_handle, "N_o_scans = %i  \n"		,	ExpPtr -> Regions[LocalRegionIndex].NumberOfScans);
		fprintf (file_handle, "N_o_Channels = %i  \n"		,	ExpPtr -> Regions[LocalRegionIndex].NumberOfChannels);  
		fprintf (file_handle, "Excitation_Energy = %f  \n"		,	ExpPtr -> Regions[LocalRegionIndex].ExcitationEnergy);  
	
		if( ExpPtr -> Regions[LocalRegionIndex].ScanMode == SCAN_MODE_DITHERED )		fprintf (file_handle, "Scan_mode = 0  (DITHERED)\n\n");        
		else	 fprintf (file_handle, "Scan_mode = 1  (FIXED)\n\n");   
	}
	fclose (file_handle);					// close the file      	
	SetWaitCursor (0);	
	
	return 0;
}





//// saves data from an experiment...
//// assumes filename passed is valid... will append to end of file....
//int scan_save_data_in_file( char pathname[MAX_PATHNAME_LEN],  struct regionStruct Region, int number_of_data_points,
//				double *Energies, double *Counts )
//{
// 	//char pathname[MAX_PATHNAME_LEN], file_name[13];			
//	int i;
//	FILE *file_handle;
//	
//	SetWaitCursor (1);        
//	file_handle = fopen (pathname, "a");	/* open file in append mode	*/      	
//
//	/****here we save the needed parameters to know how the expt was performed ****/
//	
//	// depend upon regionIndex being correct: (a global variable)
//	fprintf (file_handle, "[ Region %i Data ]\n"			,regionIndex+1    ); 
//	//fprintf (file_handle, "N_o_data_points= %i\n"	,number_of_data_points );
//	
//	/**** then we save the data from this expt ************************************/
//		
//	for ( i=0; i < number_of_data_points ; i++ ) 
//	{
//		fprintf(file_handle, "%-8.3f %12.8e \n", Energies[i], Counts[i] );
//	}
// 	fprintf(file_handle, "\n");
// 		
//	fclose (file_handle);					/* close the file	*/       	
//	SetWaitCursor (0);								
//
//	return 0;
//}
//

// saves data from an experiment...
// assumes filename passed is valid... will append to end of file....
int scan_save_curve_in_file( char pathname[MAX_PATHNAME_LEN],  data_type_pointer Curve)
{
	int i;
	FILE *file_handle;
	
	double firstBE;
	double stepBE;
	
	
	SetWaitCursor (1);        
	file_handle = fopen (pathname, "a");	/* open file in append mode	*/      	

	/****here we save the needed parameters to describe the accumulated data ****/
	
	// depend upon regionIndex being correct: (a global variable)
	fprintf (file_handle, "[ Region %i Curve ]\n"			,regionIndex+1    ); 
	//fprintf (file_handle, "N_o_data_points= %i\n"	,number_of_data_points );
	
	/**** then we save the data from this expt ************************************/
	fprintf(file_handle, "BEcenter= %-8.3f\n", 	Curve->center_energy );
	fprintf(file_handle, "BEwidth= %-8.3f\n", 	-fabs(Curve->width)  );				  /*always negative?*/
	fprintf(file_handle, "Excitation= %-8.3f\n", Curve->excitation_energy ); 
	
	fprintf(file_handle, "Time_p_channel= %-8.3f\n", Curve->time_p_channel);
	fprintf(file_handle, "Size= %i\n", 				Curve->size);							  /*num points*/
	
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
		firstBE = Curve->center_energy - 0.5*Curve->width;					/*get maxBE*/
		stepBE  =  Curve->width / (double) (Curve->size - 1 );		/*negative */
		
		for ( i=0; i < Curve->size ; i++ ) 
			fprintf(file_handle, "%-8.3f %12.8e \n", firstBE + stepBE*i , Curve->data[i] );
	}
	else  // positive width, currently have {BEmin...BEmax} tabulation, want {BEmax...BEmin}
	{
		firstBE = Curve->center_energy - 0.5*Curve->width;				/* get minBE */
		stepBE  = Curve->width / (double) (Curve->size - 1 );	/* positive  */ 
		
		for ( i = Curve->size - 1;  i >= 0 ;  i-- ) 
			fprintf(file_handle, "%-8.3f %12.8e \n", firstBE + stepBE*i , Curve->data[i] );
	}
	//==========================================================================
	
	
 		
	fclose (file_handle);					/* close the file	*/       	
	SetWaitCursor (0);								

	return 0;
	
}
















int CVICALLBACK display_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	int H, W;  //height and width of panel...
	
	switch (event)
	{
		// if user resizes panel, resize the graph to fill the new size...
		case EVENT_PANEL_SIZE:
			GetPanelAttribute (pDataMonitor, ATTR_HEIGHT, &H );
			GetPanelAttribute (pDataMonitor, ATTR_WIDTH , &W );
			
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH, ATTR_HEIGHT, H-20);
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH, ATTR_WIDTH , W-6);
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH, ATTR_LEFT, -3);
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH, ATTR_TOP,20);
			break;
		
		case EVENT_CLOSE:
			ShowDataMonitor = FALSE;
			SetCtrlVal (pScan, pCONTROL_SHOW_DATAMONITOR,  ShowDataMonitor);
			HidePanel( pDataMonitor );
			break;
	}
	return 0;
}



void CVICALLBACK display_menu (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	int checked;
	
	
	switch( menuItem ){
		case mDISPLAY_PANEL_CLOSE:
			ShowDataMonitor = FALSE;
			SetCtrlVal (pScan, pCONTROL_SHOW_DATAMONITOR,  ShowDataMonitor);
			HidePanel( pDataMonitor );
			break;
		
		case mDISPLAY_VIEW_KINETIC :
			// make sure menu item is checked
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_KINETIC, ATTR_CHECKED, TRUE);  
			// and uncheck the binding menu item
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_BINDING, ATTR_CHECKED, FALSE);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_CHANNEL, ATTR_CHECKED, FALSE);    
			
			graph_control( PLOT_KINETIC, cDataCurve );        
			break;
			
		case mDISPLAY_VIEW_BINDING :
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_BINDING, ATTR_CHECKED, TRUE);  
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_KINETIC, ATTR_CHECKED, FALSE);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_CHANNEL, ATTR_CHECKED, FALSE);  
			
			graph_control( PLOT_BINDING, cDataCurve ); 
			break;
			
		case mDISPLAY_VIEW_CHANNEL :
			graph_control( PLOT_CHANNEL, cDataCurve ); 
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_CHANNEL, ATTR_CHECKED, TRUE);   
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_BINDING, ATTR_CHECKED, FALSE);  
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_KINETIC, ATTR_CHECKED, FALSE); 
			
			
			break;	
			
			
			
		case mDISPLAY_VIEW_FLIP :
			GetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, &checked );
			// see if menu item is checked... if it is then uncheck it and unflip the axis        
			if( checked )
			{
				  SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, FALSE);
				  SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH , ATTR_XREVERSE, FALSE);     
			}	
			// otherwise check it and flip the axis
			else
			{	  
				  SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, TRUE); 
				  SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH, ATTR_XREVERSE, TRUE); 
			}
			break;	
		
		case mDISPLAY_VIEW_STYLE_THIN_LINE : 
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_LINE, ATTR_CHECKED, 1);    
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_CONNECTED, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_SCATTER, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_VERTICAL_BAR, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_STEP, ATTR_CHECKED, 0);
		 	Plot.Style = VAL_THIN_LINE;
			break;
		
		case mDISPLAY_VIEW_STYLE_CONNECTED :
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_LINE, ATTR_CHECKED, 0);        
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_CONNECTED, ATTR_CHECKED, 1);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_SCATTER, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_VERTICAL_BAR, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_STEP, ATTR_CHECKED, 0);
		 	Plot.Style = VAL_CONNECTED_POINTS;
			break;
			
		case mDISPLAY_VIEW_STYLE_SCATTER :
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_LINE, ATTR_CHECKED, 0);        
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_CONNECTED, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_SCATTER, ATTR_CHECKED, 1);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_VERTICAL_BAR, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_STEP, ATTR_CHECKED, 0);
			Plot.Style = VAL_SCATTER;
			break;
			
		case mDISPLAY_VIEW_STYLE_VERTICAL_BAR :
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_LINE, ATTR_CHECKED, 0);        
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_CONNECTED, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_SCATTER, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_VERTICAL_BAR, ATTR_CHECKED, 1);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_STEP, ATTR_CHECKED, 0);
			Plot.Style = VAL_VERTICAL_BAR;
			break;
			
		case mDISPLAY_VIEW_STYLE_THIN_STEP :
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_LINE, ATTR_CHECKED, 0);        
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_CONNECTED, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_SCATTER, 		ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_VERTICAL_BAR, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_STYLE_THIN_STEP, 	ATTR_CHECKED, 1);
			Plot.Style = VAL_THIN_STEP;
			break;
			
		
		
		
		
		
		
		case mDISPLAY_VIEW_POINT_SIMPLE_DOT:
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_SIMPLE_DOT,	ATTR_CHECKED, 1);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_EMPTY_CIRCLE, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_EMPTY_SQUARE, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_SMALL_X, ATTR_CHECKED, 0);
	 		Plot.TracePointStyle = VAL_SIMPLE_DOT;
			break;
			
		case mDISPLAY_VIEW_POINT_EMPTY_CIRCLE:
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_SIMPLE_DOT,	ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_EMPTY_CIRCLE, 	ATTR_CHECKED, 1);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_EMPTY_SQUARE, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_SMALL_X, ATTR_CHECKED, 0);
		 	Plot.TracePointStyle = VAL_EMPTY_CIRCLE; 
			break;
			
		case mDISPLAY_VIEW_POINT_EMPTY_SQUARE:
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_SIMPLE_DOT,	ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_EMPTY_CIRCLE, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_EMPTY_SQUARE, ATTR_CHECKED, 1);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_SMALL_X, ATTR_CHECKED, 0);
			Plot.TracePointStyle = VAL_EMPTY_SQUARE;
	 		break;
	 		
	 	case mDISPLAY_VIEW_POINT_SMALL_X :
	 		SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_SIMPLE_DOT,	ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_EMPTY_CIRCLE, 	ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_EMPTY_SQUARE, ATTR_CHECKED, 0);
			SetMenuBarAttribute (menuBar, mDISPLAY_VIEW_POINT_SMALL_X, ATTR_CHECKED, 1);	
			Plot.TracePointStyle =  VAL_SMALL_X;
	 		break;
	 
	
	}
	 
	SetPlotAttribute (pDataMonitor, pDISPLAY_GRAPH, Plot.Handle, ATTR_PLOT_STYLE, Plot.Style);
	SetPlotAttribute (pDataMonitor, pDISPLAY_GRAPH, Plot.Handle, ATTR_TRACE_POINT_STYLE, Plot.TracePointStyle);  
	SetPlotAttribute (pDataMonitor, pDISPLAY_GRAPH, Plot.Handle, ATTR_LINE_STYLE, Plot.LineStyle);          
	RefreshGraph (pDataMonitor, pDISPLAY_GRAPH);
	
}


void display_initialize_plot( void )
{
	Plot.Style = VAL_THIN_LINE;  
	Plot.TracePointStyle =  VAL_SMALL_X;     
	Plot.LineStyle = VAL_SOLID;
	Plot.Color = VAL_RED;
	
	Plot.Handle = PlotPoint (pDataMonitor, pDISPLAY_GRAPH, 0.0, 0.0, Plot.TracePointStyle, Plot.Color);

}










//deals with pause requests 
int scan_pause_go( int PauseType )
{

	int panel, control;

	// we have requested a pause...
	Paused =TRUE;
	// and the default is to resume from where the pause occured...
	Continue = CONTINUE;
	
	
	if( ( pPaused = LoadPanel (0, "scan.uir", pPAUSED)) < 0)
		return -1;
	
	set_front_end_message( "EXPERIMENT HAS BEEN PAUSED...", VAL_BLUE );          
	// first turn on the default selection		
	SetCtrlAttribute (pPaused,  pPAUSED_CONTINUE, 			ATTR_CTRL_VAL, ON);
	// turn all other selections off... 	
	SetCtrlAttribute (pPaused, pPAUSED_STOP_REGION, 		ATTR_CTRL_VAL, OFF);
	SetCtrlAttribute (pPaused, pPAUSED_STOP_EXPERIMENT, 	ATTR_CTRL_VAL, OFF);
	SetCtrlAttribute (pPaused, pPAUSED_REDO_REGION, 		ATTR_CTRL_VAL, OFF);
	SetCtrlAttribute (pPaused, pPAUSED_REDO_EXPERIMENT, 	ATTR_CTRL_VAL, OFF);
	InstallPopup (pPaused);

	while ( Paused )
	{
		// wait for the user to press a control...
		GetUserEvent (1, &panel, &control);
		//if( Paused == FALSE )	break;
	}
	
	return 0;
}



int CVICALLBACK pause_choice (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
		
			// first turn all selection off						
			SetCtrlAttribute (pPaused, pPAUSED_CONTINUE, 			ATTR_CTRL_VAL, OFF);
			SetCtrlAttribute (pPaused, pPAUSED_STOP_REGION, 		ATTR_CTRL_VAL, OFF);
			SetCtrlAttribute (pPaused, pPAUSED_STOP_EXPERIMENT, 	ATTR_CTRL_VAL, OFF);
			SetCtrlAttribute (pPaused, pPAUSED_REDO_REGION, 		ATTR_CTRL_VAL, OFF);
			SetCtrlAttribute (pPaused, pPAUSED_REDO_EXPERIMENT, 	ATTR_CTRL_VAL, OFF);
			
			SetCtrlAttribute (pPaused, control, 	ATTR_CTRL_VAL, ON);    
		
			// then turn back on the selected region
			switch( control ) {
				case pPAUSED_CONTINUE:
					SetCtrlAttribute (pPaused, pPAUSED_CONTINUE, 			ATTR_CTRL_VAL, ON);
					Continue = CONTINUE;
					break;
				case pPAUSED_STOP_REGION:
					SetCtrlAttribute (pPaused, pPAUSED_STOP_REGION, 		ATTR_CTRL_VAL, ON); 
					Continue = CONTINUE_KILL_CURRENT_REGION;
					break;
				case pPAUSED_STOP_EXPERIMENT:
					SetCtrlAttribute (pPaused, pPAUSED_STOP_EXPERIMENT, 	ATTR_CTRL_VAL, ON);
					Continue = CONTINUE_KILL_EXPERIMENT;        
					break;
				case pPAUSED_REDO_REGION:
					SetCtrlAttribute (pPaused, pPAUSED_REDO_REGION, 		ATTR_CTRL_VAL, ON); 
					Continue =  CONTINUE_RESTART_CURRENT_REGION;         
					break;
				case pPAUSED_REDO_EXPERIMENT:
					SetCtrlAttribute (pPaused, pPAUSED_REDO_EXPERIMENT, 	ATTR_CTRL_VAL, ON); 
					Continue =  CONTINUE_RESTART_CURRENT_EXPERIMENT;
					break;
				
			}   
			
			break;
	}
	return 0;
}


int CVICALLBACK pause_done (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	if( event == EVENT_COMMIT )
	{
		//close the panel
		RemovePopup (0);
		Paused = FALSE;
		set_front_end_message( "EXPERIMENT IN PROGRESS...", VAL_BLUE );           
		
	}
	return 0;
}







int CVICALLBACK control_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	int H,W;
	
	switch (event)
	{
		case EVENT_PANEL_MOVE:
			// check that user did not position outside of the screen...  
			// during a scan the PC can slow down enough that user can accidently 
			// move window right off the screen and then not be able to access it any more!
			GetScreenSize (&H, &W);
			break;
		
		case EVENT_CLOSE:
			// should act as a push on the pause / close buttons ??
			break;
	}
	return 0;
}




void graph_control( int EnergyAxisType, struct data_type Curve )
{
	switch( EnergyAxisType )
	{
		
		case PLOT_CHANNEL:
			SetCtrlAttribute (pDataMonitor,pDISPLAY_GRAPH , ATTR_XAXIS_GAIN, 1.0); 
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH , ATTR_XAXIS_OFFSET, 0.0); 
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH, ATTR_XPRECISION, 0);
			SetCtrlAttribute (pDataMonitor,pDISPLAY_GRAPH , ATTR_XNAME, "Channels");
			break;
	
	
		case PLOT_KINETIC:
					  	// Multiply binding energy by -1.0 ...
			SetCtrlAttribute (pDataMonitor,pDISPLAY_GRAPH , ATTR_XAXIS_GAIN, -Curve.width/Curve.size); 
						// ...and add the excitation energy to this to get kinetic ...
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH , ATTR_XAXIS_OFFSET, Curve.excitation_energy - (Curve.center_energy - 0.5*Curve.width)); 
					  //  ...then make sure precision is ok....
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH, ATTR_XPRECISION, 2);
						
						//.. then update the axis label.
			SetCtrlAttribute (pDataMonitor,pDISPLAY_GRAPH , ATTR_XNAME, "Kinetic Energy (eV)");
			break;
	
		case PLOT_BINDING:
			// update the graph attributes... just want to plot the array taht was sent to it.. no gain, no offset...
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH , ATTR_XAXIS_OFFSET, Curve.center_energy - 0.5*Curve.width);
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH , ATTR_XAXIS_GAIN, Curve.width/Curve.size);
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH, ATTR_XPRECISION, 2);
			SetCtrlAttribute (pDataMonitor, pDISPLAY_GRAPH , ATTR_XNAME, "Binding Energy (eV)");
			break;		


	}
}

int CVICALLBACK scan_panel_controls (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	if( event == EVENT_COMMIT )
	{
		switch( control )
		{
			case pCONTROL_SHOW_DETMONITOR:
				ShowDetectorMonitor = TRUE;
				if(!DetMonitorPanelOpen)  detector_monitor_go();
				break;
				
			case  pCONTROL_SHOW_DATAMONITOR:
				ShowDataMonitor = TRUE;
				DisplayPanel( pDataMonitor ) ;
			
				break;
				
			case pCONTROL_PAUSE:
				// Just update the global... 
				//		...the scan functions check the status of this flag
				//       ...to see if the scan has been paused
				Paused = TRUE;
				break;
				
			case pCONTROL_CLOSE:
				DiscardPanel( pDataMonitor );
				DiscardPanel( pScan );
				break;
		
		
		}
	
	
	}
	return 0;
}
