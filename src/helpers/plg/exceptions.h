#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include <string>
#include <stdexcept>

// using namespace std;

class MyException : public std::exception
{
public:
    MyException(const std::string &msg) : m_msg(msg)
    {
       //  std::cout << "MyException::MyException - set m_msg to:" << m_msg << std::endl;
       // std::cout << "Exception msg:" << m_msg << std::endl;
    }

    ~MyException()
    {
       // std::cout << "MyException::~MyException" << std::endl;
    }

    virtual const char *what() const throw()
    {
        // cout << "MyException::what" << endl;
        return m_msg.c_str();
    }

    const std::string m_msg;
};


#endif // EXCEPTIONS_H