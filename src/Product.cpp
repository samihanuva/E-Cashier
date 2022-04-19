#include "Product.h"
#include "Branch.h"

void Product::setProductInfo()
{
    ofstream f(file_path);
    string input;
    input = productID + "\n" + name + "\n" + category + "\n" + to_string(unitCostPrice) + "\n" + unit;
    f<<input;
}

void Product::getProductInfo()
{
    ifstream f(file_path);
    getline(f,productID);
    getline(f,name);
    getline(f,category);
    string s;
    getline(f,s);
    unitCostPrice = stod(s);
    getline(f,unit);
}

string Product::viewProductDetails()
{
    string res;
    res = "Product ID : "+productID
          +"\r\nName : "+name
          +"\r\nCategory : "+category
          +"\r\nCost Price : " + to_string(unitCostPrice) + "Tk. / "+ unit
          +"\r\n\r\n";
    return res;
}

string Product::viewProductDetailsInBranches()
{
    string branch;
    string res = "\r\n--- Daily requirement and available quantity of product "+productID+" in different brances ---";
    for(int i=1; i<=Branch::branches; i++)
    {
        branch = "B"+to_string(i);
        ifstream temp("branch/"+branch+"/"+branch+".txt");
        if(!temp) continue;
        Branch b("branch/"+branch+"/"+branch+".txt");
        b.getBranchInfo();
        b.getProductInfoOfBranch();
        map <string, pair <double,double> > ::iterator it;
        res = res + "\r\nBranch ID : "+branch;
        for(it = b.products.begin(); it!=b.products.end(); it++)
        {
            res = res + "\r\nProduct ID : "+it->first+"\r\nDaily requirement of unit : "+to_string(it->second.first) + "\r\nAvailable quantity of unit : " + to_string(it->second.second);
        }
        res+="\r\n";
    }
    return res;

}
