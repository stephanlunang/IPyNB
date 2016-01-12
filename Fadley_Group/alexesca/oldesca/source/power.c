#include <dataacq.h>
#include <easyio.h>
#include <utility.h>
#include <ansi_c.h>
#include "define.h"
#include "globals.h"

#include "power.h"	

#include "powerh.h"
#include "escah.h"

//panel handle
static int pPower ; 

	
unsigned char  BCDdigits[4];		//for sending to the HP power supply
unsigned char sign;				// for the sign of the voltage... needed? 
unsigned char port3data, port4data, port5data;


double ReferenceEnergy =   1384.035; // value used in origianl tests 1365.7;  	// reference energy for the HP ESCA supply  1372.6??       





int CVICALLBACK power_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event) {
	
		
		case EVENT_CLOSE:
			power_utility_stop();         
			break;
	}
	return 0;
}

// strt up the power supply utility
int power_utility_go( void )
{
	// if the panel is currently not open, then open it..
	if (  PowerPanelOpen == FALSE )
	{
		if( (pPower = LoadPanel (0, "power.uir", pPOWER)) < 0 )
			return -1;
		// need to do anything to panel before showing it to user ??
		PowerPanelOpen = TRUE;
		
		// set the supply voltage control limits
		SetCtrlAttribute (pPower, pPOWER_PS, ATTR_MAX_VALUE, MAX_HP_SUPPLY_VOLTAGE);
		SetCtrlAttribute (pPower, pPOWER_PS, ATTR_MIN_VALUE, MIN_HP_SUPPLY_VOLTAGE); 
	
		SetCtrlVal (pPower, pPOWER_UPDATE_STRING, "Ready...");     
		DisplayPanel(pPower);
	}
	// ..else just bring to foreground...
	else
	   SetActivePanel (pPower);
	   
	return 0;
}


// uses user input to adjust other control values 
int CVICALLBACK power_param (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double supplyVoltage;	// requested voltage
	double bindingEnergy;
	double kineticEnergy;
	
	
	switch (event) 
	{
		case EVENT_COMMIT:
			
			switch ( control ) 
			{
			
				case pPOWER_PS:  // user changed the value of the power supply voltage
					
					//get the value the user has set for the supply volatge
					GetCtrlVal (pPower, pPOWER_PS, &supplyVoltage);
					// make sure it is a valid value
					supplyVoltage =  power_round_supply_request( supplyVoltage );   
					// use to compute the kinetic energy
					kineticEnergy =   power_PS_to_KE( supplyVoltage );        
					// and then the binging energy
					bindingEnergy = EscaWest.ExcitationEnergy - kineticEnergy ;  
					
					break;
				
				case pPOWER_KE:  // user changed the requestes kinetic energy setting
					
					// get the value user has set for the KineticE
					GetCtrlVal (pPower, pPOWER_KE, &kineticEnergy); 
					// first compute the cooreponding supply voltage
					supplyVoltage =  power_KE_to_PS ( kineticEnergy );    
					// next round that supply voltage to nearest actual possible voltage
					supplyVoltage =  power_round_supply_request( supplyVoltage );       
					// use this to get an updated KineticE
					kineticEnergy =   power_PS_to_KE( supplyVoltage );     
					// and finally compute the cooreponding BindingE
					bindingEnergy = EscaWest.ExcitationEnergy - kineticEnergy ;
				
					break;
					
				case pPOWER_BE:   // user changed the requested binding energy setting
					
					// get the value the user has set for the BindingE
					GetCtrlVal (pPower, pPOWER_BE, &bindingEnergy); 
					// Convert to KineticE
					kineticEnergy = EscaWest.ExcitationEnergy - bindingEnergy ;
					//  convert KE to supplyV
					supplyVoltage =  power_KE_to_PS ( kineticEnergy );      
					//round to nearest possible
					supplyVoltage =  power_round_supply_request( supplyVoltage ); 
					// use this to get an updated KineticE
					kineticEnergy =   power_PS_to_KE( supplyVoltage );     
					// and finally compute the cooreponding BindingE
					bindingEnergy = EscaWest.ExcitationEnergy - kineticEnergy ;
				
					
					break;
					
				default:
					printf("power_param() could not find proper control case \n");
					break; 
					
			}

	

			// set the cooresponding BCD values
			power_get_bcd( supplyVoltage ) ;  
			// update the controls and indicators
			power_update_display(  supplyVoltage,  kineticEnergy,  bindingEnergy  );

			SetCtrlVal (pPower, pPOWER_UPDATE_STRING, "Press the Set Supply button to send request to 6131C PS" );   
			break;
	}
	return 0;
}

