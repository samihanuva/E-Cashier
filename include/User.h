#ifndef USER_H
#define USER_H
#include <bits/stdc++.h>
#include<Person.h>

using namespace std;

class User:virtual public Person
{
protected:

public:
    string userID;
    string password;
    virtual string viewDetails()=0;
    virtual void setInfo()=0;
    void changePassword(string);

private:
};

#endif // USER_H
