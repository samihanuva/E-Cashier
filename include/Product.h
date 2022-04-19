#ifndef PRODUCT_H
#define PRODUCT_H
#include<bits/stdc++.h>

using namespace std;

class Product
{
public:
    static int products;
    string file_path;
    string productID;
    string name;
    string category;
    string unit;
    double unitCostPrice;
    Product(string path)
    {
        file_path=path;
    }
    void setProductInfo();
    void getProductInfo();
    string viewProductDetails();
    string viewProductDetailsInBranches();

protected:

private:

};

#endif // PRODUCT_H
