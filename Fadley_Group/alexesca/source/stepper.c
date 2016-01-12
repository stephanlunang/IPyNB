#include <ansi_c.h>
#include <easyio.h>
#include <userint.h>

#include "stepper.h"
#include "define.h"
#include "globals.h"
#include "stepperh.h"


#define STEPPER_WINDOW_MIN 170
#define STEPPER_WINDOW_MAX 290


int StepperPanelOpen=0; 
int StepperTesting=0;



// panel handle
int  pStepper;

EnableRemote = FALSE;
EnablePC	 = FALSE;
PCStepOn = FALSE;




int stepper_utility_go( void )
{
	if( StepperPanelOpen == FALSE )
	{
		
		StepperPanelOpen = TRUE ;
		if (  ( pStepper = LoadPanel (0, "stepper.uir", pSTEPPER) ) < 0 )
			return -1;
		// fix up controls before displaying??
		SetCtrlVal (pStepper, pSTEPPER_ENABLE_REMOTE, EnableRemote);
		SetCtrlVal (pStepper, pSTEPPER_ENABLE_PC	, EnablePC    );
		
		if( EnablePC ) SetPanelAttribute (pStepper, ATTR_HEIGHT, STEPPER_WINDOW_MAX);
		else SetPanelAttribute (pStepper, ATTR_HEIGHT, STEPPER_WINDOW_MIN);

		DisplayPanel( pStepper);
	
	}
	
	else SetActivePanel (pStepper);

	return 0;
}


int stepper_close_up_shop( void )
{


	if( StepperPanelOpen == TRUE ){
		// make sure stepper request is turned off??
	
		// update current position in stepper.ini ??
		StepperPanelOpen = FALSE ;   
		DiscardPanel( pStepper );
	}
	
	return 0;
}


int CVICALLBACK stepper_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event) 
	{	 
		case EVENT_CLOSE:
			stepper_close_up_shop();
			break;
	}
	return 0;
}


int CVICALLBACK stepper_manual_control (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	int direction;
	
	switch (event) {
		case EVENT_COMMIT:

			switch ( control ) 
			{
				 case pSTEPPER_ENABLE_REMOTE:
				 	 
				 	EnableRemote 	= TRUE	;
				 	EnablePC 		= FALSE	;
				 	 
				   SetCtrlVal (pStepper, pSTEPPER_ENABLE_REMOTE, TRUE );
				   SetCtrlVal (pStepper, pSTEPPER_ENABLE_PC,     FALSE);            
				    
				   SetPanelAttribute (pStepper, ATTR_HEIGHT, STEPPER_WINDOW_MIN); 
				    
				 	break;
				 	
				 case pSTEPPER_ENABLE_PC:
				 
				 	EnableRemote 	= FALSE	;
				 	EnablePC 		= TRUE	;     
				 	 
				   SetCtrlVal (pStepper, pSTEPPER_ENABLE_REMOTE, FALSE);
				   SetCtrlVal (pStepper, pSTEPPER_ENABLE_PC,     TRUE ); 
				   
				   SetPanelAttribute (pStepper, ATTR_HEIGHT, STEPPER_WINDOW_MAX);
				
				 	break;
			
				 case pSTEPPER_GO:
				 	GetCtrlVal( pStepper, pSTEPPER_GO, &PCStepOn );
				 	
				   
				 	break;
				 	
				 case pSTEPPER_DIRECTION:
				 	break;
				 	
				 case pSTEPPER_SPEED:
					break;
			
			}
			
			break;
	}
	return 0;
}


