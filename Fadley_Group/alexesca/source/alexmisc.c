//alexmisc.c
//
//misc. useful functions... all ANSI c

#include "stdlib.h" 
#include "alexmisc.h"
  
//rounds double (or float) to integer  
int Round( double Num )
{
	int RndNum;
	
	
	//ROUNDS DOUBLE TO NEAREST INT
	
	RndNum = Num;
	
	if( Num >= 0.0  && ((Num - (double)RndNum) >=  0.5 ) )
		RndNum++;
	else if( Num < 0.0 && ((Num - (double)RndNum ) <= -0.5 )) 
		RndNum--; 
	
	return RndNum;
}


//gives the number of char (including NULL terminator)
//  in the passed string
int get_string_size( char *String )
{
	int i=0;
	
	while( String[i] != NULL ) i++;
	// add one more for the NULL
	return ++i ;
}


//deletes white space from front and back ends of 
// the passed character array (string)
//
// NOTE: string MUST be null terminated or bad things
//        can happen!
void strip_white_ends_from_string( char *String )
{
	int white=0;
	int i=0;
	
	//count the number of leading white spaces:
	while( String[white] == ' ' )
		white++;
	//shift the string to eliminate leading white spaces
	if( white > 0 )
		while( String[i] != NULL )
		{
			String[i] = String[white + i];
			i++;
		}
		
	//get position of NULL
	i=0;
	while( String[i++] != NULL ) ;
	
	// now eliminate the trailing white space
	//    i gives position of NULL, so start at one back
	if( --i > 0 )
		while( String[--i] == ' ' )
			String[i]=NULL;
}
