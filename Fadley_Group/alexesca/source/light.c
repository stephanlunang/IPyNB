
/*================================================================*/
/*																						*/
/* FILE:  			light.c 														*/
/* 																					*/
/*																						*/
/* PURPOSE: 		Contains functions pertaining to the excitation	*/
/*						energy setting utility used in the UCDavis		*/
/*						ESCAWEST system											*/
/*																						*/
/* USES: 			*see includes*												*/
/*																						*/
/*	COMPILER:		LabWindows 4.0.1 has been used						*/
/*																						*/
/* WRITTEN BY:  	Alex Kay, Summer/Fall 1996								*/
/* 																					*/
/* PROPERTY OF:	C.S.Fadley Group											*/
/*						Material Science Division 								*/
/*						LBNL, Berkeley CA											*/
/*																						*/
/* NOTICE:			Do not distibute executables or 						*/
/*						source code without permission. Contact		 	*/
/*						Alex Kay or current Fadley Group 					*/
/*						software maintainer for questions or 				*/
/*						problems.													*/
/*																						*/
/*	REVISIONS:    (none)															*/
/*																						*/
/*================================================================*/


#include "define.h"
#include "globals.h"

#include "light.h"
#include "lighth.h"

#include <userint.h> 

#include "escah.h" //enables update of front end panel


//static int cExcitationEnergy = 0.0;
//static int light_status_ok = FALSE;

static int pExcite;



//int get_excitation_energy( double *Excitation )
//{
//	*Excitation = cExcitationEnergy;
//	return  light_status_ok;
//}



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
		if ( (pExcite = LoadPanel (0, "light.uir", pEXCITE)) < 0 )
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


