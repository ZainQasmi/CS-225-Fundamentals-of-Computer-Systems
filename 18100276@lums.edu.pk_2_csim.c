#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/*
Each data load (L) or store (S) operation can cause at most one cache miss. The data modify operation
(M) is treated as a load followed by a store to the same address. Thus, an M operation can result in
two cache hits, or a miss and a hit plus a possible eviction.
*/

// You are simply counting hits, misses, and evictions

// Your simulator needs to handle different values of S, E, and b (blocksize) given at run time

// we are interested only in data cache performance, so your simulator should ignore all instruction cache accesses (lines starting with “I”).

// Each data load (L) or store (S) operation can cause at most one cache miss. The data modify operation 
// (M) is treated as a load followed by a store to the same address. Thus, an M operation can result in two cache hits, or a miss and a hit plus a possible eviction.

typedef struct {
    int valid_bit;
    long long tag;
    int LRUcount;
} cache_line;

int hits = 0;
int misses = 0;
int evictions = 0; 


int replace(cache_line array[],int size)
{
	int maximum = array[0].LRUcount;
	int index = 0;
	for (int i=0;i<size;i++)
	{
		if (array[i].LRUcount > maximum)
		{
			maximum = array[i].LRUcount;
			index = i;
		}
	}
	return index;
}


int main(int argc, char *argv[] )
{
    FILE *pFile;
    

    char operation;
    unsigned long long address;
    int size;


    int type;
    //int opt;
    int s, E, b;
    char* t;
    int setCount;
    while((type = getopt(argc, argv, "s:E:b:t:")) != -1) {
        switch(type)
        {
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                t = optarg;
                pFile = fopen(t,"r");
                break;
            default:
            	printf("1");
        }
    }
    setCount = 1 <<s;
    cache_line cache[setCount][E];

    for (int i=0;i<setCount;i++)
    {
    	for (int j=0;j<E;j++){
    		cache[i][j].valid_bit = 0;
    		cache[i][j].LRUcount = 0;
    	}
    }



    if(pFile != NULL) {
        while (fscanf(pFile," %c %llx,%d", &operation, &address, &size) > 0) {   
        /* do stuff */
            //printf(" %c",operation);
            //printf(" %llx",address);
            //printf(" %d",size );

            if (operation == 'I')
            {
            	continue;
            }

            unsigned long long tagN;
            int set;

            tagN = address >> (s+b);
            set = (address >> b) & ((1<<s)-1);
            //printf("%llx\n",tagN);
            //printf("address: %llx\n",address);
            //printf("set: %d\n",set);
            int found = 0;
            int evictionNeeded = 0;
            int indexUsed;
            for (int i=0;i<E;i++)
            {
            	if (cache[set][i].valid_bit == 1 && cache[set][i].tag == tagN)
            	{
            		hits++;
            		
            		found = 1;
            		indexUsed = i;
            		cache[set][i].LRUcount = 0;
            		//printf("hit\n");
            		break;
            	}
            }

            // not a hit, could be miss/ eviction

            if (found == 0)
            {
            	misses++;
            	//printf("miss\n");
            	evictionNeeded = 1;
            	for (int i=0;i<E;i++)
            	{
            		if (cache[set][i].valid_bit == 0)
            		{
            			cache[set][i].valid_bit = 1;
            			cache[set][i].tag = tagN;
            			cache[set][i].LRUcount = 0;
            			indexUsed = i;
            			evictionNeeded = 0;
            			break;
            		}


            	}

            	
            }

            if (evictionNeeded == 1)
            {
            	//printf("eviction\n");
            	evictions++;
            	int tracker = replace(cache[set],E);
            	indexUsed = tracker;
            	cache[set][tracker].LRUcount = 0;
            	cache[set][tracker].tag = tagN;

            }

            for (int j=0;j<E;j++)
            {
            	if (j != indexUsed)
            		cache[set][j].LRUcount++;
            }




        if (operation == 'M')
        	hits++;


        }
    }
    fclose(pFile); /* always close file when done */


    //struct cache_line cache[S][E];

    




    printSummary(hits, misses, evictions);
    return 0;
}