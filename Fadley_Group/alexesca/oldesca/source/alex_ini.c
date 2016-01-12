#include "analysis.h"
#include <ansi_c.h>
#include <utility.h>
#include <cvirte.h>		/* Needed if linking in external compiler; harmless otherwise */
#include <userint.h>
#include <stdio.h>
#include <stdlib.h>

#include "alex_ini.h"

//static long filePosition;





int ini_find_section( FILE *filePtr, char *section , char start, char end, char comment ,long *filePosition )
{
	// file is asumed already open
	char c;
	int i;
	
	///note:  can modify to keep from limit of array... once 'start' is found, begin 
	///        comparing each character, one at a time... if the characters are equal until NULL of
	///			sectionName, and then there is only whitespace until 'end' then we have a match
	
	
	// go to start of file
	rewind (filePtr);
	
	while( (c = getc(filePtr)) != EOF )		//while not at the end of the file
	{
			//if first or current char is a newline , keep reading until we find something                  
		if( c == '\n' ){
			continue;
		}
		
		// check if first char is the start char                
		if (  c == start ) 
		{																					
			i=0;
			while( ((c = getc(filePtr)) != EOF ) && c != '\n' ) {
				
				// if the characters match, keep testing
				if( c == section[i] ){
					++i;
					
					// if next section char is the NULL, we have matched the section string ...
					if( section[i] == NULL ){
						
						while(  (c = getc(filePtr)) != EOF  ) {
							if( c == end ) {
							
								if( ( *filePosition = ftell (filePtr)) == -1 )
									return -1;
								else  	
									return 0;
								
							}
							else{
								// there was not an end char in the proper place...
								//  didn't match the section yet...
								i=0;
								while(  ((c = getc(filePtr)) != EOF) && c != '\n'  );             
								break;
							}
								
						}
					}
					
					
				}
				// but if they don't match, read up to next newline
				else{
		//			printf( " no match %c is not %c \n", c,section[i]);
					i=0;
					while(  ((c = getc(filePtr)) != EOF) && c != '\n'  );
					break;
				}
				
			}
			
			
		}
		else{
				// go to next newline
		//		printf( " didn't match %c to start char \n",c);
				while(  ((c = getc(filePtr)) != EOF) && c != '\n'  ); 	
		}
		
	}
	return -1;
}




// This will read the value associated with teh passed tag string.  It begins looking at the
// passed file position, which should coorespond to a section ( i.e. [Section] ) beginning.
// To get this position use ini_find_section( , , , , , ).
// 
// An arbitrary section start char ( i.e '[' ), value/tag separator (i.e. '='), and comment
// line indicator ( i.e. ';' ) is allowed.  If 0 is passed to any of these values, then the
// defaults are '[',  '=',  ';'
//
//	If there are multiple items with the same tag, only the first will be found and its 
// value will be read.  Even if there is no value following the tag, the duplicate
// tag will not be looked for.
//
//	The case of no value found after the tag results in a return value of -3 and the
// double passed by reference will be set to 0.0
//
// Note: arbitrary number of spaces between the tag & separator and the separator & value ARE allowed
//       so user does not need to worry about this feature of the file format
//			File formatting that *is* important is to use what I believe is standard Windows *.ini
//       file conventions... no spaces *before* section names or tag strings.
//
int ini_read_double_after_tag_value( FILE *filePtr, long filePosition, char start, char separator, char comment , char *tag, double *value)
{
	
	char c;
	int i=0, error = 0;
	

	// locate the tag, and move to separator
	error = move_to_after_tag( filePtr, filePosition, start, separator,  comment , tag );
	if( error != 0 ) {
		*value = 0; 
		return error;
	}
	
	// scan in one long unsigned int ...
	if(  (fscanf (filePtr, "%Lf", value)) != 1 ){
		// if not successful, default value to zero 
		*value = 0.0;
		return -5;
	}

	return 0;

}

			
			
			
			
