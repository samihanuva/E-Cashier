#include "Cashier.h"
#include "Branch.h"
#include<bits/stdc++.h>

extern string output;

void Cashier::setUserID(string uid){
    userID = uid;
}

void Cashier::setPassword(string p){
    password = p;
}

string Cashier::getUserID(){
    return userID;
}

string Cashier::getPassword(){
    return password;
}

void Cashier::setInfo(){
    ofstream handle(file_path);
    string input;
    input = getUserID()+"\n"+getPassword()+"\n"+name+"\n"+gender+"\n"+contactNum+"\n"+address+"\n"+EmpID+"\n"+BranchID+"\n"+salary;
    handle << input;
}

void Cashier::setInfo(string s1,string s2,string s3,string s4,string s5,string s6,string s7,string s8,string s9){
    userID = s1;
    password = s2;
    name = s3;
    gender = s4;
    contactNum = s5;
    address = s6;
    EmpID = s7;
    BranchID = s8;
    salary = s9;
    setInfo();
}

void Cashier::getInfo(){
       ifstream handle(file_path);
       getline(handle,userID);
       getline(handle,password);
       getline(handle,name);
       getline(handle,gender);
       getline(handle,contactNum);
       getline(handle,address);
       getline(handle,EmpID);
       getline(handle,BranchID);
       getline(handle,salary);
}

string Cashier::viewDetails(){
    string output;
    output = "Name : " + name
            +"\nGender : " + gender
            +"\nContact Number : "+ contactNum
            +"\nEmployee ID : "+ EmpID
            +"\nBranch ID : "+BranchID
            +"\nUser ID : "+ userID
            +"\nPassword : " + password
            +"\nAddress : " + address;
    return output;
}

void Cashier::viewAllProducts(){
    int i;
    output="";
    string file;
    string file2 = "branch/"+BranchID+"/"+BranchID+".txt";
    Branch b(file2);
    b.getBranchInfo();
    b.getProductInfoOfBranch();
    for(i=1;i<=Product::products;i++){
        file="product/P"+to_string(i)+".txt";
        ifstream f(file);
        if(!f) continue;
        Product p(file);
        p.getProductInfo();
        output+=p.viewProductDetails();
        output = output + "Daily requirement : " + to_string(b.products["P"+to_string(i)].first)
        + "\r\nAvailable quantity : " + to_string(b.products["P"+to_string(i)].second) + "\r\n\r\n";
    }
}

double Cashier::GenerateBill(map <string,double> orderlist)
{
    double total = 0;
    string file;
    output="--- Bill --- \r\n\r\n";
    Branch b("branch/"+BranchID+"/"+BranchID+".txt");
    b.getBranchInfo();
    b.getProductInfoOfBranch();
    map <string,double> ::iterator it;
    for(it=orderlist.begin();it!=orderlist.end();it++){
        b.products[it->first].second -= it->second;
        file="product/"+it->first+".txt";
        Product p(file);
        p.getProductInfo();
        total+=(it->second*p.unitCostPrice);
        output+="Product ID:"+it->first
                +"   Name:"+p.name
                +"    Quantity:"+p.unit+"X"+to_string(it->second)
                +"    Price:"+to_string(p.unitCostPrice)+"X"+to_string(it->second)+" Tk."
                +"\r\n";
        b.setProductInfoOfBranch();
    }
    return total;
}

