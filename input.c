/**
    @filename input.c
    @author Will Greene (wgreene)
    
    Reads a line of input.
  */
#include "input.h"

/**
    Reads a line of input and returns it as a string pointer.
    
    @param *fp pointer to a file to read from
    @return a string pointer
  */
char *readLine( FILE *fp ) {

    // MEASURE LINE - START --------------------------------
    
    long startOfLine = ftell( fp );
    
    int ch = fgetc( fp );
    if ( ch == EOF )
        return NULL;
    
    int numChar = 0;
    while ( ch != '\n' && ch != EOF ) {
        numChar++;
        ch = fgetc( fp );
    }
    
    bool addNL = true;
    if ( ch == '\n' )
        numChar++;
    if ( ch == EOF ) {
        ungetc( ch, fp );
        addNL = false;
    }
        
    fseek( fp, startOfLine, SEEK_SET );

    // MEASURE LINE - END ----------------------------------
    
    char *str = (char *) malloc(numChar * sizeof( char *));
    
    for ( int i = 0; i < numChar - 1; i++ ) {
        int ch = fgetc( fp );
        str[ i ] = ch;
    }
    
    str[ numChar - 1 ] = '\0';
    
    if ( addNL )
        fgetc( fp );
    
    return str;
}
