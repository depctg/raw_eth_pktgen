/**************************************************************************
MCFUTIL.C of ZIB optimizer MCF, SPEC version

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
/*  LAST EDIT: Thu Feb 17 21:46:06 2005 by Andreas Loebel (boss.local.de)  */
/*  $Id: mcfutil.c,v 1.11 2005/02/17 21:43:12 bzfloebe Exp $  */


#include "mcfutil.h"
#include <stdint.h>
#include "cache_init.hpp"


#ifdef _PROTO_
void refresh_neighbour_lists( network_t *net )
#else
void refresh_neighbour_lists( net )
    network_t *net;
#endif
{
    node_t *node;
    arc_t *arc;
    void *stop;
        

    node = net->nodes;
    for( stop = (void *)net->stop_nodes; node < (node_t *)stop; node++ )
    {
        node->firstin = (arc_t *)NULL;
        node->firstout = (arc_t *)NULL;
    }
    
    arc = net->arcs;

// batch
// cache line size 1k

    // static unsigned eles = 16;
    // unsigned n_blocks = N / eles;
    stop = (arc_t *)net->stop_arcs;
    unsigned N = net->stop_arcs - arc;

    uint64_t local_arc = (uint64_t)C1R::get_mut<arc_t>(arc);
    uint64_t lend = ((local_arc >> 10) + 1) << 10;
    uint64_t end = (lend - local_arc) < (N * 64) ? lend : local_arc + N * 64;

    // prelogue
    for (arc_t *pre_base = (arc_t *)local_arc; (uint64_t)pre_base < end; pre_base ++, arc ++) {
        pre_base->nextout = pre_base->tail->firstout;
        pre_base->tail->firstout = arc;
        pre_base->nextin = pre_base->head->firstin;
        pre_base->head->firstin = arc;
    }

    // Steady stage
    for (; arc < stop; arc += 16) {
        arc_t *steady_base = C1R::get_mut<arc_t>(arc);
        for (int i = 0; i < 16; ++ i) {
            if (arc + i >= stop)
                return;
            steady_base[i].nextout = steady_base[i].tail->firstout;
            steady_base[i].tail->firstout = arc + i;
            steady_base[i].nextin = steady_base[i].head->firstin;
            steady_base[i].head->firstin = arc + i; 
        }
    }

    // for( stop = (void *)net->stop_arcs; arc < (arc_t *)stop; arc++ )
    // {
    //     arc_t *r_arc = cache_access_mod_opt_mut(arc);
    //     r_arc->nextout = r_arc->tail->firstout;
    //     r_arc->tail->firstout = arc;
    //     r_arc->nextin = r_arc->head->firstin;
    //     r_arc->head->firstin = arc;
    // }
    
    return;
}










#ifdef _PROTO_
long refresh_potential( network_t *net )
#else
long refresh_potential( net )
    network_t *net;
#endif
{
    node_t *node, *tmp;
    node_t *root = net->nodes;
    long checksum = 0;
    

    root->potential = (cost_t) -MAX_ART_COST;
    tmp = node = root->child;
    while( node != root )
    {
        while( node )
        {
            arc_t *r_basic_arc = C1R::get_mut<arc_t>(node->basic_arc);
            if( node->orientation == UP ) {
                node->potential = r_basic_arc->cost + node->pred->potential;
            }
            else /* == DOWN */
            {
                node->potential = node->pred->potential - r_basic_arc->cost;
                checksum++;
            }

            tmp = node;
            node = node->child;
        }
        
        node = tmp;

        while( node->pred )
        {
            tmp = node->sibling;
            if( tmp )
            {
                node = tmp;
                break;
            }
            else
                node = node->pred;
        }
    }
    
    return checksum;
}






#ifdef _PROTO_
double flow_cost( network_t *net )
#else
double flow_cost( net )
    network_t *net;
#endif
{
    arc_t *arc;
    node_t *node;
    void *stop;
    
    long fleet = 0;
    cost_t operational_cost = 0;
    

    stop = (void *)net->stop_arcs;
    for( arc = net->arcs; arc != (arc_t *)stop; arc++ )
    {
        arc_t *r_arc = C1R::get_mut<arc_t>(arc);   
        if( r_arc->ident == AT_UPPER )
            r_arc->flow = (flow_t)1;
        else
            r_arc->flow = (flow_t)0;
    }

    stop = (void *)net->stop_nodes;
    for( node = net->nodes, node++; node != (node_t *)stop; node++ ) {
        arc_t *r_basic_arc = C1R::get_mut<arc_t>(node->basic_arc);
        r_basic_arc->flow = node->flow;
    }
    
    stop = (void *)net->stop_arcs;
    for( arc = net->arcs; arc != (arc_t *)stop; arc++ )
    {  
        arc_t *r_arc = C1R::get_mut<arc_t>(arc);
        if( r_arc->flow )
        {
            if( !(r_arc->tail->number < 0 && r_arc->head->number > 0) )
            {
                if( !r_arc->tail->number )
                {
                    operational_cost += (r_arc->cost - net->bigM);
                    fleet++;
                }
                else
                    operational_cost += r_arc->cost;
            }
        }

    }
    
    return (double)fleet * (double)net->bigM + (double)operational_cost;
}









