//typedef data_pointer* data_ptr_array;   //pointer to allocatable array of pointers to allocatable array of doubles

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




void Smooth( struct data_type *dataTypePtr, int points, int degree )   ;

void ShirleyBack( struct data_type *dataTypePtr, data_type_pointer backTypePtr ,
                          int index1, int index2,  double  value1, double value2,
                          int iterations);
                          

void LinearBack( struct data_type * dataTypePtr, data_type_pointer backTypePtr ,
						int index1, int index2, double value1, double value2)  ;
						
void CopyCurveRange( struct data_type * sourcePtr, struct data_type * destPtr, int index1, int index2 ) ;

void AdjustIndex( int *index1, int *index2, struct data_type * dataTypePtr );

void Swap( int *A, int *B );
void SwapReal( double *A, double *B );        
