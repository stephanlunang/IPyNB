#include <dataacq.h>
#include <ansi_c.h>
#include <utility.h>
#include <cvirte.h>		/* Needed if linking in external compiler; harmless otherwise */
#include <userint.h>
#include "esca.h"

#include "define.h"
#include "globals.h"

#include "escah.h"
//#include "analysih.h"
#include "powerh.h"
#include "detectoh.h"
#include "paramh.h"
#include "scanh.h"
#include "stepperh.h"
#include "aperh.h"
#include "alex_ini.h"            

// panel handles
static int  pEsca , pSplash ,pAbout , pExcite, pShutDown;
// menu handles
static int mEsca;


double ShutDownTime;   /* auto shutdown time interval */
int ShutDownEnabled = TRUE;



int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)	/* Needed if linking in external compiler; harmless otherwise */
		return -1;	/* out of memory */

	// load the front end panel
	if ( (pEsca = LoadPanel (0, "esca.uir", pHPESCA) ) < 0)
		return -1;
	mEsca = GetPanelMenuBar (pEsca);
	
	// load the splash panel for start up
	if ( (pSplash = LoadPanel (0, "esca.uir", pSPLASH) ) < 0)
		return -1;
	// display start up panel to make it look nice 
	DisplayPanel (pSplash); 
	
	// initalize boards and read ini files here
	Delay (0.5); 	// want user to at least see the splash panel
	
	
	  
	DetectorTesting = FALSE;  
	DetectorTestSignal = FALSE;  
	PowerTesting = FALSE;  
	ScanTesting = FALSE;
	StepperTesting = FALSE;
	ApertureTesting = FALSE;
	
	/* 
	DetectorTesting = TRUE;  
	DetectorTestSignal = TRUE;     
	PowerTesting = TRUE;     
	ScanTesting = TRUE;     
	StepperTesting = TRUE;     
	ApertureTesting = TRUE;
	*/       
	
	esca_read_ini(); 					// read various parameters in from initialization file(s)
		// ( most importantly, the detector and excitation variables... needed by next fcn call:
//	ExcitationEnergy = 1486.7 ;   // aluminum K-alpha  
//   DetectorWidth = 8.885;			// was 9.24 ; // the energy width across the 256 channels of the detectr      
	
	
	region_ini_read();
	
	det_load_sensitivity();    
	
//	ExcitationEnergy = 1486.7 ;   // aluminum K-alpha  
//   DetectorWidth = 8.885; 			// the energy width across the 256 channels of the detectr      
	
	if ( Testing != TRUE ) configure_DAQ_board();
	
	PowerPanelOpen 		= FALSE;           
	DetManualPanelOpen 	= FALSE;
	DetMonitorPanelOpen = FALSE;
	DetAutoPanelOpen    = FALSE;
	AperturePanelOpen 	= FALSE;                 
	StepperPanelOpen 	= FALSE;                 
	ExcitePanelOpen 	= FALSE;                 
	ShutDownPanelOpen 	= FALSE;                 
	ParametersPanelOpen = FALSE;                 
	RegionPanelOpen		= FALSE;
	StepperPanelOpen 	= FALSE;
	DetSensitivityPanelOpen = FALSE;    
	
	
	shut_down_initialize();	  //initialize the automatic shutdown parameters ... panel must be loaded first   
	
	// zero the power supply voltage
	power_set_voltage( 0.0 );
	esca_power_update( 0.0 );
	
	esca_excite_update( EscaWest.ExcitationEnergy );
	
	DiscardPanel(pSplash); 
	

	start_up_go(); 	  // tell user what to turn on and when
	/*move panel to upper left corner...*/
	SetPanelPos (pEsca, 30, 20);
	DisplayPanel (pEsca);
	
	RunUserInterface ();
	
	
	// once we are in the final exiting procedure... after QuitUserInterface is called
	pSplash = LoadPanel (0, "esca.uir", pSPLASH);	/*display start up panel to make it look nice */ 
	SetCtrlVal (pSplash, pSPLASH_STARTMESSAGE, "                        Adios.");  
	DisplayPanel (pSplash);
	
	// make sure all panels are closed and files are saved
	esca_close_up_shop();
	
	
	
	Delay(1);  /*wait a second so user can see the exiting window*/
	DiscardPanel (pSplash);
	
	return 0;
}


