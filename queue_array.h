#ifndef QUEUE_ARRAY_H
#define QUEUE_ARRAY_H


#include <cstdlib>

#include <iostream>
#include <queue>

using namespace std;

//QueueArray class
template <class T>
class QueueArray {
  public:
    QueueArray(int);
    ~QueueArray();
    int Asize();
    T Dequeue();
    int Enqueue(const T &item,const int index);
    int QAsize();
    int Qsize(int index) ;
    T* Qstate(int index);

  private:
    int size;          //size of the array
    queue<T>* array;   //the array of queues
    int totalItems;    //total number of items stored in the queues
};

//Constructor for the queue array.  It sets the default size
//to 10, initializes the private variables size and totalItems
template <class T>
QueueArray<T>::QueueArray(int sz):size(sz),totalItems(0) {
  size=sz;
  totalItems=0;
  array = new queue<T>[size];
  if( array == NULL)
  {
    cout << "Not enough memory to create the array" << endl;
    exit(-1);
  }
}

//Destructor for the queue array.
template <class T>
QueueArray<T> :: ~QueueArray() {

}

//Asize method for queue array class, returns total number of
//queues in the array
template <class T>
int QueueArray<T>::Asize() {
  return size;
}

//Qsize method for queue array class, returns total number of
//elements in a given queue in the array
template <class T>
int QueueArray<T>::Qsize(int index) {
  queue<T> tempQueue = array[index];
  return tempQueue.size();
}

//QAsize method for queue array class, returns total number of
//elements across all queues in the arrray
template <class T>
int QueueArray<T>::QAsize() {
  return totalItems;
}

//Enqueue method for the queue array class, adds an item to
//a queue at a specific index in the array. Returns -1 if
//out of bounds, 1 if it succeeds, and 0 otherwise
template <class T>
int QueueArray<T>::Enqueue(const T &item,const int index){
  if( index >= Asize())
  {
    return -1;
  }
  queue<T> tempQueue = array[index];
  tempQueue.push(item);
  array[index] = tempQueue;

  if( tempQueue.back()==item)
  {
    totalItems++;
    return 1;
  }
  return 0;
}

//Dequeue method for the queue array class, removes the first
//element from the first non-empty queue in the array. Returns
//item if it finds one, returns 0 if all queues are empty
template <class T>
T QueueArray<T>::Dequeue() {
  for(int i = 0; i < Asize(); i++)
  {
    queue<T> tempQueue = array[i];
    if(tempQueue.size() > 0)
    {
      T item = tempQueue.front();
      tempQueue.pop();
      array[i] = tempQueue;
      totalItems--;
      return item;
    }
  }
  return 0;
}

//Qstate method for the queue array class, returns the pointer
//to an array containing the elements in a queue at a given
//index in the array
template <class T>
T* QueueArray<T>::Qstate(int index) {
  if(index > Asize())
  {
    return NULL;
  }
  queue<T> tempQueue = array[index];
  T *state = new T[tempQueue.size()];
  int i = 0;
  while(!tempQueue.empty())
  {
    state[i] = tempQueue.front();
    tempQueue.pop();
    i++;
  }
  return state;
}

#endif
