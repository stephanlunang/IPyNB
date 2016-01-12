#include <analysis.h>
#include <ansi_c.h> 
#include <userint.h>
#include "Analysih.h"                   


#include "math_lib.h"



div_t divisionResult;	 /*from stlib.h*/                   
	


void Smooth( struct data_type *dataTypePtr, int points, int degree )
{
 
//double CtSum = 0.0;                     
 
	double norm, m, m2;            
   double Ct[25];
   //struct data_type *   tmpTypePtr  ;
   double *tempData;
   int i, j , m1;
   
  
	//if ( Odd(points))			points = points - 1;
   if( points%2 == 1 )  points = points - 1;         
      
      
	if (points > 25) 			m1 = 12;
	else if (points < 2)		m1 = 1;
	else {
		/*
		divisionResult = div(points, 2);
		m1 = divisionResult.quot;
		*/
		m1 = (int)( points/2);
	}
      
	if (degree < 0)			degree = 0;
	else if (degree > 5 ) 	degree = 5;
      
	
	//Compute the Ct array used in wieghting of sliding window...
	/*======CompCt(,,) in scienta code...=======*/
	
	//m1 is used as an index...
	//m is used if floating point calculations...
	m = (double) m1;
	
	if ( degree == 0 || degree == 1 )      //if deg in [0,1] then
	{
		for ( i = -m1 ; i <= m1 ; i++ )	 
			Ct[i+m1] = 1 / (2*m + 1);
	}
		
	else if ( degree == 2 || degree == 3 )	//else if deg in [2,3] then begin
	{
		m2 = m*m;
		norm = m * (4.0*m2 - 1.0) * (2.0*m + 3.0) * (m + 1.0) / 3.0;
		for (i = -m1 ; i<= m1 ; i++ )
			Ct[i+m1] = m * (m + 1.0) * (3.0*m * (m + 1.0) - 1.0 - 5.0*i*i ) / norm;
         
	}
	else if ( degree== 4 || degree == 5 )    //else if deg in [4,5] then begin
	{
		m2 = m*m;
		norm = 4.0 * (4.0*m2 - 1.0) * (4.0*m2 - 9.0) * (2.0*m + 5.0) / 15.0;
		
		for (i = -m1 ; i<= m1 ; i++ )
			Ct[i+m1] = ( (15.0*m2*m2 + 30.0*m2*m - 35.0*m2 - 50.0*m + 12.0)      
                          - 35.0 * (2.0*m2 + 2.0*m - 3.0) * i*i + 63.0*i*i*i*i ) / norm;
      
	}
	
//	CtSum = 0.0; 
//	for (i = -m1 ; i<= m1 ; i++ ) 
//	{
//		                   
//		
//		CtSum += Ct[i+m1];
//		
//		printf( "Ct[%i]=  %f \n", i+m1, Ct[i+m1]);
//	}
//	printf( "Ct Sum=  %f \n", CtSum);
//	
//	printf( "\n" );
	
	/*====== end CompCt(,,) ...=================*/    
	
	
	 //allocate temp array same size as dataTypePtr->data
	tempData = calloc( dataTypePtr->size, sizeof( double) );  //initializes all to 0.0
 
 	//build temp data...
   for (i = m1 ; i<= dataTypePtr->size-1-m1; i++ )
   {
      for (j = -m1 ; j <= m1; j++ )
      	tempData[i] +=   Ct[j+m1] * dataTypePtr->data[i+(j)];
//      printf("    data[%i]= %f \n", i, tempData[i] );
   }
      
   
   //copy temp to passed pointer to data structure...
   //    the ends ( {0 to m} and  {size-1-m to size-1}  are left alone...
   for ( i=m ; i< dataTypePtr->size - m ; i++ )
   	dataTypePtr->data[i] = tempData[i];	
   
   //deallocate memory...
   free( tempData );
   
   
}  /* end  Smooth */





