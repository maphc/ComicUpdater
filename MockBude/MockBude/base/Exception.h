#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_
#include "stdafx.h"

using namespace std;


struct Grade {
    enum Type {
        WARNING, //����
        MINOR, //��Ҫ
        MAJOR, //�ش�
        CRITICAL, //����        
        INDETERMINATE //δȷ����
    };
    static const char* toString(Type grade);
};

#define ASSIGN_VMESSAGE(m_message, msg) \
	char buf[2048]; \
	va_list ap; \
	va_start(ap, msg); \
	vsnprintf(buf, sizeof(buf), msg, ap); \
	va_end(ap); \
	m_message = buf

//�����Exception����, ����clone����ԭ��.
//���ECAUSE��cause������һ��, ����ͨ��copy���캯������.
//ֻ��ͨ����װ��, �ѱ�Ҫ����Ϣ��װ��__std_exception_wrapper��.
#define ASSIGN_CAUSE(m_cause, cause) \
	const Exception * e = dynamic_cast < const Exception * > (& cause); \
	if (e != 0) \
	m_cause = counted_ptr < exception > (e->clone()); \
	else if (typeid(cause) == typeid(const ECAUSE &)) \
	m_cause = counted_ptr < exception > (new ECAUSE(cause)); \
	else \
	m_cause = counted_ptr < exception > (new __std_exception_wrapper(cause))

/** ����һ���µ��쳣���� */
#define DECLARE_EXCEPTION(ExpClass, BaseClass)                                                        \
class ExpClass : public BaseClass {                                                                   \
public:                                                                                               \
	ExpClass(const ExpClass& e) :                                                                       \
	BaseClass(e) {                                                                                    \
}                                                                                                   \
	ExpClass(int code, const string& msg) :                                                                       \
	BaseClass(code, msg) {                                                                                    \
}                                                                                                   \
	\
	ExpClass(int code, const char*  msg, ...)  { \
	m_line = -1; m_grade = Grade::INDETERMINATE; m_code = code; \
	ASSIGN_VMESSAGE(BaseClass::m_message, msg);                                                                             \
}                                                                                                    \
	\
	ExpClass(const string& file, int line, Grade::Type grade, int code, const string & msg)                     \
	:BaseClass(file, line, grade, code, msg) {                                                         \
}                                                                                                 \
	ExpClass(const string & file, int line, Grade::Type grade, int code, const char*  msg, ...) { \
	m_file = file; m_line = line; m_grade = grade; m_code = code; \
	ASSIGN_VMESSAGE(BaseClass::m_message, msg);                                                                       \
}                                                                                                \
	\
	template<typename ECAUSE>                                                                           \
	ExpClass(const string& file, int line, const ECAUSE& cause, Grade::Type grade, int code, const string & msg)\
	:BaseClass(file, line, cause, grade, code, msg) {                                                  \
}                                                                                                    \
	\
	template < typename ECAUSE >                                                                        \
	ExpClass(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const char*  msg, ...) { \
	BaseClass::m_file = file; BaseClass::m_line = line; BaseClass::m_grade = grade; BaseClass::m_code = code; \
	ASSIGN_CAUSE(BaseClass::m_cause, cause);                                                                            \
	ASSIGN_VMESSAGE(BaseClass::m_message, msg);                                                                           \
}                                                                                               \
	\
	Exception* clone() const throw() {                                                                  \
	return new ExpClass(*this);                                                                       \
}                                                                                                   \
protected:                                                                                          \
	ExpClass() {}                                                                                     \
}


/**
 * �����쳣�Ļ��� �����쳣����, �쳣��Ϣ, �����ļ�, �쳣�������к�, �������쳣�ĸ��쳣
 * @author luzl
 */

class __std_exception_wrapper : public exception {
    string m_classname;
    string m_message;

    friend inline ostream & operator << (ostream & out, const __std_exception_wrapper & e) {
        out << e.m_classname << ": " << e.m_message;
        return out;
    }

public:
    __std_exception_wrapper(const exception & e) {
        m_classname = typeid(e).name();
        m_message = e.what();
    }

    const char * what() const throw() {
        return m_message.c_str();
    }

    virtual ~__std_exception_wrapper() throw() {
    }
};


inline ostream & operator << (ostream & out, const exception & e) {
    out << typeid(e).name() << ": " << e.what();
    return out;
}



