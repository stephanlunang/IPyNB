



#include <userint.h>
#include <ansi_c.h>
#include <utility.h>
#include "define.h"
#include "globals.h"


#include "detectoh.h"
#include "detector.h"  
#include "esca.h"
#include "escah.h"

#include "alex_ini.h"


int DetManualPanelOpen=0;  
int DetMonitorPanelOpen=0; 
int DetAutoPanelOpen=0;  
int DetSensitivityPanelOpen=0;  

int DetectorTesting=0;
int DetectorTestSignal=0;




static double Sensitivity[N_O_DETECTOR_CHANNELS];


//panel handles 
static int pDetManual, pDetMonitor, pDetAuto, pDetSensitivity;  


int CVICALLBACK detector_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event) 
	{
	
		case EVENT_CLOSE:
			detector_close_up_shop();
			break;
	}
	return 0;
}


// start up the visual detector monitoring utility
int detector_monitor_go (void )
{
	int plot_handle ;
	
	// if the panel is currently not open, then open it... 
	if( DetMonitorPanelOpen == FALSE)
	{
		// load the panel
		if ( ( pDetMonitor = LoadPanel (0, "detector.uir", pMONITOR) ) < 0 )
			return -1;; 
		// display the panel
		DisplayPanel( pDetMonitor );
		DetMonitorPanelOpen = TRUE ;     
	}
	// ..else just bring to foreground...
	else
	   SetActivePanel (pDetMonitor);
	return 0;
}





int CVICALLBACK monitor_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event) 
	{
		
		case EVENT_CLOSE:
		
			monitor_close_up_shop();
			break;
	}
	return 0;
}


// Accepts a pointer to the array to be plotted and plots the data on the detector
//     monitoring graph.
// The array size is always assumed to be the number of channels available 
//   on the detector: N_O_DETECTOR_CHANNELS.
int monitor_update_display( double *data_array )
{

	int  plot_handle;
	unsigned int total_counts;
	int i;
	
	plot_handle = PlotY (pDetMonitor, pMONITOR_GRAPH, data_array, N_O_DETECTOR_CHANNELS,
						VAL_DOUBLE, VAL_BASE_ZERO_VERTICAL_BAR, 
						VAL_NO_POINT, VAL_SOLID, 1, VAL_DK_RED);

	// delete this new graph.. however, current display remains until new graph is added 
	DeleteGraphPlot (pDetMonitor, pMONITOR_GRAPH, plot_handle,VAL_DELAYED_DRAW);

	total_counts=0;
	for( i = N_O_DETECTOR_CHANNELS - 1 ; i >= 0 ; i-- )        
	{
		total_counts += (unsigned int)data_array[i];
	}
	sprintf( Message, "Total Counts: %i", total_counts );
	SetCtrlVal (pDetMonitor, pMONITOR_TOTALCOUNTS , Message);

	
	return 0;
}	












int detector_utility_go(void)
{
	double interval;
	
	// if the auto reading utlity is open, close it first...
	if(  DetAutoPanelOpen == TRUE )
	{  
		detector_auto_close_up_shop();
	}
	
	// if the panel is currently not open, then open it...
	if( DetManualPanelOpen == FALSE )
	{
		
		if( ( pDetManual = LoadPanel (0, "detector.uir", pDETECTOR) ) < 0 )
			return -1;
		
		// before displaying panel, make sure upper limit on time request is ok
		SetCtrlAttribute (pDetManual, pDETECTOR_TIMER_VALUE, ATTR_MAX_VALUE, ACCUM_TIME_MAX );
		
		// and enable the timer at its default interval
		SetCtrlAttribute (pDetManual, pDETECTOR_STATUS_TIMER, ATTR_ENABLED, 1);
		GetCtrlAttribute (pDetManual, pDETECTOR_STATUS_TIMER, ATTR_INTERVAL, &interval ); 
		SetCtrlVal (pDetManual, pDETECTOR_STATUS_INTERVAL,  (unsigned int) ( 1000.0 * interval) );                 
	
		DetManualPanelOpen = TRUE;
		DisplayPanel(pDetManual);
		
		detector_monitor_go();
	}
	// ..else just bring to foreground...
	else
	   SetActivePanel (pDetManual);
	
	
	return 0;
}   


