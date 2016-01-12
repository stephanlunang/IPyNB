// header file for power.c 			written by alex kay for fadley group, Fall 1997
#define HP_SUPPLY_SETTLE_TIME 1			// time for power supply to settle in milliseconds
#define MAX_HP_SUPPLY_VOLTAGE 99.99
#define MIN_HP_SUPPLY_VOLTAGE -99.99

//function prototypes
int power_set_voltage( double );    // zeros the power supply
int power_utility_go( void );   // starts up the power supply utility
int power_utility_stop(void);	// closes the power utility

int power_set_voltage( double);
int power_get_bcd( double );
int power_send_commands( void );  	//  sends power request followed by toggle


double power_KE_to_PS( double ); 					//takes a kinetic energy and computes desired HP 6131B voltage
double power_PS_to_KE( double );					//takes a power supply volatge and finds cooresponding Kinetic energy
double power_BE_to_PS( double );					// takes a binding energy and finds cooresponding power supply voltage 
double power_round_supply_request(double);  		// accepts a value and forces it to be a valid supply voltage
int power_update_display(double , double , double); // updates the display with passed values 

int power_close_up_shop( void );
int power_help( void ) ;    