inline const char* Grade::toString(Grade::Type grade) {
	if(grade == Grade::WARNING)
		return "WARNING";
	else if(grade == Grade::MINOR)
		return "MINOR";
	else if(grade == Grade::MAJOR)
		return "MAJOR";
	else if(grade == Grade::CRITICAL)
		return "CRITICAL";
	else 
		return "INDETERMINATE";	
}





class Exception : public exception {
public:
    /** �������캯�� */
    explicit Exception(const Exception & e);
    
    /** ���캯�� */
    Exception(int code, const string & msg);
    
    /** ���캯�� */
    Exception(int code, const char*  msg, ...);

    /** ���캯�� */
    Exception(const string & file, int line, Grade::Type grade, int code, const string & msg);

    /** ���캯�� */
    template < typename ECAUSE >
    Exception(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const string & msg);
    
    /** ���캯�� */
    Exception(const string & file, int line, Grade::Type grade, int code, const char*  msg, ...);

    /** ���캯�� */
    template < typename ECAUSE >
    Exception(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const char*  msg, ...);

    /** �������� */
    virtual ~Exception() throw();

    /** ��׼C++�쳣��Ϣ */
    virtual const char * what() const throw();

    /** ��ȡ���� */
    Grade::Type getGrade() const throw();

    /** �쳣���� */
    int getCode() const throw();

    /** �쳣��Ϣ */
    const string & getMessage() const throw();

    /** �ļ��� */
    const string & getFile() const throw();

    /** �к� */
    int getLine() const throw();

    /** ��ԭ��, ���û�и�ԭ��, �򷵻� NULL */
    const exception * getCause() const throw();

    /** ��¡��ǰ�쳣���� */
    virtual Exception * clone() const throw();

    /** �ַ������쳣��ȫ����Ϣ, ����ԭ���ջ��Ϣ */
    string toString() const;

protected:
    Exception() {}
    
    /** �쳣���ֵ��ļ��� */
    string m_file;

    /** �쳣�������к� */
    int m_line;

    /** ���󼶱� */
    Grade::Type m_grade;

    /** �쳣���� */
    int m_code;

    /** �쳣��Ϣ */
    string m_message;

    /**
     * ������쳣�ĸ��쳣, ����һ�������ļ������ݿ��׳�SQLException, ���ĸ��쳣�п�����IOException
     * @supplierCardinality 0..1
     * @label cause
     */
    counted_ptr < exception > m_cause;
};

inline ostream & operator << (ostream & out, const Exception & e) {
	out << "[" << Grade::toString(e.getGrade()) << "]" << e.getFile() << ":" << e.getLine() << "," << typeid(e).name() << "-" <<
		e.getCode() << ": " << e.what();
	if (e.getCause() != 0) {
		out << endl << "Caused by: ";
		const Exception * cause = dynamic_cast < const Exception * > (e.getCause());
		if (cause != 0) {
			out << * cause;
		} else {
			const __std_exception_wrapper * cause1 = dynamic_cast < const __std_exception_wrapper * > (e.getCause());
			if (cause1 != 0) {
				out << * cause1;
			} else {
				out << * e.getCause();
			}
		}
	}
	return out;
}



inline Exception::Exception(const Exception & e) : m_file(e.m_file), m_line(e.m_line), m_grade(e.m_grade), m_code(e.m_code),
m_message(e.m_message), m_cause(e.m_cause) {
}

inline Exception::Exception(int code, const string & msg) : m_line(-1), m_grade(Grade::INDETERMINATE), m_code(code), m_message(msg) {
}

inline Exception::Exception(int code, const char*  msg, ...) :  m_line(-1), m_grade(Grade::INDETERMINATE), m_code(code)  {
	ASSIGN_VMESSAGE(Exception::m_message, msg);
}  

inline Exception::Exception(const string & file, int line, Grade::Type grade, int code, const string & msg) : m_file(file), m_line(line), m_grade(grade), m_code(code), m_message(msg) {
}

inline Exception::Exception(const string & file, int line, Grade::Type grade, int code, const char*  msg, ...) : m_file(file), m_line(line), m_grade(grade), m_code(code){
	ASSIGN_VMESSAGE(Exception::m_message, msg);    
}



template < typename ECAUSE >
inline Exception::Exception(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const string & msg) : m_file(file), m_line(line), m_grade(grade), m_code(code), m_message(msg) {
	ASSIGN_CAUSE(Exception::m_cause, cause);
}