int CVICALLBACK detector_utility (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int i;
	char string[40]; //size could be smaller??
	double accum_time;
	unsigned char statPSTS, statSTIO, statIRQ  ; //hold status logic levels

	double data[N_O_DETECTOR_CHANNELS];

	switch (event)
	{
		case EVENT_COMMIT:

			switch( control )
			{
				case pDETECTOR_RESET:
					// send the reset command
					det_reset_control();
					break;
			
			 	case pDETECTOR_SET_TIMER:
			 		// read the requested time
			 		GetCtrlVal (pDetManual, pDETECTOR_TIMER_VALUE, &accum_time) ;
			 		
			 		// make sure it is in allowed limits... max value is 0111 1111 = 2^15 - 1 = 32767
			 		if ( accum_time > ACCUM_TIME_MAX )
			 		{
			 			accum_time = ACCUM_TIME_MAX ;
			 			SetCtrlVal (pDetManual, pDETECTOR_TIMER_VALUE, accum_time) ; 
			 		}
			 		// no problem with minimum time.. if set to zero, we are in untimed mode.. counts forever
			
			 		
			 		//send the set timer command ... don't strobe!
			 		det_send_timer_command ( (double)accum_time /1000.0 );
 
			 		//change strobe button the hot button asnd highlight with color change
			 		//   to indicate that command has not been completed
			 		SetCtrlAttribute ( pDetManual , pDETECTOR_PCTL, ATTR_CMD_BUTTON_COLOR, VAL_YELLOW);

					SetCtrlVal (pDetManual, pDETECTOR_UPDATE_STRING, "To complete TIMER SET command, press Stobe Output Data (PCTL). ");
			 		break;
			 		
			 	case pDETECTOR_START:
			 		//send the start accum command ... don't strobe!
			 		det_send_start_command() ;
			 		//change strobe button the hot button asnd highlight with color change
			 		//   to indicate that command has not been completed
			 		SetCtrlAttribute ( pDetManual , pDETECTOR_PCTL, ATTR_CMD_BUTTON_COLOR, VAL_YELLOW);
			 		
			 		SetCtrlVal (pDetManual, pDETECTOR_UPDATE_STRING, "To complete START command, press Stobe Output Data (PCTL). ");
			 		break;
			 
			 	case pDETECTOR_STOP:
			 		//send the stop accum command ... don't strobe!
			 		det_send_stop_command();
			 		//change strobe button the hot button asnd highlight with color change
			 		//   to indicate that command has not been completed
			 		SetCtrlAttribute ( pDetManual , pDETECTOR_PCTL, ATTR_CMD_BUTTON_COLOR, VAL_YELLOW);
			 		
			 		SetCtrlVal (pDetManual, pDETECTOR_UPDATE_STRING, "To complete STOP command, press Stobe Output Data (PCTL). ");    
			 		break;
			 	
			 	case pDETECTOR_PCTL:   
			 		// make positive pulse on PCTL... strobes the current control line data into
			 		//     detector 2420B PCIO Interface.
			 		det_strobe_PCTL();
			 		// make sure button color is returned to normal...
			 		SetCtrlAttribute ( pDetManual , pDETECTOR_PCTL, ATTR_CMD_BUTTON_COLOR, VAL_OFFWHITE);
			 		
			 		SetCtrlVal (pDetManual, pDETECTOR_UPDATE_STRING, " ");    
			 		break;
			 		
			 	case pDETECTOR_CLEAR:
			 		det_clear_buffers( );
			 		
			 		SetCtrlVal (pDetManual, pDETECTOR_TIMER_VALUE, 0.0 ) ; 
			 		SetCtrlVal (pDetManual, pDETECTOR_UPDATE_STRING, "Both memory buffers have been cleared. Timer is now set to 0. ");   
			 		break;
			 		
			 	case pDETECTOR_READ:
			 		// reads the current buffer into memory 
			 		det_read_buffer( &data[0] );
			 		
			 		SetCtrlVal (pDetManual, pDETECTOR_UPDATE_STRING, "Reading the detector counts array..."); 
			 		// and displays via the control/indicator pair on the detector utility panel
			 		ResetTextBox (pDetManual, pDETECTOR_DATA, "");
			 		
			 		for ( i = 0 ; i < N_O_DETECTOR_CHANNELS; i++ )
			 		{
			 			sprintf (string, "%4i        %10.0f", i, data[i]);
			 			InsertTextBoxLine (pDetManual, pDETECTOR_DATA, -1, string);
			 		}
			 		SetCtrlVal (pDetManual, pDETECTOR_UPDATE_STRING, "Done reading and displaying the detector counts array.");           
			 		break;
			 		
			
			}
			break;
	}
	return 0;
}










// whenever the timer ticks, read the status bits and update the panel
int CVICALLBACK status_timer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned char statPSTS, statSTIO, statIRQ  ; //hold status logic levels          
	
	switch (event) 
	{
		case EVENT_TIMER_TICK:
			// read the status lines.... 
			det_check_status_lines( &statSTIO, &statIRQ, &statPSTS );
			//	...and display results
	 		
			SetCtrlVal ( pDetManual, pDETECTOR_STIO_LED, statSTIO );
			SetCtrlVal ( pDetManual, pDETECTOR_IRQ_LED, statIRQ ); 
			SetCtrlVal ( pDetManual, pDETECTOR_PSTS_LED, statPSTS ); 
			break;
	}
	return 0;
}

