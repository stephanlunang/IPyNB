


// to keep track of what has been opened...
int PowerPanelOpen;           
int DetManualPanelOpen; 
int DetMonitorPanelOpen;
int DetAutoPanelOpen;
int AperturePanelOpen;           
int StepperPanelOpen;           
int ExcitePanelOpen;           
int ShutDownPanelOpen;           
int ParametersPanelOpen;           
int RegionPanelOpen;           
int AboutPanelOpen;
int ScanPanelOpen;
int PausedPanelOpen;
int StepperPanelOpen;
int DetSensitivityPanelOpen;


int Testing, DetectorTesting, DetectorTestSignal , PowerTesting; 
int ScanTesting, StepperTesting, ApertureTesting;



char Message[256]; //used as a general buffer in which to build strings... generally for 
						// messages to be displayed to user.
						
// structure to hold instument parameters
struct instrumentStruct
{
	//spectrometer
	double ReferenceEnergy;
	
	// 
	double ExcitationEnergy;
	double CurrentPhi;				
	double CurrentTheta;
	
	// detector
	double DetectorWidth;
	double DetectorLineWidth;
	unsigned int DetectorNumberOfChannels;
	
} EscaWest;


	

//used to save a data curve... array of counts plus minimum neccessary info
//  to identify the BE and KE axis position and accum time
struct data_type
{
	double 	excitation_energy;
	double 	time_p_channel;
	int 		size;
	double 	center_energy;    /* always binding */
	double 	width;				/* always negative*/
	
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
	
	double 			ExcitationEnergy ;
	unsigned int 	StepMultiple;	// multiple of the minimum step to use as actual step  

	unsigned int 	NumberOfScans;	   // number of repeated scans of this regions
	unsigned int 	NumberOfChannels;	// number of "bins" accross the energy range... a bin width is the detector channel width * StepMultiple    
	unsigned int   NumberOfDetectorBins ;  // has max value of N_O_DETECTOR_CHANNELS, and is 
														//    just  N_O_DETECTOR_CHANNELS / StepMultiple

	double		 	DwellTime;			// time between energy steps... in integer SECONDS

	unsigned int 	ScanMode;			// dithered or fixed accumulation mode	  ]
	
	double 			TotalTime;			// total time region takes to scan (w/o adjusting for overhead... just accumulation time)
	double			PercentDone;
	
	//unsigned long *TotalCounts;    	// used to allocate an array of the proper size for the region
	double		*TotalCounts;			//need to save counts as double for two reasons: need to multiply fixed
												// mode counts by channel sensitivity factor...get a double 
												// ... also future fitting routines need doubles...
	double 		  *BindingEnergies;	//   ""
	
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
	
	
	int PauseBetweenRegions;
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
