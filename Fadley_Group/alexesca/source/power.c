#include <easyio.h>
// #include <dataacq.h> //AWK2011sep09
// #include <easyio.h>  //AWK2011sep09
//#define NT_DRIVER   //AWK2011sep09  attempting to get cbw.h inclusion of windows.h to work
//#define _MSC_VER 400
//#define RC_INVOKED
//#define _WIN32
#define WIN32_LEAN_AND_MEAN



#include <utility.h>
#include <ansi_c.h>
#include "define.h"
#include "globals.h"

#include "power.h"	

#include "powerh.h"
#include "escah.h"


#include "cbw.h"    	//AWK2011sep09: new CIODIO48 interface library (bypassing NIDAQ)      

#define DIOBOARDNUMBER	0   //AWK as configured in teh Instacal software


int PowerPanelOpen=0;                      
int PowerTesting=0;        

//panel handle
static int pPower ;

static double cVoltage; //holds current voltage (last requested voltage)
static double cKinetic; //holds current analyzer KE (last requested)

	
static unsigned char  BCDdigits[4];		//for sending to the HP power supply
static unsigned char power_request_sign;				// for the sign of the voltage... needed? 
static unsigned char port3data, port4data, port5data;
static unsigned char port5Tdata;  //port 5 with command toggle


static double ReferenceEnergy =   1384.035; // value used in origianl tests 1365.7;  	// reference energy for the HP ESCA supply  1372.6??       

//=============================================================================
//prototypes for functions to be available to only this file
int power_get_bcd( double );
int power_send_commands( void );  	//  sends power request followed by toggle
double power_round_supply_request(double);  		// accepts a value and forces it to be a valid supply voltage
int power_update_display(double , double , double); // updates the display with passed values 
int power_help( void ) ;      
//=============================================================================



void get_current_power( double *Voltage, double *KE )
{
	*Voltage = cVoltage;
	*KE 		= cKinetic;
}


	


int CVICALLBACK power_panel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event) {
		
		case EVENT_CLOSE:
			power_utility_stop();         
			break;
			
		case EVENT_RIGHT_CLICK:
			power_set_supply (panel, NULL, EVENT_COMMIT, NULL,NULL,NULL);
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
		
		power_update_display( cVoltage,  cKinetic, EscaWest.ExcitationEnergy- cKinetic );                
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
	SetCtrlVal (pPower, pPOWER_PORT5T ,  port5Tdata);  
	
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
	
	//set the global variables
	cVoltage =  voltage;
	cKinetic =  power_PS_to_KE( voltage );          
	
	return 0;
}



//  use the passed voltage to set the GLOBALS  BCDdigits[i] and port#data
// .. assumed value is valid... within acceptable limits and number of significant digits
int power_get_bcd( double voltage )
{
	double truncated;
	
	unsigned int hund_volt;	// 100 times the voltage
	int i;
	unsigned char toggle = 128; 
	
	
	// get the sign ...
	if ( voltage < 0.0 ) {
		power_request_sign = 1 ; 				// power_request_sign = 1 if voltage is negative
		voltage = - voltage;
	}
	else power_request_sign = 0;				// power_request_sign = 0 if voltage is positive
	
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
	port5data = power_request_sign * 64;
	
	port5Tdata = port5data ^ toggle ;    

	return 0;

}