// whenever the user adjusts the time period, change the timer interval to match
int CVICALLBACK status_interval_adjust (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned int interval;
	
	switch (event) 
	{
		case EVENT_COMMIT:
			// read data... an interger in milliseconds
			GetCtrlVal( pDetManual, pDETECTOR_STATUS_INTERVAL, & interval );
			// set timer interval to match
			SetCtrlAttribute (pDetManual, pDETECTOR_STATUS_TIMER, ATTR_INTERVAL, ((double) (interval)) /1000.0  );
			break;
	}
	return 0;
}



















/**********************************************************************************/
/*** interface functions for the detector through the 2420B Parallel IO Card    ***/




int det_clear_buffers( void )
{ 
	// clear both buffers... 
	//    1) read all 256 channels,
	//    2) swap buffers ( set time = 0, start, stop sequence automatically swaps)
	//    3) read all 256 channels.
	// buffers are now zeroed (through first 256 channels which is all we care about
	
	
	double junk[ N_O_DETECTOR_CHANNELS];
	
	det_read_buffer( &junk[0] );
			 		
	det_send_timer_command ( 0.0); 
	det_strobe_PCTL(); 
	det_send_start_command() ;  
	det_strobe_PCTL();  
	det_send_stop_command() ;  
	det_strobe_PCTL(); 
			 		
	det_read_buffer( &junk[0] );
	
	return 0;
}

// array that is passed MUST be at least N_O_DETECTOR_CHANNELS large
// assumes the buffer to be read is currently available... recall that
// after an accumulation, the data is not available to be read until the
// **next** start accum command is sent...
int det_read_buffer( double *array) // want to pass an array of long ints
{ 
	int i;

	if( DetectorTestSignal) 
	{
		for( i = N_O_DETECTOR_CHANNELS - 1 ; i >= 0 ; i-- )      
			array[i] = i;		// fake data
	}
	
	else
	{
		// I didn't know which end of the detector is at the higher Binding / lower kinetic energy...
		// could have been:   for( i = 0 ; i < N_O_DETECTOR_CHANNELS ; i++ )  , but the verxion
		// below is the correct looping
		
		for( i = N_O_DETECTOR_CHANNELS - 1 ; i >= 0 ; i-- )        
		{
	
		//"The output buffer address pointer is automatically reset to channel zero
		//   when the buffers are swapped"...p.6 of Model 2412A Histogramming Buffer Memory Manual
			array[i] = (double) inpw ( BASE_2420B );	// read the data that is present  
		
			///
			/// mask off the funny end channels
			///
			
			array[N_O_DETECTOR_CHANNELS - 1] = 0.0  ;
			array[N_O_DETECTOR_CHANNELS - 2] = 0.0  ;
		
			//Strobe the next channel data onto the output lines
			det_strobe_IOSTB();
		}	
	}
	
	// if the detector monitoring utility is open, display most current data on its graph
	if(  DetMonitorPanelOpen ){
		monitor_update_display( &array[0] );
	}
	
	return 0;
}












// check the status line and puts results in the address pointed to by the passed addresses
int det_check_status_lines( unsigned char *ptrSTIO, unsigned char *ptrIRQ, unsigned char *ptrPSTS )
{
	// unsigned char is 8 bits 
	
	unsigned char byte_received;	// use a unsigned char to make the size one byte ( int is 2 bytes )
	unsigned char STIOmask = 2;
	unsigned char IRQmask = 4;
	unsigned char PSTSmask = 8;
	
	if ( DetectorTesting )
	{
		*ptrSTIO =  0;		// set the accumulation to be off
		*ptrIRQ  =  IRQmask;			// set service request to be on
		*ptrPSTS =  0;   	// pretend board is powered
	}
	
	else
	{
		byte_received = inp (BASE_2420B  + 0x2 );	 // read the byte at BASE + 2 ... this is where the status lines are
	
		// to extract STIO (I/O Status) , 				perform a bitwise and with 00000010 = 2^1 = 2 mask 
		// high logic level (1) indicates that the memory is accumulating data
		// *ptrSTIO means the value that is pointed to by the ptrSTIO pointer
		*ptrSTIO = byte_received & STIOmask;
	
		// to extract IRQ  ( Interupt Request Status), 	perform a bitwise and with 00000100 = 2^2 = 4 mask 
		// high logic level (1) if accum is finished or a channel is full... look at STIO to tell which is the case
		*ptrIRQ = byte_received & IRQmask; 
	
		// to extract PSTS (Peripheral Status),  		perform a bitwise and with 00001000 = 2^3 = 8 mask  
		// low logic value (0) means buffer memory is powered and available for operation  
		*ptrPSTS = byte_received & PSTSmask; 
	}
	
	
		
	return 0;
}