int CVICALLBACK esca_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event) {
		
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}


// close panels and save files 
int esca_close_up_shop( void )
{
	// if any panel is open, call the appropriate function
	if( DetManualPanelOpen	) 	detector_close_up_shop();
	if( DetMonitorPanelOpen	) 	monitor_close_up_shop()	;
	if( RegionPanelOpen 	) 	region_close_up_shop ()	;
	if( ScanPanelOpen 		) 	scan_close_up_shop()	;

	param_close_up_shop()	;
	power_close_up_shop();    // always call this to make sure power supply is left at zero volts
	stepper_close_up_shop();
	detector_close_up_shop();
	aperture_close_up_shop();
	
	// any files to close up?
	
	// get rid of main panel
	DiscardPanel ( pEsca );
	
	return 0;
}




int CVICALLBACK about_panel_done (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			RemovePopup (0);
			break;
	}
	return 0;
}



int esca_power_update( double voltage )
{
	double KE;
	
/*	if (voltage == 0.0 )
			SetCtrlVal (pEsca, pHPESCA_POWER_LED, OFF);  
	else	SetCtrlVal (pEsca, pHPESCA_POWER_LED, ON ); 
*/	
	KE = power_PS_to_KE( voltage );      
	
	sprintf( Message, "POWER SUPPLY:       6163C= %8.2f Volts                (KE=%8.2f eV)  (BE=%8.2f eV)", 
										voltage ,
										KE,
										EscaWest.ExcitationEnergy - KE );
	SetCtrlVal (pEsca, pHPESCA_POWER_MESSAGE, Message);
	
	return 0;
}


// immediate shutdown (zero) of hp power supply
int CVICALLBACK power_quick_shutdown (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			power_set_voltage( 0.0 );
			esca_power_update( 0.0 );
			
			set_front_end_message( "POWER SUPPLY: Zero voltage request issued." , VAL_BLUE ) ;                    
			break;
	}
	return 0;
}



















int auto_shut_down_utility_go( void )
{

		/* this function should accept a value for the automatic shutdown
		   time...   the program will auto shutdown power supplies if 
		   "nothing is happening" for the interval time
		*/
	
	if( ( pShutDown = LoadPanel (0, "esca.uir", pSHUTDOWN) ) < 0 )
		return -1;
	SetCtrlVal (pShutDown, pSHUTDOWN_PERIOD, ShutDownTime);
	
	SetCtrlVal (pShutDown, pSHUTDOWN_ENABLE, ShutDownEnabled);
	SetCtrlVal (pShutDown, pSHUTDOWN_DISABLE, !(ShutDownEnabled) );        
	SetActiveCtrl (pShutDown,pSHUTDOWN_CANCEL );
	InstallPopup (pShutDown);
	
	return 0;
}


int shut_down_initialize( void )
{
	ShutDownTime = 1000.0 ; 		//1000 seconds
	ShutDownEnabled = TRUE;		// timer is on 
	SetCtrlAttribute (pEsca, pHPESCA_TIMER, ATTR_ENABLED, ShutDownEnabled);      
	SetCtrlAttribute (pEsca, pHPESCA_TIMER, ATTR_INTERVAL, ShutDownTime);   
	return 0;
}


int shutdown_enable( int enable )
{

	if ( enable == FALSE )
	{
		ShutDownEnabled = FALSE;	// disable the timer...
		SetCtrlAttribute (pEsca, pHPESCA_TIMER, ATTR_ENABLED, ShutDownEnabled);      
		return 0;  
	}
	
	
	else if ( enable == TRUE ) 
	{
		ShutDownEnabled = TRUE;		// enable the timer ... 
		SetCtrlAttribute (pEsca, pHPESCA_TIMER, ATTR_ENABLED, ShutDownEnabled);      
		return 0;  
	}


	return -1 ;  //error if one of the above cases is not true
	
}





