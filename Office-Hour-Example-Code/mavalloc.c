// The MIT License (MIT)
// 
// Copyright (c) 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

enum TYPE
{
    FREE = 0,
    USED
};

enum ALGORITHM
{
    FIRST_FIT;
    NEXT_FIT;
    BEST_FIT;
    WORST_FIT;
}

struct Node {
  size_t size;
  enum TYPE type;
  void * arena;
  struct Node * next;
  struct Node * prev;
};

struct Node *alloc_list;

void * arena;

enum ALGORITHM allocation_algorithm;

int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
  arena = malloc( ALIGN4( size ) );
  
  allocation_algorithm = algorithm;

  alloc_list = ( struct Node * )malloc( sizeof( struct Node ));

  alloc_list -> arena = arena;
  alloc_list -> size  = ALIGN4(size);
  alloc_list -> type  = FREE;
  alloc_list -> next  = NULL;
  alloc_list -> prev  = NULL;

  previous_node  = alloc_list;

  return 0;
}

void mavalloc_destroy( )
{
  free( arena );
  
  // iterate over the linked list and free the nodes

  return;
}

void * mavalloc_alloc( size_t size )
{
  struct Node * node;

  if( allocation_algorithm != NEXT_FIT )
  { 
    node = alloc_list;
  }
  else if ( allocation_algorithm == NEXT_FIT )
  {
    node = previous_node;
  }
  else
  {
    printf("ERROR: Unknown allocation algorithm!\n");
    exit(0);
  }

  size_t aligned_size = ALIGN4( size );

  if( allocation_algorithm == FIRST_FIT )
  {
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
  
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
  
          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
  }

  // Implement Next Fit
  // Implement Worst Fit
  struct Node *winner = NULL;
  int winning_size = INT_MAX;
  int losing_size = INT_MIN;
  if(allocation_algorithm == WORST_FIT)
  {
    while(node)
    {
      if(node -> free && (node -> size - size) > losing_size)
      {
        winner = node;
        losing_size = node -> size - size;
      }
      node = node -> next;
    }
    return winner;
  }
  // Implement Best Fit
  if(allocation_algorithm == BEST_FIT)
  {
    while(node)
    {
      if(node -> free && (node -> size - size) < winning_size)
      {
        winner = node;
        winning_size = node -> size - size;
      }
      node = node -> next;
    }
    return winner;
  }

  return NULL;
}

void mavalloc_free( void * ptr )
{
  return;
}

int mavalloc_size( )
{
  int number_of_nodes = 0;
  struct Node * ptr = alloc_list;

  while( ptr )
  {
    number_of_nodes ++;
    ptr = ptr -> next; 
  }

  return number_of_nodes;
}
