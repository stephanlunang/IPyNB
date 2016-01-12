// header file for detector.c		written by alex kay for fadley group, fall 1997

#define N_O_DETECTOR_CHANNELS 256      

// detector commands
#define START_COMMAND	1				// 1000 0000 0000 0001 = 2^15 + 2^0
#define STOP_COMMAND	2				// 1000 0000 0000 0000 = 2^15
#define SET_TIME_COMMAND  3

// detector masks
#define IOST_MASK	2					// 0000 0010	 = 2^1
#define IRQ_MASK	4					// 0000 0100	 = 2^2
#define PSTS_MASK	8					// 0000 1000	 = 2^3

#define ACCUM_TIME_MAX  32.767   // maximum accum time for a single timed accumulation
#define ACCUM_TIME_MIN  0.001		// zero is untimed mode!  keeps going until stoped

// logic level definitions for the quantar detector interface
#define STIO_BUSY		1
#define STIO_READY 		0
#define PSTS_AVAIL		0
#define PSTS_NOT_AVAIL 	1
#define IRQ_REQUEST		1
#define IRQ_NO_REQUEST  0

#define STIO_MASK  2
#define IRQ_MASK   4
#define PSTS_MASK  8

// function prototype header file for detector.c
int det_clear_buffers( void );  
int det_read_buffer( double * );   // do I actually want to pass an array of long ints ?? (not short)  

int det_reset_control( void );
int det_check_status_lines( unsigned char *, unsigned char *, unsigned char * );
//int det_send_timer_command( unsigned short int );	 // pass the time in number of miliseconds
int det_send_timer_command( double );	 // pass the time in seconds      
int det_send_start_command(void);
int det_send_stop_command( void );
int det_strobe_PCTL( void);
int det_strobe_IOSTB( void );

int detector_utility_go(void);
int detector_close_up_shop( void );	   // panel, utility shutdown procedure
int detector_help( void );				

int detector_monitor_go( void );             
int monitor_close_up_shop( void );	   // panel, utility shutdown procedure   
int monitor_update_display( double * ) ;          

int detector_auto_go( void );
int detector_auto_close_up_shop( void ); 


void det_sensitivity_correction( double *channels );
int det_save_sensitivity( void );
int det_load_sensitivity( void );
void det_sensitivity_default( void );
int detsensitivity_utility_go( void );          
