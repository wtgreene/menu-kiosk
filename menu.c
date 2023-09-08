/**
    @filename menu.c
    @author Will Greene (wgreene)
    
    Creates a Menu, reads and prints MenuItems, and frees memory.
  */
#include "menu.h"
#include "input.h"

/**
    Allocates storage for a (the) Menu, and initializes its fields.
    
    @return the Menu
  */
struct Menu *makeMenu() {

    struct Menu *menu = ( struct Menu * ) malloc( sizeof( struct Menu ) );
    
    menu->list = ( struct MenuItem ** ) malloc(MENU_INITIAL_CAPACITY * sizeof( struct MenuItem * ));
    menu->count = 0;
    menu->capacity = MENU_INITIAL_CAPACITY;
    
    return menu;
}

/**
    Frees the memory used to store the given Menu.
    
    @param *menu Menu to be freed
  */
void freeMenu( struct Menu *menu ) {

    for ( int i = 0; i < menu->count; i++ ) {
        free( menu->list[ i ] );
    }
    
    free( menu->list );
    free( menu );
}

/**
    Reads all MenuItems from a file with the given name.
    
    @param *filename name of file to read from
    @param *menu Menu to add to
  */
void readMenuItems( char const *filename, struct Menu *menu ) {
    
    FILE *fp = fopen( filename, "r" );
    
    if ( !fp ) {
        fprintf( stderr, "Can't open file: %s\n", filename );
        exit( EXIT_FAILURE );
    }
        
    char *str = readLine( fp );
    
    while ( str ) {
    
        char *strPlaceholder = str;
    
        // capacity check ( double if at or above capacity )
        if ( menu->count >= menu->capacity ) {
            menu->capacity *= 2;
            menu->list = realloc( menu->list, sizeof( struct MenuItem * ) * menu->capacity );
        }
        
        // malloc
        menu->list[ menu->count ] = (struct MenuItem *) calloc( 1, sizeof( struct MenuItem ) );
        
        // id assignment
        int pos1 = 0;
        sscanf( str, "%s%n", menu->list[ menu->count ]->id, &pos1 );
        
        for ( int i = 0; i < NUM_CHAR_ID; i++ ) {
        
            if ( menu->list[ menu->count ]->id[ 0 ] == '\0' ||
                 menu->list[ menu->count ]->id[ 1 ] == '\0' ||
                 menu->list[ menu->count ]->id[ 2 ] == '\0' ||
                 menu->list[ menu->count ]->id[ 3 ] == '\0' ||
                 menu->list[ menu->count ]->id[ 4 ] != '\0' ) {
             
                fprintf( stderr, "Invalid menu file: %s\n", filename );
                exit( EXIT_FAILURE );
            }
        }
        
        for ( int i = 0; i < menu->count; i++ ) {
            if ( strcmp( menu->list[ menu->count ]->id, menu->list[ i ]->id ) == 0 ) {
                fprintf( stderr, "Invalid menu file: %s\n", filename );
                exit( EXIT_FAILURE );
            }
        }
        
        // category assignment
        int pos2 = 0;
        sscanf( str + pos1, "%s%n", menu->list[ menu->count ]->category, &pos2 );
        
        bool good = false;
        for ( int i = 0; i <= MAX_NUM_CHAR_CATEGORY; i++ ) {
            if ( menu->list[ menu->count ]->category[ i ] == '\0' )
                good = true;
        }
        
        if ( !good ) {
            fprintf( stderr, "Invalid menu file: %s\n", filename );
            exit( EXIT_FAILURE );
        }
        
        // cost assignment
        int pos3 = 0;
        sscanf( str + pos1 + pos2, "%d%n", &menu->list[ menu->count ]->cost, &pos3 );
        
        if ( menu->list[ menu->count ]->cost <= 0 ) {
            fprintf( stderr, "Invalid menu file: %s\n", filename );
            exit( EXIT_FAILURE );
        }
        
        // name assignment
        strcpy( str, str + pos1 + pos2 + pos3 );
        
        while ( *str == ' ' )
            str++;
            
        if ( *str == '\n' || *str == EOF ) {
            fprintf( stderr, "Invalid menu file: %s\n", filename );
            exit( EXIT_FAILURE );
        }
        
        int i = 0;
        int lastChar = 0;
        while ( *str && *str != '\n' && *str != EOF && i < MAX_NUM_CHAR_NAME ) {
            if ( i == MAX_NUM_CHAR_NAME - 1 && *str && *str != '\n' && *str != EOF ) {
                fprintf( stderr, "Invalid menu file: %s\n", filename );
                exit( EXIT_FAILURE );
            }
            menu->list[ menu->count ]->name[ i ] = *str;
            str++;
            i++;
            lastChar = i;
        }
        
        menu->list[ menu->count ]->name[ lastChar ] = '\0';
        
        (menu->count)++;

        str = strPlaceholder;
        free( str );
        str = readLine( fp );
    }
    
    fclose( fp );
}

/**
    Sorts the MenuItems in the given Menu and then prints them.
    
    @param *menu Menu to print
    @param *compare pointer to a function that will be passed to qsort()
    @param *test pointer to a function that will filter based on category
    @param *str pointer to standard input string
  */
void listMenuItems( struct Menu *menu, int (*compare)( void const *va, void const *vb ), 
bool (*test)( struct MenuItem const *item, char const *str ), char const *str ) {

    qsort( menu->list, menu->count, sizeof( menu->list[ 0 ] ), compare );
    
    if ( strcmp( "list menu", str ) == 0 ) {
            
        while ( *str ) {
            putchar( *str );
            str++;
        }
        
        putchar( '\n' );
        
        printf( "ID   Name                 Category        Cost\n" );
    
        for ( int i = 0; i < menu->count; i++ ) {
        
            printf( "%-5s",  menu->list[ i ]->id );
            printf( "%-21s", menu->list[ i ]->name );
            printf( "%-16s", menu->list[ i ]->category );
            
            float cost = menu->list[ i ]->cost / CENTS_IN_A_DOLLAR;
            printf( "$%6.2f\n", cost );
        }
    }
    
    else {
    
        printf( "list category %s\n", str );
        printf( "ID   Name                 Category        Cost\n" );
        
        for ( int i = 0; i < menu->count; i++ ) {
        
            if ( test( menu->list[ i ], str ) ) {
                
                printf( "%-5s",  menu->list[ i ]->id );
                printf( "%-21s", menu->list[ i ]->name );
                printf( "%-16s", menu->list[ i ]->category );
                
                float cost = menu->list[ i ]->cost / CENTS_IN_A_DOLLAR;
                printf( "$%6.2f\n", cost );
            }
        }
    }
    
    printf( "\n" );
}
