//function prototypes for esca.c


int auto_shut_down_utility_go( void )  ;	// starts the auto shut down configuration utility
int shut_down_initialize( void );			// initiallzes the auto shut down sequence
int shutdown_enable( int enable );       	// enables/disables auto shut down
int configure_DAQ_board( void );			// attempts to configure the CIO-DIO-96 board
int esca_close_up_shop(void);				// shutdown procedures
int esca_general_help( void );				// calls general help info regarding the program
int esca_power_update(double );				// updates the part of the esca panel regarding the power supply
int esca_excite_update( double );



int start_up_go( void );
int start_up_done( void );

int esca_close_all_utilities( void )  ;
int esca_check_if_utilities_are_open(void);

int esca_dim_menu( int );

int esca_read_ini( void );


void set_front_end_message( char *Msg, int Color ) ;                    
