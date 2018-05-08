#include <stdio.h>
#include <stdarg.h>
#define MAXN 10000
#define ARRAYLIMIT 10000

double dshrandom( long input ) {
    /*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */
    /*	Random Number Generator					*/
    /*		first called with seed				*/
    /*		thereafter called with 0			*/
    /*	Based on code appearing in				*/
    /*	"Random number generators: good ones are hard to find"	*/
    /*		by Stephen Park and Keith Miller		*/
    /*		CACM 31 (Oct 1988) 1192-1201.			*/
    /*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */
    static long seed = 3125;
    static long a = 16807;
    static long m = 2147483647;
    static long q = 127773;      /* m div a */
    static long r = 2836;        /* m mod a */
    long lo, hi, test;
    
    if (input>0) seed = input;
    hi = seed/q;
    lo = seed - (hi*q);
    test = a*lo - r*hi;
    if (test>0) seed = test;
    else        seed = test+m;
				/* seed equally in range 0...2^31 -1  */
    return( ((double)seed)/(double)m );
}

int QCOUNT(int arg1, ...) {
    /*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */
    /*	Queries to determine a majority element			*/
    /*	QCOUNT( 0,n ) initializes a random array of length n	*/
    /*				 all Boolean values		*/
    /*		returns  0 normally				*/
    /*		returns -1 if n is out of range			*/
    /*	QCOUNT( 1,Q[] ) views array[ Q[0] ] ... array[ Q[3] ]	*/
    /*	         counts the number, num, of 1-values		*/
    /*		returns -88 if any Q[i] is out of range		*/
    /*		returns -99 if any duplicates among Q[]		*/
    /*		returns  abs( 4-2*num ), otherwise		*/
    /*		returns 0 if Boolean 1s/0s split 2,2		*/
    /*		returns 2 if split 3,1 or 1,3			*/
    /*		returns 4 if split 4,0 or 0,4			*/
    /*	QCOUNT( 2, answer ) checks whether answer is correct	*/
    /*		returns -99999 if any errors			*/
    /*		returns number of queries performed, otherwise	*/
    /*	QCOUNT( -1 ) turns on debugging 			*/
    /*	QCOUNT( -2 ) turns off debugging 			*/
    /*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */
    va_list ap;
    static int marble[ARRAYLIMIT+1];
    static int querycount, OneCount, AnswerValue, arraysize=0;
    static int debug=0;
    int i,j,t,num,*Q,answer;
    
    va_start(ap, arg1);
    
    if (arg1<-2 || arg1>4) {
        va_end(ap);
        printf("******* ERROR: QCOUNT(arg1, ...) -- 'arg1'=%d out of range **********\n",arg1);
        return(-1);
    }
    if (arg1== -1) {
        int lim, ct;
        ct = 0;
        debug = 1;
        lim = (arraysize<=100) ? arraysize : 100;
        for (i=1; i<=lim; i+=10) {
            printf("%4d:",i);
            for (j=0; j<=9; j++) {
                if (i+j<=lim) {
                    printf(" %d",marble[i+j]);
                    ct += marble[i+j];
                }
            }
            printf("    %3d\n",ct);
        }
        printf("OneCount = %d, AnswerValue = %d\n",OneCount,AnswerValue);
        return(0);
    }
    if (arg1== -2) { debug = 0; return(0); }
    if (arg1==0) {
        arraysize = va_arg(ap, int);
        debug = 0;
        va_end(ap);
        if (arraysize<4 || arraysize>ARRAYLIMIT) {
            printf("******* ERROR: QCOUNT(0,n) -- 'n'=%d out of range **********\n",arraysize);
            return(-1);
        }
        j = 0;
        //custom array for our tests
        
        /*for(i=1; i<=4; i++){
            marble[i] = 0;
        }
        for (i=5; i<=arraysize; i++) {
            marble[i] = dshrandom(0)<0.5 ? 0 : 1;
            j += marble[i];
        }*/
        
         for (i=1; i<=arraysize; i++) {
         marble[i] = dshrandom(0)<0.5 ? 0 : 1;
         j += marble[i];
         }
        /*
        for(int k = 1; k <= arraysize; ++k){
            printf("%d", marble[k]);
        }
        printf("\n");*/
        OneCount = j;
        AnswerValue = (j < arraysize-j) ? 0 : (j > arraysize-j) ? 1 : -1;
        querycount = 0;
        return(0);
    }
    if (arg1==1) {
        Q = va_arg(ap, int *);
        va_end(ap);
        num = 0;
        for (i=0; i<4; i++) {
            t = Q[i];
            if (t<1 || t>arraysize) return(-88);
            if (i<3)
                for (j=i+1; j<4; j++)
                    if (Q[j]==t) return(-99);
            num += marble[t];
        }
        querycount++;
        num = 4-2*num;
        return (num<0) ? -num : num;
    }
    if (arg1==2) {
        answer = va_arg(ap, int);
        va_end(ap);
        if (answer<0 || answer>arraysize) {
            printf("******* ERROR: QCOUNT(2,answer) -- 'answer'=%d out of range (arraysize=%d)**********\n",answer,arraysize);
            return(-99999);
        }
        if ( (answer==0 && AnswerValue<0) ||
            (answer>0 && marble[answer]==AnswerValue) ) {
            if (debug==1)
                printf("returning querycount=%d\n",querycount);
            return(querycount);
        }
        printf("******* ERROR: QCOUNT(2,answer) -- 'answer'=%d incorrect **********\n",answer);
        if (debug==1)
            printf("******* marble[%d] = %d **********\n",answer,marble[answer]);
        return(-99999);
    }
    if (arg1==3) {
        for(i = 1; i <= arraysize; i++){
            printf("%d ", marble[i]);
        }
        printf("\n");
        return(-2);
    }
    if (arg1==4) {
        for(int k = 1; k <= arraysize; ++k){
            printf("%d", marble[k]);
        }
        printf("\n");
    }
    return(-100000);
}

