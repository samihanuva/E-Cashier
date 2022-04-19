#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include<bits/stdc++.h>
#include<Person.h>

using namespace std;

class Employee:virtual public Person
{
public:
    static int employees;
    string EmpID;
    string BranchID;
    string salary;
    Employee(string path)
    {
        file_path=path;
    }
    virtual string viewDetails();
    virtual void getInfo();
    virtual void setInfo();
    virtual void setInfo(string s1,string s2,string s3,string s4,string s5,string s6,string s7);
    virtual string getPassword(){};
    virtual void setPassword(string){};
    virtual string getUserID(){};
    virtual void setUserID(string){};

protected:

private:
};

#endif // EMPLOYEE_H
