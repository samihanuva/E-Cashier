#ifndef BRANCH_H
#define BRANCH_H
#include<bits/stdc++.h>
#include "Product.h"

using namespace std;

class Branch
{
public:
    static int branches;
    string file_path;
    string BranchID;
    string address;
    int numberOfEmployees;
    vector <string> manager;
    vector <string> cashiers;
    vector <string> otherEmployees;
    map <string, pair <double,double> > products;
    Branch(string file){
       file_path = file;
    }
    string displayBranchinfo();
    void getBranchInfo();
    void setBranchInfo();
    void getProductInfoOfBranch();
    void setProductInfoOfBranch();
    void restockProducts();
    void deleteProductFromBranch(string);
    void addProductInBranch(string,double,double);
    void addManager(string);
    void addCashier(string);
    void addOtherEmployee(string);
    void deleteManager(string);
    void deleteCashier(string);
    void deleteOtherEmployee(string);
protected:

private:

};

#endif // BRANCH_H
