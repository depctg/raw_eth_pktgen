/**************************************************************************
READMIN.C of ZIB optimizer MCF, SPEC version

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
/*  LAST EDIT: Thu Feb 17 20:44:29 2005 by Andreas Loebel (boss.local.de)  */
/*  $Id: readmin.c,v 1.16 2005/02/17 19:44:40 bzfloebe Exp $  */



#include "readmin.h"
#include "cache_init.hpp"
// #include "internal_utils.h"



// Changes
// Alloc
// ref: {Loop 1}
// ref: {Loop 2}
#ifdef _PROTO_
long read_min( network_t *net )
#else
long read_min( net )
     network_t *net;
#endif
{                                       
    char instring[201];
    long t, h, c;
    long i;
    arc_t *arc;
    node_t *node;
    FILE *in;

    if(( in = fopen( net->inputfile, "r")) == NULL )
        return -1;
    // if ( !init_file(net->inputfile) )
    //     return -1;

    fgets( instring, 200, in );
    if( sscanf( instring, "%ld %ld", &t, &h ) != 2 )
        return -1;
    // if ( !read_l_l(&t, &h) )
    //     return -1;

    net->n_trips = t;
    net->m_org = h;
    net->n = (t+t+1); 
    net->m = (t+t+t+h);

    if( net->n_trips <= MAX_NB_TRIPS_FOR_SMALL_NET )
    {
        net->max_m = net->m;
        net->max_new_m = MAX_NEW_ARCS_SMALL_NET;
    }
    else
    {
#ifdef SPEC_STATIC
/*
      //net->max_m = 0x1c00000l;
*/
      net->max_m = 0x1a10000l;
#else
      net->max_m = MAX( net->m + MAX_NEW_ARCS, STRECHT(STRECHT(net->m)) );
#endif
      net->max_new_m = MAX_NEW_ARCS_LARGE_NET;
    }

    net->max_residual_new_m = net->max_m - net->m;


    assert( net->max_new_m >= 3 );

    
    // net->nodes      = (node_t *) calloc( net->n + 1, sizeof(node_t) );
    // net->dummy_arcs = (arc_t *)  calloc( net->n,   sizeof(arc_t) );
    // net->arcs       = (arc_t *)  calloc( net->max_m,   sizeof(arc_t) );
    // net->arcs       = (arc_t *)  calloc( 32 << 20,   sizeof(arc_t) );
    net->dummy_arcs = (arc_t *) C1R::alloc(sizeof(arc_t) * net->n);
    net->arcs = (arc_t *) C1R::alloc(sizeof(arc_t) * (32ULL << 20));
    net->nodes      = (node_t *) (C1R::alloc( net->n + 1 * sizeof(node_t) ) - (uint64_t)peermr.addr + (uint64_t)_rbuf);

    // if( !( net->nodes && net->arcs && net->dummy_arcs ) )
    if( !( net->nodes ))
    {
      printf( "read_min(): not enough memory\n" );
      getfree( net );
      return -1;
    }


#if defined AT_HOME
    printf( "malloc for nodes       MB %4ld\n", 
            (long)((net->n + 1)*sizeof(node_t) / 0x100000) );
    printf( "malloc for dummy arcs  MB %4ld\n", 
            (long)((net->n)*sizeof(arc_t) / 0x100000) );
    printf( "malloc for arcs        MB %4ld\n", 
            (long)((net->max_m)*sizeof(arc_t) / 0x100000) );
    printf( "------------------------------\n" );
    printf( "heap about             MB %4ld\n\n", 
            (long)((net->n +1)*sizeof(node_t) / 0x100000)
            +(long)((net->n)*sizeof(arc_t) / 0x100000)
            +(long)((net->max_m)*sizeof(arc_t) / 0x100000)
            );
#endif


    net->stop_nodes = net->nodes + net->n + 1; 
    net->stop_arcs  = net->arcs + net->m; 
    net->stop_dummy = net->dummy_arcs + net->n;


    node = net->nodes; // 81
    arc = net->arcs; // 82

    /*          
        %arg1 = %true, %arg2 = %24, %arg3 = %true, %arg4 (i) = %c1_i64, %arg5 = %82

        scf.condition(%87) %arg1, %arg2, %arg4, %arg5

        ^bb0(%arg1: i1, %arg2: i64, %arg3: i64, %arg4: !rmem.rmref<1
    */

    // label {Loop 1}
      for( i = 1; i <= net->n_trips; i++ )
    {
        fgets( instring, 200, in );

        if( sscanf( instring, "%ld %ld", &t, &h ) != 2 || t > h )
            return -1;
        // if ( !read_l_l(&t, &h) || t > h )
        //     return -1;

        node[i].number = -i;
        node[i].flow = (flow_t)-1;
            
        node[i+net->n_trips].number = i;
        node[i+net->n_trips].flow = (flow_t)1;
        
        node[i].time = t;
        node[i+net->n_trips].time = h;

        arc_t *r_arc = C1R::get_mut<arc_t>(arc);
        r_arc->tail = &(node[net->n]);
        r_arc->head = &(node[i]);
        r_arc->org_cost = r_arc->cost = (cost_t)(net->bigM+15);
        r_arc->nextout = r_arc->tail->firstout;
        r_arc->tail->firstout = arc;
        r_arc->nextin = r_arc->head->firstin;
        r_arc->head->firstin = arc; 
        arc++;

        r_arc = C1R::get_mut<arc_t>(arc);                    
        r_arc->tail = &(node[i+net->n_trips]);
        r_arc->head = &(node[net->n]);
        r_arc->org_cost = r_arc->cost = (cost_t)15;
        r_arc->nextout = r_arc->tail->firstout;
        r_arc->tail->firstout = arc;
        r_arc->nextin = r_arc->head->firstin;
        r_arc->head->firstin = arc; 
        arc++;

        r_arc = C1R::get_mut<arc_t>(arc);                    
        r_arc->tail = &(node[i]);
        r_arc->head = &(node[i+net->n_trips]);
        r_arc->org_cost = r_arc->cost = (cost_t)(2*MAX(net->bigM,(long)BIGM));
        r_arc->nextout = r_arc->tail->firstout;
        r_arc->tail->firstout = arc;
        r_arc->nextin = r_arc->head->firstin;
        r_arc->head->firstin = arc; 
        arc++;
    }

    
    if( i != net->n_trips + 1 )
        return -1;


    for( i = 0; i < net->m_org; i++, arc++ )
    {
        fgets( instring, 200, in );
        
        if( sscanf( instring, "%ld %ld %ld", &t, &h, &c ) != 3 )
                return -1;
        // if ( !read_l_l_l(&t, &h, &c) )
        //     return -1;
        arc_t *r_arc = C1R::get_mut<arc_t>(arc);                    
        r_arc->tail = &(node[t+net->n_trips]);
        r_arc->head = &(node[h]);
        r_arc->org_cost = (cost_t)c;
        r_arc->cost = (cost_t)c;
        r_arc->nextout = r_arc->tail->firstout;
        r_arc->tail->firstout = arc;
        r_arc->nextin = r_arc->head->firstin;
        r_arc->head->firstin = arc; 
    }


    if( net->stop_arcs != arc )
    {
        net->stop_arcs = arc;
        arc = net->arcs;
        for( net->m = 0; arc < net->stop_arcs; arc++ )
            (net->m)++;
        net->m_org = net->m;
    }
    
    fclose( in );
    // finish_reading();


    net->clustfile[0] = (char)0;
    
    // label: Loop 2
    for( i = 1; i <= net->n_trips; i++ )
    {
        arc_t *r_arc3i1 = C1R::get_mut<arc_t>(&net->arcs[3*i-1]);
        r_arc3i1->cost = 
            (cost_t)((-2)*MAX(net->bigM,(long) BIGM));
        r_arc3i1->org_cost = 
            (cost_t)((-2)*(MAX(net->bigM,(long) BIGM)));
    }
    
    
    return 0;
}
