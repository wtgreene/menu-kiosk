/**
    @filename menu.h
    @author Will Greene (wgreene)
    
    Header file for menu.c
  */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** initial number of Menu array elements */
#define MENU_INITIAL_CAPACITY 5

/** number of characters for a MenuItem id number ( 4 ) ( +1 for null terminator ) */
#define NUM_CHAR_ID 5

/** maximum number of characters for a MenuItem name ( 20 ) ( +1 for null terminator ) */
#define MAX_NUM_CHAR_NAME 21

/** maximum number of characters for a MenuItem category ( 15 ) ( +1 for null terminator ) */
#define MAX_NUM_CHAR_CATEGORY 16

/** number of cents in a dollar */
#define CENTS_IN_A_DOLLAR 100.0

/**
    A menu.
  */
struct Menu {
    struct MenuItem **list; // list of menu items
    int count;              // number of menu items
    int capacity;           // capacity of the list
};

/**
    A menu item.
  */
struct MenuItem {
    char id[ NUM_CHAR_ID ];                 // id number of the menu item ( stored as a string )
    char name[ MAX_NUM_CHAR_NAME ];         // name of the menu item
    char category[ MAX_NUM_CHAR_CATEGORY ]; // category of the menu item
    int cost;                               // cost of the menu item
};

/**
    Allocates storage for a (the) Menu, and initializes its fields.
    
    @return the Menu
  */
struct Menu *makeMenu();

/**
    Frees the memory used to store the given Menu.
    
    @param *menu Menu to be freed
  */
void freeMenu( struct Menu *menu );

/**
    Reads all MenuItems from a file with the given name.
    
    @param *filename name of file to read from
    @param *menu Menu to add to
  */
void readMenuItems( char const *filename, struct Menu *menu );

/**
    Sorts the MenuItems in the given Menu and then prints them.
    
    @param *menu Menu to print
    @param *compare pointer to a function that will be passed to qsort()
    @param *test pointer to a function that will filter based on category
    @param *str pointer to standard input string
  */
void listMenuItems( struct Menu *menu, int (*compare)( void const *va, void const *vb ), 
bool (*test)( struct MenuItem const *item, char const *str ), char const *str );
