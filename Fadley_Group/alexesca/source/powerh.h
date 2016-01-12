// header file for power.c 			written by alex kay for fadley group, Fall 1997
#define MAX_HP_SUPPLY_VOLTAGE 99.99
#define MIN_HP_SUPPLY_VOLTAGE -99.99
#define HP_SUPPLY_SETTLE_TIME 1			// time for power supply to settle in milliseconds



//prototypes of functions to  be available to other files

void get_current_power( double *Voltage, double *KE );    
void get_instrument_string( char *string, int size );

int power_utility_go( void );   		// starts up the power supply utility
int power_utility_stop(void);			// closes the power utility
int power_close_up_shop( void );            

int power_set_voltage( double);

double power_KE_to_PS( double );		// takes a kinetic energy and computes desired HP 6131B voltage
double power_PS_to_KE( double );		// takes a power supply volatge and finds cooresponding Kinetic energy
double power_BE_to_PS( double ); 	// takes a binding energy and finds cooresponding power supply voltage 