template < typename ECAUSE >
inline Exception::Exception(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const char*  msg, ...) : m_file(file), m_line(line), m_grade(grade), m_code(code){
	ASSIGN_CAUSE(Exception::m_cause, cause);
	ASSIGN_VMESSAGE(Exception::m_message, msg);
}

/** �������� */
inline Exception::~Exception() throw() {
}

/** ��׼C++�쳣��Ϣ */
inline const char * Exception::what() const throw() {
	return m_message.c_str();
}

/** �쳣���� */
inline Grade::Type Exception::getGrade() const throw() {
	return m_grade;
}

/** �쳣���� */
inline int Exception::getCode() const throw() {
	return m_code;
}

/** �쳣��Ϣ */
inline const string & Exception::getMessage() const throw() {
	return m_message;
}

/** �ļ��� */
inline const string & Exception::getFile() const throw() {
	return m_file;
}

/** �к� */
inline int Exception::getLine() const throw() {
	return m_line;
}

/** ��ԭ��, ���û�и�ԭ��, �򷵻� NULL */
inline const exception * Exception::getCause() const throw() {
	return m_cause.get();
}

/** ��¡��ǰ�쳣���� */
inline Exception * Exception::clone() const throw() {
	return new Exception(* this);
}

inline string Exception::toString() const {
	ostringstream buffer;
	buffer << * this;
	return buffer.str();
}

/** �׳�һ���쳣 */
#define THROW(expclass, msg) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg)
#define THROW_P1(expclass, msg, p1) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg, p1)
#define THROW_P2(expclass, msg, p1, p2) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg, p1, p2)
#define THROW_P3(expclass, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg, p1, p2, p3)
#define THROW_P4(expclass, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg, p1, p2, p3, p4)

#define THROW_C(expclass, code, msg) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg)
#define THROW_C_P1(expclass, code, msg, p1) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg, p1)
#define THROW_C_P2(expclass, code, msg, p1, p2) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg, p1, p2)
#define THROW_C_P3(expclass, code, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg, p1, p2, p3)
#define THROW_C_P4(expclass, code, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg, p1, p2, p3, p4)

#define THROW_GC(expclass, grade, code, msg) throw expclass(__FILE__, __LINE__, grade, code, msg)
#define THROW_GC_P1(expclass, grade, code, msg, p1) throw expclass(__FILE__, __LINE__, grade, code, msg, p1)
#define THROW_GC_P2(expclass, grade, code, msg, p1, p2) throw expclass(__FILE__, __LINE__, grade, code, msg, p1, p2)
#define THROW_GC_P3(expclass, grade, code, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, grade, code, msg, p1, p2, p3)
#define THROW_GC_P4(expclass, grade, code, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, grade, code, msg, p1, p2, p3, p4)

/** �׳�һ����ԭ����쳣 */
#define THROW_A(expclass, cause, msg) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg)
#define THROW_A_P1(expclass, cause, msg, p1) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg, p1)
#define THROW_A_P2(expclass, cause, msg, p1, p2) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg, p1, p2)
#define THROW_A_P3(expclass, cause, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg, p1, p2, p3)
#define THROW_A_P4(expclass, cause, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg, p1, p2, p3, p4)

#define THROW_AC(expclass, cause, code, msg) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg)
#define THROW_AC_P1(expclass, cause, code, msg, p1) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg, p1)
#define THROW_AC_P2(expclass, cause, code, msg, p1, p2) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg, p1, p2)
#define THROW_AC_P3(expclass, cause, code, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg, p1, p2, p3)
#define THROW_AC_P4(expclass, cause, code, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg, p1, p2, p3, p4)

#define THROW_AGC(expclass, cause, grade, code, msg) throw expclass(__FILE__, __LINE__, cause, grade, code, msg)
#define THROW_AGC_P1(expclass, cause, grade, code, msg, p1) throw expclass(__FILE__, __LINE__, cause, grade, code, msg, p1)
#define THROW_AGC_P2(expclass, cause, grade, code, msg, p1, p2) throw expclass(__FILE__, __LINE__, cause, grade, code, msg, p1, p2)
#define THROW_AGC_P3(expclass, cause, grade, code, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, cause, grade, code, msg, p1, p2, p3)
#define THROW_AGC_P4(expclass, cause, grade, code, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, cause, grade, code, msg, p1, p2, p3, p4)



#endif //_EXCEPTION_H_
