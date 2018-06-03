//
//  CrossLinkDataStructure.hpp
//  Compressor
//
//  Created by Avi Miller on 5/17/18.
//  Copyright © 2018 Avi Miller. All rights reserved.
//

#ifndef CrossLinkDataStructure_hpp
#define CrossLinkDataStructure_hpp

#include <stdio.h>
#include "HashLinkNode.hpp"

/*
 *  The cross-link data structure, at its top level, is a dictionary that indexes into a
 *  DAG graph. The elements of the graph are HashLinkNodes arranged such that following
 *  the 'next' pointer on a node yeilds another node containing an item of the same hash,
 *  and following the 'sequential' pointer on a node yeilds the next character in the
 *  original string
 */

template <typename T> class CrossLinkDataStructure{
    int* (*hashFunction)(T);
    int maxElements = 2048;
    int buckets = 10;
    int currentElementCount = 0;
    HashLinkNode<T>** bucketArray;  //the head of each bucket is the oldest in the bucket
    HashLinkNode<T>** tailBucketArray;  //the tail is the newest
    HashLinkNode<T>* oldestNode = nullptr;
    HashLinkNode<T>* newestNode = nullptr;
    
    int hash(T element);
    
public:
    CrossLinkDataStructure<T>(int (*hashFunction)(T), int initMaxElements = 2048, int initialBucketCount = 10);
    HashLinkNode<T>* getList(T element);
    bool isBucketEmpty(T element);
    void addElement(T element);
    void deleteOldest();
};

#endif /* CrossLinkDataStructure_hpp */
