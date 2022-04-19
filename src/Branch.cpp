#include "Branch.h"

void Branch::setBranchInfo(){
    ofstream handle;
    handle.open(file_path);
    string input;
    int i;
    input= BranchID + "\n" + address + "\n" + to_string(numberOfEmployees) + "\n" + to_string(manager.size()) + "\n";
    for(i=0;i<manager.size();i++){
        input+=(manager[i]+"\n");
    }
    input += (to_string(cashiers.size()) + "\n");
    for(i=0;i<cashiers.size();i++){
        input+=(cashiers[i]+"\n");
    }
    input += (to_string(otherEmployees.size()) + "\n");
    for(i=0;i<otherEmployees.size();i++){
        input+=(otherEmployees[i]+"\n");
    }
    handle << input;

}

void Branch::getBranchInfo(){
    ifstream handle;
    handle.open(file_path);
    int i,m,c,o;
    string s;
    getline(handle,BranchID);
    getline(handle,address);
    getline(handle,s);
    numberOfEmployees = stoi(s);
    getline(handle,s);
    m = stoi(s);
    for(i=0;i<m;i++){
        getline(handle,s);
        manager.push_back(s);
    }
    getline(handle,s);
    c = stoi(s);
    for(i=0;i<c;i++){
        getline(handle,s);
        cashiers.push_back(s);
    }
    getline(handle,s);
    o = stoi(s);
    for(i=0;i<o;i++){
        getline(handle,s);
        otherEmployees.push_back(s);
    }
}

void Branch::addManager(string s){
    manager.push_back(s);
    numberOfEmployees++;
    setBranchInfo();
}

void Branch::addCashier(string s){
    cashiers.push_back(s);
    numberOfEmployees++;
    setBranchInfo();
}

void Branch::addOtherEmployee(string s){
    otherEmployees.push_back(s);
    numberOfEmployees++;
    setBranchInfo();
}

void Branch::deleteManager(string s){
    vector <string> :: iterator it;
    it = find(manager.begin(),manager.end(),s);
    manager.erase(it);
    numberOfEmployees--;
    setBranchInfo();
}

void Branch::deleteCashier(string s){
    vector <string> :: iterator it;
    it = find(cashiers.begin(),cashiers.end(),s);
    cashiers.erase(it);
    numberOfEmployees--;
    setBranchInfo();
}

void Branch::deleteOtherEmployee(string s){
vector <string> :: iterator it;
    it = find(otherEmployees.begin(),otherEmployees.end(),s);
    otherEmployees.erase(it);
    numberOfEmployees--;
    setBranchInfo();
}

string Branch::displayBranchinfo(){
    string res = "",tempfile;
    ifstream h;
    int i,j;
    res = "Branch ID : "+BranchID
    +"\r\n\r\nAddress : "+address
    +"\r\nNumber of employees : "+ to_string(numberOfEmployees)
    +"\r\nManager : "+to_string(manager.size())+"\r\n";
    for(i=0;i<manager.size();i++){
        string m[10];
        tempfile = "branch/"+BranchID+"/employees/manager/" + manager[i] + ".txt";
        h.open(tempfile);
        for(j=0;j<9;j++){
            getline(h,m[j]);
        }
        res = res + "Employee ID : " + m[0]
            +"\r\nName : " + m[2]
            +"\r\nGender : " + m[3]
            +"\r\nContact Number : "+ m[4]
            +"\r\nAddress : "+ m[5]
            +"\r\nUser ID : "+ m[6]
            +"\r\nBranch ID : "+ m[7]
            +"\r\nSalary : " + m[8] + "\r\n";
    h.close();
    }
    res+=("\r\nCashiers : "+to_string(cashiers.size())+"\r\n");
    for(i=0;i<cashiers.size();i++){
        string m[10];
        tempfile = "branch/"+BranchID+"/employees/cashiers/" + cashiers[i] + ".txt";
        h.open(tempfile);
        for(j=0;j<9;j++){
            getline(h,m[j]);
        }
        res = res + "Employee ID : " + m[0]
            +"\r\nName : " + m[2]
            +"\r\nGender : " + m[3]
            +"\r\nContact Number : "+ m[4]
            +"\r\nAddress : "+ m[5]
            +"\r\nUser ID : "+ m[6]
            +"\r\nBranch ID : "+ m[7]
            +"\r\nSalary : " + m[8] + "\r\n";
    h.close();
    }
    res+=("\r\nOthers : "+to_string(otherEmployees.size())+"\r\n");
    for(i=0;i<otherEmployees.size();i++){
        string m[10];
        tempfile = "branch/"+BranchID+"/employees/" + otherEmployees[i] + ".txt";
        h.open(tempfile);
        for(j=0;j<7;j++){
            getline(h,m[j]);
        }
        res = res
            +"\r\nEmployee ID : " + m[4]
            + "\r\nName : " + m[0]
            +"\r\nGender : " + m[1]
            +"\r\nContact Number : "+ m[2]
            +"\r\nAddress : "+ m[3]
            +"\r\nBranch ID : "+ m[5]
            +"\r\nSalary : " + m[6] + "\r\n";
    h.close();
    }
    res+="\r\n";
    return res;
}

void Branch::setProductInfoOfBranch()
{
    ofstream f("branch/"+BranchID+"/products.txt");
    string input = "";
    map <string, pair <double,double> > ::iterator it;
    for(it = products.begin(); it!=products.end(); it++){
        input = input + it->first + "\n" + to_string(it->second.first) + "\n" + to_string(it->second.second) + "\n";
    }
    f<<input;
}

void Branch::deleteProductFromBranch(string s)
{
    products.erase(s);
    setProductInfoOfBranch();
}

void Branch::addProductInBranch(string id,double req,double available){
    products[id] = make_pair(req,available);
    setProductInfoOfBranch();
}

void Branch::getProductInfoOfBranch()
{
    string s1,s2,s3,filename = "branch/"+BranchID+"/products.txt";
    ifstream f(filename);
    for(int i=1;i<=Product::products;i++){
        ifstream tempfile("product/P"+to_string(i)+".txt");
        if(!tempfile) continue;
        getline(f,s1);
        getline(f,s2);
        getline(f,s3);
        products[s1] = make_pair(stod(s2),stod(s3));
    }
}

void Branch::restockProducts()
{
    ofstream f("branch/"+BranchID+"/products.txt");
    string input = "";
    map <string, pair <double,double> > ::iterator it;
    for(it = products.begin(); it!=products.end(); it++){
        input = input + it->first + "\n" + to_string(it->second.first) + "\n" + to_string(it->second.first) + "\n";
    }
    f<<input;
}

