#include <stdlib.h>
//#include "COMPARE.c"

typedef struct link link;
struct link{
    int index;
    struct link* next;
};

int runoff(int index, int sizes[], link* losers[]);

int doalg( int n, int k, int Best[]){
    
    link* initialLosers[n+1];
    int sizes[n+1];
    
    initialLosers[0] = 0;
    for(int i = 0; i < n; ++i){
        link* temp = malloc(sizeof(link));
        temp->index = i+1;
        temp->next = initialLosers[0];
        initialLosers[0] = temp;
	initialLosers[i+1] = 0;
        sizes[i+1] = 0;
    }
    sizes[0] = n;
    
    
    int lastHighest = 0;
    for(int i = 0; i < k; ++i){
        lastHighest = runoff(lastHighest, sizes, initialLosers);
        Best[i] = lastHighest;
    }
    
    
    return 1;
}

/*
void runoff(int index, link* losers[], int length){

    link* current = losers[index];
    link* copy = 0;
    int count = 0;
    
    while(current){
        if(!copy){
            copy = malloc(sizeof(link));
            copy->index = current->index;
        } else {
            link* temp = malloc(sizeof(link));
            temp->index = current->index;
            temp->next = copy;
            copy = temp;
        }
        current = current->next;
        ++count;
    }
    
    while(count > 1){
        
    }
    
}
*/

int runoff(int index, int sizes[], link* losers[]){
    
    int remaining = sizes[index];
    int localLosers[remaining];
    
    int i = 0;
    link* current = losers[index];
    while(current){
        localLosers[i] = current->index;
        current = current->next;
        ++i;
    }

    while(remaining > 1){
        for(i = 0; i < remaining-1; i+=2){    //check for the odd man out
            int result = COMPARE(localLosers[i], localLosers[i+1]);
            link* newLoser = malloc(sizeof(link));
            
            if(result == 1){
                ++sizes[localLosers[i]];
                newLoser->index = localLosers[i+1];
                newLoser->next = losers[localLosers[i]];
                losers[localLosers[i]] = newLoser;
                localLosers[i/2] = localLosers[i];
                
            } else if(result == 2) {
                ++sizes[localLosers[i+1]];
                newLoser->index = localLosers[i];
                newLoser->next = losers[localLosers[i+1]];
                losers[localLosers[i+1]] = newLoser;
                localLosers[i/2] = localLosers[i+1];
                
            } else {
                
            }
        }
        
        if(remaining%2 != 0)
            localLosers[i/2] = localLosers[i];
        
        remaining = remaining/2 + remaining%2;
    }
    
    return localLosers[0];
}