//takes a kinetic energy and computes desired HP 6131B voltage (assumes EscaWest.ExcitationEnergy is correct)
double  power_KE_to_PS( double KineticE )
{
	double PSvoltage;
	
	//binding energy = excitation energy - kinetic energy
	PSvoltage = (0.1) * ( ReferenceEnergy - EscaWest.ExcitationEnergy + KineticE - 1000.0 ) ;
	
	return PSvoltage;
}


// accepts a supply voltage and computes a Kinetic Energy (assumes EscaWest.ExcitationEnergy is correct )
double  power_PS_to_KE( double PSvoltage )
{
	double KineticE;
	
	//binding energy = excitation energy - kinetic energy
	KineticE = 10.0 * PSvoltage + EscaWest.ExcitationEnergy - ReferenceEnergy + 1000.0;
	
	return KineticE;
}


//takes a binding energy and computes desired HP 6131B voltage     
double power_BE_to_PS( double BindingE )
{
	double PSvoltage;
	//binding energy = excitation energy - kinetic energy
	PSvoltage = (0.1) * ( ReferenceEnergy - BindingE - 1000.0 ) ;
	
	
	return PSvoltage;
}
	

// takes a double and forcs it into an acceptable voltge request for the power supply
// ...does not account for not being in x10 mode yet
double power_round_supply_request( double volts )
{
	// if above max or below min, set to max or min, respectivley
	if ( volts >  MAX_HP_SUPPLY_VOLTAGE ) 
		  volts =  MAX_HP_SUPPLY_VOLTAGE;
	
	if ( volts <  MIN_HP_SUPPLY_VOLTAGE ) 
		  volts =  MIN_HP_SUPPLY_VOLTAGE;
	
	// if too many signifcant digits, round to proper value ( 4 signif. digits allowed)
	// mult by 100.0 , round to nearest integer, devide by 100.0          
	volts = (double) ( RoundRealToNearestInteger (100.0 * volts) / 100.0 );   

	return volts;
	
}

// uses the globals BCDdigits to update the display
int power_update_display( double PowerS, double KinE, double BindE )
{

	// update the controls
	SetCtrlVal (pPower, pPOWER_BE, BindE	);   	
	SetCtrlVal (pPower, pPOWER_KE, KinE		);   	
	SetCtrlVal (pPower, pPOWER_PS, PowerS	);   
	
	// update the indicators
	SetCtrlVal (pPower, pPOWER_DIGIT_1, BCDdigits[0]);
	SetCtrlVal (pPower, pPOWER_DIGIT_2, BCDdigits[1]);    
	SetCtrlVal (pPower, pPOWER_DIGIT_3, BCDdigits[2]);    
	SetCtrlVal (pPower, pPOWER_DIGIT_4, BCDdigits[3]);
	SetCtrlVal (pPower, pPOWER_PORT3 ,  port3data);    
	SetCtrlVal (pPower, pPOWER_PORT4 ,  port4data);    
	SetCtrlVal (pPower, pPOWER_PORT5 ,  port5data);    
	
	return 0;
}

// set supply to currrent request
int CVICALLBACK power_set_supply (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double voltage;
	
	switch (event) 
	{
		case EVENT_COMMIT:
		
			// assume the power supply voltage is displaying what user wants
			GetCtrlVal (pPower, pPOWER_PS, &voltage);
			// remember that we only want 4 significant digits :##.## .. discard extra 
			//voltage = (double) ( RoundRealToNearestInteger (100.0 * voltage) / 100.0 );
			SetCtrlVal (pPower, pPOWER_PS, voltage);
			
			power_set_voltage( voltage );
			
			SetCtrlVal (pPower, pPOWER_UPDATE_STRING, "Voltage request has been sent...");   
			break;
	}
	return 0;
}


// close the power supply utility
int CVICALLBACK power_done (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			power_utility_stop();
			break;
	}
	return 0;
}


