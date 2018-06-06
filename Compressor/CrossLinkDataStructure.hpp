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
    int (*hashFunction)(T);
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

template <typename T>
CrossLinkDataStructure<T>::CrossLinkDataStructure(int (*newHashFunction)(T), int initMaxElements, int initialBucketCount){
    
    hashFunction = newHashFunction;
    maxElements = initMaxElements;
    buckets = initialBucketCount;
    bucketArray = new HashLinkNode<T>*[buckets];
    tailBucketArray = new HashLinkNode<T>*[buckets];
    for(int i = 0; i < buckets; ++i){
        bucketArray[i] = nullptr;
        tailBucketArray[i] = nullptr;
    }
}

template <typename T>
void CrossLinkDataStructure<T>::addElement(T element){
    int newestHash = hash(element);
    HashLinkNode<T>* tailOfBucket = tailBucketArray[newestHash];
    HashLinkNode<T>* newNode = new HashLinkNode<T>(element, nullptr, tailOfBucket, nullptr);
    
    if(newestNode)
        newestNode->sequential = newNode;
    newestNode = newNode;
    
    if(tailOfBucket)
        tailOfBucket->next = newNode;
    tailBucketArray[newestHash] = newNode;
    
    if(!bucketArray[newestHash])
        bucketArray[newestHash] = newNode;
    
    if(!oldestNode)
        oldestNode = newNode;
    
    if(++currentElementCount > maxElements)
        deleteOldest();
}

template <typename T>
bool CrossLinkDataStructure<T>::isBucketEmpty(T element){
    return bucketArray[hash(element)] == nullptr;
}

template <typename T>
int CrossLinkDataStructure<T>::hash(T element){
    return hashFunction(element) % buckets;
}

template <typename T>
void CrossLinkDataStructure<T>::deleteOldest(){
    HashLinkNode<T>* nextOldest = oldestNode->sequential;
    int oldestHash = hash(oldestNode->value);
    
    //the oldest node is guaranteed to be at the top of its linked list in its bucket
    bucketArray[oldestHash] = oldestNode->next;
    
    //it may also be at the end of that linked list too
    if(tailBucketArray[oldestHash] == oldestNode)
        tailBucketArray[oldestHash] = nullptr;
    
    delete oldestNode;
    
    oldestNode = nextOldest;
    oldestNode->back = nullptr;
    --currentElementCount;
}


template <typename T>
HashLinkNode<T>* CrossLinkDataStructure<T>::getList(T element){
    return bucketArray[hash(element)];
}

#endif /* CrossLinkDataStructure_hpp */
