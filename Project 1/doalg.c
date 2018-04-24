#include <stdlib.h>

//===========================================================
// struct to represent a link in our linked list
//===========================================================
typedef struct link link;
struct link{
    int index;
    struct link* next;
};

//forward declaration
int runoff(int index, int sizes[], link* losers[]);

//===========================================================
// doalg function. Initializes an array of linked lists to hold
// the losers for the runoff process, and an array of ints to
// hold the sizes of the linked lists by index. Then creates
// a linked list for all indices from 1 to n into the COMPARE
// array, and stores them in index 0 of the losers array.
//===========================================================
int doalg( int n, int k, int Best[]){
    
    //check for valid input
    if(k > n || k < 0)   //k needs to be <= n and >= 0
        return 0;
    if(n < 0)   //n needs to be >= 0
        return 0;
    
    
    link* initialLosers[n+1];   //declare the losers array; stores all losers from the runoff
    int sizes[n+1];     //delare sizes array; will have lengths of each list of initialLosers
    
    initialLosers[0] = 0;   //zero the first index
    for(int i = 0; i < n; ++i){ //loop a number of times equal to the number of elements...
        link* temp = malloc(sizeof(link));  //create a new link for the linked list
        temp->index = i+1;  //set the value of the link to the current index
        temp->next = initialLosers[0];  //and point it's next at the first element of the list
        initialLosers[0] = temp;    //then make it the head of the list
        initialLosers[i+1] = 0; //also zero the other elements of initialLosers
        sizes[i+1] = 0;     //zero elements of sizes as well
    }
    sizes[0] = n;   //finally, set sizes[0] to the number of elements we just added
    
    
    //because the system we're using is a 1-indexed system, we can conveniently use
    //the zero index of the data structure for our first runoff. From then on, each
    //other index in initialLosers contains the indices that lost a comparison to
    //that index. For instance if index 8 lost to index 5, then initialLosers[5] would
    //contain a link with value 8.
    
    int lastHighest = 0;    //our first runoff starts with the full list in initialLosers[0]
    for(int i = 0; i < k; ++i){ //loop k times....
        //find the highest element among the elements that lost to the previous highest
        //which we then know is the next highest element
        lastHighest = runoff(lastHighest, sizes, initialLosers);
        
        //put that element into the final array
        Best[i] = lastHighest;
    }
    
    
    return 1;
}

//===========================================================
// runoff function. Performs a runoff of the elements of
// losers[index], using sizes[index] to know the array needed
// to store these elements. The runoff compares every two
// elements and drops the loser after recording it at the
// winner's index in the losers[] array, and continues doing
// this until a single winner is found. Then it returns this
// winner.
//===========================================================
int runoff(int index, int sizes[], link* losers[]){
    
    int remaining = sizes[index];   //the initial number of elements in the runoff
    int localWinners[remaining]; //an array of size 'remaining' to store the elements
    
    //this part just copies the elements from the linked list into an array
    //for ease of access
    int i = 0;
    link* current = losers[index];
    while(current){
        localWinners[i] = current->index;
        current = current->next;
        ++i;
    }
    
    //this is where the runoff is actually performed
    
    //the outer loop iterates through each round of the runoff
    while(remaining > 1){
        
        //the inner loop compares each pair of elements, ignoring the spare, if any
        for(i = 0; i < remaining-1; i+=2){
            
            //call to COMPARE for the current pair
            int result = COMPARE(localWinners[i], localWinners[i+1]);
            
            link* newLoser = malloc(sizeof(link)); //create a new link
            
            //determine which side won, then add the new link to that index's
            //list in the losers data structure, setting its value to the loser's
            //index. Then put the winner into the localWinners array to prepare
            //for the next round.
            if(result == 1){
                ++sizes[localWinners[i]];
                newLoser->index = localWinners[i+1];
                newLoser->next = losers[localWinners[i]];
                losers[localWinners[i]] = newLoser;
                localWinners[i/2] = localWinners[i];
                
            } else if(result == 2) {
                ++sizes[localWinners[i+1]];
                newLoser->index = localWinners[i];
                newLoser->next = losers[localWinners[i+1]];
                losers[localWinners[i+1]] = newLoser;
                localWinners[i/2] = localWinners[i+1];
                
            } else {
                //error generated by COMPARE function
                return 0;
            }
        }
        
        //see if the number of elements this round is odd
        if(remaining%2 != 0)
            localWinners[i/2] = localWinners[i];
            //if it is, put the spare into the next round
        
        //calculate the number of elements in the next round
        remaining = remaining/2 + remaining%2;
    }
    
    //when there is only one element left, return it; it has beaten all other
    //elements in the runoff.
    return localWinners[0];
}
