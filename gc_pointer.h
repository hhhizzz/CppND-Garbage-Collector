#include <cstdlib>
#include <iostream>
#include <list>
#include <typeinfo>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <class T, int size = 0>
class Pointer {
 private:
  // refContainer maintains the garbage collection list.
  static std::list<PtrDetails<T> > refContainer;
  // addr points to the allocated memory to which
  // this Pointer pointer currently points.
  T *addr;
  /*  isArray is true if this Pointer points
      to an allocated array. It is false
      otherwise.
  */
  bool isArray;
  // true if pointing to array
  // If this Pointer is pointing to an allocated
  // array, then arraySize contains its size.
  unsigned arraySize;  // size of the array
  static bool first;   // true when first Pointer is created
  // Return an iterator to pointer details in refContainer.
  typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);

 public:
  // Define an iterator type for Pointer<T>.
  typedef Iter<T> GCiterator;
  // Empty constructor
  // NOTE: templates aren't able to have prototypes with default arguments
  // this is why constructor is designed like this:
  Pointer() { Pointer(NULL); }
  Pointer(T *);
  // Copy constructor.
  Pointer(const Pointer &);
  // Destructor for Pointer.
  ~Pointer();
  // Collect garbage. Returns true if at least
  // one object was freed.
  static bool collect();
  // Overload assignment of pointer to Pointer.
  T *operator=(T *t);
  // Overload assignment of Pointer to Pointer.
  Pointer &operator=(Pointer &rv);
  // Return a reference to the object pointed
  // to by this Pointer.
  T &operator*() { return *addr; }
  // Return the address being pointed to.
  T *operator->() { return addr; }
  // Return a reference to the object at the
  // index specified by i.
  T &operator[](int i) { return addr[i]; }
  // Conversion function to T *.
  operator T *() { return addr; }
  // Return an Iter to the start of the allocated memory.
  Iter<T> begin() {
    int _size;
    if (isArray)
      _size = arraySize;
    else
      _size = 1;
    return Iter<T>(addr, addr, addr + _size);
  }
  // Return an Iter to one past the end of an allocated array.
  Iter<T> end() {
    int _size;
    if (isArray)
      _size = arraySize;
    else
      _size = 1;
    return Iter<T>(addr + _size, addr, addr + _size);
  }
  // Return the size of refContainer for this type of Pointer.
  static int refContainerSize() { return refContainer.size(); }
  // A utility function that displays refContainer.
  static void showlist();
  // Clear refContainer when program exits.
  static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class
// interface
template <class T, int size>
Pointer<T, size>::Pointer(T *t) {
  // Register shutdown() as an exit function.
  if (first) {
    atexit(shutdown);
  }
  first = false;

  // TODO: Implement Pointer constructor
  // Lab: Smart Pointer Project Lab
  if (size > 0) {
    this->isArray = true;
    this->arraySize = size;
  }
  this->addr = t;
  // std::cout << "new pointer: " << t << " the size is " << size << std::endl;
  auto detail = PtrDetails<T>(t, size);
  refContainer.push_back(detail);
}
// Copy constructor.
template <class T, int size>
Pointer<T, size>::Pointer(const Pointer &ob) {
  // TODO: Implement Pointer constructor
  // Lab: Smart Pointer Project Lab
  // std::cout << "copy pointer: " << ob.addr << std::endl;
  auto p = findPtrInfo(this->addr);
  p->refcount--;
  collect();

  p = findPtrInfo(ob.addr);
  if (p != refContainer.end()) {
    p->refcount++;
  } else {
    if (size > 0) {
      this->isArray = true;
    }
    this->arraySize = size;
    auto detail = PtrDetails<T>(ob.addr, size);
    refContainer.push_back(detail);
  }
  this->addr = ob.addr;
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer() {
  // TODO: Implement Pointer destructor
  // Lab: New and Delete Project Lab
  // std::cout << "delete pointer: " << this->addr << std::endl;
  typename std::list<PtrDetails<T> >::iterator p;
  p = findPtrInfo(addr);
  p->refcount--;
  collect();
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect() {
  // std::cout << "**** start to gc ****" << std::endl;
  bool memfreed = false;
  typename std::list<PtrDetails<T> >::iterator p;
  do {
    // Scan refContainer looking for unreferenced pointers.
    for (p = refContainer.begin(); p != refContainer.end(); p++) {
      // TODO: Implement collect()
      // If in-use, skip.
      if (p->refcount != 0) {
        continue;
      }
      // Remove unused entry from refContainer.
      refContainer.erase(p);
      // Free memory unless the Pointer is null.
      if (p->memPtr != nullptr) {
        memfreed = true;
        if (p->isArray) {
          // std::cout << "pointer: " << p->memPtr
          //           << " released with delete[], the array size is "
          //           << p->arraySize << std::endl;
          delete[] p->memPtr;
        } else {
          // std::cout << "pointer: " << p->memPtr << " released with delete"
          //           << std::endl;
          delete p->memPtr;
        }
      }
      // Restart the search.
      p = refContainer.begin();
      break;
    }
  } while (p != refContainer.end());
  // std::cout << "**** gc finished with: " << memfreed << " ****" << std::endl;
  return memfreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t) {
  // TODO: Implement operator==
  // LAB: Smart Pointer Project Lab
  auto newSize = sizeof(*t) / sizeof(T);
  // std::cout << "make pointer: " << this->addr << " point to: " << t
  //           << " the size is " << newSize << std::endl;
  auto p = findPtrInfo(this->addr);
  p->refcount--;
  collect();

  p = findPtrInfo(t);
  if (p != refContainer.end()) {
    p->refcount++;
  } else {
    if (newSize > 1) {
      this->isArray = true;
    }
    this->arraySize = newSize;
    auto detail = PtrDetails<T>(t, size);
    refContainer.push_back(detail);
  }
  return this->addr = t;
}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv) {
  // TODO: Implement operator==
  // LAB: Smart Pointer Project Lab
  return this->addr == rv.addr;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist() {
  typename std::list<PtrDetails<T> >::iterator p;
  std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
  std::cout << "memPtr refcount value\n ";
  if (refContainer.begin() == refContainer.end()) {
    std::cout << " Container is empty!\n\n ";
  }
  for (p = refContainer.begin(); p != refContainer.end(); p++) {
    std::cout << "[" << (void *)p->memPtr << "]"
              << " " << p->refcount << " ";
    if (p->memPtr)
      std::cout << " " << *p->memPtr;
    else
      std::cout << "---";
    std::cout << std::endl;
  }
  std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator Pointer<T, size>::findPtrInfo(
    T *ptr) {
  typename std::list<PtrDetails<T> >::iterator p;
  // Find ptr in refContainer.
  for (p = refContainer.begin(); p != refContainer.end(); p++) {
    if (p->memPtr == ptr) {
      // std::cout << "find ptr: " << p->memPtr << std::endl;
      return p;
    }
  }
  return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown() {
  if (refContainerSize() == 0) return;  // list is empty
  typename std::list<PtrDetails<T> >::iterator p;
  for (p = refContainer.begin(); p != refContainer.end(); p++) {
    // Set all reference counts to zero
    p->refcount = 0;
  }
  collect();
}