//pass the number of SECONDS you want... will round to nearest millisecond...
int det_send_timer_command( double time)
{ 
	// command is interpreted ans the set time command if the most significant bit is zero
	// send:  0ttt tttt tttt tttt    where t represents the time requested, between 0 and 32767 milliseconds
	
	// unsigned short int is 16 bits     
	unsigned short TimeCommand;
	unsigned short MilliSecTime;
	
	
	time *= 1000.0;  //convert seconds to milliseconds...
	if( time < 0.0 ) time = 0.0;
	
	MilliSecTime = time;   //truncates... can be off by a millisec still so check:
	if( (time - (double)MilliSecTime) >=  0.5  ) MilliSecTime++;       
	
	TimeCommand = MilliSecTime & 32767 ;  	 // ensures most significant bit is zero 0111 1111 1111 1111 = 32767
	
	if ( DetectorTesting )
		printf( "SET_TIME command would be sent now if not in test mode. Time = %i msec.\n", TimeCommand);
	else
		outpw ((BASE_2420B ),  TimeCommand ); 
	
	return 0;
}


int det_send_start_command(void)
{ 
	// unsigned short int is 16 bits
	unsigned short StartCommand =  32769 ;		//  1000 0000 0000 0001  = 2^15 + 2^0 = 32769

	if ( DetectorTesting )
		printf( "START command would be sent now if not in test mode. Value = %i.\n", StartCommand);
	else
		outpw ((BASE_2420B ),  StartCommand );

	return 0;
}


int det_send_stop_command( void )
{ 
	// unsigned short int is 16 bits
	unsigned short StopCommand =  32768;		//  1000 0000 0000 0000  = 2^15 0= 32768
	
	if ( DetectorTesting )
		printf( "STOP command would be sent now if not in test mode. Value = %i.\n", StopCommand);
	else
	outpw ( (BASE_2420B ),  StopCommand);

	return 0;
}



// PCTL strobe ( peripc\heral control) is used to transfer data into the buffer memory from the computer.
// Such a strobe should follow the placement of a "start", "stop", or "set time" command on the control lines
// ... then this command is what is transfered.
int det_strobe_PCTL( void)
{ 
	unsigned char PCTL_on = 16;	// 0001 0000 = 16 ... bit 4 (of 0 to 7)
	unsigned char PCTL_off = 0 ;
	
	if ( DetectorTesting )
	{
		printf( "PCTL STROBE command would be sent now if not in test mode. \n");
	}
	else 
	{
		outp ( (BASE_2420B  + 0x2 ),  PCTL_off); // make sure line is in low logis level
		outp ( (BASE_2420B  + 0x2 ),  PCTL_on);  // .... raise to high 
		outp ( (BASE_2420B  + 0x2 ),  PCTL_off); //  .... and back to low to complete the toggle
	}
	
	return 0;
}

// IOSTB is the command line that is strobed (toggled) to get initiate a buffer memory output data
//   request.  Valide output data is available 1.5 MICROSECONDS after the toggle.  Data is held
//   until the next stobe cycle.
//
// Used in reading the buffer: strobe in a channel's data, read, stobe in the next, read, etc...
int det_strobe_IOSTB( void )
{ 
	unsigned char IOSTB_on = 1	;	// 0000 0001 = 1... bit 0 (of 0 to 7)
	unsigned char IOSTB_off = 0 ;
	
	if ( DetectorTesting ) 
	{
	//	printf( "IOSTB STROBE command would be sent now if not in test mode. \n");
	}
	else 
	{
		outp ( (BASE_2420B  + 0x2 ),  IOSTB_on);  //  raise ,ine to high logic level
		outp ( (BASE_2420B  + 0x2 ),  IOSTB_off); //  .... and back to low to complete the toggle
	}

	// data should now be ready to read

	return 0;
}


//a high logic level on the RESET input initializes the buffer memory control logic but does
//  not alter the memory contents.  It must be held at a low logic level during normal operation
//  This command should be issued by a software start-up sequence.
//
//  Basically, makes sure PCTL, IOSTB ( and maybe IRQ ?? ) are at low logic level
int det_reset_control( void )
{ 
	unsigned char RESET_on = 1	;	// 0000 1000 = 8 ... bit 3 (of 0 to 7)
	unsigned char RESET_off = 0 ;

	if ( DetectorTesting )
	{
		printf( "RESET STROBE command would be sent now if not in test mode. \n");
	}
	else 
	{
		outp ( (BASE_2420B  + 0x2 ),  RESET_off); 	//  make sure line is at low logic level 
		outp ( (BASE_2420B  + 0x2 ),  RESET_on);  	//  raise ,line to high logic level
		outp ( (BASE_2420B  + 0x2 ),  RESET_off); 	//  .... and back to low to complete the toggle
   }
    
	return 0;
}


















