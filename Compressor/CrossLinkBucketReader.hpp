//
//  CrossLinkReader.hpp
//  Compressor
//
//  Created by Avi Miller on 5/17/18.
//  Copyright © 2018 Avi Miller. All rights reserved.
//

#ifndef CrossLinkBucketReader_hpp
#define CrossLinkBucketReader_hpp

#include <stdio.h>
#include "CrossLinkDataStructure.hpp"

template <typename T>
class CrossLinkBucketReader {
    CrossLinkDataStructure<T>* dataStructureToRead;
    HashLinkNode<T>* currentInBucket;
    HashLinkNode<T>* nBucket = nullptr;
    HashLinkNode<T>* currentOverall;
    bool validPosition = false;
    
public:
    CrossLinkBucketReader(T startElement, CrossLinkDataStructure<T>* dataStructure);
    bool isValid(){return validPosition;}
    bool nextInBucket();
    bool nextInSequence();
    T currentNode();
    
};

template <typename T>
CrossLinkBucketReader<T>::CrossLinkBucketReader(T startElement, CrossLinkDataStructure<T>* dataStructure) {
    currentInBucket = currentOverall = dataStructure->getList(startElement);
    validPosition = (currentOverall != nullptr);
    if(validPosition)
        nBucket = currentInBucket->next;
}

template <typename T>
bool CrossLinkBucketReader<T>::nextInBucket(){
    currentInBucket = nBucket;
    if(nBucket != nullptr)
        nBucket = nBucket->next;
    currentOverall = currentInBucket;
    validPosition = (currentOverall != nullptr);
    return validPosition;
}

template <typename T>
bool CrossLinkBucketReader<T>::nextInSequence(){
    currentOverall = currentOverall->sequential;
    validPosition = (currentOverall != nullptr);
    return validPosition;
}

template <typename T>
T CrossLinkBucketReader<T>::currentNode(){
    if(!validPosition)
        throw "CurrentNode called on invalid position";
    return currentOverall->value;
}

#endif /* CrossLinkBucketReader_hpp */