// input: data curve ( evenly distributed doubles along arbintrary axis
//        index1,2 the first amd last points to use in the fit...with values 1 and 2
//	number if iterations determines the accuracy of the fit... supposedly... does it?
//
// ... constructs a shirley type background curve in the passed backgound curve array.
void ShirleyBackground( struct data_type *dataTypePtr, data_type_pointer backTypePtr ,
                          int index1, int index2,  double  value1, double value2,
                          int iterations)
{                         
   /*                                                                   */
   /*  Practical Surface Analysis by D.Briggs & M.P.Seah, p 465-467     */
   /*                                                                   */
   
   int   i, j ;
   double a, b, QplusP, Q, BO, Qc, temp ;
  
   if (index1 > index2) {  //swap values
   	temp = value1;
   	value1 = value2;
   	value2 = temp;
   }
   
	AdjustIndex( &index1, &index2, dataTypePtr );  
//	CopyCurveRange( dataTypePtr, backTypePtr, index1, index2 );
//  
//  
//   a = value1;
//   b = value2;
//   for ( i=0 ; i <= backTypePtr->size-1 ; i++ ) backTypePtr->data[i] = b;
//
//   for ( j=1; j<= iterations; j++ )
//   {
//      Q = 0.0;
//      for (i = index1 ; i <=index2; i++) Q = Q + dataTypePtr->data[i] - backTypePtr->data[i-index1];
//      Q = Q - 0.5 * (dataTypePtr->data[index1] - backTypePtr->data[0] +
//                      dataTypePtr->data[index2] - backTypePtr->data[index2-index1]);
//      QplusP = Q;
//      for ( i= index1 ; i <= index2-1; i++ ) 
//      {
//         BO = (a - b) * (Q / QplusP) + b;
//         Q = Q - 0.5 * (dataTypePtr->data[i+1] - backTypePtr->data[i+1-index1] +
//                            dataTypePtr->data[i] - backTypePtr->data[i-index1]);
//         backTypePtr->data[i-index1] = BO;
//      }/* for */
//      
//   } /* for */
//  
   a = value1;
   b = value2;
   for ( i=0 ; i <= backTypePtr->size-1 ; i++ ) backTypePtr->data[i] = b;

	for( i = 0 ; i < index1 ; i++ ) 						backTypePtr->data[i]=0.0;
	for( i = index2+1 ; i < backTypePtr->size; i++ ) 	backTypePtr->data[i]=0.0; 
	
	
   for ( j=1; j<= iterations; j++ )
   {
      Q = 0.0;
      for (i = index1 ; i <=index2; i++) Q = Q + dataTypePtr->data[i] - backTypePtr->data[i];
      Q = Q - 0.5 * (dataTypePtr->data[index1] - backTypePtr->data[index1] +
                      dataTypePtr->data[index2] - backTypePtr->data[index2]);
      QplusP = Q;
      for ( i= index1 ; i <= index2-1; i++ ) 
      {
         BO = (a - b) * (Q / QplusP) + b;
         Q = Q - 0.5 * (dataTypePtr->data[i+1] - backTypePtr->data[i+1] +
                            dataTypePtr->data[i] - backTypePtr->data[i]);
         backTypePtr->data[i] = BO;
      }/* for */
      
   } /* for */
   
}  /* end of ShirleyBack */



void NoBackground( struct data_type * dataTypePtr, data_type_pointer backTypePtr ,
						int index1, int index2, double value1, double value2) 
{
	int i;
	
	for( i=0 ; i< backTypePtr->size; i++ )
		backTypePtr->data[i] = 0.0;
		
}




// input: data curve ( evenly distributed doubles along arbintrary axis
//        index1,2 the first amd last points to use in the fit...with values 1 an 2
// ... constructs a linear background curve in the passed backgound curve array.
void LinearBackground( struct data_type * dataTypePtr, data_type_pointer backTypePtr ,
						int index1, int index2, double value1, double value2)  
{
	int i;
	double k,m;
   
   //make sure the data struct (curve and associated items) is valid...
   if (index1 > index2)  SwapReal( &value1 , &value2 );
   AdjustIndex( &index1, &index2, dataTypePtr );
   
//   // prepare background data struct...
//   CopyCurveRange( dataTypePtr, backTypePtr, index1, index2 );
//   
//   //clear the background data ...
//   for ( i = 0 ; i < backTypePtr->size -1  ; i++)    
//   	backTypePtr->data[i] = 0.0;
//   
//   //get intercept and slope...	
//   k = (value2 - value1) / (index2 - index1);
//   m = value1 - (k * index1);
//   
//   //build the background... 
//   for ( i = index1 ; i <= index2 ; i++)
//   	backTypePtr->data[i-index1 ] = k*(double)i + m  ;
   
  
   //clear the background data ...
   for ( i = 0 ; i < backTypePtr->size -1  ; i++)    
   	backTypePtr->data[i] = 0.0;
   
   //get intercept and slope...	
   k = (value2 - value1) / (index2 - index1);
   m = value1 - (k * index1);
   
   //build the background... 
   for ( i = index1 ; i <= index2 ; i++)
   	backTypePtr->data[i] = k*(double)i + m  ;
   
   
}  /* end of linearBack */





void CopyCurveRange( struct data_type * sourcePtr, struct data_type * destPtr, int index1, int index2 )
{
	int i;
	data_type_pointer tempDataPtr;
	
	
	AdjustIndex( &index1, &index2, sourcePtr );
	tempDataPtr = sourcePtr;
	
	tempDataPtr->size = index2 - index1 + 1;
	
	/*AdjustData( tempDataPtr, tempDataPtr->size );  */
	
	tempDataPtr->width =   IndexToBinding( sourcePtr, index2)  -  IndexToBinding( sourcePtr, index1);
	tempDataPtr->center_energy =  IndexToBinding( sourcePtr, index1)  + 0.5* tempDataPtr->width;
	
	//copy source data in requested range to the temp 
	for( i = index1 ; i <= index2 ; i++ )
	{
		tempDataPtr->data[i-index1] = sourcePtr->data[i];
	}
	
	// copy the temp to the destination...
	destPtr = tempDataPtr;

}



