//
//  HashLinkNode.hpp
//  Compressor
//
//  Created by Avi Miller on 5/17/18.
//  Copyright © 2018 Avi Miller. All rights reserved.
//

#ifndef HashLinkNode_hpp
#define HashLinkNode_hpp

#include <stdio.h>

//will be used to store the digits
template <typename T> class HashLinkNode{

public:
    HashLinkNode(T newValue):value(newValue){};
    HashLinkNode(T newValue, HashLinkNode<T>* newNext, HashLinkNode<T>* newBack, HashLinkNode<T>* newSequential):value(newValue),next(newNext),back(newBack),sequential(newSequential){}
    
    HashLinkNode<T>* next = nullptr;
    HashLinkNode<T>* back = nullptr;
    HashLinkNode<T>* sequential = nullptr;
    T value;
    
};

#endif /* HashLinkNode_hpp */
