/**************************************************************************
IMPLICIT.C of ZIB optimizer MCF, SPEC version

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
/*  LAST EDIT: Thu Feb 17 21:45:45 2005 by Andreas Loebel (boss.local.de)  */
/*  $Id: implicit.c,v 1.20 2005/02/17 21:43:12 bzfloebe Exp $  */



#include "implicit.h"
#include "common.h"
#include "cache_init.hpp"
#include <stdint.h>


#ifdef _PROTO_
long resize_prob( network_t *net )
#else
long resize_prob( net )
     network_t *net;
#endif
{
    arc_t *arc;
    node_t *node, *stop, *root;
    size_t off;
    
    
    assert( net->max_new_m >= 3 );


    net->max_m += net->max_new_m;
    net->max_residual_new_m += net->max_new_m;
    

#if defined AT_HOME
    printf( "\nresize arcs to %4ld MB (%ld elements a %d B)\n\n",
            net->max_m * sizeof(arc_t) / 0x100000,
            net->max_m,
            sizeof(arc_t) );
    fflush( stdout );
#endif

    arc = net->arcs;
    // arc = (arc_t *) realloc( net->arcs, net->max_m * sizeof(arc_t) );
    if( !arc )
    {
        printf( "network %s: not enough memory\n", net->inputfile );
        fflush( stdout );
        return -1;
    }
    
    off = (size_t)arc - (size_t)net->arcs;
        
    net->arcs = arc;
    net->stop_arcs = arc + net->m;

    root = node = net->nodes;
    for( node++, stop = net->stop_nodes; node < stop; node++ )
        if( node->pred != root )
            node->basic_arc = (arc_t *)((size_t)node->basic_arc + off);
        
    return 0;
}





#ifdef _PROTO_
void insert_new_arc( arc_t *rnew, long newpos, node_t *tail, node_t *head,
                     cost_t cost, cost_t red_cost )
#else
void insert_new_arc( new, newpos, tail, head, cost, red_cost )
     arc_t *new;
     long newpos;
     node_t *tail;
     node_t *head;
     cost_t cost;
     cost_t red_cost;
#endif
{
    long pos;
    
    arc_t *new_l = (arc_t *) C1R::get_mut<arc_t>(rnew);

    new_l->tail      = tail;
    new_l->head      = head;
    new_l->org_cost  = cost;
    new_l->cost      = cost;
    new_l->flow      = (flow_t)red_cost; 
    
    pos = newpos+1;

    arc_t *new_pos21 = (arc_t *) C1R::get_mut<arc_t>(rnew + pos/2 - 1);

    while( pos-1 && red_cost > (cost_t)new_pos21->flow )
    {
        arc_t *new_pos1 = (arc_t *) C1R::get_mut<arc_t>(rnew + pos - 1);

        new_pos1->tail     = new_pos21->tail;
        new_pos1->head     = new_pos21->head;
        new_pos1->cost     = new_pos21->cost;
        new_pos1->org_cost = new_pos21->cost;
        new_pos1->flow     = new_pos21->flow;
        
        pos = pos/2;

        new_pos21->tail     = tail;
        new_pos21->head     = head;
        new_pos21->cost     = cost;
        new_pos21->org_cost = cost;
        new_pos21->flow     = (flow_t)red_cost; 

        new_pos21 = C1R::get_mut<arc_t>(rnew + pos/2 - 1);
    }

    return;
}   



#ifdef _PROTO_
void replace_weaker_arc( network_t *net, arc_t *rnew, node_t *tail, node_t *head,
                         cost_t cost, cost_t red_cost )
#else
void replace_weaker_arc( net, new, tail, head, cost, red_cost )
     network *net;
     arc_t *new;
     node_t *tail;
     node_t *head;
     cost_t cost;
     cost_t red_cost;