int CVICALLBACK shutdown (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double tempPeriod;
	
	if( event == EVENT_COMMIT )
	{
		switch( control )
		{
			case pSHUTDOWN_OK:
				/* is the shutdown timer enabled? */
				GetCtrlVal (panel, pSHUTDOWN_ENABLE, &ShutDownEnabled);
				/* if it is, then enable the timer */
				SetCtrlAttribute (pEsca, pHPESCA_TIMER, ATTR_ENABLED, ShutDownEnabled);
			
			
				/* get the requested timer interval */
				GetCtrlVal (panel, pSHUTDOWN_PERIOD, &ShutDownTime);
				/* ... and set the timer to this value */
				SetCtrlAttribute (pEsca, pHPESCA_TIMER, ATTR_INTERVAL, ShutDownTime);
				/* note, if timer has been disabled, the timer still "ticks"
			      but callback function is not performed */
				RemovePopup (0);
				
				break;
			case pSHUTDOWN_CANCEL:
				RemovePopup (0);      
				break;
				
			case pSHUTDOWN_ENABLE:
				ShutDownEnabled = TRUE;
				SetCtrlVal (pShutDown, pSHUTDOWN_ENABLE, ShutDownEnabled);
				SetCtrlVal (pShutDown, pSHUTDOWN_DISABLE, !(ShutDownEnabled) );        
				break;
				
			case pSHUTDOWN_DISABLE:
				ShutDownEnabled = FALSE;
				SetCtrlVal (pShutDown, pSHUTDOWN_ENABLE, ShutDownEnabled);
				SetCtrlVal (pShutDown, pSHUTDOWN_DISABLE, !(ShutDownEnabled) );        
				break;
				
			case pSHUTDOWN_PERIOD:
				GetCtrlVal( pShutDown, pSHUTDOWN_PERIOD, &tempPeriod );
				//if too short, can cause problems...
				if( tempPeriod < 10.0 ) tempPeriod = 10.0;
				SetCtrlVal( pShutDown, pSHUTDOWN_PERIOD, tempPeriod ); 
			
				break;
		
		 }
	}
	return 0;
}




int CVICALLBACK timer_shut_down (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	
	switch (event) 
	{
		case EVENT_TIMER_TICK:		/*  The timer went off so...  */
			
			power_set_voltage( 0.0 );
			esca_power_update( 0.0 );
			
			sprintf( Message, "AUTO SHUTDOWN ACTIVATED:  Program was idle. Power supply has been set to zero." );
			
			
			Beep();
			Beep();
			
			set_front_end_message( Message, VAL_RED ) ;                    
			
			break;
	}
	return 0;
}











// configures the CIO-DIO-48 board... used National Instuments library for their
//         ...  there does not seem to be a problem... ComputerBoards Inc. seems to 
//				have designed the board as a National Instrument knock-off
int configure_DAQ_board( void )
{

	// configure ports 3,4,and 5 for output to the HP power supply ( port 5 also has aperture bits )
	/* if( ( DIG_Prt_Config (1, 3, 0, 1) ) != 0 )
		printf(" Error configuring port 3 of the DIO-48\n");
	
	if( ( DIG_Prt_Config (1, 4, 0, 1) ) != 0 )
		printf(" Error configuring port 4 of the DIO-48\n");
	
	if( ( DIG_Prt_Config (1, 5, 0, 1) ) != 0 )
		printf(" Error configuring port 5 of the DIO-48\n");   */

	// configure port 1 (input) and 2 (output) for the stepper
	
	/* if( ( DIG_Prt_Config (1, 1, 0, 0) ) != 0 )
		printf(" Error configuring port 1 of the DIO-48\n");
		
	if( ( DIG_Prt_Config (1, 2, 0, 1) ) != 0 )
		printf(" Error configuring port 2 of the DIO-48\n");	*/
	
	return 0;
	
}



int esca_general_help( )
{
	// *should* call the windows help program with proper help file
	MessagePopup ("Oops...",
              "Help files are not ready... ask Alex for assistance.");


	return 0;
}


