// stepperh.h	written by alex kay for fadley group, winter 1997

// function definition header for stepper.c
int stepper_utility_go( void );           
int stepper_close_up_shop( void );

int stepper_check_status(unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned char *);
							
int stepper_send_request( unsigned char ) ;     
