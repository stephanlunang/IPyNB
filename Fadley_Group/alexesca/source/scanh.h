// function prototypes for scan.c

int scan_start_experiment(  struct experimentStruct *E )  ;

int scan_fixed(    struct experimentStruct *Expt , int regionIndex , struct data_type *cDataPtr);       
int scan_dithered( struct experimentStruct *Expt , int regionIndex , struct data_type *cDataPtr);                     
//int scan_dithered_rev( struct experimentStruct *Expt , int regionIndex, struct data_type *cDataPtr);       

int scan_pause_go( int );            
int scan_close_up_shop( void );              

//outdated: int scan_save_data_in_file( char pathname[MAX_PATHNAME_LEN],  struct regionStruct Region, int number_of_data_points, double *Energies, double *Counts );
int scan_save_experiment_info_in_file( struct experimentStruct *Expmnt );
int scan_save_curve_in_file( char pathname[MAX_PATHNAME_LEN],  data_type_pointer Curve , int lIndex, int rIndex);

void display_initialize_plot( void );          
int scan_adjust_cursors( double, double, double, double );      
void get_percent_expt_done( struct experimentStruct * );      
void get_expt_total_time( struct experimentStruct *ExPtr );        


void graph_control( int EnergyAxisType, struct data_type Curve ) ;


int layer_manual_wait_go( int cLayer, int totalLayers ) ;        
int region_manual_wait_go( int cRegion, int totalRegions );
int layer_timed_wait_go( int cLayer, int totalLayers, double TimeToWait );
int layer_polar_wait_go( int cLayer, int totalLayers, double trueTheta ) ;    