// callback for all esca panel menu items
void CVICALLBACK esca_menu (int menuBar, int menuItem, void *callbackData,
		int panel)
{  
	 switch( menuItem )
	 {
	 	//case mHPESCA_FILE_VIEWPREV:
	 		//analysis_utility_go();   
	 	//	break;
		case mHPESCA_FILE_EXIT:
			QuitUserInterface (0);      
	 		break;
	 	case mHPESCA_UTILITIES_POWER:
	 		power_utility_go();
	 		break;
	 	case mHPESCA_UTILITIES_DET_MANUAL:
	 		detector_utility_go();   
	 		break;
	 	case mHPESCA_UTILITIES_DET_AUTO:
	 		detector_auto_go();   
	 		break;	
	 	case mHPESCA_UTILITIES_DET_MONITOR:
	 		detector_monitor_go();  
	 		break;
	 	case mHPESCA_UTILITIES_DET_SENS:
	 		detsensitivity_utility_go();  
	 		break;
	 	case mHPESCA_UTILITIES_EXCITATION:
	 		excite_utility_go(); 
	 		break;
	 	case mHPESCA_UTILITIES_APERTURE:
	 		aperture_utility_go();   
	 		break;
	 	case mHPESCA_UTILITIES_STEPPER:
	 		stepper_utility_go();
	 		break;
	    case mHPESCA_UTILITIES_AUTOSHUTDOWN:
	    	auto_shut_down_utility_go();	  
	 		break;
	 	case mHPESCA_EXPERIMENT_PARAM:
	 		parameters_go(); 
	 		break;
	 	case mHPESCA_EXPERIMENT_REGIONS:
	 		region_define_go(); 
	 		break;
	 	case mHPESCA_HELP_GENERAL:
	 		esca_general_help();
	 		break;
	 	case mHPESCA_HELP_ABOUT:
	 		pAbout = LoadPanel (0, "esca.uir", pABOUT);   
	 		InstallPopup (pAbout);
	 		break;
	 	default:
	 		printf( " Error in esca_menu(): case = %i not found \n", menuItem);
	 		break;
	 }



}



int esca_excite_update( double energy)
{
	
	sprintf( Message, "Excitation Energy: %8.2f eV", energy );
	
	SetCtrlVal( pEsca, pHPESCA_EXCITATION_MESSAGE, Message );
	return 0;
}






/*===============================================================================*/
/*=======================excitation energy utility begin ========================*/    
int CVICALLBACK excite_command (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	

	
	if( event == EVENT_COMMIT )	
	{
		switch( control )
		{
		
			case pEXCITE_OK:
				GetCtrlVal (pExcite, pEXCITE_ENERGY, &EscaWest.ExcitationEnergy);    
			
				//close the utility
				excite_utility_done();
				//update excitation energy on the front end panel...
				esca_excite_update(  EscaWest.ExcitationEnergy );  
				
				break;
				
			case pEXCITE_CANCEL:
			
				//close the utility w/o update of global excitation value
				excite_utility_done();
		
				//update excitation energy on the front end panel...doesn't need it but doesn't hurt.
				esca_excite_update(  EscaWest.ExcitationEnergy );    
				break;
		}

	}
	return 0;
}



int excite_utility_go ( void )
{
	
	if( ExcitePanelOpen == FALSE )
	{
		if ( (pExcite = LoadPanel (0, "esca.uir", pEXCITE)) < 0 )
			return -1;
		ExcitePanelOpen = TRUE;
		//set numeric to current excitaiton energy...
		SetCtrlVal (pExcite, pEXCITE_ENERGY, EscaWest.ExcitationEnergy);     
		InstallPopup (pExcite);
	}

	return 0;
}



int excite_utility_done( void )
{
	ExcitePanelOpen = FALSE;
	RemovePopup (0);
	DiscardPanel( pExcite );
	return 0;
}

/*=======================excitation energy utility end===========================*/
/*===============================================================================*/   






static int  pStartUp;
static int  StartUpPanelOpen = FALSE;


int start_up_go( void )
{
	
	if( StartUpPanelOpen == FALSE )
	{
		//load panel if not yet shown        
		if (  ( pStartUp = LoadPanel (0, "esca.uir", pSTARTUP) ) < 0 )
			return -1;     //load panel failed
		
		StartUpPanelOpen = TRUE;
		
		SetCtrlVal (pStartUp, pSTARTUP_CHECK_1, FALSE );
		SetCtrlVal (pStartUp, pSTARTUP_CHECK_2, FALSE );      
		SetCtrlVal (pStartUp, pSTARTUP_CHECK_3, FALSE );      
		
		// dim all except first message/check box
		SetCtrlAttribute (pStartUp, pSTARTUP_CHECK_2  , ATTR_DIMMED, TRUE);
		SetCtrlAttribute (pStartUp, pSTARTUP_MSG_2    , ATTR_DIMMED, TRUE);  
		SetCtrlAttribute (pStartUp, pSTARTUP_CHECK_3  , ATTR_DIMMED, TRUE);  
		SetCtrlAttribute (pStartUp, pSTARTUP_MSG_3    , ATTR_DIMMED, TRUE);  
	
		// show panel as a popup
		InstallPopup (pStartUp);
		return 0;
	}
	
	return 1;
	
}