int detector_close_up_shop( void )
{
	
	
	
	if( DetManualPanelOpen == TRUE )
	{ 
		// send a stop command and clear buffers command...
	
		// disable the timer so that we are not continuaously reading the status for no reason
		SetCtrlAttribute (pDetManual, pDETECTOR_STATUS_TIMER, ATTR_ENABLED, FALSE);        
	
		DetManualPanelOpen = FALSE;
		//update_main_menu();
		DiscardPanel( pDetManual); 
	}
	
	return 0;
}


int monitor_close_up_shop( void )	
{
	if( DetMonitorPanelOpen == TRUE )
	{
		DetMonitorPanelOpen = FALSE;
		DiscardPanel( pDetMonitor );
	}
	return 0;
}



//callback for all detector utility menu items
void CVICALLBACK detector_menu (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	switch( menuItem )
	{
		case ( mDETECTOR_CONTROLS_AUTO ):
			// user wants to automatically/contiuously detect and display results
			detector_auto_go();
			break;
		
		
		case ( mDETECTOR_CONTROLS_MONITOR ):
			// user wants to see the detector monitor
			detector_monitor_go();
			break;
		
		case ( mDETECTOR_CONTROLS_CLOSE ):
			// user wants to close detector utility
			detector_close_up_shop();
			break;
			
		case ( mDETECTOR_HELP_PANEL ):
			// user needs help with the detector utility
			detector_help();
			break;
			
		case ( mDETECTOR_HELP_GENERAL ):
			// user wants help with the whole program
			esca_general_help();
			break;
			
	   default:
	   	printf( "detector_menu() failed to find case = %i. \n", menuItem );
			break;
	 }

}



int detector_help( void )
{
	MessagePopup ("Oops...",
              "Help files are not ready... ask Alex for assistance." );
	return 0;
}






int detector_auto_go( void )
{

	// if the manual control utility is open, close it before starting this one..
	if(  DetManualPanelOpen == TRUE )
	{  
		detector_close_up_shop();
	}

	// if the panel is currently not open, then open it...
	if(  DetAutoPanelOpen == FALSE )
	{
		
		if( ( pDetAuto = LoadPanel (0, "detector.uir", pAUTO) )< 0 )
			return -1 ;
		
		// before displaying panel, make sure upper limit on time request is ok
		SetCtrlAttribute (pDetAuto, pAUTO_INTERVAL, ATTR_MIN_VALUE, ACCUM_TIME_MIN );     
		SetCtrlAttribute (pDetAuto, pAUTO_INTERVAL, ATTR_MAX_VALUE, ACCUM_TIME_MAX );
		// set default value 1000 msec = 1 sec 
		SetCtrlVal ( pDetAuto, pAUTO_INTERVAL, 1.000 );
		
		// check status bits every 100 msec
		SetCtrlAttribute (pDetAuto, pAUTO_TIMER, ATTR_INTERVAL, 0.3 );    
		// and make sure timer is currently disabled 
		SetCtrlAttribute (pDetAuto, pAUTO_TIMER, ATTR_ENABLED, FALSE);
		
		
		SetCtrlVal (pDetAuto, pAUTO_UPDATE_STRING, "Ready...");
		// dim the HALT button
		SetCtrlAttribute (pDetAuto, pAUTO_HALT  , ATTR_DIMMED, TRUE ); 
		// set interval control to hot mode
		SetCtrlAttribute (pDetAuto, pAUTO_INTERVAL , ATTR_CTRL_MODE, VAL_HOT );
		// undim the BEGIN button
		SetCtrlAttribute (pDetAuto, pAUTO_BEGIN , ATTR_DIMMED, FALSE);     
		
		DetAutoPanelOpen = TRUE;
		DisplayPanel(pDetAuto);
		
		detector_monitor_go();
	}
	// ..else just bring to foreground...
	else
	{
	   SetActivePanel (pDetAuto);
	}
	
	return 0;
}




int detector_auto_close_up_shop( void )
{
	if( DetAutoPanelOpen == TRUE )
	{ 
		// send a stop command and clear buffers command...        
	
		// disable the timer so that we are not continuously reading the status for no reason
		SetCtrlAttribute (pDetAuto, pAUTO_TIMER, ATTR_ENABLED, FALSE);        
	
		DetAutoPanelOpen = FALSE;
		//update_main_menu();
		DiscardPanel( pDetAuto); 
	}

	return 0;
}







int CVICALLBACK autoread_panel (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event) 
	{
		 case EVENT_CLOSE:
			detector_auto_close_up_shop( );
			break;
	}
	return 0;
}

