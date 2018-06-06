#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <iterator>
#include <ctime>
#include "CrossLinkBucketReader.hpp"

using namespace std;

bool debugMode = 0;
int outputBytes = 0;

char windowOffsetBits = 11; //11;
char matchLengthBits = 4;   //4    //2^matchlengthbits
char literalStringBits = 3; //3

int windowOffset;
int matchLengthRaw;
int literalStringLength;

typedef pair<char, int> entry;
typedef pair<int, int> matchData;

//has function used for the CrossLinkDataStructure on pairs<char, int>
int hashForPairs(const entry toHash){
    return toHash.first;
}

//handles each individual bit
void addToByte(bool bit, bool* outBuffer, int& bufferSize){
    outBuffer[bufferSize] = bit;
    ++bufferSize;
    if(bufferSize == 8){
        char out = 0;
        for(int i = 0; i < 8; ++i){
            //cout << outBuffer[i];
            out += outBuffer[i] << i;
        }
        //cout << " ";
        cout << out;
        outputBytes++;
        bufferSize = 0;
    }
}

//output an incomplete byte by just appending zeros
void flushByte(bool* outBuffer, int& bufferSize){
    int remaining = (8 - bufferSize) % 8;
    //cout << "flush";
    for(int i = 0; i < remaining; ++i){
        addToByte(false, outBuffer, bufferSize);
    }
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
    //cout << "raw encode of length: " << length << " from index: " << startIndex << " of characters: ";
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
        //cout << data[i] << ",";
        for(int j = 7; j >= 0; --j){
            char currentBit = 1 << j;
            bool o = data[i] & currentBit;
            addToByte(o, outBuffer, bufferSize);
        }
    }
    //cout << endl;
}

//encodes a sequence of characters that matches an earlier sequence
void encodeBackreference(int length, int relativeStart, bool* outBuffer, int& bufferSize){
    //matchLengthBits for the length of the match
    //and windowOffsetBits for the offset
    
    //cout << "match encode of length: " << length << " with relative start: " << relativeStart << endl;
    
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

//outputs the parameters for this compression
void encodeSettings(){
    cout << windowOffsetBits << matchLengthBits << literalStringBits;
}

//finds longest matching substring that it can find
matchData findLongestSubstring(int index, char*& data, size_t dataSize, CrossLinkDataStructure<entry> &dataStructure){
    int longestMatchRelativeLocation = 0;
    int longestMatchLength = 0;
    CrossLinkBucketReader<entry> reader(entry(data[index], index), &dataStructure);
    
    
    //loop through the bucket (substrings beginning with this character)
    while(reader.isValid()){
        int currentLength = 0;
        entry currentNode = reader.currentNode();
        int currentRelativeLocation = index - currentNode.second;
        
        //iterate along the current substring
        while(reader.isValid() && index + currentLength < dataSize){
            currentNode = reader.currentNode();
            if(currentNode.first == data[index+currentLength]){    //found a match for the current element
                ++currentLength;
                if(currentLength > longestMatchLength){
                    longestMatchLength = currentLength;
                    longestMatchRelativeLocation = currentRelativeLocation;
                    
                    //we can add this element to the data structure now
                    dataStructure.addElement(entry(data[index+currentLength-1], index+currentLength-1));
                    
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
    
    if(longestMatchLength == 0)
        dataStructure.addElement(entry(data[index], index));
    
    return matchData(longestMatchLength, longestMatchRelativeLocation);
}

//parsing command line arguments
string parseUserInput(int argc, char* argv[]){
    if(debugMode == 1){
        return "book1";
    }
    char* fileName = 0;
    if(argc==1){
        printf("Filename required");
        exit(1);
    } else if(argc>=2 && argc <= 5){
        for(int i = 1; i < argc; ++i){
            if(argv[i][0] == '-' && argv[i][2] == '='){
                try{
                    string numstr = string(argv[i]).substr(3);
                    int param = stoi(numstr);
                    if(argv[i][1] == 'N'){
                        windowOffsetBits = param;
                    } else if (argv[i][1] == 'L'){
                        matchLengthBits = param;
                    } else if (argv[i][1] == 'S'){
                        literalStringBits = param;
                    } else {
                        printf("Incorrect argument format");
                        exit(1);
                    }
                } catch(exception e) {
                    printf("Number not found");
                    exit(1);
                }
            } else {
                if(fileName == 0)
                    fileName = argv[i];
                else{
                    printf("Incorrect argument format");
                    exit(1);
                }
            }
        }
        if(fileName == 0){
            printf("Filename required");
            exit(1);
        }
        return fileName;
    } else {
        printf("Too many arguments");
        exit(1);
    }
    return argv[0];
}


//main
int main(int argc, char *argv[]){
    string filename = parseUserInput(argc, argv);
    
    //add in parameter reading later
    windowOffset = (1 << windowOffsetBits)-1;
    matchLengthRaw = (1 << matchLengthBits)-1;
    literalStringLength = (1 << literalStringBits) - 1;
    
    unsigned long startTime=clock();
    
    
    ifstream inFile;
    int bufferSize = 0;
    bool outBuffer[8];
    char* oData = 0;
    CrossLinkDataStructure<entry> dataStructure(hashForPairs, windowOffset, 256);
    
    //fill the window with spaces
    int prefixWindow = windowOffset - matchLengthRaw;
    for(int i = 0; i < prefixWindow; ++i){
        dataStructure.addElement(entry(' ', i - prefixWindow));
    }
    
    
    //open the infile
    inFile.open(filename, ios::in|ios::binary|ios::ate);
    if (!inFile) {
        cerr << "Unable to open file" << endl;
        exit(1);   // call system to stop
    }
    
    
    inFile.seekg(0, ios::end); // set the pointer to the end
    size_t size = inFile.tellg() ; // get the length of the file
    size--;
    //cout << "Size of file: " << size;
    inFile.seekg(0, ios::beg); // set the pointer to the beginning
    
    oData = new char[ size ];
    inFile.read( oData, size );
    inFile.close();
    
    //output the encoding parameters
    encodeSettings();
    
    //iterate through all of the characters; i is the head of the current substring
    //this also fills the rest of the window as it goes
    for(int i = 0; i < size; ){
        int unmatchCount = 0;
        matchData longestMatchData;
        
        while(i + unmatchCount < size
              && (longestMatchData = findLongestSubstring(i + unmatchCount, oData, size, dataStructure)).first < 2
              && unmatchCount < literalStringLength){
            ++unmatchCount;
        }
        //cout << "i is: " << i << " and unmatchCount is: " << unmatchCount << " and longest match is: " << longestMatchData.first << endl;
        if(unmatchCount > 0)
            encodeRaw(i, unmatchCount, oData, outBuffer, bufferSize);
        if(longestMatchData.first > 1){
            encodeBackreference(longestMatchData.first, longestMatchData.second, outBuffer, bufferSize);
            unmatchCount += longestMatchData.first;
        }
        i += unmatchCount;
        
    }
    
    encodeTerminator(outBuffer, bufferSize);
    flushByte(outBuffer, bufferSize);
    cout << endl;
    
    unsigned long stopTime=clock();
    if(debugMode == 1){
        cerr << "CPU time: " << (stopTime - startTime)/double(CLOCKS_PER_SEC) << endl;
        cerr << "compressed size: " << float(outputBytes)/float(size) * 100 << endl;
    }
    
    return 0;
}
