void Smooth( struct data_type *dataTypePtr, int points, int degree )
{
 
	double norm, m, m2;            
   double Ct[25];
   //struct data_type *   tmpTypePtr  ;
   double *tempData;
   int i, j , m1;
   
  
	//if ( Odd(points))			points = points - 1;
      
	if (points > 25) 			m1 = 12;
	else if (points < 2)		m1 = 1;
	else {
		divisionResult = div(points, 2);
		m1 = divisionResult.quot;
	}
      
	if (degree < 0)			degree = 0;
	else if (degree > 5 ) 	degree = 5;
      
	
	
	//CompCt( Ct, degree, m );
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
			Ct[i+m1] = m * (m + 1.0) * (3.0*m * (m + 1.0) - 1.0 - 5.0*pow (i, 2) ) / norm;
         
	}
	else if ( degree== 4 || degree == 5 )    //else if deg in [4,5] then begin
	{
		m2 = pow(m,2);
		norm = 4.0 * (4.0*m2 - 1.0) * (4.0*m2 - 9.0) * (2.0*m + 5.0) / 15.0;
		for (i = -m1 ; i<= m1 ; i++ )
			Ct[i+m1] = ( (15.0*pow (m2, 2) + 30.0*m2*m - 35.0*m2 - 50.0*m + 12.0)       / //continue next line
                          - 35.0 * (2.0*m2 + 2.0*m - 3.0) * pow (i, 2)				 / //continue next line  
                          + 63.0*pow (i, 4) ) / norm;
      
	}
	/*====== end CompCt(,,) ...=================*/    
	
	
	 //allocate temp array same size as dataTypePtr->data
	tempData = calloc( dataTypePtr->size, sizeof( double) ); 
 
 	//build temp data...
   for (i = m ; i<= dataTypePtr->size-1-m; i++ )
      for (j = -m ; j <= m; j++ )
      	tempData[i] +=   Ct[j] * dataTypePtr->data[i+j];
   
   //copy temp to passed pointer to data structure...
   for ( i=0 ; i< dataTypePtr->size ; i++ )
   	dataTypePtr->data[i] = tempData[i];	
   
   //deallocate memory...
   free( tempData );
   
   
}  /* end  Smooth */





// input: data curve ( evenly distributed doubles along arbintrary axis
//        index1,2 the first amd last points to use in the fit...with values 1 and 2
//	number if iterations determines the accuracy of the fit... supposedly... does it?
//
// ... constructs a shirley type background curve in the passed backgound curve array.
void ShirleyBack( struct data_type *dataTypePtr, data_type_pointer backTypePtr ,
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
   CopyCurveRange( dataTypePtr, backTypePtr, index1, index2 );
  
  
   a = value1;
   b = value2;
   for ( i=0 ; i <= backTypePtr->size-1 ; i++ ) backTypePtr->data[i] = b;

   for ( j=1; j<= iterations; j++ )
   {
      Q = 0.0;
      for (i = index1 ; i <=index2; i++) Q = Q + dataTypePtr->data[i] - backTypePtr->data[i-index1];
      Q = Q - 0.5 * (dataTypePtr->data[index1] - backTypePtr->data[0] +
                      dataTypePtr->data[index2] - backTypePtr->data[index2-index1]);
      QplusP = Q;
      for ( i= index1 ; i <= index2-1; i++ ) 
      {
         BO = (a - b) * (Q / QplusP) + b;
         Q = Q - 0.5 * (dataTypePtr->data[i+1] - backTypePtr->data[i+1-index1] +
                            dataTypePtr->data[i] - backTypePtr->data[i-index1]);
         backTypePtr->data[i-index1] = BO;
      }/* for */
      
   } /* for */
   
   
}  /* end of ShirleyBack */








// input: data curve ( evenly distributed doubles along arbintrary axis
//        index1,2 the first amd last points to use in the fit...with values 1 an 2
// ... constructs a linear background curve in the passed backgound curve array.
void LinearBack( struct data_type * dataTypePtr, data_type_pointer backTypePtr ,
						int index1, int index2, double value1, double value2)  
{
	int i;
	double k,m;
   
   //make sure the data struct (curve and associated items) is valid...
   if (index1 > index2)  SwapReal( &value1 , &value2 );
   AdjustIndex( &index1, &index2, dataTypePtr );
   
   // prepare background data struct...
   CopyCurveRange( dataTypePtr, backTypePtr, index1, index2 );
   
   //clear the background data ...
   for ( i = 0 ; i < backTypePtr->size -1  ; i++)    
   	backTypePtr->data[i] = 0;
   
   //get intercept and slope...	
   k = (value2 - value1) / (index2 - index1);
   m = value1 - (k * index1);
   
   //build the background... 
   for ( i = index1 ; i <= index2 ; i++)
   	backTypePtr->data[i-index1 ] = k*(double)i + m  ;
   
   
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