// This will read the value associated with the passed tag string as an unsigned long int. 
// It begins looking at the  passed file position, which should 
// coorespond to a section ( i.e. [Section] ) beginning.
// To get this position use ini_find_section( , , , , , ).
// 
// An arbitrary section start char ( i.e '[' ), value/tag separator (i.e. '='), and comment
// line indicator ( i.e. ';' ) is allowed.  If 0 is passed to any of these values, then the
// defaults are '[',  '=',  ';'		 (*******not true yet...feature needs to be added*******)
//
//	If there are multiple items with the same tag, only the first will be found and its 
// value will be read.  Even if there is no value following the tag, the duplicate
// tag will not be looked for.
//
//	The case of no value found after the tag results in a return value of -3 and the
// double passed by reference will be set to 0.0
//
// Note: arbitrary number of spaces between the tag & separator and the separator & value ARE allowed
//       so user does not need to worry about this feature of the file format
//			File formatting that *is* important is to use what I believe is standard Windows *.ini
//       file conventions... no spaces *before* section names or tag strings.
//
int ini_read_unsigned_long_after_tag_value( FILE *filePtr, long filePosition, char start,
		char separator, char comment , char *tag, unsigned long *value)
{
	
	char c;
	int i=0, error = 0;
	
	// locate the tag, and move to separator
	error = move_to_after_tag( filePtr, filePosition, start, separator,  comment , tag );
	if( error != 0 ) {
		*value = 0; 
		return error;
	}
	
	// scan in one long unsigned int ...
	if(  (fscanf (filePtr, "%lu", value)) != 1 ){
		// if not successful, default value to zero 
		*value = 0;
		return -5;
	}

	return 0;

}



// This will read the value associated with the passed tag string as an int. 
// It begins looking at the  passed file position, which should 
// coorespond to a section ( i.e. [Section] ) beginning.
// To get this position use ini_find_section( , , , , , ).
// 
// An arbitrary section start char ( i.e '[' ), value/tag separator (i.e. '='), and comment
// line indicator ( i.e. ';' ) is allowed.  If 0 is passed to any of these values, then the
// defaults are '[',  '=',  ';'		 (*******not true yet...feature needs to be added*******)
//
//	If there are multiple items with the same tag, only the first will be found and its 
// value will be read.  Even if there is no value following the tag, the duplicate
// tag will not be looked for.
//
//	The case of no value found after the tag results in a return value of -3 and the
// double passed by reference will be set to 0.0
//
// Note: arbitrary number of spaces between the tag & separator and the separator & value ARE allowed
//       so user does not need to worry about this feature of the file format
//			File formatting that *is* important is to use what I believe is standard Windows *.ini
//       file conventions... no spaces *before* section names or tag strings.
//
int ini_read_int_after_tag_value( FILE *filePtr, long filePosition, char start,
		char separator, char comment , char *tag, int *value)
{
	
	char c;
	int i=0, error = 0;
	
	// locate the tag, and move to separator
	error = move_to_after_tag( filePtr, filePosition, start, separator,  comment , tag );
	if( error != 0 ) {
		*value = 0; 
		return error;
	}
	
	// scan in one long unsigned int ...
	if(  (fscanf (filePtr, "%i", value)) != 1 ){
		// if not successful, default value to zero 
		*value = 0;
		return -5;
	}

	return 0;

}





// NOTE: after string is read in, we should strip all spaces at the end, appending NULL
//       at the location of the first end space (??) NO! if user left spaces before /n, read them in!

