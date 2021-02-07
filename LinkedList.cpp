#include <iostream>
#include "LinkedList.h"

Node::Node(char value, Node* next) :
    value(value),
    next(next) 
{}

LinkedList::LinkedList() {
    this->head = nullptr;
    this->size = 0;
}
LinkedList::LinkedList(const LinkedList& other) {

}
LinkedList::~LinkedList() {
    delete this;
}

int LinkedList::getSize() {
    return size;
}

char LinkedList::get(int index) {

    int outputValue = 0;
    int count = 0;
    Node* current = head;
    
    if(index < size) {
        while(count < index) {
            ++count;
            current = current->next;
        }
        outputValue = current->value;
    }
    else {
        // change to a throw statement
        std::cout << "out of range" << std::endl;
    }
    return outputValue;

}

bool LinkedList::get(int index, char& returnValue)  {
   bool error = true;
   int count = 0;
   Node* current = head;
   if(&(this->head) == nullptr) {
       return error;
   }
   if (index >= 0 && index < size) {
      while(count < index) {
         ++count;
         current = current->next;
      }
      returnValue = current->value;
      error = false;
   }
   
   return error;
}

void LinkedList::addFront(char value) {
    Node* toAdd = new Node(value,nullptr);
    if(this->head != nullptr) {
        toAdd->next = this->head;  
    }
    this->head = toAdd;
    size++;
}

void LinkedList::addBack(char value) {
   Node* toAdd = new Node(value, nullptr);
   if (this->head == nullptr) {
      this->head = toAdd;
   } else {
      Node* current = this->head;
      while(current->next != nullptr) {
         current = current->next;
      }
      current->next = toAdd;
   }
   size++;
}

void LinkedList::removeBack() {
    if(size>0){
        if(this->head != nullptr) {
            if(this->head->next != nullptr) {
                Node* current = this->head;
                while(current->next->next != nullptr) {
                    current = current->next;
                }
                current->next = nullptr;
            }
            else {
                this->head = nullptr;
            }
        }
          size--;
    }  
}

void LinkedList::removeFront() {
    if(size>0){
        Node* current = this->head;
        if(current->next==nullptr){
            this->head = nullptr;
        }
        else{
            this->head = current->next;
        } 
        size--;
    }
}

void LinkedList::clear() {   
    int startingSize = size; 
    for(int i=0; i<startingSize; ++i) {
        this->removeFront();
    }
}

void LinkedList::removeNodeAtIndex(int index) {
    Node* current = this->head;
    if(index > 0) {
        for(int i=0; i<index-1; ++i) {
            current = current->next; 
        }
        Node* temp = current->next;
        current->next = temp->next;
        temp = nullptr;
        size--;
    }
    else {
        this->removeFront();
    }
}

bool LinkedList::contains(char input){

    Node* current = this->head;
    bool found = false;
    while(!found && current != nullptr) {
        if(current->value == input){
            found = true;
        }
        current = current->next;
    }
    return found;
}

void LinkedList::print() {
    Node* current = this->head;
    while(current != nullptr) {
        std::cout << current->value << " ";
        current = current->next;
    }
}