#endif
{
    long pos;
    long cmp;

    arc_t *new_0 = (arc_t *) C1R::get_mut<arc_t>(rnew); 

    new_0->tail     = tail;
    new_0->head     = head;
    new_0->org_cost = cost;
    new_0->cost     = cost;
    new_0->flow     = (flow_t)red_cost; 
                    
    pos = 1;

    
    arc_t *new_1 = C1R::get_mut<arc_t>(rnew + 1);
    arc_t *new_2 = C1R::get_mut<arc_t>(rnew + 2);

    cmp = (new_1->flow > new_2->flow) ? 2 : 3;

    arc_t *new_cmp1 = (arc_t *) C1R::get_mut<arc_t>(rnew + cmp - 1);

    while( cmp <= net->max_residual_new_m && red_cost < new_cmp1->flow )
    {
      arc_t *new_pos1 = C1R::get_mut<arc_t>(rnew + pos - 1);

        new_pos1->tail =     new_cmp1->tail;
        new_pos1->head =     new_cmp1->head;
        new_pos1->cost =     new_cmp1->cost;
        new_pos1->org_cost = new_cmp1->cost;
        new_pos1->flow =     new_cmp1->flow;
        
        new_cmp1->tail = tail;
        new_cmp1->head = head;
        new_cmp1->cost = cost;
        new_cmp1->org_cost = cost;
        new_cmp1->flow = (flow_t)red_cost; 

        pos = cmp;
        cmp *= 2;
        if( cmp + 1 <= net->max_residual_new_m ) {
            new_cmp1 = C1R::get_mut<arc_t>(rnew + cmp - 1);
            arc_t *new_cmp = C1R::get_mut<arc_t>(rnew + cmp);
            if( new_cmp1->flow < new_cmp->flow ) 
                cmp++;
        } 

        new_cmp1 = (arc_t *) C1R::get_mut<arc_t>(rnew + cmp - 1);
    }
    
    return;
}   




#if defined AT_HOME
#include <sys/time.h>
double Get_Time( void  ) 
{
    struct timeval tp;
    struct timezone tzp;
    if( gettimeofday( &tp, &tzp ) == 0 )
        return (double)(tp.tv_sec) + (double)(tp.tv_usec)/1.0e6;
    else
        return 0.0;
}
static double wall_time = 0; 
#endif


#ifdef _PROTO_
long price_out_impl( network_t *net )
#else
long price_out_impl( net )
     network_t *net;