int start_up_done ( void )
{
	if( StartUpPanelOpen == TRUE )
	{
		StartUpPanelOpen = FALSE ;
		// discard popup panel
		RemovePopup (0);
		return 0;
	}
	
	return 1;
}





int CVICALLBACK start_up (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			switch ( control )
			{
				case pSTARTUP_CHECK_1:
					SetCtrlAttribute (pStartUp, pSTARTUP_CHECK_1  , ATTR_DIMMED, TRUE );
					SetCtrlAttribute (pStartUp, pSTARTUP_MSG_1    , ATTR_DIMMED, TRUE );  
					// undim the next message and check box
					SetCtrlAttribute (pStartUp, pSTARTUP_CHECK_2  , ATTR_DIMMED, FALSE);
					SetCtrlAttribute (pStartUp, pSTARTUP_MSG_2    , ATTR_DIMMED, FALSE); 
					
					// check board is present and buffer memory is powered
					
						//if not, warn user ... abort program ?? 
					break;
					
				case pSTARTUP_CHECK_2:
					SetCtrlAttribute (pStartUp, pSTARTUP_CHECK_2  , ATTR_DIMMED, TRUE );
					SetCtrlAttribute (pStartUp, pSTARTUP_MSG_2    , ATTR_DIMMED, TRUE );  
					// undim the next message and check box
					SetCtrlAttribute (pStartUp, pSTARTUP_CHECK_3  , ATTR_DIMMED, FALSE);
					SetCtrlAttribute (pStartUp, pSTARTUP_MSG_3    , ATTR_DIMMED, FALSE);
					
					// send zero voltage request
				
				
					break;
			
				case pSTARTUP_CHECK_3:
				
					SetCtrlAttribute (pStartUp, pSTARTUP_CHECK_2  , ATTR_DIMMED, TRUE );
					SetCtrlAttribute (pStartUp, pSTARTUP_MSG_2    , ATTR_DIMMED, TRUE ); 
					// close panel and proceed
					start_up_done();     
					
				
					break;
			
			
			}




			break;
	}
	return 0;
}

int CVICALLBACK startup_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			// if use closed panel, he must want to abort the program
			start_up_done();
			QuitUserInterface (0);     
			break;
	}
	return 0;
}





int esca_dim_menu( int Dim)
{
	//if Dim = 1 = TRUE, we dim the whole menu
	//if Dim = 0 = FALSE, we undim the whole menu
	SetMenuBarAttribute (mEsca, mHPESCA_FILE, ATTR_DIMMED, Dim);
	SetMenuBarAttribute (mEsca, mHPESCA_UTILITIES, ATTR_DIMMED, Dim);    
	SetMenuBarAttribute (mEsca, mHPESCA_EXPERIMENT, ATTR_DIMMED, Dim);    
	SetMenuBarAttribute (mEsca, mHPESCA_HELP, ATTR_DIMMED, Dim);    
	return 0;
}





int esca_close_all_utilities( void )
{

	// check the flag to see if each utility is open...
	//    ...if it is then close it...
	aperture_close_up_shop();			//the aperature utility
	detector_close_up_shop();			// the detector manual control utility
	detector_auto_close_up_shop();   // the detector auto accumulate utility
	//monitor_close_up_shop();			// the detecotr monitor... is allowed open always so don't close it
	power_close_up_shop();    			// always call this to make sure power supply is left at zero volts
	stepper_close_up_shop();			// the stepper motor utility
	return 0;
}


