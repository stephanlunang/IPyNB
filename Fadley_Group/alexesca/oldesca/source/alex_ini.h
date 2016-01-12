//
//alex_ini.h

#include <stdio.h>
#include <stdlib.h>


int ini_find_section( FILE *, char * , char , char , char  , long * ) ;     
int move_to_after_tag(  FILE *, long , char ,char , char  , char * );
int ini_read_double_after_tag_value( FILE *, long , char , char , char  , char *, double *) ;
int ini_read_unsigned_long_after_tag_value( FILE *, long , char , char , char , char *, unsigned long int *);
int ini_read_int_after_tag_value( FILE *filePtr, long , char , char , char  , char *, int *);
int ini_read_string_after_tag_value( FILE *, long , char , char , char  , char *, char *, int );
