// paramh.h					Written by Alex Kay Fall/Winter 1996/97
//
// This is the funciton prototype header for param.c
//





int param_fill_avail_regions( void ) ;
int parameters_go ( void ) ;
int param_close_up_shop( void ) ;
int region_add_to_available ( void );
int region_help(void);
int param_help(void );

int region_read_file(void) ;
int region_write_file(void) ; 

int region_ini_read(void);
int region_ini_write(void);


int region_define_go( void ) ;
int region_close_up_shop ( void ) ;
struct regionStruct 	Initialize_new_region( void ) ;
struct regionStruct 	Read_region_parameters( void ) ;
struct regionStruct 	Adjust_region_parameters(  struct regionStruct rgn ) ;
int Update_region_interface_panel(  struct regionStruct rgn ) ;

int param_get_a_filename( char Pathname[MAX_PATHNAME_LEN] )   ;

struct experimentStruct experiment_calculate_times( struct experimentStruct Expt ); 



int region_build_available_menu( void );         
void region_select_from_menu (int menuBar, int menuItem, void *callbackData, int panel)  ;

int region_check_if_description_is_already_used( char *Description ) ;
int region_replace( int RegionIndex );


void layers_set_default(void);
void update_layer_display( void);             
