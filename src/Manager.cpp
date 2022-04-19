#include "Manager.h"
#include "Branch.h"
#include "Product.h"

extern string output;

void Manager::setUserID(string uid){
    userID = uid;
}

void Manager::setPassword(string p){
    password = p;
}

string Manager::getUserID(){
    return userID;
}

string Manager::getPassword(){
    return password;
}

void Manager::setInfo(){
    ofstream handle(file_path);
    string input;
    input = getUserID()+"\n"+getPassword()+"\n"+name+"\n"+gender+"\n"+contactNum+"\n"+address+"\n"+EmpID+"\n"+BranchID+"\n"+salary;
    handle << input;
}

void Manager::setInfo(string s1,string s2,string s3,string s4,string s5,string s6,string s7,string s8,string s9){
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

void Manager::getInfo(){
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

string Manager::viewDetails(){
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

void Manager::viewBranchEmployees()
{
    string s,filename = "branch/"+BranchID+"/"+BranchID+".txt",tempfile;
    ifstream handle,h;
    handle.open(filename);
    int cnt,i,j;
    output="";
    getline(handle,s);
    getline(handle,s);
    getline(handle,s);
    output+="\r\nTotal employees in the branch : ";
    output = output + s + " (including manager)\r\n\r\n";
    getline(handle,s);
    cnt = stoi(s);
    for(i=0;i<cnt;i++){
        getline(handle,s);
    }
    output = output + "\r\nCashier : ";
    getline(handle,s);
    output = output + s + "\r\n\r\n";
    cnt = stoi(s);
    for(i=0;i<cnt;i++){
        string m[10];
        getline(handle,s);
        tempfile = "branch/"+BranchID+"/employees/cashiers/" + s + ".txt";
        h.open(tempfile);
        for(j=0;j<9;j++){
            getline(h,m[j]);
        }
        output = output + "Employee ID : " + m[0]
            +"\r\nName : " + m[2]
            +"\r\nGender : " + m[3]
            +"\r\nContact Number : "+ m[4]
            +"\r\nAddress : "+ m[5]
            +"\r\nUser ID : "+ m[6]
            +"\r\nBranch ID : "+ m[7]
            +"\r\nSalary : " + m[8] + "\r\n\r\n";
    h.close();
    }
    output = output + "\r\nOthers : ";
    getline(handle,s);
    output = output + s + "\r\n";
    cnt = stoi(s);
    for(i=0;i<cnt;i++){
        string m[10];
        getline(handle,s);
        tempfile = "branch/"+BranchID+"/employees/" + s + ".txt";
        h.open(tempfile);
        for(j=0;j<7;j++){
            getline(h,m[j]);
        }
        output = output
            +"\r\nEmployee ID : " + m[4]
            + "\r\nName : " + m[0]
            +"\r\nGender : " + m[1]
            +"\r\nContact Number : "+ m[2]
            +"\r\nAddress : "+ m[3]
            +"\r\nBranch ID : "+ m[5]
            +"\r\nSalary : " + m[6] + "\r\n";
    h.close();
    }
    handle.close();

}

string Manager::viewBranchDetails()
{
    string file = "branch/"+BranchID+"/"+BranchID+".txt";
    Branch b(file);
    b.getBranchInfo();
    b.getProductInfoOfBranch();
    string res;
    int i;
    res = "Branch ID : "+ b.BranchID + "\r\nAddress : " + b.address + "\r\n\r\nNumber of employees : " + to_string(b.numberOfEmployees)
    + "\r\n\r\nManager : " + to_string(b.manager.size()) + "\r\n";
    for(i=0;i<b.manager.size();i++){
        res+=(b.manager[i]+"\r\n");
    }
    res += ("\r\nCashiers : "+ to_string(b.cashiers.size()) + "\r\n");
    for(i=0;i<b.cashiers.size();i++){
        res+=(b.cashiers[i]+"\r\n");
    }
    res += ("\r\nOther employees : "+to_string(b.otherEmployees.size()) + "\r\n");
    for(i=0;i<b.otherEmployees.size();i++){
        res+=(b.otherEmployees[i]+"\r\n");
    }
    res += ("\r\nProducts : "+to_string(b.products.size()) + "\r\n");
    map <string, pair <double,double> > ::iterator it;
    for(it = b.products.begin(); it!=b.products.end(); it++){
        res = res + "Product ID : "+ it->first + "\r\nDaily requirement : " + to_string(it->second.first) + "\r\nAvailable quantity : " + to_string(it->second.second) + "\r\n";
    }
    return res;
}

void Manager::viewAllProducts(){
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

