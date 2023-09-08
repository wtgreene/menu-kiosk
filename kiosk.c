/**
    @filename kiosk.c
    @author Will Greene (wgreene)
    
    Controls functionality for a user interacting with the kiosk to create and
    manipulate an order.
  */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "input.h"
#include "menu.h"

/** number of required arguments at the end of the command line. */
#define REQUIRED_ARGS 1

/** initial number of Order array elements */
#define ORDER_INITIAL_CAPACITY 5

/** maximum number of characters for user input */
#define MAX_NUM_CHARS_INPUT 100

/**
    An order item.
  */
struct OrderItem {
    struct MenuItem *menuItem; // order item characteristics ( pointer to a MenuItem struct )
    int quantity;              // quantity of this type of order item
};

/**
    An order.
  */
struct Order {
    struct OrderItem **list; // list of order items
    int count;               // number of order items
    int capacity;            // capacity of the list
};

/**
    Helper function for qsort(). Compares 2 pointers ( MenuItem's in this case ) to
    determine order ( based on category, then id, in this case ).
    
    @param *aptr void pointer ( to a MenuItem in this case )
    @param *bptr void pointer ( to a MenuItem in this case )
    @return a negative number if *aptr comes before *bptr,
            a positive number if *bptr comes before *aptr,
            and 0 if the pointers are identical
  */
int listMenuComp( const void *aptr, const void *bptr )
{
    const struct MenuItem *a = *(const struct MenuItem **) aptr;
    const struct MenuItem *b = *(const struct MenuItem **) bptr;
    
    if ( strcmp( a->category, b->category ) != 0 )
        return strcmp( a->category, b->category );
    
    if ( strcmp( a->id, b->id ) != 0 )
        return strcmp( a->id, b->id );
        
    return 0;
}

/**
    Helper function for qsort(). Compares 2 pointers ( MenuItem's in this case ) to
    determine order ( based on id, in this case ).
        
    @param *aptr void pointer ( to a MenuItem in this case )
    @param *bptr void pointer ( to a MenuItem in this case )
    @return a negative number if *aptr comes before *bptr,
            a positive number if *bptr comes before *aptr,
            and 0 if the pointers are identical
  */
int listCategoryComp( const void *aptr, const void *bptr )
{
    const struct MenuItem *a = *(const struct MenuItem **) aptr;
    const struct MenuItem *b = *(const struct MenuItem **) bptr;
            
    return strcmp( a->id, b->id );
}

/**
    Helper function for qsort(). Compares 2 pointers ( OrderItem's in this case ) to
    determine order ( based on cost * quantity, then id, in this case ).
    
    @param *va void pointer ( to an OrderItem in this case )
    @param *vb void pointer ( to an OrderItem in this case )
    @return a negative number if *va comes before *vb,
            a positive number if *vb comes before *va,
            and 0 if the pointers are identical
  */
int listOrderComp( void const *va, void const *vb )
{
    const struct OrderItem *a = *(const struct OrderItem **) va;
    const struct OrderItem *b = *(const struct OrderItem **) vb;
    
    if ( a->menuItem->cost * a->quantity > b->menuItem->cost * b->quantity )
        return -1;
    
    if ( a->menuItem->cost * a->quantity < b->menuItem->cost * b->quantity )
        return 1;
    
    return strcmp( a->menuItem->id, b->menuItem->id );
}

/**
    Determines whether a MenuItem belongs to the given category.
    
    @param *item MenuItem to compare to category
    @param *str category
    @return true if it belongs, false if not
  */
bool isCategory( struct MenuItem const *item, char const *str )
{
    char const *p1 = item->category;
    char const *p2 = str;

    while ( *p2 ) {
    
        if ( *p1 != *p2 )
            return false;
            
        p1++;
        p2++;
    }
    
    if ( *p1 != *p2 )
        return false;
    
    return true;
}

/**
    Sorts the OrderItems in the given Order and then prints them.
    
    @param *order Order to print
    @param *compare pointer to a function that will be passed to qsort()
  */
void listOrderItems( struct Order *order, int (*compare)( void const *va, void const *vb )) {
    
    qsort( order->list, order->count, sizeof( order->list[ 0 ] ), compare );
    
    printf( "ID   Name                 Quantity Category        Cost\n" );
    
    if ( order->count == 0 )
        printf( "Total                                              $  0.00\n\n");
    
    else {
        
        float sumCost = 0.0;
        for ( int i = 0; i < order->count; i++ ) {
        
            printf( "%-5s",   order->list[ i ]->menuItem->id );
            printf( "%-21s",  order->list[ i ]->menuItem->name );
            printf( "%8d ",   order->list[ i ]->quantity );
            printf( "%-16s",  order->list[ i ]->menuItem->category );
            
            float cost = order->list[ i ]->menuItem->cost * order->list[ i ]->quantity / 
                         CENTS_IN_A_DOLLAR;
                         
            printf( "$%6.2f\n", cost );
            
            sumCost += cost;
        }
        
        printf( "Total                                              $%6.2f\n\n", sumCost );
    }
}

/**
    Starting point. Contains command line error checking. Contains functionality for
    interacting with the program until the user / input selects to quit the program.
    
    @param argc number of arguments
    @param *argv[] array of pointers to command line arguments
    @return exit status
  */
