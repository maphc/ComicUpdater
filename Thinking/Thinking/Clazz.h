#include <iostream>
using namespace std;
class Integer {
	long i;
public:
	Integer* getThis() {
		return this;
	}
	Integer(long ii) :
	i(ii) {

	}

	friend const Integer& operator+(const Integer& i);
	friend const Integer& operator-(const Integer& i);
	friend const Integer& operator++(Integer& i); //prefix
	friend const Integer operator++(Integer& i, int); //postfix
	friend const Integer& operator--(Integer& i);
	friend const Integer operator--(Integer& i, int);
	friend int  operator!(const Integer& i);
	friend const Integer operator~(const Integer& i);
	friend Integer& operator*(Integer& i);
	friend Integer* operator&(Integer& i);
	//friend ostream& operator<<(ostream& os, Integer& i);
};

Integer& operator +(Integer & i) {
	cout << "operator +" << endl;
	return i;
}

Integer& operator -(Integer & i) {
	cout << "operator -" << endl;
	return i;
}

const Integer & operator ++(Integer & i) {
	cout << "++i" << endl;
	++i.i;
	return i;
}

const Integer operator ++(Integer & i, int) {
	cout << "i++" << endl;
	Integer before(i.i);
	i.i++;
	return before;
}

const Integer & operator --(Integer & i) {
	cout << "--i" << endl;
	--i.i;
	return i;
}

const Integer operator --(Integer & i, int) {
	cout << "i--" << endl;
	Integer before(i.i);
	i.i--;
	return before;
}

int operator !(const Integer & i) {
	cout << "!i" << endl;
	return !i.i;
}

const Integer operator ~(const Integer & i) {
	cout << "!i" << endl;
	return Integer(!i.i);
}

const Integer & operator *(const Integer & i) {
	cout << "*i" << endl;
	return i;
}

Integer *operator &(Integer & i) {
	cout << "&i" << endl;
	return i.getThis();
}

