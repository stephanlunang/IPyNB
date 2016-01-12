// aperh.h 				written by Alex Kay for Fadley Group, Winter 1997

// the function prototypes for aper.c


int aperture_utility_go( void )   ;
int aperture_close_up_shop( void )   ;

int aperture_check_status(unsigned char *open, unsigned char *closed )   ;
int aperture_send_command( int AperatureOpen )    ;
