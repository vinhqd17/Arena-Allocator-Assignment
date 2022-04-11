//Vinh Dao 1001683799
//Augustine Nguyen 1001654628
//3320-003

// The MIT License (MIT)
//
// Copyright (c) 2022 Trevor Bakker
//
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
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
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

struct Node {
  size_t size;
  enum TYPE type;
  void * arena;
  struct Node * next;
  struct Node * prev;
};

struct Node *alloc_list;
struct Node *previous_node;


void * arena;

enum ALGORITHM allocation_algorithm = FIRST_FIT;

//Allocates arena and alloc_list using malloc
//Initializes the List with defult/empty values to be set later in mavalloc
    //Things like:  arena, size, type, next, previous_next
//Also sets previous_node to alloc_list for use in next fit later
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

//Destroys current list and arena
//frees arena and then traverses alloc_list to free each node individually (making sure to save the current node before freeing it, in order to traverse)
void mavalloc_destroy()
{
  free( arena );

  struct Node * temp;

  while (alloc_list)
  {
      temp = alloc_list;
      alloc_list = alloc_list->next;
      free(temp);
  }

  return;
}

void * mavalloc_alloc( size_t size )
{
  // only return NULL on failure
  struct Node * node;

  //Sets node to be traversed depending on which allocating algorithm
  //All algos excepts next fit start at the start of the List
  //Next fit starts at the previous_node, wherever allocation left off
    //In the case that it doesn't have a value, it just goes back to the top of the list
  //If the algo is invalid, errors
  if( allocation_algorithm != NEXT_FIT )
  {
    node = alloc_list;
  }
  else if ( allocation_algorithm == NEXT_FIT )
  {
    if (previous_node == NULL) previous_node = alloc_list;
    node = previous_node;
  }
  else
  {
    printf("ERROR: Unknown allocation algorithm!\n");
    exit(0);
  }

  //get aligned size
  size_t aligned_size = ALIGN4( size );

  //First fit
  //Traverses from top of the list
  //Tracks to see which node isn't used and of the correct size
  //Once a node is found to fit both parameters, changes node attributes to the correct size and type
  //A new node is made for any leftover space that wasn't allocated to the new nodes, splitting the previous node blocks
  //New node is inserted, being pointed to by the allocated node and pointing to the previous next node
  //Also sets previous node to the allocated node, allowing first fit to start from there
  //returns the arena
  if( allocation_algorithm == FIRST_FIT )
  {
    while( node != NULL )
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

  //Next Fit
  //Does the EXACT same thing as first fit
  //EXCEPT, it starts from last allocated nodes
  //Does this by utilising the previous_node rather than the top of the list
  //Also returns arena
  if (allocation_algorithm == NEXT_FIT)
  {
    while (node != NULL)
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

      node = node->next;

      if (node == previous_node) break;
      if (node == NULL)
        node = alloc_list;
    }
  }

  // Traverse through the whole Linked List
  // to find a portion of memory(node) where it is free,
  // the size of that free memory minus the passed in size
  // is less than the smallest (int) in order to find
  // the worst possible fit and the size of free
  // memory is greater than or equal to the requested size
  // set the winning node to be that node and size to be
  // the size of that memory minus that passed in size
  // Implement Worst Fit
  struct Node *winner = NULL;
  int winning_size = INT_MAX;
  int losing_size = INT_MIN;

  if(allocation_algorithm == WORST_FIT)
  {
    while(node != NULL)
    {
      if(node -> type == FREE && ((node -> size - size) < losing_size) && node->size >= aligned_size)
      {
        winner = node;
        losing_size = node -> size - size;
      }
      node = node -> next;
    }
    return winner;
  }

  // Traverse through the whole Linked List
  // to find a portion of memory(node) where it is free,
  // the size of that free memory minus the passed in size
  // is less than the largest (int) in order to find
  // the best possible fit and the size of free
  // memory is greater than or equal to the requested size
  // set the winning node to be that node and size to be
  // the size of that memory minus that passed in size
  // Implement Best Fit
  if(allocation_algorithm == BEST_FIT)
  {
    while(node != NULL)
    {
      if(node -> type == FREE && ((node -> size - size) < winning_size) && node->size >= aligned_size)
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

//Ptr is of type void, however, it's true value is arena
//Traverse list and looks for node with matching arena ptr values
//Once found, node is set to type free
//Traverse list again to look for consecutive free nodes
//If found, saves next node for freeing later, sets front node to have the size of both its current size and the next node size, and then sets the next node to the next next node, skipping over the previous next node
//then frees the previous next node
//essentially, combining nodes
void mavalloc_free( void * ptr )
{
  //courtesy of Professor Bakker
  struct Node * node = alloc_list;

  // find the node to free
  while( node )
  {
    if( node -> arena == ptr )
    {
      if( node -> type == FREE )
      {
        printf("Warning: Double free detected\n");
      }

      node -> type = FREE;

      break;
    }
    node = node -> next;
  }

  node = alloc_list;

  // search the list to combine the nodes
  while( node )
  {
    if( node -> next && node -> type == FREE && node -> next -> type == FREE  )
    {
      struct Node * previous = node -> next;
      node -> size = node -> size + node -> next -> size;
      node -> next = node -> next -> next;
      free( previous );
      continue;
    }
    node = node -> next;
  }
  return;
}

//traverses list and counts the number of nodes in list
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