#endif
{
    long i;
    long trips;
    long new_arcs = 0;
    long resized = 0;
    long latest;
    long min_impl_duration = 15;

    cost_t bigM = net->bigM;
    cost_t head_potential;
    cost_t arc_cost = 30;
    cost_t red_cost;
    cost_t bigM_minus_min_impl_duration;
    
    arc_t *arcout, *arcin, *arcnew, *stop;
    arc_t *first_of_sparse_list;
    node_t *tail, *head;


#if defined AT_HOME
    wall_time -= Get_Time();
#endif

    
    bigM_minus_min_impl_duration = (cost_t)bigM - min_impl_duration;
    

    if( net->n_trips <= MAX_NB_TRIPS_FOR_SMALL_NET )
    {
      if( net->m + net->max_new_m > net->max_m 
          &&
          (net->n_trips*net->n_trips)/2 + net->m > net->max_m
          )
      {
        resized = 1;
        if( resize_prob( net ) )
          return -1;
        refresh_neighbour_lists( net );
      }
    }
#if !defined SPEC_STATIC
    else
    {
      if( net->m + net->max_new_m > net->max_m 
          &&
          (net->n_trips*net->n_trips)/2 + net->m > net->max_m
          )
      {
        resized = 1;
        if( resize_prob( net ) )
          return -1;
        
        refresh_neighbour_lists( net );
      }
    }
#endif

        
    arcnew = net->stop_arcs;
    trips = net->n_trips;

    arcout = net->arcs;

    arc_t *arcout1 = C1R::get_mut<arc_t>(arcout + 1);

    // for( i = 0; i < trips && arcout[1].ident == FIXED; i++, arcout += 3 );

    for (i = 0; i < trips; i ++) {
        if (arcout1->ident != FIXED)
            break;
        arcout += 3;
        arcout1 = C1R::get_mut<arc_t>(arcout + 1);
    }
    first_of_sparse_list = (arc_t *)NULL;

    for( ; i < trips; i++, arcout += 3)
    {
        arcout1 = C1R::get_mut<arc_t>(arcout + 1); 
        if( arcout1->ident != FIXED )
        {
            arc_t *r_arcout = C1R::get_mut<arc_t>(arcout); 
            arc_t *r_fistout = C1R::get_mut<arc_t>(r_arcout->head->firstout);

            r_fistout->head->arc_tmp = first_of_sparse_list;
            first_of_sparse_list = arcout + 1;
        }
        arc_t *r_arcout = C1R::get_mut<arc_t>(arcout); 
        if( r_arcout->ident == FIXED )
            continue;
        
        head = r_arcout->head;
        latest = head->time - r_arcout->org_cost 
            + (long)bigM_minus_min_impl_duration;
                
        head_potential = head->potential;
        
        arc_t *r_first = C1R::get_mut<arc_t>(first_of_sparse_list);
        arcin = r_first->tail->arc_tmp;
        while( arcin )
        {   
            // printf("%p\n", arcin);
            arc_t *r_arcin = C1R::get_mut<arc_t>(arcin);
            tail = r_arcin->tail;

            if( tail->time + r_arcin->org_cost > latest )
            {
                arcin = tail->arc_tmp;
                continue;
            }
            
            red_cost = arc_cost - tail->potential + head->potential;
            
            if( red_cost < 0 )
            {
                // printf("insert or replace\n");
                if( new_arcs < net->max_residual_new_m )
                {
                    insert_new_arc( arcnew, new_arcs, tail, head, 
                                    arc_cost, red_cost );
                    new_arcs++;                 
                }
                else {
                    arc_t *r_arcnew = C1R::get_mut<arc_t>(arcnew);
                    if( (cost_t)r_arcnew->flow > red_cost ) {
                        replace_weaker_arc( net, arcnew, tail, head, 
                                            arc_cost, red_cost );
                    }
                } 
            }
            // printf("get out\n");
            arcin = tail->arc_tmp;
        }
    }
    if( new_arcs )
    {
        arcnew = net->stop_arcs;
        net->stop_arcs += new_arcs;
        stop = net->stop_arcs;
        if( resized )
        {
            for( ; arcnew != stop; arcnew++ )
            {
                arc_t *r_arcnew = C1R::get_mut<arc_t>(arcnew);
                r_arcnew->flow = (flow_t)0;
                r_arcnew->ident = AT_LOWER;
            }
        }
        else
        {
            for( ; arcnew != stop; arcnew++ )
            {
                arc_t *r_arcnew = C1R::get_mut<arc_t>(arcnew);
                r_arcnew->flow = (flow_t)0;
                r_arcnew->ident = AT_LOWER;
                r_arcnew->nextout = r_arcnew->tail->firstout;
                r_arcnew->tail->firstout = arcnew;
                r_arcnew->nextin = r_arcnew->head->firstin;
                r_arcnew->head->firstin = arcnew;
            }
        }
        
        net->m += new_arcs;
        net->m_impl += new_arcs;
        net->max_residual_new_m -= new_arcs;
    }
    

#if defined AT_HOME
    wall_time += Get_Time();
    printf( "total time price_out_impl(): %0.0f\n", wall_time );
#endif


    return new_arcs;
}   




#ifdef _PROTO_
long suspend_impl( network_t *net, cost_t threshold, long all )
#else
long suspend_impl( net, threshold, all )
     network_t *net;
     cost_t threshold;
     long all;
#endif
{
    long susp;
    
    cost_t red_cost;
    arc_t *new_arc, *arc;
    void *stop;

    

    if( all )
        susp = net->m_impl;
    else
    {
        stop = (void *)net->stop_arcs;
        new_arc = &(net->arcs[net->m - net->m_impl]);
        for( susp = 0, arc = new_arc; arc < (arc_t *)stop; arc++ )
        {
            arc_t *r_arc = C1R::get_mut<arc_t>(arc);
            if( r_arc->ident == AT_LOWER )
                red_cost = r_arc->cost - r_arc->tail->potential 
                        + r_arc->head->potential;
            else
            {
                red_cost = (cost_t)-2;
                
                if( r_arc->ident == BASIC )
                {
                    if( r_arc->tail->basic_arc == arc )
                        r_arc->tail->basic_arc = new_arc;
                    else
                        r_arc->head->basic_arc = new_arc;
                }
            }
            
            if( red_cost > threshold )
                susp++;
            else
            {
                arc_t *r_new_arc = C1R::get_mut<arc_t>(new_arc);
                *r_new_arc = *r_arc;
                new_arc++;
            }
        }
    }
    
        
#if defined AT_HOME
    printf( "\nremove %ld arcs\n\n", susp );
    fflush( stdout );
#endif

    if( susp )
    {
        net->m -= susp;
        net->m_impl -= susp;
        net->stop_arcs -= susp;
        net->max_residual_new_m += susp;
        
        refresh_neighbour_lists( net );
    }

    return susp;
}



