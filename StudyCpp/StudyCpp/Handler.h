/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "stdafx.h"

template <class T>
class Handler
{
public:
	Handler(T *p=0):ptr(p),use(new size_t(1)){};
	T& operator*();
	T* operator->();
	const T& operator*() const;
	const T* operator->() const;
	Handler(const Handler& h):ptr(h.ptr),use(h.use){++*use;}
	Handler& operator=(const Handler&);
	~Handler(void){ rem_ref();}

private:
	T* ptr;
	size_t *use;
	void rem_ref(){
		if(--*use==0){
			cout<<"�Ѿ�û��ʹ���ߣ�����"<<endl;
			delete ptr;
			delete use;
		} 
	}
};

template<class T>
inline T& Handler<T>::operator*(){
	if(ptr){
		return *ptr;
	}
	throw runtime_error("dereference of unbound Handle"); 
}

template<class T>
inline T* Handler<T>::operator->(){
	if(ptr){
		return ptr;
	}
	throw runtime_error("dereference of unbound Handle"); 
}

template<class T>
inline const T& Handler<T>::operator*() const{
	if(ptr){
		return *ptr;
	}
	throw runtime_error("dereference of unbound Handle"); 
}

template<class T>
inline const T* Handler<T>::operator->() const {
	if(ptr){
		return ptr;
	}
	throw runtime_error("dereference of unbound Handle"); 
}

template<class T>
inline Handler<T>& Handler<T>::operator =(const Handler& rhs){
	++*rhs.use;
	rem_ref();
	use=rhs.use;
	ptr=rhs.ptr;
	return *this;
}