int main( int argc, char *argv[] ) {
    
    // parameter error checking
    if ( argc < REQUIRED_ARGS + 1 ) {
        fprintf( stderr, "usage: kiosk <menu-file>*\n" );
        exit( EXIT_FAILURE );
    }
    
    struct Menu *menu = makeMenu();
    
    for ( int i = 1; i < argc; i++ )
        readMenuItems( argv[ i ], menu );
        
    struct Order *order = (struct Order *) malloc( sizeof( struct Order ) );
    
    order->list = ( struct OrderItem ** ) malloc(ORDER_INITIAL_CAPACITY * 
                  sizeof( struct OrderItem * ));
                  
    order->count = 0;
    order->capacity = ORDER_INITIAL_CAPACITY;
    
    // command 1 options
    char cList[] = "list";
    char cAdd[] = "add";
    char cRemove[] = "remove";
    char cQuit[] = "quit";
    
    // command 2 options ( if command 1 is "list" )
    char cMenu[] = "menu";
    char cCategory[] = "category";
    char cOrder[] = "order";
    
    bool quit = false;
    
    int itemRemoved = 0;
    
    while ( !quit ) {
    
        char input [ MAX_NUM_CHARS_INPUT ] = {};
        char input1[ MAX_NUM_CHARS_INPUT ] = {};
        char input2[ MAX_NUM_CHARS_INPUT ] = {};
        char input3[ MAX_NUM_CHARS_INPUT ] = {};
        
        printf( "cmd> " );
        
        char ch;
        int idx = 0;
        while( scanf( "%c", &ch ) == 1 ) {
        
            if ( ch == '\n' )
                goto out;
                
            input[ idx ] = ch;
            idx++;
        }
        
        goto end;
        
        out:
        input[ idx ] = '\0';
        
        int pos2 = 0;
        sscanf( input, "%s%n", input1, &pos2 );
        
        int pos3 = 0;
        sscanf( input + pos2, "%s%n", input2, &pos3 );
        
        sscanf( input + pos2 + pos3, "%s", input3 );
        
        if ( strcmp( input1, cList ) == 0 && strcmp( input2, cMenu ) == 0 && input3[ 0 ] != '\0' )
            goto else1;
        
        // list ( print ) items
        if ( strcmp( input1, cList ) == 0 ) {
            if ( strcmp( input2, cMenu ) == 0 )
                listMenuItems( menu, *listMenuComp, isCategory, input );
            else if ( strcmp( input2, cCategory ) == 0 )
                listMenuItems( menu, listCategoryComp, isCategory, input3 );
            else if ( strcmp( input2, cOrder ) == 0 ) {
                printf( "%s\n", input );
                listOrderItems( order, listOrderComp );
            } else
                printf( "Invalid command\n" );
        }
            
        // add item
        else if ( strcmp( input1, cAdd ) == 0 ) {
        
            // if already in order
            for ( int i = 0; i < order->count; i++ ) {
                if ( strcmp( input2, order->list[ i ]->menuItem->id ) == 0 ) {
                    int q = atoi( input3 );
                    if ( q < 1 ) {
                        printf( "Invalid command\n" );
                        goto print2;
                    } else {
                        order->list[ i ]->quantity += q;
                        goto print1;
                    }
                }
            }
            
            bool found = false;
            // if not already in order
            for ( int i = 0; i < menu->count; i++ ) {
                if ( strcmp( input2, menu->list[ i ]->id ) == 0 ) {
                    int q = atoi( input3 );
                    if ( q < 1 ) {
                        printf( "Invalid command" );
                        goto print2;
                    } else {
                        order->list[ order->count ] = (struct OrderItem *)
                                                      malloc( sizeof( struct OrderItem ) );
                                                      
                        order->list[ order->count ]->quantity = q;
                        order->list[ order->count ]->menuItem = menu->list[ i ];
                        (order->count)++;
                        itemRemoved = 0;
                        if ( order->count >= order->capacity ) {
                            order->capacity *= 2;
                            order->list = realloc( order->list, sizeof( struct OrderItem * ) *
                                          order->capacity );
                        }
                    }
                    
                    found = true;
                }
            }
            
            if ( !found )
                goto else1;
            
            print1:
            printf( input );
            printf( "\n" );
            print2:
            printf( "\n" );
        }
            
        // remove item
        else if ( strcmp( input1, cRemove ) == 0 ) {
        
            bool found2 = false;
            for ( int i = 0; i < order->count; i++ ) {
                if ( strcmp( order->list[ i ]->menuItem->id, input2 ) == 0 ) {
                    int q = atoi( input3 );
                    if ( q == order->list[ i ]-> quantity ) {
                    
                        free( order->list[ i ] );
                        for ( int j = i; j < order->count - 1; j++ )
                            order->list[ j ] = order->list[ j + 1 ];
                        order->list[ order->count - 1 ] = NULL;
                        (order->count)--;
                        itemRemoved = 1;
                        goto print3;
                        
                    } else if ( q < order->list[ i ]->quantity ) {
                        order->list[ i ]->quantity -= q;
                        goto print3;
                    
                    } else {
                        printf( "Invalid command\n" );
                        goto print4;
                    }
                }
                
                found2 = true;
            }
            
            if ( !found2 )
                goto else1;
            
            print3:
            printf( input );
            printf( "\n" );
            print4:
            printf( "\n" );
        }
            
        // quit
        else if ( strcmp( input1, cQuit ) == 0 ) {
            quit = true;
            printf( "quit\n" );
        }
        
        else {
            else1:
            printf( "%s\n", input );
            printf( "Invalid command\n\n" );
        }
    }
    
    end:
    
    for ( int i = 0; i < order->count + itemRemoved ; i++ )
        free( order->list[ i ] );
        
    free( order->list );
    free( order );
    
    freeMenu( menu );
                
    return EXIT_SUCCESS;
}
