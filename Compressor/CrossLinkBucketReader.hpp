//
//  CrossLinkReader.hpp
//  Compressor
//
//  Created by Avi Miller on 5/17/18.
//  Copyright © 2018 Avi Miller. All rights reserved.
//

#include <stdio.h>
#include "CrossLinkDataStructure.hpp"

#ifndef CrossLinkBucketReader_hpp
#define CrossLinkBucketReader_hpp

template <typename T>
class CrossLinkBucketReader {
    CrossLinkDataStructure<T>* dataStructureToRead;
    HashLinkNode<T>* currentInBucket;
    HashLinkNode<T>* currentOverall;
    bool validPosition = false;
    
public:
    CrossLinkBucketReader(T startElement, CrossLinkDataStructure<T>* dataStructure);
    bool isValid(){return validPosition;}
    bool nextInBucket();
    bool nextInSequence();
    T currentNode();
    
};

#endif /* CrossLinkBucketReader_hpp */
