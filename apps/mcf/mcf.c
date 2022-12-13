/**************************************************************************
MCF.H of ZIB optimizer MCF, SPEC version

This software was developed at ZIB Berlin. Maintenance and revisions 
solely on responsibility of Andreas Loebel

Dr. Andreas Loebel
Ortlerweg 29b, 12207 Berlin

Konrad-Zuse-Zentrum fuer Informationstechnik Berlin (ZIB)
Scientific Computing - Optimization
Takustr. 7, 14195 Berlin-Dahlem

Copyright (c) 1998-2000 ZIB.           
Copyright (c) 2000-2002 ZIB & Loebel.  
Copyright (c) 2003-2005 Andreas Loebel.
**************************************************************************/
/*  LAST EDIT: Thu Feb 17 22:10:51 2005 by Andreas Loebel (boss.local.de)  */
/*  $Id: mcf.c,v 1.15 2005/02/17 21:43:12 bzfloebe Exp $  */



#include "mcf.h"
#include "common.h"
#include "cache.h"

#define REPORT

extern long min_impl_duration;
network_t net;


#ifdef _PROTO_
long global_opt( void )
#else
long global_opt( )
#endif
{
    long new_arcs;
    long residual_nb_it;
    

    new_arcs = -1;
    residual_nb_it = net.n_trips <= MAX_NB_TRIPS_FOR_SMALL_NET ?
        MAX_NB_ITERATIONS_SMALL_NET : MAX_NB_ITERATIONS_LARGE_NET;

    while( new_arcs )
    {
        uint64_t opt_start = getCurNs();
#ifdef REPORT
        printf( "active arcs                : %ld\n", net.m );
#endif

        primal_net_simplex( &net );

        uint64_t simplex_end = getCurNs();

#ifdef REPORT
        printf( "simplex iterations         : %ld\n", net.iterations );
        printf( "objective value            : %0.0f\n", flow_cost(&net) );
        printf( "simplex %f us\n", (simplex_end - opt_start)/1e3 );
#endif


#if defined AT_HOME
        printf( "%ld residual iterations\n", residual_nb_it );
#endif

        if( !residual_nb_it )
            break;


        if( net.m_impl )
        {
          new_arcs = suspend_impl( &net, (cost_t)-1, 0 );
          uint64_t if_suspend_end = getCurNs();
#ifdef REPORT
          if( new_arcs ) {
            printf( "erased arcs                : %ld\n", new_arcs );
            printf( "suspend %f us\n", (if_suspend_end - simplex_end)/1e3 );
          }
#endif
        }
        uint64_t suspend_check_point = getCurNs();

        new_arcs = price_out_impl( &net );

        uint64_t price_out_end = getCurNs();

#ifdef REPORT
        if( new_arcs ) {
            printf( "new implicit arcs          : %ld\n", new_arcs );
            printf( "price out %f us\n", (price_out_end - suspend_check_point)/1e3 );
        }
#endif
        
        if( new_arcs < 0 )
        {
#ifdef REPORT
            printf( "not enough memory, exit(-1)\n" );
#endif

            exit(-1);
        }

        uint64_t opt_end = getCurNs();
#ifndef REPORT
        printf( "\n" );
        printf("opt iter %d us\n", (opt_end - opt_start)/1e3);
#endif

        residual_nb_it--;
    }

    printf( "checksum                   : %ld\n", net.checksum );

    return 0;
}






#ifdef _PROTO_
int main( int argc, char *argv[] )
#else
int main( argc, argv )
    int argc;
    char *argv[];
#endif
{
    if( argc < 2 )
        return -1;

    init_client();
    cache_init();

    printf( "\nMCF SPEC CPU2006 version 1.10\n" );
    printf( "Copyright (c) 1998-2000 Zuse Institut Berlin (ZIB)\n" );
    printf( "Copyright (c) 2000-2002 Andreas Loebel & ZIB\n" );
    printf( "Copyright (c) 2003-2005 Andreas Loebel\n" );
    printf( "\n" );



    memset( (void *)(&net), 0, (size_t)sizeof(network_t) );
    net.bigM = (long)BIGM;

    strcpy( net.inputfile, argv[1] );
    
    if( read_min( &net ) )
    {
        printf( "read error, exit\n" );
        getfree( &net );
        return -1;
    }

    uint64_t start_us = microtime();

#ifdef REPORT
    printf( "nodes                      : %ld\n", net.n_trips );
#endif


    primal_start_artificial( &net );
    global_opt( );


#ifdef REPORT
    printf( "done\n" );
#endif

    uint64_t end_us = microtime();
    printf("Exec time wo read_min and write out %lu us\n", end_us - start_us);
    

    if( write_circulations( "mcf.out", &net ) )
    {
        getfree( &net );
        return -1;    
    }



    getfree( &net );
    return 0;
}
