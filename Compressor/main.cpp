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

int windowOffset;
int matchLengthRaw;
int literalStringLength;

typedef pair<char, int> entry;
typedef pair<int, int> matchData;

//has function used for the CrossLinkDataStructure on pairs<char, int>
int hashForPairs(const pair<char, int> toHash){
    return toHash.first;
}

//handles each individual bit
void addToByte(bool bit, bool* outBuffer, int& bufferSize){
    outBuffer[bufferSize] = bit;
    cout << bit << endl;
    ++bufferSize;
    if(bufferSize == 8){
        char out = 0;
        for(int i = 0; i < 8; ++i){
            out += outBuffer[i] << i;
        }
        //cout << out << endl;
        bufferSize = 0;
    }
}

//output an incomplete byte by just appending zeros
void flushByte(bool* outBuffer, int& bufferSize){
    for( ; bufferSize < 8; ++bufferSize){
        outBuffer[bufferSize] = 0;
        cout << 0 << endl;
    }
    char out = 0;
    for(int i = 0; i < 8; ++i){
        out += outBuffer[i] << i;
    }
    //cout << out << endl;
}

//encodes L+S zeros
void encodeTerminator(bool* outBuffer, int& bufferSize){
    for(int i = 0; i < matchLengthBits + literalStringBits; ++i){
        addToByte(false, outBuffer, bufferSize);
    }
}


//encodes a sequence of characters without a reference
void encodeRaw(int startIndex, int length, char*& data, bool* outBuffer, int& bufferSize){
    if(length == 0)
        return;
    
    //this part always needs to be matchLengthBits number of zeros
    for(int i = 0; i < matchLengthBits; ++i){
        addToByte(false, outBuffer, bufferSize);
    }
    for(int i = literalStringBits-1; i >= 0; --i){
        int currentBit = 1 << i;
        bool o = length & currentBit;
        addToByte(o, outBuffer, bufferSize);
    }
    for(int i = startIndex; i < startIndex + length; ++i){
        for(int j = 7; j >= 0; --j){
            char currentBit = 1 << j;
            bool o = data[i] & currentBit;
            addToByte(o, outBuffer, bufferSize);
        }
    }
}

//encodes a sequence of characters that matches an earlier sequence
void encodeBackreference(int length, int relativeStart, bool* outBuffer, int& bufferSize){
    //matchLengthBits for the length of the match
    //and windowOffsetBits for the offset
    
    for(int i = matchLengthBits-1; i >= 0; --i){
        int currentBit = 1 << i;
        bool o = length & currentBit;
        addToByte(o, outBuffer, bufferSize);
    }
    for(int i = windowOffsetBits-1; i >= 0; --i){
        int currentBit = 1 << i;
        bool o = relativeStart & currentBit;
        addToByte(o, outBuffer, bufferSize);
    }
    
}

//finds longest matching substring that it can find
matchData findLongestSubstring(int index, char*& data, CrossLinkDataStructure<entry> &dataStructure){
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
                    
                    if(currentLength == matchLengthRaw)
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

//just for testing
int mainTEST(){
    int bufferSize = 0;
    bool outBuffer[8];
    char* oData = new char[ 1 ];
    oData[0] = '@';
    encodeRaw(0, 1, oData, outBuffer, bufferSize);
    return 0;
}

int main(int argc, char *argv[]){
    
    //add in parameter reading later
    windowOffset = (1 << windowOffsetBits)-1;
    matchLengthRaw = (1 << matchLengthBits)-1;
    literalStringLength = (1 << literalStringBits) - 1;
    
    
    ifstream inFile;
    int bufferSize = 0;
    bool outBuffer[8];
    char* oData = 0;
    CrossLinkDataStructure<entry> dataStructure(hashForPairs, windowOffset, 128);
    
    int prefixWindow = windowOffset - matchLengthRaw;
    for(int i = 0; i < prefixWindow; ++i){
        dataStructure.addElement(entry(' ', i - prefixWindow));
    }
    
    
    //open the infile
    inFile.open("testfile.txt", ios::in|ios::binary|ios::ate);
    if (!inFile) {
        cerr << "Unable to open file testfile.txt" << endl;
        exit(1);   // call system to stop
    }
    
    
    inFile.seekg(0, ios::end); // set the pointer to the end
    size_t size = inFile.tellg() ; // get the length of the file
    cout << "Size of file: " << size;
    inFile.seekg(0, ios::beg); // set the pointer to the beginning
    
    oData = new char[ size ]; //  for the '\0'
    inFile.read( oData, size-1 );
    //oData[size] = '\0' ; // set '\0'
    cout << " oData size: " << strlen(oData) << "\n";
    
    
    
    
    // If we need to load the data in chunks we should do it in
    // another loop outside of this part, and be clever about
    // it, because there always needs to be at least 2 to the
    // 'matchlengthbits' characters in the buffer at the end
    
    //iterate through all of the characters; i is the head of the current substring
    for(int i = 0; i < size; ){
        int unmatchCount = 0;
        matchData longestMatchData;
        
        while(i + unmatchCount < size
              && (longestMatchData = findLongestSubstring(i + unmatchCount, oData, dataStructure)).first < 2
              && unmatchCount < literalStringLength){
            ++unmatchCount;
        }
        cout << "i is: " << i << " and unmatchCount is: " << unmatchCount << " and longest match is: " << longestMatchData.first << endl;
        if(unmatchCount > 0)
            encodeRaw(i, unmatchCount, oData, outBuffer, bufferSize);
        if(longestMatchData.first > 1){
            encodeBackreference(longestMatchData.first, longestMatchData.second, outBuffer, bufferSize);
            unmatchCount += longestMatchData.first;
        }
        i += unmatchCount;
        
    }
    
    //encodeTerminator(outBuffer, bufferSize);
    //flushByte(outBuffer, bufferSize);
    
    
    inFile.close();
    
    return 0;
}
