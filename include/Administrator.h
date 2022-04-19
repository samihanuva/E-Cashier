#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H
#include<bits/stdc++.h>
#include<User.h>
#include "Branch.h"
#include "Product.h"

using namespace std;

class Administrator:public User
{
private:

public:
    static int admin;
    Administrator(string path)
    {
        file_path=path;
    }
    void setInfo();
    void setInfo(string,string,string,string,string,string);
    void getInfo();
    bool viewEmployeesByBranchID(string b);
    string viewEmployeeInfoByEmpFile(string f);
    void deleteProduct(string,string);
    void addProduct(Product*,double,double);
    string viewDetails();
    bool searchBranch(string);
    void viewAllBranches();
    void viewAllProducts();

protected:

};

#endif // ADMINISTRATOR_H
