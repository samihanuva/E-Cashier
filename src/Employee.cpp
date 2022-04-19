#include "Employee.h"

void Employee::setInfo(){
    ofstream handle(file_path);
    string input;
    input = name+"\n"+gender+"\n"+contactNum+"\n"+address+"\n"+EmpID+"\n"+BranchID+"\n"+salary;
    handle << input;
}

void Employee::setInfo(string s1,string s2,string s3,string s4,string s5,string s6,string s7){
    name = s1;
    gender = s2;
    contactNum = s3;
    address = s4;
    EmpID = s5;
    BranchID = s6;
    salary = s7;
    setInfo();
}

void Employee::getInfo(){
       ifstream handle(file_path);
       getline(handle,name);
       getline(handle,gender);
       getline(handle,contactNum);
       getline(handle,address);
       getline(handle,EmpID);
       getline(handle,BranchID);
       getline(handle,salary);
}

string Employee::viewDetails(){
    string output;
    output = "Name : " + name
            +"\nGender : " + gender
            +"\nContact Number : "+ contactNum
            +"\nEmployee ID : "+ EmpID
            +"\nBranch ID : "+BranchID
            +"\nAddress : " + address;
    return output;
}