int esca_check_if_utilities_are_open(void)
{
	// just check the flags... if any are 1 (TRUE)
	//   then return 1 (TRUE)
	//     otherwise return 0 (FALSE)
	
	if( PowerPanelOpen)	 		 	return 1;               
 	else if(DetManualPanelOpen)  	return 1;    
 	//else if(DetMonitorPanelOpen)  	return 1;     
 	else if(DetAutoPanelOpen)  	return 1;    
 	else if(AperturePanelOpen)  	return 1;    
 	else if(StepperPanelOpen)  	return 1;           
   else if(ExcitePanelOpen) 		return 1;          
   else if(ShutDownPanelOpen) 	return 1;                 
 	//else if(ParametersPanelOpen)  	return 1;             
 	//else if(RegionPanelOpen)  		return 1;              
 	else if( StepperPanelOpen) 	return 1;
 	//else if(AnalysisPanelOpen)  	return 1;     

	else return 0;
}





int esca_read_ini( void )
{
	int i, ReturnStatus = 0;
	char tempString[256];   // 256 is more than enough room for this...
	unsigned long tempUnsignedLong  = 0;
	double tempDouble = 0;
	int error;

	char directory[MAX_PATHNAME_LEN];
	char *pathname;
	
	FILE *file; 
	long filePos;
	
	// open the choosen file
	SetWaitCursor (1);
	
	// find out what the execuable directory is...
	if(  (error = GetProjectDir (directory)) != 0 ) return -1;;
	// the *.ini files are supposed to be stored here
	pathname = strcat (directory, "\\escawest.ini");
	//printf( "file = %s \n" , pathname );
	
	file = fopen (pathname, "r");	/* open file in read only mode	*/      	 
	
	
	
	// read the spectrometer/power supply parameters
	error = ini_find_section( file, "Spectrometer" , '[', ']', ';' , &filePos );       
		//reference energy
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Reference_Energy_(eV)", &EscaWest.ReferenceEnergy );
		
		//printf( " Ref En = %f \n", EscaWest.ReferenceEnergy);
	
	//error = ini_read_string_after_tag_value( file, filePos, '[', '=', ';' , 
	//							"Experiment Description", ThisExperiment.Description , 256);
	
	
	
	
	// read the detector parameters
	error = ini_find_section( file, "Detector" , '[', ']', ';' , &filePos );       
		// number of channels is 256... hardwired into program for now
		error = ini_read_unsigned_long_after_tag_value( file, filePos, '[', '=', ';' , 
								"Number_Of_Channels", &EscaWest.DetectorNumberOfChannels );  
		
		// detector width... eV width of the "EscaWest.DetectorNumberOfChannels"  channels
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Detector_Width_(eV)", &EscaWest.DetectorWidth );
							
		// these allow us to compute the next variable:
		EscaWest.DetectorLineWidth = EscaWest.DetectorWidth / EscaWest.DetectorNumberOfChannels ;
		
	//	printf( "num ch= %i, det width= %f\n", EscaWest.DetectorNumberOfChannels, EscaWest.DetectorWidth );
	
	
	
	//read in the excitation energy
	error = ini_find_section( file, "Light" , '[', ']', ';' , &filePos );
		// get the excitation energy... single value for the Esca-West
		error = ini_read_double_after_tag_value( file, filePos, '[', '=', ';' , 
							"Excitation_Energy_(eV)", &EscaWest.ExcitationEnergy );
		
	//	printf( " exc en = %f \n", EscaWest.ExcitationEnergy );
	
	// read the directories??
	
	
	
	//close the file
	fclose(file);



	return 0;
}





void set_front_end_message( char *Msg, int Color )
{

	SetCtrlAttribute ( pEsca , pHPESCA_MESSAGE, ATTR_TEXT_COLOR, Color);       
	SetCtrlVal( pEsca , pHPESCA_MESSAGE, Msg );
}


int CVICALLBACK frontend_controls (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if( event ==  EVENT_LEFT_DOUBLE_CLICK )
	{
		switch( control )
		{
			case pHPESCA_POWER_MESSAGE:
				power_utility_go();                
				break;
			case pHPESCA_EXCITATION_MESSAGE:
				excite_utility_go();   
				break;
		 }
	}
	
	
	if( control == pHPESCA_POWER_SHUTDOWN && event == EVENT_COMMIT )
	{
		power_set_voltage( 0.0 );
		esca_power_update( 0.0 );
			
		set_front_end_message( "POWER SUPPLY: Zero voltage request issued." , VAL_BLUE ) ;                    
	}
	
	return 0;
}