void CVICALLBACK auto_menu (int menuBar, int menuItem, void *callbackData, int panel)
{
	switch( menuItem )
	{
		case ( mAUTO_CONTROLS_MANUAL ):
			// user wants to automatically/contiuously detect and display results
			detector_utility_go();
			break;
		
		case ( mAUTO_CONTROLS_MONITOR ):
			// user wants to see the detector monitor
			detector_monitor_go();
			break;
		
		case ( mAUTO_CONTROLS_CLOSE ):
			// user wants to close detector utility
			detector_auto_close_up_shop();
			break;
			
		case ( mAUTO_HELP_PANEL ):
			// user needs help with the detector utility
			
			break;
			
		case ( mAUTO_HELP_GENERAL ):
			// user wants help with the whole program
			esca_general_help();
			break;
			
	   default:
	   		printf( "auto_menu() failed to find case = %i. \n", menuItem );
			break;
	 }
}

int CVICALLBACK auto_controls (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	unsigned char statPSTS, statSTIO, statIRQ  ; //hold status logic levels          
	double time;
	
	switch (event) 
	{
		case EVENT_COMMIT:


			switch ( control )
			{
				case pAUTO_INTERVAL:
					// no need to do anything... correct limits were set when panel was opened
					break;
					
				case pAUTO_BEGIN:
					// read the status lines.... 
					det_check_status_lines( &statSTIO, &statIRQ, &statPSTS );
					//	...and display results
					SetCtrlVal ( pDetAuto, pAUTO_STIO_LED,	statSTIO);
					SetCtrlVal ( pDetAuto, pAUTO_IRQ_LED,	statIRQ	); 
					SetCtrlVal ( pDetAuto, pAUTO_PSTS_LED,	statPSTS); 
					
					// if PSTS is FALSE, we have power to buffer memory, if not, then cancel and warn user...
					if ( statPSTS == TRUE ) 
					{
						// disable the timer and end auto accum...
						SetCtrlAttribute (pDetAuto, pAUTO_TIMER, ATTR_ENABLED, FALSE);       
				
						// send a stop command... 
						det_send_stop_command( );
						det_strobe_PCTL();      
				
						// dim the HALT button
						SetCtrlAttribute (pDetAuto, pAUTO_HALT  , ATTR_DIMMED, TRUE );       
						// undim the BEGIN button 
						SetCtrlAttribute (pDetAuto, pAUTO_BEGIN , ATTR_DIMMED, FALSE);
						// make interval control hot      
						SetCtrlAttribute (pDetAuto, pAUTO_INTERVAL , ATTR_CTRL_MODE, VAL_HOT);           
				
						SetCtrlVal (pDetAuto, pAUTO_UPDATE_STRING, "PSTS status indicates no power to the buffer memory. Scans halted.");       
						return -1;	
				
					}
					
					
					// send a clear buffers command, set time, start and enable panel timer
					//		to handle continuous updates...
					det_clear_buffers( );
					SetCtrlVal (pDetAuto, pAUTO_UPDATE_STRING, "Clearing memory buffers..."); 
					// read timer request value... could have been changed...
					GetCtrlVal (pDetAuto, pAUTO_INTERVAL, &time);
					// send time commmand
					det_send_timer_command( time );
					det_strobe_PCTL();
					
					
					// enable panel timer
					SetCtrlAttribute (pDetAuto, pAUTO_TIMER, ATTR_ENABLED, TRUE); 
					// disable main auto shutdown timer...
					shutdown_enable( FALSE );
					
					// send start command, which swaps memory buffers...  
					det_send_start_command();
					det_strobe_PCTL();
					
					SetCtrlVal (pDetAuto, pAUTO_UPDATE_STRING, "Accumulating....");      
					
					
					
					// dim the BEGIN button
					SetCtrlAttribute (pDetAuto, pAUTO_BEGIN, ATTR_DIMMED, TRUE);
					//  make the interval control and indicator      
					SetCtrlAttribute (pDetAuto, pAUTO_INTERVAL , ATTR_CTRL_MODE, VAL_INDICATOR);          
					// undim the HALT button
					SetCtrlAttribute (pDetAuto, pAUTO_HALT , ATTR_DIMMED, FALSE);
					
					break;
					
				case pAUTO_HALT:
					// disable panel timer...
					SetCtrlAttribute (pDetAuto, pAUTO_TIMER, ATTR_ENABLED, FALSE);       
					
					// enable main auto shutdown timer...
					shutdown_enable( TRUE );
					
					// send a stop command... 
					det_send_stop_command( );
					det_strobe_PCTL(); 
					
					// dim the HALT button
					SetCtrlAttribute (pDetAuto, pAUTO_HALT  , ATTR_DIMMED, TRUE );       
					// undim the BEGIN button
					SetCtrlAttribute (pDetAuto, pAUTO_BEGIN , ATTR_DIMMED, FALSE);
					// make interval control hot
					SetCtrlAttribute (pDetAuto, pAUTO_INTERVAL , ATTR_CTRL_MODE, VAL_HOT);       
					
					SetCtrlVal (pDetAuto, pAUTO_UPDATE_STRING, "Ready...");  
					break;
			
			
			}
			break;
	}
	return 0;
}




