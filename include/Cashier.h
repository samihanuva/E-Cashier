#ifndef CASHIER_H
#define CASHIER_H
#include<bits/stdc++.h>
#include<User.h>
#include<Employee.h>

using namespace std;

class Cashier:public User,public Employee
{
public:
    static int cashiers;
    Cashier(string path):Employee(path){}
    void setInfo();
    void setInfo(string s1,string s2,string s3,string s4,string s5,string s6,string s7,string s8,string s9);
    void getInfo();
    string getPassword();
    void setPassword(string);
    string getUserID();
    void setUserID(string);
    void viewAllProducts();
    double GenerateBill(map <string,double>);
    string viewDetails();
protected:

private:

};

#endif // CASHIER_H
