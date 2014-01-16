// -*- C++ -*-
#ifndef INCLUDED_SmartPtr_h
#define INCLUDED_SmartPtr_h
//
// SmartPtr.h
///



/**
 * @class counted_ptr<T>
 *
 * @brief 智能指针模板。
 *
 * 通过引用计数实现对指针所指对象的管理。主要管理动态生成的局部对象，
 * 通过该指针对象实现在对象生命周期结束时自动调用delete 方法。
 */

template <class Ty>
class counted_ptr {
  ///
  Ty*   ptr;
  ///
  long* count;
  
  template<class Y> friend class counted_ptr;
  
  template<class Y, class U> 
  friend counted_ptr<Y> dynamic_pointer_cast(counted_ptr<U> const & r);
  
  counted_ptr(Ty* p, long* c):ptr(p),count(c)  {
     if(ptr==0) {
        count = new long(1);
     } else {
         ++*count;
     }
        
  }
  
public:
  ///
  explicit counted_ptr(Ty* p = 0)
    : ptr(p), count(new long(1)) {
  }

  ///
  counted_ptr(const counted_ptr<Ty>& p)
    : ptr(p.ptr), count(p.count) {
    ++*count;
  }
  
  template<class Y>
  counted_ptr(const counted_ptr<Y>& p)
    : ptr(p.ptr), count(p.count) {
    ++*count;
  }
  
  
  ///
  ~counted_ptr() {
    dispose();
  }

  ///
  counted_ptr<Ty>& operator=(const counted_ptr<Ty>& p) {
    if (this != &p) {
      dispose();
      ptr   = p.ptr;
      count = p.count;
      ++*count;
    }
    return *this;
  }
  
  template<class Y>
    counted_ptr & operator=(counted_ptr<Y> const & p) // never throws
    {
       if ((void*)this != (void*)&p) {
      dispose();
      ptr   = p.ptr;
      count = p.count;
      ++*count;
    }
    return *this;
    }

  ///
  Ty& operator*() const {
    return *ptr;
  }

  ///
  Ty* operator->() const {
    return ptr;
  }

  ///
  Ty* get() const {
    return ptr;
  }
  
  operator bool () const
    {
        return ptr != 0;
    }
    
private:
  ///
  void dispose() {
    if (--*count == 0) {
      delete count;
      delete ptr;
    }
  }
};

template<class T, class U> inline counted_ptr<T> dynamic_pointer_cast(counted_ptr<U> const & r)
{
    return counted_ptr<T>(dynamic_cast<T*>(r.ptr), r.count);
}

#endif // INCLUDED_SmartPtr_h


