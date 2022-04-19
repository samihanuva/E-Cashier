#ifndef PERSON_H
#define PERSON_H
#include <bits/stdc++.h>

using namespace std;

class Person
{
protected:

public:
    string file_path;
    string name;
    string gender;
    string contactNum;
    string address;
    virtual string viewDetails()=0;

private:
};

#endif // PERSON_H
