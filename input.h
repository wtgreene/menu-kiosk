/**
    @filename input.h
    @author Will Greene (wgreene)
    
    Header file for input.c
  */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
    Reads a line of input and returns it as a string pointer.
    
    @param *fp pointer to a file to read from
    @return a string pointer
  */
char *readLine( FILE *fp );