#ifdef _PROTO_
double flow_org_cost( network_t *net )
#else
double flow_org_cost( net )
    network_t *net;
#endif
{
    arc_t *arc;
    node_t *node;
    void *stop;
    
    long fleet = 0;
    cost_t operational_cost = 0;
    

    stop = (void *)net->stop_arcs;
    for( arc = net->arcs; arc != (arc_t *)stop; arc++ )
    {
        if( arc->ident == AT_UPPER )
            arc->flow = (flow_t)1;
        else
            arc->flow = (flow_t)0;
    }

    stop = (void *)net->stop_nodes;
    for( node = net->nodes, node++; node != (node_t *)stop; node++ )
        node->basic_arc->flow = node->flow;
    
    stop = (void *)net->stop_arcs;
    for( arc = net->arcs; arc != (arc_t *)stop; arc++ )
    {
        if( arc->flow )
        {
            if( !(arc->tail->number < 0 && arc->head->number > 0) )
            {
                if( !arc->tail->number )
                {
                    operational_cost += (arc->org_cost - net->bigM);
                    fleet++;
                }
                else
                    operational_cost += arc->org_cost;
            }
        }
    }
    
    return (double)fleet * (double)net->bigM + (double)operational_cost;
}










#ifdef _PROTO_
long primal_feasible( network_t *net )
#else
long primal_feasible( net )
    network_t *net;
#endif
{
    void *stop;
    node_t *node;
    arc_t *dummy = net->dummy_arcs;
    arc_t *stop_dummy = net->stop_dummy;
    arc_t *arc;
    flow_t flow;
    

    node = net->nodes;
    stop = (void *)net->stop_nodes;

    for( node++; node < (node_t *)stop; node++ )
    {
        arc = node->basic_arc;
        flow = node->flow;
        if( arc >= dummy && arc < stop_dummy )
        {
            if( ABS(flow) > (flow_t)net->feas_tol )
            {
                printf( "PRIMAL NETWORK SIMPLEX: " );
                printf( "artificial arc with nonzero flow, node %d (%ld)\n",
                        node->number, flow );
            }
        }
        else
        {
            if( flow < (flow_t)(-net->feas_tol)
               || flow - (flow_t)1 > (flow_t)net->feas_tol )
            {
                printf( "PRIMAL NETWORK SIMPLEX: " );
                printf( "basis primal infeasible (%ld)\n", flow );
                net->feasible = 0;
                return 1;
            }
        }
    }
    
    net->feasible = 1;
    
    return 0;
}










#ifdef _PROTO_
long dual_feasible( network_t *net )
#else
long dual_feasible(  net )
    network_t *net;
#endif
{
    arc_t         *arc;
    arc_t         *stop     = net->stop_arcs;
    cost_t        red_cost;
    
    

    for( arc = net->arcs; arc < stop; arc++ )
    {
        arc_t *r_arc = C1R::get_mut<arc_t>(arc);
        red_cost = r_arc->cost - r_arc->tail->potential 
            + r_arc->head->potential;
        switch( r_arc->ident )
        {
        case BASIC:
#ifdef AT_ZERO
        case AT_ZERO:
            if( ABS(red_cost) > (cost_t)net->feas_tol )
#ifdef DEBUG
                printf("%d %d %d %ld\n", arc->tail->number, arc->head->number,
                       arc->ident, red_cost );
#else
                // goto DUAL_INFEAS;
                fprintf( stderr, "DUAL NETWORK SIMPLEX: " );
                fprintf( stderr, "basis dual infeasible\n" );
                return 1;
#endif
            
            break;
#endif
        case AT_LOWER:
            if( red_cost < (cost_t)-net->feas_tol )
#ifdef DEBUG
                printf("%d %d %d %ld\n", arc->tail->number, arc->head->number,
                       arc->ident, red_cost );
#else
                // goto DUAL_INFEAS;
                fprintf( stderr, "DUAL NETWORK SIMPLEX: " );
                fprintf( stderr, "basis dual infeasible\n" );
                return 1;
#endif

            break;
        case AT_UPPER:
            if( red_cost > (cost_t)net->feas_tol )
#ifdef DEBUG
                printf("%d %d %d %ld\n", arc->tail->number, arc->head->number,
                       arc->ident, red_cost );
#else
                // goto DUAL_INFEAS;
                fprintf( stderr, "DUAL NETWORK SIMPLEX: " );
                fprintf( stderr, "basis dual infeasible\n" );
                return 1;
#endif

            break;
        case FIXED:
        default:
            break;
        }
    }
    
    return 0;
    
DUAL_INFEAS:
    fprintf( stderr, "DUAL NETWORK SIMPLEX: " );
    fprintf( stderr, "basis dual infeasible\n" );
    return 1;
}







#ifdef _PROTO_
long getfree( 
            network_t *net
            )
#else
long getfree( net )
     network_t *net;
#endif
{  
    FREE( net->nodes );
    // FREE( net->arcs );
    // FREE( net->dummy_arcs );
    net->nodes = net->stop_nodes = NULL;
    net->arcs = net->stop_arcs = NULL;
    net->dummy_arcs = net->stop_dummy = NULL;

    return 0;
}



