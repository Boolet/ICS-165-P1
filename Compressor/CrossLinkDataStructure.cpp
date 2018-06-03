//
//  CrossLinkDataStructure.cpp
//  Compressor
//
//  Created by Avi Miller on 5/17/18.
//  Copyright © 2018 Avi Miller. All rights reserved.
//

#include "CrossLinkDataStructure.hpp"

template <typename T>
CrossLinkDataStructure<T>::CrossLinkDataStructure(int (*newHashFunction)(T), int initMaxElements, int initialBucketCount){
    
    hashFunction = newHashFunction;
    maxElements = initMaxElements;
    buckets = initialBucketCount;
    bucketArray = new T*[buckets];
    tailBucketArray = new T*[buckets];
}

template <typename T>
void CrossLinkDataStructure<T>::addElement(T element){
    int newestHash = hash(element);
    HashLinkNode<T>* tailOfBucket = tailBucketArray[newestHash];
    HashLinkNode<T>* newNode = new HashLinkNode<T>(element, nullptr, tailOfBucket, nullptr);
    
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
    --currentElementCount;
}


template <typename T>
HashLinkNode<T>* CrossLinkDataStructure<T>::getList(T element){
    return bucketArray[hash(element)];
}







