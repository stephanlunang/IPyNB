
#ifndef GLOBALS_H
#define GLOBALS_H


#include "userint.h"  //for MAX_PAHTNAME_LEN

// to keep track of what has been opened...
extern int PowerPanelOpen;           
extern int DetManualPanelOpen; 
extern int DetMonitorPanelOpen;
extern int DetAutoPanelOpen;
extern int AperturePanelOpen;           
extern int StepperPanelOpen;           
extern int ExcitePanelOpen;           
extern int ShutDownPanelOpen;           
extern int ParametersPanelOpen;           
extern int RegionPanelOpen;           
extern int AboutPanelOpen;
extern int ScanPanelOpen;
extern int PausedPanelOpen;
extern int StepperPanelOpen;
extern int DetSensitivityPanelOpen;


extern int Testing;
extern int DetectorTesting;
extern int DetectorTestSignal;
extern int PowerTesting; 
extern int ScanTesting;
extern int StepperTesting;
extern int ApertureTesting;



extern char Message[256]; //used as a general buffer in which to build strings... generally for 
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
	
};
extern struct instrumentStruct  EscaWest;


	

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
	
	//inappropriate to be here; double 			ExcitationEnergy ;
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



struct ManualLayer
{
	int Size;
};

struct PolarLayer
{
	int Size;
	double Initial;
	double Final;
	double Step;
};

struct TimedLayer
{
	int Size;
	double Time;
	double InitialTime;	 //used during scan
};

struct layerStruct
{
	char Description[101];
	int  Type;
	int Size;
	
	struct ManualLayer Manual;
	struct PolarLayer  Polar;
	struct TimedLayer	 Timed;
};
	
struct experimentStruct
{
	char Pathname[MAX_PATHNAME_LEN];       
	char Description[101];	   // expt name/description
	int n_o_regions;  			// number of regions in the experiment
	int CurrentRegion;
	double Excitation;		   // excitation energy in eV 
	char Instrument[101];		   	// name of the instrument
	//char *Date;				   	// expt date finished
	//char *Time;				  		// expt time finished
	
	double TotalTime;				// number of seconds long the experiment is
	double PercentDone;
	
	char DateBegan[11];		 // YYYY/MM/DD format = 10char + NULL = 11char
	char DateFinished[11];
	char TimeBegan[9];		 //hh:mm:ss format = 8 + NULL = 9char
	char TimeFinished[9];
	
	int regionIndex;
	int layerIndex;
	
	struct regionStruct *Regions;	 // pointer to an array of region structures
	
	struct layerStruct Layers;
	
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


#endif