void AdjustIndex( int *index1, int *index2, struct data_type * dataTypePtr )
{
	int temp;
	
	
	if( *index1 > *index2 )  					Swap( index1, index2 );
	if( *index1 < 0 ) 							*index1 = 0;
	if( *index2 > dataTypePtr->size -1 )	*index2 = dataTypePtr->size -1; 
	
	
}



void Swap( int *A, int *B )
{
	int temp;
	temp = *A;
	*A = *B;
	*B = temp;
}




void SwapReal( double *A, double *B )
{
	double temp;
	temp = *A;
	*A = *B;
	*B = temp;
}


/*

void GetArea ( double *Area, struct data_type * dataPtr, 
						struct data_type * refPtr, int index1, int index2 )
{

	int i;
	data_type_pointer tempPtr;
	double e1,e2;
	
	AdjustIndex( &index1, &index2, dataPtr );	  //??
	
	tempPtr = dataPtr;
	if( refPtr != NULL  )
	{
		e1 = IndexToBinding( tempPtr, index1 );
		e2 = IndexToBinding( tempPtr, index2 );
		AdjustChannels( tempPtr, Round( tempPtr->width/refPtr->width/refPtr->size) );
		index1 = Round( BindingToIndex( tempPtr, e1 ) );
		index2 = Round( BindingToIndex( tempPtr, e2 ) );
	}
	*Area = 0.0;
	for( i= index1 ; i <= index2 ; i++ )
		if( tempPtr->data[i] > 0 )  *Area += tempPtr->data[i];
	

}
*/










/***************************************************************************/
/*  All conversion functions use the following formula:                    */
/*    Kinetic_energy = Excitation_energy - Binding_energy - Work_function  */
/*     (but we ignore the work_function... assume it is 0.0      				*/
/***************************************************************************/


double BindingToKinetic( double be, double excitation )
{
	return excitation - be ;
}


double KineticToBinding( double ke, double excitation)
{
	return excitation - ke ;
}


//rounding problems??
double BindingToIndex( struct data_type * dataTypePtr, double be )
{
	return (double)(be - (dataTypePtr->center_energy + dataTypePtr->width/2.0)/(dataTypePtr->width/(dataTypePtr->size -1)));
}


double IndexToBinding( struct data_type * dataTypePtr, double index )
{
	return (double)index*dataTypePtr->width/(dataTypePtr->size -1) + dataTypePtr->center_energy - dataTypePtr->width/2.0 ;
}

//rounding problems??     
double KineticToIndex( struct data_type * dataTypePtr, double ke )
{
	double be;
	
	be = KineticToBinding( ke, dataTypePtr->excitation_energy );
	return  (double)(be - dataTypePtr->center_energy + dataTypePtr->width/2.0)/( dataTypePtr->width/(dataTypePtr->size - 1));	
}


double IndexToKinetic( struct data_type * dataTypePtr, double index ) 
{
	double be;
	
	be = (double)index*dataTypePtr->width/(dataTypePtr->size -1) + dataTypePtr->center_energy - dataTypePtr->width/2.0;
	return BindingToKinetic( be, dataTypePtr->excitation_energy );
}



// uses the info in dataTypePtr (width,center_energy) to get
//  the start and stop (inital and final) energies
void ChangeDescription( double *start, double *stop, double *step ,
									struct data_type * dataTypePtr, int beFlag)
{
	
	
	*start = dataTypePtr->center_energy - dataTypePtr->width/2.0 ;
	*stop  = *start + dataTypePtr->width;
	*step  = dataTypePtr->width/( dataTypePtr->size - 1 );
	
	if( !beFlag )	 //ke... beFlag =FALSE
	{
		*start = BindingToKinetic( *start, dataTypePtr->excitation_energy);
		*stop  = BindingToKinetic( *stop , dataTypePtr->excitation_energy);
		*step  = -*step;
	}
	
}


//returns the sum of the counts in the data.data array within (and including)
//  the window defined by index1,index2
//
// area is in counts.
void GetArea( double *Area,  struct data_type *data, int index1, int index2 )
{
	int i;
	
	AdjustIndex( &index1, &index2, data ); //make sure indices define a valid window
	
	*Area = 0.0;
	
	for( i = index1 ; i<index2 ; i++ )
		*Area +=  data->data[i];

}









