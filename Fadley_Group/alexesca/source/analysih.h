
/*
//typedef data_pointer* data_ptr_array;   //pointer to allocatable array of pointers to allocatable array of doubles

struct data_type
{
	double 	excitation_energy;
	double 	time_p_channel;
	int 		size;
	double 	center_energy;    // always binding 
	double 	width;				// always negative
	
	double *data;
};

typedef struct data_type* data_type_pointer ;     

 


// define a region structure
struct regionStruct
{
	
	char 	Description[101];

	double			BEcenter;			// center binding energy of the region
	double 			BEwidth	;			// approx width of region (will use to fix number of channels
	double			BEstepSize;			// 
	
//	double 			ExcitationEnergy ;
	unsigned int 	StepMultiple;	// multiple of the minimum step to use as actual step  

	unsigned int 	NumberOfScans;	   // number of repeated scans of this regions
	unsigned int 	NumberOfChannels;	// number of "bins" accross the energy range... a bin width is the detector channel width * StepMultiple    
	unsigned int   NumberOfDetectorBins ;  // has max value of N_O_DETECTOR_CHANNELS, and is 
														//    just  N_O_DETECTOR_CHANNELS / StepMultiple

	double		 	DwellTime;			// time between energy steps... in integer milliseconds!
	double		   TimePerChannel;
	unsigned int 	ScanMode;			// dithered or fixed accumulation mode	  ]
	
	double 			TotalTime;			// total time region takes to scan (w/o adjusting for overhead... just accumulation time)
	double			PercentDone;
	
	//unsigned long *TotalCounts;    	// used to allocate an array of the proper size for the region
//	double		*TotalCounts;			//need to save counts as double for two reasons: need to multiply fixed
												// mode counts by channel sensitivity factor...get a double 
												// ... also future fitting routines need doubles...
//	double 		  *BindingEnergies;	//   ""
	
};


struct experimentStruct
{
	char Pathname[MAX_PATHNAME_LEN];       
	char Description[101];	   // expt name/description
	int n_o_regions;  			// number of regions in the experiment
	int CurrentRegion;
	double Excitation;		   // excitation energy in eV 
	char *Instrument;		   	// name of the instrument
	//char *Date;				   	// expt date finished
	//char *Time;				  		// expt time finished
	
	double TotalTime;				// number of seconds long the experiment is
	double PercentDone;
	
	char *DateBegan;
	char *DateFinished;
	char *TimeBegan;
	char *TimeFinished;
	
	struct regionStruct *Regions;	 // pointer to an array of region structures
} ;


  
struct PlotStruct
{

	int Handle;         
	int Style;
	int TracePointStyle;
	int LineStyle;
	int Color;
	int BackgroundColor;
	
} ;
*/   

#include "globals.h"

int analysis_utility_go( void ) ;     
int analysis_utility_close_up_shop( void );       
int analysis_select_experiment(  char pathname[MAX_PATHNAME_LEN] );     
int analysis_update_panel( int panel, struct regionStruct Region );       
int analysis_read_exp_param( char pathname[MAX_PATHNAME_LEN]);   
int analysis_build_region_menu( void ) ;
int analysis_build_layer_menu( void );         
void  select_region_from_menu (int menuBar, int menuItem, void *callbackData, int panel) ;
void  select_layer_from_menu (int menuBar, int menuItem, void *callbackData, int panel)  ;
int analysis_load_an_experiment( void ) ;                      




int print_utility_go ( void );
int print_utility_done( void );
int print_send_to_printer( void );  
int print_set_printer_attributes(  void );       



int read_exp_param_from_file(char pathname[MAX_PATHNAME_LEN]); 
int read_region_param_from_file( char pathname[MAX_PATHNAME_LEN], int RgnIdx );        
//int read_region_data_from_file( char pathname[MAX_PATHNAME_LEN], int RgnIdx  );                
int read_region_curve_from_file( char pathn[MAX_PATHNAME_LEN], int LyrIndx,
							int RgnIdx , data_type_pointer CurvePtr	);
						
int read_layer_param_from_file( char pathn[MAX_PATHNAME_LEN] );   						
						
						
//int update_panel_data( struct regionStruct Reg ) ;
int update_displayed_region( char pathn[MAX_PATHNAME_LEN], int LayerIndex, int RegionIndex ) ;
int update_panel_data( struct regionStruct Reg, struct data_type *CurvePtr  );  

void build_datatype_from_region( struct regionStruct Reg,  struct data_type *DataType ) ;    

int CopyDataType( struct data_type *Source, struct data_type *Target, int CopyArrayFlag );   


int Plot_A_DataType_Curve( struct data_type *Source,  struct PlotStruct   *Plot ,
											int index1, int index2, int UseRightYAxis, int panel, int control ) ;

void display_fit_results( double area, double fwhm,  struct data_type *data );
void display_window_values( int index1, int ,  struct data_type *dataPtr ) ; 


void update_displayed_units( int units ) ;           
