//
//  CrossLinkReader.cpp
//  Compressor
//
//  Created by Avi Miller on 5/17/18.
//  Copyright © 2018 Avi Miller. All rights reserved.
//

#include "CrossLinkBucketReader.hpp"

template <typename T>
CrossLinkBucketReader<T>::CrossLinkBucketReader(T startElement, CrossLinkDataStructure<T>* dataStructure) {
    currentInBucket = currentOverall = dataStructure->getList(startElement);
    validPosition = (currentOverall != nullptr);
}

template <typename T>
bool CrossLinkBucketReader<T>::nextInBucket(){
    currentInBucket = currentInBucket->next;
    currentOverall = currentInBucket;
    validPosition = (currentOverall != nullptr);
    return validPosition;
}

template <typename T>
bool CrossLinkBucketReader<T>::nextInSequence(){
    currentOverall = currentOverall->forward;
    validPosition = (currentOverall != nullptr);
    return validPosition;
}

template <typename T>
T CrossLinkBucketReader<T>::currentNode(){
    if(!validPosition)
        throw "CurrentNode called on invalid position";
    return currentOverall->value;
}