int CVICALLBACK auto_timer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	unsigned char statPSTS, statSTIO, statIRQ  ; //hold status logic levels      
	double counts[256];
	double time;
	
	switch (event) 
	{
		case EVENT_TIMER_TICK:
			// with every timer click, update the status bits and act on their values

			
			// read the status lines.... 
			det_check_status_lines( &statSTIO, &statIRQ, &statPSTS );
			//	...and display results
			SetCtrlVal ( pDetAuto, pAUTO_STIO_LED,	statSTIO);	   // TRUE = accumulating
			SetCtrlVal ( pDetAuto, pAUTO_IRQ_LED,	statIRQ	); 	   // TRUE = done accumulating
			SetCtrlVal ( pDetAuto, pAUTO_PSTS_LED,	statPSTS); 	   // TRUE = no power to buffer memory
			
			// if PSTS is FALSE, we have power to buffer memory, if not, then cancel and warn user...
			if ( statPSTS != FALSE ) 
			{
				// disable the timer and end auto accum...
				SetCtrlAttribute (pDetAuto, pAUTO_TIMER, ATTR_ENABLED, FALSE);       
				
				// send a stop command... 
				det_send_stop_command( );
				det_strobe_PCTL();      
				
				// dim the HALT button
				SetCtrlAttribute (pDetAuto, pAUTO_HALT  , ATTR_DIMMED, TRUE );       
				// undim the BEGIN button
				SetCtrlAttribute (pDetAuto, pAUTO_BEGIN , ATTR_DIMMED, FALSE); 
				
				SetCtrlVal (pDetAuto, pAUTO_UPDATE_STRING, "PSTS status indicates no power to the buffer memory. Scans halted.");       
				return -1;	
				
			}
			
			
			// if STIO is on, we are still accumulating... nothing to do here... :
			//if( statSTIO == TRUE & statIRQ == FALSE)
			
			
			// if timer was enabled under correct conditions ( after a clear buffer call) then IRQ
			//     line should only be on after a timer accum has completed and data is available...
			//     use this to determine when a new accum is started, buffers swapped, and data read.
			if ( statIRQ != FALSE & statSTIO == FALSE ) 
			{         
			
				
				// read timer request value... could have been changed...
				GetCtrlVal (pDetAuto, pAUTO_INTERVAL, &time);
				
				// send time commmand
				//det_send_timer_command( time );
				//det_strobe_PCTL();      
				
				// send start command, which swaps memory buffers...  
				det_send_start_command();
				det_strobe_PCTL();
				
				// read available memory buffer... will automatically display an graph if 
				//    the monitor is open...
				SetCtrlVal (pDetAuto, pAUTO_UPDATE_STRING, "Reading buffer...");  
				det_read_buffer( &counts[0] );
				SetCtrlVal (pDetAuto, pAUTO_UPDATE_STRING, "Buffer read. Accumulating again...");            
				
			}
		
			break;
	}
	return 0;
}

int CVICALLBACK detmonitor_control (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int mode;
	double min,max ;
	
	switch (event) {
		case EVENT_COMMIT:

			switch ( control ){
				case pMONITOR_FIXSCALE:
					SetAxisScalingMode (panel, pMONITOR_GRAPH, VAL_LEFT_YAXIS, VAL_LOCK, 0.0, 0.0);
					SetCtrlAttribute ( panel, pMONITOR_UP, ATTR_DIMMED, 0);
					SetCtrlAttribute ( panel, pMONITOR_DOWN, ATTR_DIMMED, 0);
					SetCtrlVal ( panel, pMONITOR_FIXSCALE, 1 );
					SetCtrlVal ( panel, pMONITOR_AUTOSCALE, 0 );
					break;
					
				case pMONITOR_AUTOSCALE:
					SetAxisScalingMode (panel, pMONITOR_GRAPH, VAL_LEFT_YAXIS,  VAL_AUTOSCALE, 0.0, 0.0);
					SetCtrlAttribute ( panel, pMONITOR_UP, ATTR_DIMMED, 1);
					SetCtrlAttribute ( panel, pMONITOR_DOWN, ATTR_DIMMED, 1);
					SetCtrlVal ( panel, pMONITOR_FIXSCALE, 0);
					SetCtrlVal ( panel, pMONITOR_AUTOSCALE, 1 );
					break;
					
				case pMONITOR_UP:
					GetAxisScalingMode ( panel, pMONITOR_GRAPH, VAL_LEFT_YAXIS, &mode, &min, &max);
					SetAxisScalingMode (panel, pMONITOR_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, 0.0, max*2);      
					break;
					
				case pMONITOR_DOWN:
					GetAxisScalingMode ( panel, pMONITOR_GRAPH, VAL_LEFT_YAXIS, &mode, &min, &max);
					SetAxisScalingMode (panel, pMONITOR_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, 0.0, max/2);   
					break;

			}
			break;
	}
	return 0;
}



