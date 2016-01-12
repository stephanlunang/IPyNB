#include <easyio.h>
#include <ansi_c.h>
// aper.c 					written by Alex Kay for Fadley Group, Winter 1997


#include <userint.h>
#include "aper.h"
#include "define.h"
#include "globals.h"
#include "aperh.h"

// panel handles   
static int pAperture;

int aperture_utility_go( void )
{
	if( AperturePanelOpen == FALSE )
	{
		
		AperturePanelOpen = TRUE ;
		if( ( pAperture = LoadPanel (0, "aper.uir", pAPERTURE) ) < 0 )
			return -1;
		// fix up controls before displaying??
		// set control switch to match current position... if it is unknown (closed = open = 0)
		//  then set to closed and set close command ??
		
		InstallPopup ( pAperture);
	
	}
	
	else SetActivePanel (pAperture);

	return 0;
}


int aperture_close_up_shop( void )
{

	if( AperturePanelOpen == TRUE ){
		// disable the timer??... should be automatic when panel is discarded I think
	
		// update current position in stepper.ini ??
		AperturePanelOpen = FALSE ; 
		RemovePopup (0);
		DiscardPanel( pAperture );
	}
	
	return 0;
}

void CVICALLBACK aperture_menu (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	switch ( menuItem )
	{
		 case mAPERTURE_UTILITY_EXIT:
		    aperture_close_up_shop();
		 	break;
		 case mAPERTURE_HELP_PANEL:
		 
		 	break;
		 	
		 case mAPERTURE_HELP_GENERAL:
		 
		 	break;
	
	
	}

}

// callback for the toggle switch for open/close aperature requests
int CVICALLBACK aperture_control (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event) 
	{
		case EVENT_COMMIT:
		
			MessagePopup ("Why are you so lazy?", 
				"The real toggle switches are up on your right.  Just use those.");

			break;
	}
	return 0;
}

int CVICALLBACK aperture_timer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned char Open, Closed;
	
	switch (event) 
	{
		case EVENT_TIMER_TICK:
			aperture_check_status( &Open, &Closed );
			
			SetCtrlVal (pAperture, pAPERTURE_OPEN_LED,  	Open);
			SetCtrlVal (pAperture, pAPERTURE_CLOSED_LED,    Closed);     
			
			// this **should** make the led light up only if both open and closed are 0
			SetCtrlVal (pAperture, pAPERTURE_CHANGING_LED,  !Open && !Closed  );         
		
			break;
	}
	return 0;
}

int aperture_check_status(unsigned char *open, unsigned char *closed )
{
	int error;
	
	// read the port and use masks to extract the information
	unsigned long byte_recieved;
	
	unsigned char openMask	 = 8 ; 	//0000 1000
	unsigned char closedMask = 4 ; 	//0000 0100 
	
	// the status bits are on port 5 of the CIO-DIO-48 board
	///  note that this port is also used by the 6131C power supply!
	
	if ( ApertureTesting )
	{
		printf( "Aperture Status bits would be read now if not in test mode.\n");
		byte_recieved = 0;
	}
	else 
	{
	
		// better insist on the port being configured for read..
		error = ReadFromDigitalPort (1, "5", 8, 1, &byte_recieved);
		if( error != 0 ) printf( "ReadFromDigitalPort ( ) error ... port 5(Aperture Status)\n" );     
	}
	
	*open 	= (unsigned char) byte_recieved & openMask 		;
	*closed	= (unsigned char) byte_recieved & closedMask 	;
	
	return 0;
}




// this will probably not be used... interferes with the HP power supply because of
// port assignments to the CIO-DIO-48
int aperture_send_command( int AperatureOpen )
{
	int error;
	
	unsigned char OpenCommand   = 16; 		//0001 0000
	unsigned char CloseCommand = 0 ; 		//0000 0000
	unsigned char Command;
	// a potitive TTL on the proper bit opens the aperature... takes a few minutes
	if ( AperatureOpen == TRUE ) Command = OpenCommand ;
	else						 Command = CloseCommand;
	
	
	// the bit is #4 (of 0 to 7) on port 5 of the CIO-DIO-48 board
	if ( ApertureTesting )
	{
		printf( "Aperture Open/Close command would be sent now if not in test mode.\n");
	}
	else 
	{
		// better insist on the port being configured for read..
		error = WriteToDigitalPort (1, "5", 8, 1, (unsigned long) Command);
		if( error != 0 ) printf( "WriteToDigitalPort ( ) error...port 5(Aperture Control)\n" );     
	}
	


	return 0;
}

int CVICALLBACK aperture_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event) 
	{
		
		case EVENT_CLOSE:
			aperture_close_up_shop();
			break;
	}
	return 0;
}
