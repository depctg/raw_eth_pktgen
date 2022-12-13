/**************************************************************************
OUTPUT.C of ZIB optimizer MCF, SPEC version

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
/*  LAST EDIT: Sun Nov 21 16:21:54 2004 by Andreas Loebel (boss.local.de)  */
/*  $Id: output.c,v 1.10 2005/02/17 19:42:33 bzfloebe Exp $  */



#include "output.h"
#include "cache.h"




#ifdef _PROTO_
long write_circulations(
                   char *outfile,
                   network_t *net
                   )
#else
long write_circulations( outfile, net )
     char *outfile;
     network_t *net;
#endif 
{
    FILE *out = NULL;
    arc_t *block;
    arc_t *arc;
    arc_t *arc2;
    arc_t *first_impl = net->stop_arcs - net->m_impl;

    if(( out = fopen( outfile, "w" )) == NULL )
        return -1;

    refresh_neighbour_lists( net );
    
    for( block = net->nodes[net->n].firstout; block; block = block->nextout )
    {
        arc_t *r_block = cache_access_mod_opt(block);
        if( r_block->flow )
        {
            fprintf( out, "()\n" );
            
            arc = block;
            while( arc )
            {
                if( arc >= first_impl )
                    fprintf( out, "***\n" );

                arc_t *r_arc = cache_access_mod_opt(arc);
                fprintf( out, "%d\n", - r_arc->head->number );
                arc2 = r_arc->head[net->n_trips].firstout; 
                for( ; arc2; ) {
                    arc_t *r_arc2 = cache_access_mod_opt(arc2); 
                    if( r_arc2->flow )
                        break;
                    arc2 = r_arc2->nextout;
                }
                if( !arc2 )
                {
                    fclose( out );
                    return -1;
                }
                arc_t *r_arc2 = cache_access_mod_opt(arc2);  
                if( r_arc2->head->number )
                    arc = arc2;
                else
                    arc = NULL;
            }
        }
    }
    


    fclose(out);
    
    return 0;
}
