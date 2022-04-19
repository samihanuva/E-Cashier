#include "Customer.h"

void Customer::getInfo()
{
    ifstream handle(file_path);
    getline(handle,name);
    getline(handle,gender);
    getline(handle,contactNum);
    getline(handle,address);
    getline(handle,memberID);
    getline(handle,membershipDate);
    getline(handle,validity);
    string s;
    getline(handle,s);
    discount = stod(s);
}

void Customer::setInfo(){
    ofstream handle(file_path);
    string input;
    input = name+"\n"+gender+"\n"+contactNum+"\n"+address+"\n"+memberID+"\n"+membershipDate+"\n"+validity+"\n"+to_string(discount);
    handle << input;
}

string Customer::viewDetails()
{
    string res = "Member ID : "+memberID+"\r\nName : "+name+"\r\nGender : "+gender+"\r\nContact Number : "+contactNum
                +"\r\nAddress : "+address+"\r\nMembership Initiation Date : "+membershipDate
                +"\r\nValidity : "+validity+"\r\nDiscount : "+to_string(discount)+"%\r\n\r\n";
    return res;
}

string Customer::viewAllCustomers()
{
    int i;
    string res = "";
    string file;
    for(i=1; i<=Customer::members; i++)
    {
        file="customer/CUST"+to_string(i)+".txt";
        ifstream f(file);
        if(!f) continue;
        Customer c(file);
        c.getInfo();
        res += c.viewDetails();
    }
    return res;
}