int det_load_sensitivity( void )
{
	int i;
	FILE *file; 
	long filePos;
	char tempString[256];   // 256 is more than enough room for this...
	int junk;
	int error=0;
	
	
	
	
	for( i=0; i<N_O_DETECTOR_CHANNELS ; i++ )
		Sensitivity[i] = 1.0;
	
	file = fopen ("c:\\alexesca\\inifiles\\detect.ini", "r");	/* open file in read only mode	*/      	 
	if( file == NULL && errno != 0 ){
		printf( "error opening file to read region data \n");
		return -1;
	}
	
	SetWaitCursor (1);	                 
	// find the correct position in file...
	error = ini_find_section( file, " Sensitivity " , '[', ']', ';' , &filePos );        
	
	for( i = 0 ; i < N_O_DETECTOR_CHANNELS ; i++ )
		fscanf (file, "%i%Lf", &junk, &Sensitivity[i] );   
		
	error = fclose (file);
	if( error == EOF &&  errno < 0)
		printf( "  error closing file \n");
	SetWaitCursor (0);    

	return 0;
}



int det_save_sensitivity( void )
{
	int i, error=0;
	FILE *file; 
	
	
	SetWaitCursor (1);	
	
	for( i=0; i<N_O_DETECTOR_CHANNELS ; i++ )
		Sensitivity[i] = 1.0;
	
	file = fopen ("c:\\alexesca\\inifiles\\detect.ini", "w");	/* open file in read only mode	*/      	 
	if( file == NULL && errno < 0 ){
		printf( "error opening file to read region data \n");
		return -1;
	}
	
	fprintf( file, "\n" );
	fprintf( file, "[ Sensitivity ]\n" );
	
	for( i = 0 ; i < N_O_DETECTOR_CHANNELS ; i++ )
		fprintf (file, "%i    %Lf\n", i, Sensitivity[i] );   
		
	error = fclose (file);
	if( error == EOF &&  errno < 0)
		printf( "  error closing file \n");
	SetWaitCursor (0);    

	return 0;
}


void det_sensitivity_correction( double *channels )
{
	int i;
	
	for( i = 0 ; i < N_O_DETECTOR_CHANNELS ; i++ )
		channels[i] =  channels[i] * Sensitivity[i];
		
}

void det_sensitivity_default( void )
{
 	int i;
	
	for( i = 0 ; i < N_O_DETECTOR_CHANNELS ; i++ )
		Sensitivity[i] = 1.0;
}




int detsensitivity_utility_go( void )
{
	
	if( DetSensitivityPanelOpen == FALSE)
	{
		// load the panel
		if ( ( pDetSensitivity = LoadPanel (0, "detector.uir", pDETSENS) ) < 0 )
			return -1;
			
		//load sensitivity array onto graph:
		DeleteGraphPlot (pDetSensitivity, pDETSENS_GRAPH, -1, VAL_DELAYED_DRAW);
		PlotY (pDetSensitivity, pDETSENS_GRAPH, 
					Sensitivity,N_O_DETECTOR_CHANNELS,
					VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE,
					VAL_SOLID,1, VAL_RED);

		
		// display the panel
		DisplayPanel( pDetSensitivity );
		DetSensitivityPanelOpen = TRUE ;     
	}
	// ..else just bring to foreground...
	else
	   SetActivePanel (pDetSensitivity);
	return 0;
}



int CVICALLBACK sensitivity_controls (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if( event == EVENT_COMMIT )
	{
		switch( control )
		{
			case pDETSENS_BEGIN:
				break;
			
			case pDETSENS_HALT:
			
				break;
				
			case pDETSENS_EXIT:
				DiscardPanel( pDetSensitivity );
				DetSensitivityPanelOpen = FALSE ;     
				break;
				
			case pDETSENS_RESET:
				if( ConfirmPopup ("Sensitivty Calibration Will Be Lost!",
              "This will set all array elelents to 1.0.  Do you wish to proceed?") )
            {  
            	det_sensitivity_default(); 
            	det_save_sensitivity();
			
				}
				break;
		
		
		
		}
	}
	return 0;
}
