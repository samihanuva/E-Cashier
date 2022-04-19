#include "Administrator.h"
#include "Branch.h"
#include "Product.h"

extern string output;

using namespace std;

void Administrator::setInfo(){
    ofstream handle(file_path);
    string input;
    input = userID+"\n"+password+"\n"+name+"\n"+gender+"\n"+contactNum+"\n"+address;
    handle << input;
}

void Administrator::setInfo(string s1,string s2,string s3,string s4,string s5,string s6){
    userID = s1;
    password = s2;
    name = s3;
    gender = s4;
    contactNum = s5;
    address = s6;
    setInfo();
}

void Administrator::getInfo(){
       ifstream handle(file_path);
       getline(handle,userID);
       getline(handle,password);
       getline(handle,name);
       getline(handle,gender);
       getline(handle,contactNum);
       getline(handle,address);
}

string Administrator::viewDetails(){
    string output;
    output = "Name : " + name
            +"\nGender : " + gender
            +"\nContact Number : "+ contactNum
            +"\nUser ID : "+ userID
            +"\nPassword : " + password
            +"\nAddress : " + address;
    return output;
}

bool Administrator::viewEmployeesByBranchID(string b){
    string s,filename = "branch/"+b+"/"+b+".txt", tempfile;
    ifstream handle,h;
    handle.open(filename);
    int cnt,i,j;
    if(!handle) return false;
    output="";
    getline(handle,s);
    getline(handle,s);
    output+="\r\nTotal employees : ";
    getline(handle,s);
    output = output + s + "\r\n\r\n\r\nManager : ";
    getline(handle,s);
    output = output + s + "\r\n\r\n";
    cnt = stoi(s);
    for(i=0;i<cnt;i++){
        string m[10];
        getline(handle,s);
        tempfile = "branch/"+b+"/employees/manager/" + s + ".txt";
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
    output = output + "\r\nCashier : ";
    getline(handle,s);
    output = output + s + "\r\n\r\n";
    cnt = stoi(s);
    for(i=0;i<cnt;i++){
        string m[10];
        getline(handle,s);
        tempfile = "branch/"+b+"/employees/cashiers/" + s + ".txt";
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
        tempfile = "branch/"+b+"/employees/" + s + ".txt";
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
    return true;
}

string Administrator::viewEmployeeInfoByEmpFile(string f){
    ifstream handle;
    handle.open(f);
    string result="",s[12];
    int i = 1;
    while(getline(handle,s[i])){
        i++;
    }
    if(i>8){
        result = "Name : " + s[3]
            +"\r\nGender : " + s[4]
            +"\r\nContact Number : "+ s[5]
            +"\r\nEmployee ID : "+ s[7]
            +"\r\nBranch ID : "+ s[8]
            +"\r\nUser ID : "+ s[1]
            +"\r\nAddress : " + s[6]
            +"\r\nSalary : " + s[9];
    }
    else{
        result = "Name : " + s[1]
            +"\r\nGender : " + s[2]
            +"\r\nContact Number : "+ s[3]
            +"\r\nEmployee ID : "+ s[5]
            +"\r\nBranch ID : "+ s[6]
            +"\r\nAddress : " + s[4]
            +"\r\nSalary : " + s[7];
    }
    return result;
}

bool Administrator::searchBranch(string path){
    ifstream f(path);
    if(f){
        Branch br(path);
        br.getBranchInfo();
        output+=br.displayBranchinfo();
        f.close();
        return true;
    }
    f.close();
    return false;

}

void Administrator::viewAllBranches(){
    int i;
    output="";
    string file;
    for(i=1;i<=Branch::branches;i++){
        file="branch/B"+to_string(i)+"/B"+to_string(i)+".txt";
        bool r = searchBranch(file);
        if(r) output+="\r\n";
    }
}

void Administrator::viewAllProducts(){
    int i;
    output="";
    string file;
    for(i=1;i<=Product::products;i++){
        file="product/P"+to_string(i)+".txt";
        ifstream f(file);
        if(!f) continue;
        Product p(file);
        p.getProductInfo();
        output+=p.viewProductDetails();
    }

}

void Administrator::deleteProduct(string id,string path)
{
    int k = remove(path.c_str());
    for(int i=1;i<=Branch::branches;i++){
        ifstream tempfile("branch/B"+to_string(i)+"/B"+to_string(i)+".txt");
        if(!tempfile) continue;
        Branch br("branch/B"+to_string(i)+"/B"+to_string(i)+".txt");
        br.getBranchInfo();
        br.getProductInfoOfBranch();
        br.deleteProductFromBranch(id);
    }
}

void Administrator::addProduct(Product *p,double req,double available)
{
    p->setProductInfo();
    for(int i=1;i<=Branch::branches;i++){
        ifstream tempfile("branch/B"+to_string(i)+"/B"+to_string(i)+".txt");
        if(!tempfile) continue;
        Branch br("branch/B"+to_string(i)+"/B"+to_string(i)+".txt");
        br.getBranchInfo();
        br.getProductInfoOfBranch();
        br.addProductInBranch(p->productID,req,available);
    }
}
