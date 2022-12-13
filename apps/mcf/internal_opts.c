// #include "implicit.h"
// #include <stdint.h>

// __int128_t cache_request(uint64_t vaddr);
// void * cache_access(__int128_t token);
// void * cache_access_mut(__int128_t token);

// #ifdef _PROTO_
// void insert_new_arc( arc_t *new, long newpos, node_t *tail, node_t *head,
//                      cost_t cost, cost_t red_cost )
// #else
// void insert_new_arc( new, newpos, tail, head, cost, red_cost )
//      arc_t *new;
//      long newpos;
//      node_t *tail;
//      node_t *head;
//      cost_t cost;
//      cost_t red_cost;
// #endif
// {
//     long pos;
    
//     __int128_t tk = cache_request((uint64_t) (new + newpos));
//     arc_t *new_l = (arc_t *) cache_access_mut(tk);

//     new_l->tail      = tail;
//     new_l->head      = head;
//     new_l->org_cost  = cost;
//     new_l->cost      = cost;
//     new_l->flow      = (flow_t)red_cost; 
    
//     pos = newpos+1;

//     __int128_t tk_pos21 = cache_request((uint64_t) (new + pos/2 - 1));
//     arc_t *new_cond = (arc_t *) cache_access(tk_pos21);

//     while( pos-1 && red_cost > (cost_t)new_cond->flow )
//     {
//         __int128_t tk_pos1 = cache_request((uint64_t) (new + pos - 1));
//         arc_t *new_pos1 = (arc_t *) cache_access_mut(tk_pos1);

//         tk_pos21 = cache_request((uint64_t) (new + pos/2 - 1)); 
//         new_cond = (arc_t *) cache_access(tk_pos21); 

//         new_pos1->tail     = new_cond->tail;
//         new_pos1->head     = new_cond->head;
//         new_pos1->cost     = new_cond->cost;
//         new_pos1->org_cost = new_cond->cost;
//         new_pos1->flow     = new_cond->flow;
        
//         pos = pos/2;

//         tk_pos1 = cache_request((uint64_t) (new + pos - 1));
//         new_pos1 = (arc_t *) cache_access_mut(tk_pos1);

//         new_pos1->tail     = tail;
//         new_pos1->head     = head;
//         new_pos1->cost     = cost;
//         new_pos1->org_cost = cost;
//         new_pos1->flow     = (flow_t)red_cost; 

//         tk_pos21 = cache_request((uint64_t) (new + pos/2 - 1)); 
//         new_cond = (arc_t *) cache_access(tk_pos21); 
//     }

//     return;
// }  

// #ifdef _PROTO_
// void replace_weaker_arc( network_t *net, arc_t *new, node_t *tail, node_t *head,
//                          cost_t cost, cost_t red_cost )
// #else
// void replace_weaker_arc( net, new, tail, head, cost, red_cost )
//      network *net;
//      arc_t *new;
//      node_t *tail;
//      node_t *head;
//      cost_t cost;
//      cost_t red_cost;
// #endif
// {
//     long pos;
//     long cmp;

//     __int128_t tk_0 = cache_request((uint64_t) (new));
//     arc_t *new_0 = (arc_t *) cache_access_mut(tk_0); 

//     new_0->tail     = tail;
//     new_0->head     = head;
//     new_0->org_cost = cost;
//     new_0->cost     = cost;
//     new_0->flow     = (flow_t)red_cost; 
                    
//     pos = 1;

//     __int128_t tk_1 = cache_request((uint64_t) (new + 1));
//     arc_t *new_1 = (arc_t *) cache_access(tk_1); 
//     __int128_t tk_2 = cache_request((uint64_t) (new + 2));
//     arc_t *new_2 = (arc_t *) cache_access(tk_2); 

//     cmp = (new_1->flow > new_2->flow) ? 2 : 3;

//     __int128_t tk_cmp1 = cache_request((uint64_t) (new + cmp - 1));
//     arc_t *new_cmp1 = (arc_t *) cache_access(tk_cmp1); 

//     while( cmp <= net->max_residual_new_m && red_cost < new_cmp1->flow )
//     {
//       __int128_t tk_pos1 = cache_request((uint64_t) (new + pos - 1));
//       arc_t *new_pos1 = (arc_t *) cache_access_mut(tk_pos1); 

//       tk_cmp1 = cache_request((uint64_t) (new + cmp - 1));
//       new_cmp1 = (arc_t *) cache_access(tk_cmp1); 

//         new_pos1->tail =     new_cmp1->tail;
//         new_pos1->head =     new_cmp1->head;
//         new_pos1->cost =     new_cmp1->cost;
//         new_pos1->org_cost = new_cmp1->cost;
//         new_pos1->flow =     new_cmp1->flow;
        
//         new_cmp1->tail = tail;
//         new_cmp1->head = head;
//         new_cmp1->cost = cost;
//         new_cmp1->org_cost = cost;
//         new_cmp1->flow = (flow_t)red_cost; 

//         pos = cmp;
//         cmp *= 2;
//         if( cmp + 1 <= net->max_residual_new_m ) {
//           tk_cmp1 = cache_request((uint64_t) (new + cmp - 1));
//           new_cmp1 = (arc_t *) cache_access(tk_cmp1); 

//           __int128_t tk_cmp = cache_request((uint64_t) (new + cmp));
//           arc_t *new_cmp = (arc_t *) cache_access(tk_cmp); 

//           if( new_cmp1->flow < new_cmp->flow ) 
//               cmp++;
//         } 

//         tk_cmp1 = cache_request((uint64_t) (new + cmp - 1));
//         new_cmp1 = (arc_t *) cache_access_mut(tk_cmp1); 
//     }
    
//     return;
// }   