#define BACKGROUND_NONE 		0
#define BACKGROUND_LINEAR 		1
#define BACKGROUND_SHIRLEY 	2

#define SMOOTHING_NONE					0
#define SMOOTHING_SLIDING_WINDOW		1

#define FITTING_NONE 		0
#define FITTING_SPLINE		1


void Smooth( struct data_type *dataTypePtr, int points, int degree )   ;

void ShirleyBackground( struct data_type *dataTypePtr, data_type_pointer backTypePtr ,
                          int index1, int index2,  double  value1, double value2,
                          int iterations);
                          

void LinearBackground( struct data_type * dataTypePtr, data_type_pointer backTypePtr ,
						int index1, int index2, double value1, double value2)  ;
						
void NoBackground( struct data_type * dataTypePtr, data_type_pointer backTypePtr ,
						int index1, int index2, double value1, double value2)  ;						
						
void CopyCurveRange( struct data_type * sourcePtr, struct data_type * destPtr, int index1, int index2 ) ;

void AdjustIndex( int *index1, int *index2, struct data_type * dataTypePtr );

void Swap( int *A, int *B );
void SwapReal( double *A, double *B );       
//void GetArea ( double *Area, struct data_type * dataPtr, 
//						struct data_type * refPtr, int index1, int index2 );
void GetArea( double *Area,  struct data_type *data, int index1, int index2 );             						
						

double BindingToKinetic( double be, double excitation );            
double KineticToBinding( double ke, double excitation);  
double BindingToIndex( struct data_type * dataTypePtr, double be ) ;
double IndexToBinding( struct data_type * dataTypePtr, double index );   
double KineticToIndex( struct data_type * dataTypePtr, double ke );    
double IndexToKinetic( struct data_type * dataTypePtr, double index );   
void ChangeDescription( double *start, double *stop, double *step ,
									struct data_type * dataTypePtr, int beFlag);

void GetPeak( double *PeakPosition, double *PeakHeight, 
					double *Fwhm, double *left, double *right,
					  int index1, int index2,
					    struct data_type *data ) ;

void FindMinMax( double *min, double *max, 
						int   *minIndex, int *maxIndex,
							int index1, int index2 ,
								struct data_type *data ) ;   
