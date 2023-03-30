/**************************************************************************
PBEAMPP.C of ZIB optimizer MCF, SPEC version

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
/*  LAST EDIT: Sun Nov 21 16:22:04 2004 by Andreas Loebel (boss.local.de)  */
/*  $Id: pbeampp.c,v 1.10 2005/02/17 19:42:32 bzfloebe Exp $  */



#define K 300
#define B  50



#include "pbeampp.h"
#include "cache_init.hpp"

#ifdef _PROTO_
int bea_is_dual_infeasible( arc_t *arc, cost_t red_cost )
#else
int bea_is_dual_infeasible( arc, red_cost )
    arc_t *arc;
    cost_t red_cost;
#endif
{
    return(    (red_cost < 0 && arc->ident == AT_LOWER)
            || (red_cost > 0 && arc->ident == AT_UPPER) );
}





typedef struct basket
{
    arc_t *a;
    cost_t cost;
    cost_t abs_cost;
} BASKET;

static long basket_size;
static BASKET basket[B+K+1];
static BASKET *perm[B+K+1];



#ifdef _PROTO_
void sort_basket( long min, long max )
#else
void sort_basket( min, max )
    long min, max;
#endif
{
    long l, r;
    cost_t cut;
    BASKET *xchange;

    l = min; r = max;

    cut = perm[ (long)( (l+r) / 2 ) ]->abs_cost;

    do
    {
        while( perm[l]->abs_cost > cut )
            l++;
        while( cut > perm[r]->abs_cost )
            r--;
            
        if( l < r )
        {
            xchange = perm[l];
            perm[l] = perm[r];
            perm[r] = xchange;
        }
        if( l <= r )
        {
            l++; r--;
        }

    }
    while( l <= r );

    if( min < r )
        sort_basket( min, r );
    if( l < max && l <= B )
        sort_basket( l, max ); 
}






static long nr_group;
static long group_pos;


static long initialize = 1;

// Changes 
// ref: {Loop 1}
// ref: {Loop 2}
#ifdef _PROTO_
arc_t *primal_bea_mpp( long m,  arc_t *arcs, arc_t *stop_arcs, 
                              cost_t *red_cost_of_bea, uint64_t *loop1, uint64_t *loop2)
#else
arc_t *primal_bea_mpp( m, arcs, stop_arcs, red_cost_of_bea )
    long m;
    arc_t *arcs;
    arc_t *stop_arcs;
    cost_t *red_cost_of_bea;
#endif
{
    long i, next, old_group_pos;
    arc_t *arc;
    cost_t red_cost;

#ifdef SIMP_BREAK
    uint64_t loop1_time = 0;
    uint64_t loop2_time = 0;
#endif

    if( initialize )
    {
        for( i=1; i < K+B+1; i++ )
            perm[i] = &(basket[i]);
        nr_group = ( (m-1) / K ) + 1;
        group_pos = 0;
        basket_size = 0;
        initialize = 0;
    }
    else
    {   
        // label: Loop 1
        // next < i, no conflict can prefetch
#ifdef SIMP_BREAK
        uint64_t loop1_start = getCurNs();
#endif
        for( i = 2, next = 0; i <= B && i <= basket_size; i++ )
        {
            arc = perm[i]->a;
            arc_t *r_arc = C1R::get<arc_t>(arc);

            red_cost = r_arc->cost - r_arc->tail->potential + r_arc->head->potential;
            if( (red_cost < 0 && r_arc->ident == AT_LOWER)
                || (red_cost > 0 && r_arc->ident == AT_UPPER) )
            {
                next++;
                perm[next]->a = arc;
                perm[next]->cost = red_cost;
                perm[next]->abs_cost = ABS(red_cost);
            }
        }   
        basket_size = next;
#ifdef SIMP_BREAK
        loop1_time += getCurNs() - loop1_start;
#endif
    }

    old_group_pos = group_pos;

#ifdef SIMP_BREAK
    uint64_t  loop2_start = getCurNs();
#endif
    auto bd = [&](arc_t *r_arc) {
        if( r_arc->ident > BASIC )
        {
            /* red_cost = bea_compute_red_cost( arc ); */
            red_cost = r_arc->cost - r_arc->tail->potential + r_arc->head->potential;
            if( bea_is_dual_infeasible( r_arc, red_cost ) )
            {
                basket_size++;
                perm[basket_size]->a = arc;
                perm[basket_size]->cost = red_cost;
                perm[basket_size]->abs_cost = ABS(red_cost);
            }
        } 
    };
    do {
        /* price next group */
        arc = arcs + group_pos;
        // label Loop 2
        // tile it
        constexpr int epi = C1::Value::linesize / sizeof(arc_t);
        // if (nr_group <= epi/2) {
        if (0) {
            int soff = (uint64_t)arc % C1::Value::linesize;
            int eoff = (uint64_t)(stop_arcs - 1) % C1::Value::linesize;

            int s = soff / sizeof(arc_t);
            int e = eoff / sizeof(arc_t);

            int iters = (uint64_t)stop_arcs / C1::Value::linesize
                      - (uint64_t)arc / C1::Value::linesize;
            
            // prologue
            arc_t *base = arc - s;
            arc_t *l = C1R::get<arc_t>(base);
            int j = s;
            for (; j < epi; j += nr_group) {
                bd(l + j);
            }
            j -= epi;
            // steady
            for (int i = 0; i < iters; ++i) {
                arc_t *l = C1R::get<arc_t>(base + i * epi);
                for (; j < epi; j += nr_group)
                    bd(l + j);
                j -= epi;
            }
            // epilogue
            l = C1R::get<arc_t>(base + iters * epi);
            for (; j < e + 1; j += nr_group) {
                bd(l + j);
            }
        }
        else {
            for( ; arc < stop_arcs; arc += nr_group )
            {   
                arc_t *r_arc = C1R::get<arc_t>(arc);
                bd(r_arc);
            }
        }

        if( ++group_pos == nr_group )
            group_pos = 0;
    } while ( basket_size < B && group_pos != old_group_pos );
#ifdef SIMP_BREAK
    loop2_time += getCurNs() - loop2_start;
#endif

    
    if( basket_size == 0 )
    {
        initialize = 1;
        *red_cost_of_bea = 0; 
        return NULL;
    }
    sort_basket( 1, basket_size );
    *red_cost_of_bea = perm[1]->cost;

    #ifdef SIMP_BREAK
        *loop1 = loop1_time;
        *loop2 = loop2_time;
    #endif
    return( perm[1]->a );
}