// sends the BCD data and power_request_sign bit and toggles the 
//	request into the HP 6131C Power Supply.	
// Currently, this function looks at globals to determine
//  the value to set the supplies to... should be passed the 
//  info to cut down on need for globals
int power_send_commands( void )
{
	static short firstpass=1;
	int error;
	short board;
	unsigned char toggle = 128;  // 2^7 = 128 = bit 7  (of 0 to 7)       
	
	unsigned long ports_data, ports_data_toggle;
	
	int port; 
	int port3,port4,port5;
	
	int method =1;	  //1 or 2
	

	
	//ports_data = port3data + 256* port4data + 65536* port5data;
	//ports_data_toggle  = (unsigned long)( port3data ) + 256* (unsigned long) (port4data) + 65536* (unsigned long) ( port5data ^ toggle );  
	
	if ( PowerTesting )
	{
		printf( "SEND_BCD command would be sent to HP 6131 now if not in test mode.\n");
	}
	else 
	{
		char boardName[1024];
		
		char errorString[ERRSTRLEN+1];
		
		if(firstpass){
		
			printf("\n==============================================================\n");    
			
			///cbErrHandling (PRINTALL,DONTSTOP);
			
			//cbGetConfigString (BOARDINFO, DIOBOARDNUMBER, BOARDINFO, 
            //                  DIOBOARDNUMBER, char* ConfigVal, int* maxConfigLen);
			
		    error= cbGetBoardName (DIOBOARDNUMBER, boardName) ;
			cbGetErrMsg(error,errorString );
			if( error != 0 ) 
				printf( "cbGetBoardName ( ) error= %i = %s.\n", error, errorString ); 
			printf( "cbGetBoardName result= %s \n", boardName );  
			
			printf("\n==============================================================\n");        
			firstpass=0;
		}
		
		
		
		
		// on the first write, configure the port to make sure it is set to write
		//NIDAQ:
		//error = WriteToDigitalPort (1, "3", 24, 1, ports_data);
		
		
		
		//NOTE TO SELF is the  cbOutByte() fcn needed here for some reason??
		
		//MEASCOMP: 
		printf("\n---------------------------------------\n");   
		
		if(method==1){
		
			//ports available:
			//    FIRSTPORTA, FIRSTPORTB, FIRSTPORTCL, FIRSTPORTCH
			//	  SECONDPORTA, SECONDPORTB, SECONDPORTCL, SECONDPORTCH   
			// "C" ports are 4bit... HIGH and LOW
			//  A and B are normally addressed 8-bit ports.
			//
			//  NIDAQ ports 3,4,5 coorespond to 2A, 2B, and 2C (High portion needed).
			
			port= 10;//SECONDPORTA;
			error= cbDConfigPort (DIOBOARDNUMBER, port, DIGITALOUT);
			cbGetErrMsg(error,errorString );
			printf( "cbDConfigPort ( ) error= %i = %s... port 2A\n", error, errorString );     
		
			error= cbDOut(DIOBOARDNUMBER, port, port3data);
			printf( "2A  WRITE: 0x%x\n",  port3data  );       
			cbGetErrMsg(error,errorString );
			printf( "cbDOut ( ) error= %i = %s... port 2A \n", error, errorString );   
		
		
			port= 11; //SECONDPORTB;
			error= cbDConfigPort (DIOBOARDNUMBER, port, DIGITALOUT);
			cbGetErrMsg(error,errorString );
			printf( "cbDConfigPort ( ) error= %i = %s... port 2B\n", error, errorString );     
		
			error= cbDOut(DIOBOARDNUMBER, port, port4data);
			printf( "2B  WRITE: 0x%x\n",  port4data  );   
			cbGetErrMsg(error,errorString );
			printf( "cbDOut ( ) error= %i = %s... port 2B \n", error, errorString );  
		
		
			port= 13; //SECONDPORTCH;  // port C is only addressable as HI and LO (4 bits) for some goofy reason      
			error= cbDConfigPort (DIOBOARDNUMBER, port, DIGITALOUT);
			cbGetErrMsg(error,errorString );
			printf( "cbDConfigPort ( ) error= %i = %s... port 2CH\n", error, errorString );       
		
			error= cbDOut(DIOBOARDNUMBER, port, port5data >> 4);  //need bit shifts due to 4bit port    
			cbGetErrMsg(error,errorString );
			printf( "2CH WRITE: 0x%x\n", ( port5data )>>4 );        
			printf( "cbDOut ( ) error= %i = %s... port 2CH\n", error, errorString );  
		

			Delay (0.01);									    
			//NEED TO TOGGLE GATE BIT TO FORCE INTERFACE TO ACKNOWLEDGE COMMAND
			port= 13;//SECONDPORTCH;  
			error= cbDConfigPort (DIOBOARDNUMBER, port, DIGITALOUT);
			//cbGetErrMsg(error,errorString );
			//printf( "cbDConfigPort ( ) error= %i = %s... port 5\n", error, errorString );      
		
			error= cbDOut(DIOBOARDNUMBER, port, ( port5data ^ toggle) >>4);   //need bit shifts due to 4bit port
			cbGetErrMsg(error,errorString );
			printf( "2CH WRITE: 0x%x\n", ( port5data ^ toggle )>>4 );
			printf( "cbDOut ( ) error= %i = %s... port 2CH\n", error, errorString );  
		
			Delay (0.01);  
		
			//COMPLETE TOGGLE
			port= 17;//SECONDPORTCH
			error= cbDConfigPort (DIOBOARDNUMBER, port, DIGITALOUT);
			//cbGetErrMsg(error,errorString );   
			//printf( "cbDConfigPort ( ) error= %i = %s... port 5\n", error, errorString );     
		
			error= cbDOut(DIOBOARDNUMBER, port, port5data >> 4);//;	//need bit shifts due to 4bit port    
			cbGetErrMsg(error,errorString );   
			printf( "2CH WRITE: 0x%x\n", ( port5data )>>4 );   
			printf( "cbDOut ( ) error= %i = %s... port 2CH\n", error, errorString );  
		}
		else if (method==2){
			printf("\n---------------------------------------\n");
		
			//these functions MAY work; 
			// MeasComp states that they are ONLY for ISA boards, but that
			//   is what we have.
			// Strange that there is no CONFIG needed, but this may simply be embedded.  
			// 
			
			//use OutByte commands... might be alternate method to ConfigPort+Dout
			error= cbOutByte (DIOBOARDNUMBER, 14, port3data); 
			cbGetErrMsg(error,errorString ); 
			printf( "cbOutByte ( ) error= %i = %s... port 3\n", error, errorString ); 
		     
		
			error= cbOutByte (DIOBOARDNUMBER, 15, port4data); 
			cbGetErrMsg(error,errorString );  
			printf( "cbOutByte ( ) error= %i = %s... port 4\n", error, errorString );     
		
			error= cbOutByte (DIOBOARDNUMBER, 16, port5data); 
			cbGetErrMsg(error,errorString ); 
			printf( "cbOutByte ( ) error= %i = %s... port 5\n", error, errorString );     
		
		
			Delay (0.001);   
			error= cbOutByte (DIOBOARDNUMBER, 16, port5data ^ toggle); 
			cbGetErrMsg(error,errorString ); 
			printf( "cbOutByte ( ) error= %i = %s... port 5 TOGGLE\n", error, errorString );     
		
			Delay (0.001);   
			error= cbOutByte (DIOBOARDNUMBER, 16, port5data); 
			cbGetErrMsg(error,errorString ); 
			printf( "cbOutByte ( ) error= %i = %s... port 5\n", error, errorString );     
		}
		else
		{
			printf( "WARNING: No communication method found\n");   	
		}
		printf("\n---------------------------------------\n");   
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


//size=string size NOT including NULL
void get_instrument_string( char *string, int size )
{
	const char label[13] = "HP ESCA West";
	int i;
	
	for( i=0; i<size-1 ; i++ )
		if( i < 13 ) 
			string[i] = label[i];
	
	string[size] = NULL;

}	