/* ================================================================================ */

int query(int a, int b, int c, int d){
    int queryArray[] = {a,b,c,d};
    return QCOUNT(1, queryArray);
}

void setFourhead(int a, int b, int c, int* fourhead){
    fourhead[0] = a;
    fourhead[1] = b;
    fourhead[2] = c;
}

//returns 0 if not known, 1 if typeA is majority, 2 if typeB is majority
int majorityKnown(int typeA, int typeB, int n){
    if(typeA > n/2) return 1;
    if(typeB > n/2) return 2;
    if(typeA + typeB == n) return 3;
    return 0;
}

void twosRunner(int* twos, int twosCount, int* fours, int* foursCount, int* typeA, int* typeB, int* fourhead, int* pairs, int* pairsCount, int* bIndex){
    
    int currentHeadParity = -1; //0 is even parity, 1 is odd parity, -1 for unknown; tracking CURRENT twos
    int pairsStartEven[twosCount];    //for the first two unchecked elements being of even parity
    int pairsStartOdd[twosCount]; // and for odd parity
    int pairsIndex = 0;
    int foundZeros = 0;
    int knownStart = -1;
    
    int i;
    for(i = 0; i < twosCount-1; ++i){
        //query the end of the previous group of twos and the front of the next
        int result = query(twos[i]+2, twos[i]+3, twos[i+1], twos[i+1]+1);
        if(result == 2){
            (*typeA) += 1;
            (*typeB) += 1;
            if(currentHeadParity == 0){
                pairsStartEven[pairsIndex] = twos[i]+2;
                pairsStartOdd[pairsIndex++] = twos[i]+2;
            } else if(currentHeadParity == 1){
                //then we have a 1 and a 0 in the first two elements
                //and two of the same in the second two
                pairsStartEven[pairsIndex] = twos[i+1];
                pairsStartOdd[pairsIndex++] = twos[i+1];
            } else if(currentHeadParity == -1){
                if(foundZeros % 2 == 0){
                    pairsStartEven[pairsIndex] = twos[i+1];
                    pairsStartOdd[pairsIndex++] = twos[i]+2;
                } else {
                    pairsStartEven[pairsIndex] = twos[i]+2;
                    pairsStartOdd[pairsIndex++] = twos[i+1];
                }
            }
            
        } else if(result == 4){
            if(fourhead[0] == 0){
                setFourhead(twos[i]+2, twos[i]+3, twos[i+1], fourhead);
            }
            if(currentHeadParity == -1){  //if this chain of twos starts at the beginning
                knownStart = (foundZeros + 1)  % 2;
                if(knownStart == 1){
                    (*typeA)++;
                    (*typeB)++;
                }
            }
            currentHeadParity = 1;  //and the next head's parity is always odd
            //now we need to save the group of four to check later
            fours[(*foursCount)++] = twos[i]+2;
            
        } else if(result == 0){
            (*typeA) += 2;
            (*typeB) += 2;
            ++foundZeros;
            if(currentHeadParity == 0){
                currentHeadParity = 1;
            } else if(currentHeadParity == 1) {
                currentHeadParity = 0;
            } else if(currentHeadParity == -1){
                //do nothing
            }
        }
    }
    
    if(knownStart == -1) {
        //we don't know what parity we started with
        int result = query(twos[0],twos[0]+1,fourhead[0], fourhead[1]);
        if (result == 0){
            knownStart = 0;
            (*typeB) += 2;
	    (*bIndex) = twos[0];
        }else if(result == 4){
            knownStart = 0;
            (*typeA) += 2;
        } else if (result == 2){
            knownStart = 1;
            (*typeA) += 1;
            (*typeB) += 1;
        }
    } else if(knownStart == 0){
        pairsStartEven[pairsIndex++] = twos[0];
    }
    
    if(currentHeadParity == -1){    //never found a four
        //add the end two elements based on what we know
        if(foundZeros % 2 == 0 && knownStart == 1){
            pairsStartOdd[pairsIndex++] = twos[i]+2;
        } else if(foundZeros % 2 == 1 && knownStart == 0){
            pairsStartEven[pairsIndex++] = twos[i]+2;
        } else {
            (*typeA)++;
            (*typeB)++;
        }
    } else {    //found a four
        if(currentHeadParity == 0){
            if(knownStart == 0)
                pairsStartEven[pairsIndex++] = twos[i]+2;
            else
                pairsStartOdd[pairsIndex++] = twos[i]+2;
        }
        else{
            (*typeA)++;
            (*typeB)++;
        }
    }
    
    for(int j = 0; j < pairsIndex; ++j){
        pairs[j] = knownStart == 1 ? pairsStartOdd[j] : pairsStartEven[j];
    }
    (*pairsCount) = pairsIndex;
    
}