//uses a spline fit to extract the peak position,
// peak height, and full_width_half_max from a data curve.
//
void GetPeak( double *PeakPosition, double *PeakHeight, 
					double *Fwhm, double *left, double *right,
					  int index1, int index2,
					    struct data_type *dataPtr )
{

	int i,j, max;
	double a,b,half;
	
	double *Xvals;
	double *SecondDerivatives;
	double B1, B2; //bndry conditions
	
	double testIndex;
	double testHeight;

	
	AdjustIndex( &index1, &index2, dataPtr ); //make sure indices define a valid window  
	
	
	
	//a= min curve value
	//b= max curve value
	//i= min curve value index
	//max = max curve value index
	FindMinMax( &a, &b, &i, &max, index1, index2, dataPtr );

//Get PEAK POSITION AND HEIGHT...	
	//fit curve to spline...
	Xvals = calloc( dataPtr->size , sizeof(double) );
	for( i=0 ; i< dataPtr->size ; i++ ) {
		Xvals[i] = (double) i;
	}
		
	SecondDerivatives = calloc( dataPtr->size, sizeof(double) );
	B1 =  ( dataPtr->data[1] +  dataPtr->data[0] )/2.0;
	B2 =  ( dataPtr->data[dataPtr->size-1] +  dataPtr->data[dataPtr->size-2] )/2.0;  
	Spline ( Xvals, dataPtr->data, dataPtr->size, B1, B2, SecondDerivatives);
	
	//get the maximum value & index of the spline fit curve...
	//   for now just go from max -1 to max + 1 in 100 steps and get max value...
	*PeakHeight = dataPtr->data[max];
	
	for( j = 0 ; j< 100 ; j++ )
	{
		testIndex =  max - 1.0 + (double)j/50.0 ;
		if( testIndex >= (double) index1 && testIndex <= (double)index2 )
			SpInterp (Xvals, dataPtr->data , SecondDerivatives, dataPtr->size, testIndex, &testHeight);
		if( testHeight > *PeakHeight )
		{
			*PeakHeight = 	testHeight;
			*PeakPosition = testIndex;
		}
	     
	}
	
	
	//use peak postion and max to find FWHM:
	half = *PeakHeight/2.0 ;
	
	
//Get LEFT Half-Max channel: start at max point...
	i = max;
	// ...and decrement the channel until we have found
	//      the first channel <= half height (or reached edge of window (index1))...
	while( i>index1 && dataPtr->data[i] > half ) i=i-1.0;
	// ...if we are still above half height, then left is defaulted to i (index1 now)
	if( dataPtr->data[i]  > half ) *left = i;
	// ...but if we have found a channel < half-max... (stored in i)
	else
	{
		//increment until we are back above half...
		for( j = 0 ; j< 100 ; )
		{
			testIndex =  i + (double)j/100.0 ;
			
			SpInterp (Xvals, dataPtr->data , SecondDerivatives, dataPtr->size, testIndex, &testHeight);
			
			if( testHeight > half )
			{
				//once we are above, decrement once and use this as left value
				j--;
				*left = i + (double)j/100.0;
				j = 100; //break from loop...
			}
			else j++;
			if( j == 99 ) *left = i + (double)j/100.0;    
	     
		}
		
	}
	
//Get RIGHT Half-Max channel: start at max point...
	i=max;
	// ...and increment the channel until we have found
	//      the first channel <= half height (or reached edge of window (index2))...
	while( i<index2 && dataPtr->data[i] > half ) i=i+1.0;
	// ...once we have the first channel < half-max... (in variable i)
	if ( dataPtr->data[i] < half )
	{
		for( j = 0 ; j< 100 ; )
		{
			testIndex =  i - (double)j/100.0 ;	  //back up an increment...
			
			//find value at this index...
			SpInterp (Xvals, dataPtr->data , SecondDerivatives, dataPtr->size, testIndex, &testHeight);
			
			//check if we when back above half...
			if( testHeight > half )
			{
				j--;
				*right = i - (double)j/100.0 ;
				j = 100; //break from loop...
			}
			else j++;
			if( j == 99 ) *right = i - (double)j/100.0 ;        
	     
		}
	}
	// ...if we never got below half height, default right to i (index2).
	else *right = i;
	
	//FWHM is just the number of channels between left and right...
	*Fwhm = *right - *left;
	
	free( Xvals );
	free( SecondDerivatives );

}




void FindMinMax( double *min, double *max, 
						int   *minIndex, int *maxIndex,
							int index1, int index2,
								struct data_type *data )    
{
	int i;
	
	*minIndex = index1;
	*maxIndex = index1;
	
	for ( i= index1; i<= index2 ; i++ )
	{
		if( data->data[i] < data->data[*minIndex] ) *minIndex = i;
		
		if( data->data[i] > data->data[*maxIndex] ) *maxIndex = i;
	}
	
	*min = data->data[*minIndex];
	*max = data->data[*maxIndex];
	
}