// checks the status bits for the stepper motor... returns status to passed address pointers
int stepper_check_status(unsigned char *handle_out, unsigned char *jog_CW, 
							unsigned char *jogCCW, unsigned char *limit1, unsigned char *limit2)
{
	int error;
	
	// use a unsigned char to make the size one byte ( int is 2 bytes )
	unsigned long byte_recieved;
	unsigned char handleMask = 128	; 		//1000 0000
	unsigned char jog_CWMask = 64	; 		//0100 0000
	unsigned char jogCCWMask = 32	; 		//0010 0000
	unsigned char limit1Mask = 16	; 		//0001 0000
	unsigned char limit2Mask = 8	; 		//0000 1000

	// the status bits are on port 1 of the CIO-DIO-48 board
	
	if ( StepperTesting )
	{
		printf( "Stepper Motor Status bits would be read now if not in test mode.\n");
		byte_recieved = 0;
	}
	else 
	{
		error = ReadFromDigitalPort (1, "1", 8, 0, &byte_recieved);
		if( error != 0 ) printf( "ReadFromDigitalPort ( ) error ... port 1(Stepper Status)\n" );     
	}
	
	*handle_out =   (unsigned char) byte_recieved & handleMask  ;
	*jog_CW		= !((unsigned char) byte_recieved & jog_CWMask) ;
	*jogCCW		= !((unsigned char) byte_recieved & jogCCWMask) ;
	*limit1 	= !((unsigned char) byte_recieved & limit1Mask) ;   
	*limit2 	= !((unsigned char) byte_recieved & limit2Mask) ;    	

	return 0;
}

// just make a step in the direction requested 
int stepper_send_request( unsigned char Direction )
{
	int error;
	unsigned char port_data;
	
	unsigned char directionMask = 8 ;  //0000 1000
	unsigned char stepMask		= 4 ;  //0000 0100    
	if ( StepperTesting )
	{
		printf( "Stepper Motor would be sent a motion request now if not in test mode.\n");
	}
	else 
	{
		// toggle step bit on
		port_data = Direction * directionMask + stepMask;
		error = WriteToDigitalPort (1, "2", 8, 0, port_data);
		if( error != 0 ) printf( "ReadFromDigitalPort ( ) error ... port 1(Stepper Status)\n" ); 
		
		// toggle step bit back off
		port_data = Direction * directionMask + 0;
		error = WriteToDigitalPort (1, "2", 8, 0, port_data);
		if( error != 0 ) printf( "ReadFromDigitalPort ( ) error ... port 1(Stepper Status)\n" ); 
	}
	
	return 0;
}

void CVICALLBACK stepper_menu (int menuBar, int menuItem, void *callbackData,
		int panel)
{

	switch( menuItem )
	{
		case mSTEPPER_UTILITY_EXIT:
			stepper_close_up_shop();    
			break;
			
		case mSTEPPER_HELP_PANEL:
			MessagePopup ("OOPS", "Help topics are not yet available");
			break;
		
		case mSTEPPER_HELP_GENERAL:
			MessagePopup ("OOPS", "Help topics are not yet available");
			break;
	
	
	
	
	}


}

int CVICALLBACK stepper_timer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned char handle_out, jog_CW, jogCCW, limit1, limit2;
	int direction;
	
	switch (event) 
	{
		case EVENT_TIMER_TICK:
			
			stepper_check_status(&handle_out, &jog_CW, &jogCCW, &limit1, &limit2);
			
			SetCtrlVal (pStepper, pSTEPPER_HANDLE_LED,  handle_out);
			SetCtrlVal (pStepper, pSTEPPER_JOG_CW_LED,  jog_CW);
			SetCtrlVal (pStepper, pSTEPPER_JOG_CCW_LED, jogCCW);
			SetCtrlVal (pStepper, pSTEPPER_LIMIT_1_LED, limit1);
			SetCtrlVal (pStepper, pSTEPPER_LIMIT_2_LED, limit2);
			
			if( jog_CW & (! handle_out) & EnableRemote ) 
			{
				//send a rotate clockwise command
				//printf(" Send a CW rotation command.\n");
				stepper_send_request(0);  
				
				
				
				
			}
			else if ( jogCCW & (! handle_out) & EnableRemote ) 
			{   
				//send a rotate counter-clockwise command
				//printf(" Send a CCW rotation command. \n");
				stepper_send_request(1);  
			}
			
			else if ( (!handle_out) & EnablePC & PCStepOn )
			{
				GetCtrlVal( pStepper, pSTEPPER_DIRECTION, &direction);
				stepper_send_request( direction );
			}
			
			
			break;
	}
	return 0;
}