//we know that abcd is a 2 first; fourhead is a three-element array
int bruteForceOnFive(int a, int b, int c, int d, int e, int* fourhead){
    int output = 3;
    int result = query(b, c, d, e);
    if(result == 4){    // 2/4 -> 01111
        setFourhead(b, c, d, fourhead);
        output = 4;
        
    } else if(result == 2){ // 2/2/? ->
        
        int subresult = query(a, b, d, e);
        if(subresult == 4){ // 2/2/4 -> 11011
            setFourhead(a, b, d, fourhead);
            output = 4;
        } else if(subresult == 2){  // 2/2/2 -> 11101
            setFourhead(a, c, e, fourhead);
            output = 4;
        } else if(subresult == 0){  // 2/2/0 -> 10001
            setFourhead(b, c, d, fourhead);
        }
        
    } else if(result == 0){ // 2/0/? ->
        
        int subresult = query(a, c, d, e);
        if(subresult == 2){ // 2/0/2 -> 01001
            setFourhead(a, c, d, fourhead);
        } else if(subresult == 0){  // 2/0/2/? ->
            int subsubresult = query(a, b, c, e);
            if(subsubresult == 2){  // 2/0/2/2 -> 11100
                setFourhead(a, b, c, fourhead);
            } else if(subsubresult == 0){   // 2/0/2/0 -> 11010
                setFourhead(a, b, d, fourhead);
            }
        }
        
    }
    return output;
}

void handlePairs(int* pairs, int pairsCount, int* fourhead, int* bIndex, int* typeA, int* typeB){
    int i;
    for(i = 0; i < pairsCount-1; i+=2){
        int result = query(pairs[i], pairs[i+1], fourhead[0], fourhead[1]);
        if(result == 0){
            (*typeB) += 4;
            (*bIndex) = pairs[i];
        } else if(result == 2){
            (*typeA) += 2;
            (*typeB) += 2;
        } else if(result == 4){
            (*typeA) += 4;
        }
    }
    
    //leftover
    if(pairsCount % 2 == 1){
        int result = query(pairs[i], pairs[i]+1, fourhead[0], fourhead[1]);
        if(result == 0){
            (*typeB) += 2;
            (*bIndex) = pairs[i];
        } else if(result == 4){
            (*typeA) += 2;
        }
    }
}

void handleFours(int* fours, int foursCount, int* fourhead, int* bIndex, int* typeA, int* typeB){
    int i;
    for(i = 0; i < foursCount-1; i+=2){
        int result = query(fours[i], fours[i+1], fourhead[0], fourhead[1]);
        if(result == 0){
            (*typeB) += 8;
            (*bIndex) = fours[i];
        } else if(result == 2){
            (*typeA) += 4;
            (*typeB) += 4;
        } else if(result == 4){
            (*typeA) += 8;
        }
    }
    
    //leftover
    if(foursCount % 2 == 1){
        int result = query(fours[i], fours[i]+1, fourhead[0], fourhead[1]);
        if(result == 0){
            (*typeB) += 4;
            (*bIndex) = fours[i];
        } else if(result == 4){
            (*typeA) += 4;
        }
    }
}


void handleLeftovers(){
    
}