int ini_read_string_after_tag_value( FILE *filePtr, long filePosition, char start,
		char separator, char comment , char *tag, char *string, int maxLength)
{
	
	char c;
	int  j= 0, error = 0;
	
	// locate the tag, and move to separator
	error = move_to_after_tag( filePtr, filePosition, start, separator,  comment , tag );
	if( error != 0 ) {
		string[0] = NULL;
		return error;
	}
	
	
	/******* now read past all initial spaces:  *********/
	while( (c = getc(filePtr)) != EOF ){
		if( c != ' ' ) 	break;
		else {
			//**/printf( "  -found a white space \n");
		}
	}		
	
	/******* we now have a non space char:		 ***********/
	//		test it, and get another char if necessary
//**/	printf( "  -read past white spaces \n");             
	j=0;								  // position in string index
	while( j < maxLength  ){		
								
		// check that we are not at end of file
		if( c == EOF ){
//**/			printf( "  -reached EOF \n");
			return -1;
		}
							
		// if we found a nweline, we are at the end of the string so append a null
		if ( c == '\n' ){
//**/			printf( "  -read a newline \n");
//**/			printf( "  - string is {{%s}} \n", string);    
								
			string[j] = NULL;
			return 0;  //success ... got entire line into string
		}
								
		//once we are past all the spaces, read the rest
		//  (up to the newline or the max char) of
		//   the line into the string...
//**/		printf( "  -made it to next character...\n");
												
		string[j] = c;		 		// put the char in the string
		++j;					  		// increment string position
						
		c = getc( filePtr ) ;	// get the next char
			
	}  //end while to read in value string                 
						
							
	// if we are still in the function at this point
	//    we must have filled the string without reaching 
	//    a newline... warn user by returning positive 1
	//
	//   but first check to see if the *next* char is the newline
	//    ... if it is, then we exactly read teh whole line into the string
	//        so we can return 0 (no error)
							
	string[j] = NULL;		  // has to be true
//**/	printf( "  - string is {%s}\n", string);         
							
	// we already have the char from the last iteration above...
	// if a newline is found, terminate the string
	if ( c == '\n' )  return 0;
	else 					return 1;
	
	// we only get here if nothing was found... set double value to zero??
//	string[0] = NULL; 
//	return -3;
	
}











// return values: 	0 	= success
//							-1	= could not go to file position requested
//							-2 = reached EOF without matching tag		 
//							-3 = reached next 'start' char (next section) without matching tag
//							-4 = could not match 'separator' char after tag




// leaves file pointer ready to read char just aftertag/value separator
int move_to_after_tag(  FILE *filePtr, long filePosition, char start,
		char separator, char comment , char *tag )
{
	int error, i;
	char c;

	// goto the filePosition given ... this should be just after the 'end' char
	if( (error = fseek (filePtr, filePosition, SEEK_SET)) != 0 ) {
		printf( " error setting file position: fseek()" );
		return -1;
	}
	
	i = 0;
								   
	/*******  BEGIN FINDING THE TAG ************************************************/
	// go to the end of line... find next newline:
	while( (c = getc(filePtr)) != EOF ) 
	{		
		if ( c == '\n'){ 
//**/		printf( "  -found newline \n");
			continue;
		}

		if( c == EOF )		{
/**/		printf( "  -EOF \n");
			return -2;
		}
		
		if( c == comment ){
//**/		printf( "  -found a comment \n");
			// ignore the rest of the line... continue to next while iteration...
			//read until another newline is found
			while( ((c = getc(filePtr)) != EOF ) && c != '\n' );  
			continue;
		}
		
		if( c == start )	{
//**/		printf( "   -found a 'start' without matching the tag \n" );                   
			// ... break from while loop... tag not found in this section
			return -3;
		}
		
		if( c == tag[i] ){
//			printf( " matched %c \n", c);
			i++;
			if( tag[i] == NULL ){
			//	printf( "matched the tag!\n");
				break;  //we matched the string!
			}
			
		}
		else {
//			printf( "match failed ... c = %c is not %c \n", c,tag[i]);
			i = 0;  // no match, reset character index
			while( ((c = getc(filePtr)) != EOF ) && c != '\n' );      // get to end of line
			continue;
		}
	}

	/*******  DONE FINDING THE TAG  **************************************/     		
	
	
	
	/******* now find the separator (typically '=' )******/
	while( (c = getc(filePtr)) != EOF ){
		if( c == separator ) {
//**/			printf( "  -found the separator \n");
			break;
		}
		if( c == '\n' ){
//**/				printf("  -no separator found\n");
			return -4;
		}
	}						
	

	return 0;	  // if we are here, success!
}




