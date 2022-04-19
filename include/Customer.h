#ifndef CUSTOMER_H
#define CUSTOMER_H
#include<bits/stdc++.h>
#include<Person.h>

using namespace std;

class Customer:public Person
{
public:
    static int members;
    string memberID;
    string membershipDate;
    string validity;
    double discount;
    Customer(string path)
    {
        file_path=path;
    }
    void getInfo();
    void setInfo();
    string viewDetails();
    static string viewAllCustomers();

protected:

private:


};

#endif // CUSTOMER_H