int mysub(int n){
    
    int fourhead[] = {0, 0, 0};
    int bIndex = -1;
    int typeA = 0;
    int typeB = 0;
    int twoCount = 0;
    int twos[n/4];
    int foursCount = 0;
    int fours[n/4];
    int pairs[n/4];
    int pairsCount = 0;
    int majOut;
    int i;
    
    for(i = 1; i <= n-3; i+=4){
        int result = query(i, i+1, i+2, i+3);
        if(result == 0){    //handling zeros
            typeA += 2;
            typeB += 2;
        } else if(result == 4){
            if(fourhead[0] == 0){
                setFourhead(i, i+1, i+2, fourhead);
                typeA += 4;
            } else {
                fours[foursCount++] = i;    //handling fours
            }
        } else if(result == 2){
            if(fourhead[0] == 0){
                int spare = i+4 > n ? 1 : i+4;
                int out = bruteForceOnFive(i, i+1, i+2, i+3, spare, fourhead);
                typeA += out;
                typeB += 5-out;
                ++i;
            } else {
                twos[twoCount++] = i;   //maybe make discovered 2's count as one of typeA and one of typeB
            }
        }
    }
    
    if(twoCount > 1)
        twosRunner(twos, twoCount, fours, &foursCount, &typeA, &typeB, fourhead, pairs, &pairsCount, &bIndex);

    
    handlePairs(pairs, pairsCount, fourhead, &bIndex, &typeA, &typeB);
    handleFours(fours, foursCount, fourhead, &bIndex, &typeA, &typeB);
    
    //handling spare 2
    if(twoCount == 1){
        int result = query(twos[0], twos[0]+1, fourhead[0], fourhead[1]);
        if(result == 0){
            typeB += 3;
            typeA += 1;
            bIndex = twos[0];
        } else if(result == 2){
            int subresult = query(twos[0]+2, twos[0]+3, fourhead[0], fourhead[1]);
            if(subresult == 0){
                typeB += 3;
                typeA += 1;
                bIndex = twos[0]+2;
            } else if(subresult == 4){
                typeA += 3;
                typeB += 1;
            }
        } else if(result == 4){
            typeA += 3;
            typeB += 1;
        }
    }
    
    //stop before needing to handle spares (typeA > n/2 || typeB > n/2 || typeA + typeB == n)
    if((majOut = majorityKnown(typeA, typeB, n)) != 0){
        //we're done
        if(majOut == 1)
            return fourhead[0];
        if(majOut == 2){
            return bIndex;
	}
        return 0;
    }
    
    //handle spare elements
    int spareElements = n - i + 1;
    if(fourhead[0] == 0){
        if(spareElements == 1)
            return n;
        else if(spareElements >= 2){
            int result = query(i-1, i-2, i-3, n-1); //n-5 to n-2 are a 0 group
            if(query(i-1, i-2, i-3, n) == result){
                return n;
	    }
            else if(spareElements == 3)
                return n-2;
            else
                return 0;
        }
    } else {
        for( ; i <= n; ++i){
            int result = query(fourhead[0], fourhead[1], fourhead[2], i);
            if(result == 2){
                typeB++;
                bIndex = i;
            } else {
                typeA++;
            }
        }
    }
    
    if((majOut = majorityKnown(typeA, typeB, n)) != 0){
        //we're done
        if(majOut == 1)
            return fourhead[0];
        if(majOut == 2)
            return bIndex;
        return 0;
    }
    
    
    //handle:
    //have a bIndex (only left in case of leftovers [all zeroes])
    printf("Type A: %d, Type B: %d\n", typeA, typeB);
    return -10;
}










/* ================================================================================ */

main(){
    int N[] = {20,200,2000,17,18,19,0};
    int n,ct,loop,round,answer,total,biggest,errflag;
#define NLOOP 10000
    
    for (round = 0; N[round]>10; round++) {
        n = N[round];
        total = 0;
        biggest = -1;
        errflag = 0;
        for (loop=1; loop<=NLOOP; loop++) {
            //printf("Loop number: %d - contents: ", loop);
            QCOUNT(0,n);
            if(loop==121 && n == 18)
                QCOUNT(4);
            answer = mysub( n );
	    /*if(loop==216 && n==17)
		return(0);*/
            //QCOUNT(3);
            if (answer<0) {
                printf(" *** flagged error %d at n=%d, loop=%d\n",answer,n,loop);
                errflag = 1;
                break;
            }
            ct = QCOUNT(2,answer);
            if (ct<=0) {
                printf(" *** FAILED at n=%d, loop=%d\n",n,loop);
                errflag = 1;
                break;
            }
            if (ct>biggest) biggest=ct;
            total += ct;
        }
        if (errflag==0)
            printf("n=%5d,   max=%6d,   avg=%8.2f\n",
                   n,biggest,total/(1.*NLOOP));
        fflush(stdout);
    }
}