int power_utility_stop( void )
{
	PowerPanelOpen = FALSE;
	DiscardPanel( pPower); 

	return 0;
}





// sets the HP 6131 power supply to thes passed voltage
//   the DAQ board is assumed to be configured proior to this function call
int power_set_voltage( double voltage)
{
	double time;
	
	power_get_bcd( voltage);	// sets the globals: voltage ---> BCDdata ---> port#data
	esca_power_update( voltage );
	power_send_commands();			// sends the request stored in the above globals via the easy i/o library
	return 0;
}



//  use the passed voltage to set the GLOBALS  BCDdigits[i] and port#data
// .. assumed value is valid... within acceptable limits and number of significant digits
int power_get_bcd( double voltage )
{
	double truncated;
	
	unsigned int hund_volt;	// 100 times the voltage
	int i;
	
	
	// get the sign ...
	if ( voltage < 0.0 ) {
		sign = 1 ; 				// sign = 1 if voltage is negative
		voltage = - voltage;
	}
	else sign = 0;				// sign = 0 if voltage is positive
	
	// convert to BCD (binary coded decimal
	
	// since we can only write 4 ##.## to the power supply, we first round
	//		any extra significant digits
	// do this by rounding 100 times the voltage requested 
	hund_volt = ( short unsigned int) ( RoundRealToNearestInteger (100.0 * voltage) );       
	
	BCDdigits[0] = ( unsigned char) (hund_volt/1000.0 );
	
	hund_volt =  hund_volt - 1000.0 * BCDdigits[0];
	BCDdigits[1] =  ( unsigned char) ( hund_volt/100.0)  ;
	 
	hund_volt =  hund_volt -  100.0 * BCDdigits[1];   
	BCDdigits[2] =   ( unsigned char)( hund_volt/10.0)  ; 
	
	hund_volt =  hund_volt -  10.0* BCDdigits[2];   
	BCDdigits[3] =    ( unsigned char) ( hund_volt/1.0)  ;   
	
	// more globals to be set ... redundant and not really needed
	port3data = 16* BCDdigits[0] + BCDdigits[1];
	port4data = 16* BCDdigits[2] + BCDdigits[3];
	port5data = sign * 64;

	return 0;

}


// sends the BCD data and sign bit and toggles the 
//	request into the HP 6131C Power Supply.	
// Currently, this function looks at globals to determine
//  the value to set the supplies to... should be passed the 
//  info to cut down on need for globals
int power_send_commands( void )
{
	int error;
	short board;
	unsigned char toggle = 128;  // 2^7 = 128 = bit 7  (of 0 to 7)       
	
	unsigned long ports_data, ports_data_toggle;
	
	ports_data = port3data + 256* port4data + 65536* port5data;
	ports_data_toggle  = (unsigned long)( port3data ) + 256* (unsigned long) (port4data) + 65536* (unsigned long) ( port5data ^ toggle );  
	
	if ( PowerTesting )
	{
		printf( "SEND_BCD command would be sent to HP 6131 now if not in test mode.\n");
	}
	else 
	{
	
		// on the first write, configure the port to make sure it is set to write
		error = WriteToDigitalPort (1, "3", 24, 1, ports_data);
		if( error != 0 ) printf( "WriteToDigitalPort ( ) error ... port 3,4,5 \n" );     
		
		Delay (0.001);
		
		error = WriteToDigitalPort (1, "3", 24, 0, ports_data_toggle);
		if( error != 0 ) printf( "WriteToDigitalPort ( ) error ... port 3,4,5 \n" );     
		
		Delay (0.001);  
		
		error = WriteToDigitalPort (1, "3", 24, 0, ports_data);
		if( error != 0 ) printf( "WriteToDigitalPort ( ) error ... port 3,4,5 \n" );     
		
	}
	
	return 0;
}





int power_close_up_shop( void )
{
	// zero the power supply voltage
	power_set_voltage( 0.0 );
	// close and eliminate panel from memory     
	if( PowerPanelOpen )
		DiscardPanel( pPower );
	
	// not necessary during final exciting of program, but can't hurt
	PowerPanelOpen = FALSE;

	return 0;
}


int power_help( void )
{
	MessagePopup ("Oops...",
              "Help files are not ready... ask Alex for assistance.");
	return 0;
}
