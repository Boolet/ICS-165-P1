#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>
#include "CrossLinkBucketReader.hpp"

using namespace std;

int windowOffsetBits = 11;
int matchLengthBits = 4;    //2^matchlengthbits
int literalStringBits = 3;

typedef pair<char, int> entry;
typedef pair<int, int> matchData;
typedef pair<int, matchData> unmatchData;

//has function used for the CrossLinkDataStructure on pairs<char, int>
int hashForPairs(const pair<char, int> toHash){
    return toHash.first;
}

//encodes a sequence of characters without a reference
void encodeRaw(int startIndex, int length, char*& data){
    
}

//encodes a sequence of characters that matches an earlier sequence
void encodeBackreference(int relativeStart, int length){
    
}

//finds and encodes the longest matching substring that it can find
//we will need to limit it to 2^matchLengthBits number of characters
matchData findLongestSubstring(int index, char*& data, CrossLinkDataStructure<entry> &dataStructure){
    int maxMatchLength = pow(2, matchLengthBits)-1;
    int longestMatchRelativeLocation = 0;
    int longestMatchLength = 0;
    CrossLinkBucketReader<entry> reader(entry(data[index], index), &dataStructure);
    
    
    //loop through the bucket (substrings beginning with this character)
    while(reader.isValid()){
        int currentLength = 0;
        pair<char, int> currentNode = reader.currentNode();
        int currentRelativeLocation = index - currentNode.second;
        
        //iterate along the current substring
        while(reader.isValid()){
            currentNode = reader.currentNode();
            if(currentNode.first == data[index+currentLength]){    //found a match for the current element
                //we can add this element to the data structure now
                dataStructure.addElement(pair<char, int>(data[index+currentLength], index+currentLength));
                ++currentLength;
                if(currentLength > longestMatchLength){
                    longestMatchLength = currentLength;
                    longestMatchRelativeLocation = currentRelativeLocation;
                    
                    if(currentLength == maxMatchLength)
                        return matchData(longestMatchLength, longestMatchRelativeLocation);
                }
            } else {
                break;
            }
            
            reader.nextInSequence();
        }
        
        
        reader.nextInBucket();
    }
    
    return matchData(longestMatchLength, longestMatchRelativeLocation);
}

//finds the longest substring of characters for which no two sequential characters have a match
unmatchData findLongestUnmatched(int index, char*& data, CrossLinkDataStructure<entry> &dataStructure){
    int i = 2;
    dataStructure.addElement(entry(data[index+1], index+1));
    
    matchData info;
    int maxUnmatchLength = pow(2, literalStringBits);
    while(i < maxUnmatchLength){
        info = findLongestSubstring(index+i, data, dataStructure);
        if(info.first > 1)
            break;
        else if(info.first == 0){
            dataStructure.addElement(entry(data[index+i], index+i));
        }
        ++i;
    }
    
    return unmatchData(i, info);
}



int main(int argc, char *argv[]){
    
    ifstream inFile;
    char* oData = 0;
    int windowSize = pow(2, windowOffsetBits)-1;
    CrossLinkDataStructure<entry> dataStructure(hashForPairs, windowSize, 128);
    
    //add in parameter reading later
    
    
    //open the file
    inFile.open("test.txt", ios::in|ios::binary|ios::ate);
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1);   // call system to stop
    }
    
    
    inFile.seekg(0, ios::end); // set the pointer to the end
    size_t size = inFile.tellg() ; // get the length of the file
    cout << "Size of file: " << size;
    inFile.seekg(0, ios::beg); // set the pointer to the beginning
    
    oData = new char[ size+1 ]; //  for the '\0'
    inFile.read( oData, size );
    oData[size] = '\0' ; // set '\0'
    cout << " oData size: " << strlen(oData) << "\n";
    
    
    // If we need to load the data in chunks we should do it in
    // another loop outside of this part, and be clever about
    // it, because there always needs to be at least 2 to the
    // 'matchlengthbits' characters in the buffer at the end
    
    //iterate through all of the characters; i is the head of the current substring
    for(int i = 0; i < size; ){
        matchData longestMatchData = findLongestSubstring(i, oData, dataStructure);
        
        //if we matched 0 or 1 character, then we'll have to do a raw enconding
        if(longestMatchData.first < 2){
            unmatchData info = findLongestUnmatched(i, oData, dataStructure);
            encodeRaw(i, info.first, oData);
            encodeBackreference(info.second.first, info.second.second);
            i+=info.first+info.second.first;    //we looked at all the elements unmatched and matched
        } else {
            encodeBackreference(longestMatchData.first, longestMatchData.second);
            i+=longestMatchData.first;  //we only looked at the matched elements
        }
    }
    
    
    inFile.close();
    
    return 0;
}
