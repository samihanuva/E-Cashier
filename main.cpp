#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <bits/stdc++.h>
#include <tchar.h>
#include <windows.h>
#include <typeinfo>
#include <ctime>
#include <Person.h>
#include <User.h>
#include <Administrator.h>
#include <Manager.h>
#include <Cashier.h>
#include <Employee.h>
#include <Branch.h>
#include <Customer.h>
#include <Product.h>

using namespace std;

#define XSCREEN 500 //CW_USEDEFAULT can be used
#define YSCREEN 90
#define MaxTextBox 570

LRESULT CALLBACK WinPro(HWND, UINT, WPARAM, LPARAM);
Administrator* a;
Manager *m;
Cashier *c;
Employee *e,*e2;
Branch *b;
Customer *cust;
Product *prodct;

ofstream writeFile;
ifstream readFile;
string file,current_user,branch,output;
WNDCLASSEX temp;

/* handles or pointers to our windows */
HWND edit[15],textbox1,textbox2,textbox3,textbox4,hwnd,hwndE,hwndE1,hwndE2,hwndE3,hwndB,hwndB1,hwndB2,hwndP,hwndP1,hwndP2,hwndP3,hwndc,hwndc1,hwndc2,
     hwnd1,hwnd2,hwnd3,hwnd4,hwnd5,hwnd6,hwnd7,hwnd8;
int start = 0,return_val;
stack <HWND> levels;
map <string,double> orderlist;
map <string,double> ::iterator it;
bool bill,member;

//static variables
int Branch::branches;
int Manager::managers;
int Cashier::cashiers;
int Employee::employees;
int Product::products;
int Customer::members;

void setStaticValues()
{
    string s;
    readFile.open("Total.txt");

    getline(readFile,s);
    Branch::branches = stoi(s);

    getline(readFile,s);
    Manager::managers = stoi(s);

    getline(readFile,s);
    Cashier::cashiers = stoi(s);

    getline(readFile,s);
    Employee::employees = stoi(s);

    getline(readFile,s);
    Product::products = stoi(s);

    getline(readFile,s);
    Customer::members = stoi(s);

    readFile.close();
}

void saveStaticValues()
{
    string s;
    writeFile.open("Total.txt");
    s = to_string(Branch::branches)+"\n"+to_string(Manager::managers)+"\n"+to_string(Cashier::cashiers)
        +"\n"+to_string(Employee::employees)+"\n"+to_string(Product::products)+"\n"+to_string(Customer::members);
    writeFile<<s;
    writeFile.close();
}

bool authorization(int call, HWND h)
{
    if(call == 1) file = "administrator/";
    else file = "branch/";
    char temp[102];
    bool success = false;
    string userid,pass,str1,str2;
    GetWindowText(textbox1,&temp[0],100);
    userid = temp;
    int i=1;
    branch = "B";
    while(1)
    {
        if(userid[i]>='0' && userid[i]<='9')
        {
            branch+=userid[i];
            i++;
        }
        else break;
    }
    if(call!=1) file = file + branch + "/employees/";
    if(call == 2) file += "manager/";
    else if(call == 3) file += "cashiers/";
    GetWindowText(textbox2,&temp[0],100);
    pass = temp;
    readFile.close();
    writeFile.close();
    file = file+userid+".txt";
    readFile.open(file);
    if(!readFile)
    {
        MessageBox(h,"Incorrect User ID or Password.\nTry again!","Log in failed!",MB_OK);
    }
    else
    {
        getline(readFile,str1);
        getline(readFile,str2);

        if(str1==userid && str2==pass)
        {
            MessageBox(h,"Welcome!","Log in successful!",MB_OK);
            success = true;
        }
        else
        {
            MessageBox(h,"Incorrect User ID or Password.\nTry again!","Log in failed!",MB_OK);
        }
    }
    current_user = userid;
    SetWindowText(textbox1,0);
    SetWindowText(textbox2,0);
    readFile.close();
    return success;

}

int LogOut(string user)
{
    int res = MessageBox(levels.top(),"Do you want to log out?","Logging Out!",MB_OKCANCEL);
    if(res==2) return -1;
    DestroyWindow(levels.top());
    levels.pop();
    DestroyWindow(levels.top());
    levels.pop();
    EnableWindow(levels.top(),true);
    string id = user+" "+current_user+" Logged Out!";
    MessageBox(levels.top(),id.c_str(),"Logged Out!",MB_OK);
    return 1;
}

void changePassword(User *u)
{
    EnableWindow(hwnd8,false);
    char new_pass[102];
    int l = GetWindowText(textbox4,&new_pass[0],100);
    if(l<=0)
    {
        MessageBox(hwnd8,"Password length should be greater than 0.","Password could not be changed!",MB_OK);
        SetWindowText(textbox4,0);
        EnableWindow(hwnd8,true);
        return;
    }
    if(new_pass == u->password)
    {
        MessageBox(hwnd8,"Password is not changed.","Password is not changed!",MB_OK);
        SetWindowText(textbox4,0);
        EnableWindow(hwnd8,true);
        return;
    }
    u->changePassword(new_pass);
    u->setInfo();
    MessageBox(hwnd8,"You successfully changed your password.","Password changed!",MB_OK);
    SetWindowText(textbox4,0);
    HWND h1,h2,h3;
    h1 = levels.top();
    levels.pop();
    h2 = levels.top();
    levels.pop();
    h3 = levels.top();
    EnableWindow(h3,true);
    EnableWindow(h2,true);
    DestroyWindow(h1);
    DestroyWindow(h2);
    EnableWindow(h3,false);
    string output = "MY PROFILE\n\n"
                    + u->viewDetails();
    EnableWindow(levels.top(),true);
    hwnd7 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","About Me",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                           XSCREEN, /* x */
                           YSCREEN, /* y */
                           500, /* width */
                           600, /* height */
                           NULL,NULL,temp.hInstance,NULL);
    CreateWindow("STATIC",output.c_str(),
                 WS_VISIBLE|WS_CHILD,
                 0,5,500,200, /* X , Y, Width , height */
                 hwnd7,NULL,NULL,NULL);
    Administrator *temp_admin;
    Manager *temp_man;
    if(temp_admin = dynamic_cast <Administrator*> (u))
    {
        CreateWindow("BUTTON","Change Password",
                     WS_VISIBLE|WS_CHILD,
                     150,280,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)17,NULL,NULL);
        CreateWindow("BUTTON","Update Personal Info",
                     WS_VISIBLE|WS_CHILD,
                     150,320,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)18,NULL,NULL);
    }
    else if(temp_man = dynamic_cast <Manager*> (u))
    {
        CreateWindow("BUTTON","Change Password",
                     WS_VISIBLE|WS_CHILD,
                     150,280,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)27,NULL,NULL);
        CreateWindow("BUTTON","Update address",
                     WS_VISIBLE|WS_CHILD,
                     150,320,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)28,NULL,NULL);
        CreateWindow("BUTTON","Update Contact Number",
                     WS_VISIBLE|WS_CHILD,
                     150,360,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)29,NULL,NULL);
    }
    else
    {
        CreateWindow("BUTTON","Change Password",
                     WS_VISIBLE|WS_CHILD,
                     150,280,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)37,NULL,NULL);
        CreateWindow("BUTTON","Update address",
                     WS_VISIBLE|WS_CHILD,
                     150,320,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)38,NULL,NULL);
        CreateWindow("BUTTON","Update Contact Number",
                     WS_VISIBLE|WS_CHILD,
                     150,360,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)39,NULL,NULL);
    }
    levels.push(hwnd7);
    hwnd8 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Change Your Password",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                           XSCREEN, /* x */
                           YSCREEN, /* y */
                           500, /* width */
                           280, /* height */
                           NULL,NULL,temp.hInstance,NULL);

    string temp = "Old Password : "+ u->password;
    CreateWindow("STATIC",temp.c_str(),
                 WS_BORDER|WS_VISIBLE|WS_CHILD,
                 10,10,470,25, /* X , Y, Width , height */
                 hwnd8,NULL,NULL,NULL);
    CreateWindow("STATIC","New Password : ",
                 WS_BORDER |WS_VISIBLE|WS_CHILD,
                 10,40,120,25, /* X , Y, Width , height */
                 hwnd8,NULL,NULL,NULL);
    textbox4 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                            135,40,345,25,
                            hwnd8,NULL,NULL,NULL);
    CreateWindow("BUTTON","Update",
                 WS_VISIBLE|WS_CHILD,
                 200,160,100,20, /* X , Y, Width , height */
                 hwnd8,(HMENU)110,NULL,NULL);
    EnableWindow(hwnd7,false);
    levels.push(hwnd8);
}

void updatePersonalInfo(User *u,string field_name)
{
    char tempstr[1000];
    string body = "You did not provide "+field_name+".\n"+field_name+" remain same as before.";
    string title = "Could not update your "+field_name+"!";
    int s = GetWindowText(edit[0],&tempstr[0],1000);
    if(s<=0)
    {
        MessageBox(hwnd8,body.c_str(),title.c_str(),MB_OK);
        HWND h = levels.top();
        levels.pop();
        EnableWindow(levels.top(),true);
        DestroyWindow(h);
        return;
    }
    if(field_name == "Address") u->address = tempstr;
    else u->contactNum = tempstr;
    u->setInfo();
    title = field_name + " updated!";
    body = "Updated "+field_name+" : "+tempstr;
    MessageBox(hwnd8,body.c_str(),title.c_str(),MB_OK);

    Manager *temp_man;

    HWND h = levels.top();
    levels.pop();
    EnableWindow(levels.top(),true);
    DestroyWindow(h);
    h = levels.top();
    levels.pop();
    EnableWindow(levels.top(),true);
    DestroyWindow(h);

    string output = "MY PROFILE\n\n"
                    + u->viewDetails();

    hwnd7 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","About Me",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                           XSCREEN, /* x */
                           YSCREEN, /* y */
                           500, /* width */
                           600, /* height */
                           NULL,NULL,temp.hInstance,NULL);
    CreateWindow("STATIC",output.c_str(),
                 WS_VISIBLE|WS_CHILD,
                 0,5,500,200, /* X , Y, Width , height */
                 hwnd7,NULL,NULL,NULL);

    if(temp_man = dynamic_cast<Manager*>(u))
    {
        CreateWindow("BUTTON","Change Password",
                     WS_VISIBLE|WS_CHILD,
                     150,280,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)27,NULL,NULL);
        CreateWindow("BUTTON","Update address",
                     WS_VISIBLE|WS_CHILD,
                     150,320,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)28,NULL,NULL);
        CreateWindow("BUTTON","Update Contact Number",
                     WS_VISIBLE|WS_CHILD,
                     150,360,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)29,NULL,NULL);
    }
    else
    {
        CreateWindow("BUTTON","Change Password",
                     WS_VISIBLE|WS_CHILD,
                     150,280,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)37,NULL,NULL);
        CreateWindow("BUTTON","Update address",
                     WS_VISIBLE|WS_CHILD,
                     150,320,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)38,NULL,NULL);
        CreateWindow("BUTTON","Update Contact Number",
                     WS_VISIBLE|WS_CHILD,
                     150,360,200,25, /* X , Y, Width , height */
                     hwnd7,(HMENU)39,NULL,NULL);
    }
    EnableWindow(levels.top(),false);
    levels.push(hwnd7);
    EnableWindow(levels.top(),true);

}

int updatePersonalInfoWindow(User *u,string field_name)
{
    string s = ("Update "+field_name+" : ");
    hwnd8 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window",s.c_str(),WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                           XSCREEN, /* x */
                           YSCREEN, /* y */
                           500, /* width */
                           550, /* height */
                           NULL,NULL,temp.hInstance,NULL);
    CreateWindow("STATIC",s.c_str(),
                 WS_BORDER |WS_VISIBLE|WS_CHILD,
                 10,40,200,25, /* X , Y, Width , height */
                 hwnd8,NULL,NULL,NULL);
    string tempstr;
    if(field_name == "Address") tempstr = u->address;
    else tempstr = u->contactNum;
    edit[0] = CreateWindow("EDIT",tempstr.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                           10,80,470,25,
                           hwnd8,NULL,NULL,NULL);
    Manager *temp_man;
    if(temp_man = dynamic_cast<Manager*>(u))
    {
        CreateWindow("BUTTON","Update",
                     WS_VISIBLE|WS_CHILD,
                     200,160,100,20, /* X , Y, Width , height */
                     hwnd8,(HMENU)211,NULL,NULL);
    }
    else
    {
        CreateWindow("BUTTON","Update",
                     WS_VISIBLE|WS_CHILD,
                     200,160,100,20, /* X , Y, Width , height */
                     hwnd8,(HMENU)311,NULL,NULL);
    }
    levels.push(hwnd8);
    if(field_name == "Address") return 1;
    else return 2;

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    setStaticValues();
    bill = false;
    member = false;
    WNDCLASSEX base; /* a struct that holds property of window */
    MSG msg; /* a location for messages */
    HBRUSH hcolor = CreateSolidBrush(RGB(0,0, 126)); /* Color */
    memset(&base,0,sizeof(base));/* memset by 0 for the struct and set the options as required */
    base.cbSize = sizeof(WNDCLASSEX);
    base.lpfnWndProc = WinPro; /* To send messages */
    base.hInstance = hInstance;
    base.hCursor = LoadCursor(NULL, IDC_ARROW);
    base.lpszClassName = "Window";
    base.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    base.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
    base.hbrBackground = (HBRUSH) hcolor;

    if(!RegisterClassEx(&base))
    {
        MessageBox(NULL, "WINDOW REGISTRATION FAILED!","ERROR OCCURRED!",MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","POS SYSTEM",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                          XSCREEN, /* x */
                          YSCREEN, /* y */
                          600, /* width */
                          700, /* height */
                          NULL,NULL,temp.hInstance,NULL);
    levels.push(hwnd);

    if(hwnd == NULL)
    {
        MessageBox(NULL,"WINDOW CREATION FAILED!","ERROR OCCURRED!",MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }

    while(GetMessage(&msg,NULL,0,0)>0) /* No error */
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}


/* Windows function DispatchMessage() calls this */
LRESULT CALLBACK WinPro(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle different cases of messages */
    {
    case WM_CREATE:
        if(start==0)
        {
            CreateWindow("BUTTON","ADMINISTRATOR", /*Main screen administrator button*/
                         WS_VISIBLE|WS_CHILD,
                         190,80,200,80, /* X , Y, Width , height */
                         hwnd,(HMENU)1,NULL,NULL);

            CreateWindow("BUTTON","MANAGER", /*Main screen manager button*/
                         WS_VISIBLE|WS_CHILD,
                         190,200,200,80, /* X , Y, Width , height */
                         hwnd,(HMENU)2,NULL,NULL);

            CreateWindow("BUTTON","CASHIER", /*Main screen cashier button*/
                         WS_VISIBLE|WS_CHILD,
                         190,320,200,80, /* X , Y, Width , height */
                         hwnd,(HMENU)3,NULL,NULL);
            start++;
        }
        else
        {
            EnableWindow(levels.top(),false);
        }
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case 1:
            hwnd1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Administrator Log In",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN+50, /* x */
                                   YSCREEN+100, /* y */
                                   500, /* width */
                                   400, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            EnableWindow(hwnd,false);
            CreateWindow("STATIC","User ID :",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,20,80,25, /* X , Y, Width , height */
                         hwnd1,NULL,NULL,NULL);
            CreateWindow("STATIC","Password :",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,55,80,25, /* X , Y, Width , height */
                         hwnd1,NULL,NULL,NULL);
            textbox1 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    100,20,310,25,
                                    hwnd1,NULL,NULL,NULL);
            textbox2 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    100,55,310,25,
                                    hwnd1,NULL,NULL,NULL);
            CreateWindow("BUTTON","Log In",
                         WS_VISIBLE|WS_CHILD,
                         100,150,300,25, /* X , Y, Width , height */
                         hwnd1,(HMENU)11,NULL,NULL);
            levels.push(hwnd1);
            break;
        case 2:
            hwnd2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Manager Log In",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN+50, /* x */
                                   YSCREEN+100, /* y */
                                   500, /* width */
                                   400, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            EnableWindow(hwnd,false);
            CreateWindow("STATIC","User ID :",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,20,80,25, /* X , Y, Width , height */
                         hwnd2,NULL,NULL,NULL);
            CreateWindow("STATIC","Password :",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,55,80,25, /* X , Y, Width , height */
                         hwnd2,NULL,NULL,NULL);
            textbox1 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    100,20,310,25,
                                    hwnd2,NULL,NULL,NULL);
            textbox2 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    100,55,310,25,
                                    hwnd2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Log In",
                         WS_VISIBLE|WS_CHILD,
                         100,150,300,25, /* X , Y, Width , height */
                         hwnd2,(HMENU)21,NULL,NULL);
            levels.push(hwnd2);
            break;
        case 3:
            hwnd3 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Cashier Log In",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN+50, /* x */
                                   YSCREEN+100, /* y */
                                   500, /* width */
                                   400, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            EnableWindow(hwnd,false);
            CreateWindow("STATIC","User ID :",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,20,80,25, /* X , Y, Width , height */
                         hwnd3,NULL,NULL,NULL);
            CreateWindow("STATIC","Password :",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,55,80,25, /* X , Y, Width , height */
                         hwnd3,NULL,NULL,NULL);
            textbox1 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    100,20,310,25,
                                    hwnd3,NULL,NULL,NULL);
            textbox2 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    100,55,310,25,
                                    hwnd3,NULL,NULL,NULL);
            CreateWindow("BUTTON","Log In",
                         WS_VISIBLE|WS_CHILD,
                         100,150,300,25, /* X , Y, Width , height */
                         hwnd3,(HMENU)31,NULL,NULL);
            levels.push(hwnd3);
            break;
        case 11:
        {
            bool success = authorization(1,hwnd1);
            if(success)
            {
                hwnd4 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Administrator",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                       XSCREEN, /* x */
                                       YSCREEN, /* y */
                                       600, /* width */
                                       700, /* height */
                                       NULL,NULL,temp.hInstance,NULL);
                string id = "Administrator Logged In\nUser ID : "+current_user;
                CreateWindow("STATIC",id.c_str(),
                             WS_VISIBLE|WS_CHILD,
                             0,5,600,35, /* X , Y, Width , height */
                             hwnd4,NULL,NULL,NULL);
                CreateWindow("BUTTON","Branch Info",
                             WS_VISIBLE|WS_CHILD,
                             190,60,200,80, /* X , Y, Width , height */
                             hwnd4,(HMENU)12,NULL,NULL);

                CreateWindow("BUTTON","Employee Info",
                             WS_VISIBLE|WS_CHILD,
                             190,160,200,80, /* X , Y, Width , height */
                             hwnd4,(HMENU)13,NULL,NULL);

                CreateWindow("BUTTON","Product Info",
                             WS_VISIBLE|WS_CHILD,
                             190,260,200,80, /* X , Y, Width , height */
                             hwnd4,(HMENU)14,NULL,NULL);
                CreateWindow("BUTTON","About Me",
                             WS_VISIBLE|WS_CHILD,
                             190,460,200,80, /* X , Y, Width , height */
                             hwnd4,(HMENU)15,NULL,NULL);
                CreateWindow("BUTTON","Log Out",
                             WS_VISIBLE|WS_CHILD,
                             190,560,200,80, /* X , Y, Width , height */
                             hwnd4,(HMENU)16,NULL,NULL);
                CreateWindow("BUTTON","Customer Membership Info",
                             WS_VISIBLE|WS_CHILD,
                             190,360,200,80, /* X , Y, Width , height */
                             hwnd4,(HMENU)33,NULL,NULL);
                levels.push(hwnd4);
                if(a) delete a;
                a = new Administrator("administrator/"+current_user+".txt");
                a->getInfo();
            }

            break;
        }
        case 14:
        {
            hwndP3 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Product Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    400, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("BUTTON","View All Products",
                         WS_VISIBLE|WS_CHILD,
                         200,100,200,25, /* X , Y, Width , height */
                         hwndP3,(HMENU)71,NULL,NULL);
            CreateWindow("BUTTON","Add A New Product",
                         WS_VISIBLE|WS_CHILD,
                         200,150,200,25, /* X , Y, Width , height */
                         hwndP3,(HMENU)72,NULL,NULL);
            levels.push(hwndP3);
            break;
        }
        case 72:
        {
            hwndP = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Add product",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   700, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",("Product ID : P"+to_string(Product::products+1)).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         0,1,600,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[0] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,80,MaxTextBox,25,
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("STATIC","Category : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,120,120,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,160,MaxTextBox,25,
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("STATIC","Unit cost price : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,200,120,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[2] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,240,MaxTextBox,25,
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("STATIC","Unit : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,280,120,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,320,MaxTextBox,25,
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("STATIC","Required quanity(unit) in any branch : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,360,250,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,400,MaxTextBox,25,
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("STATIC","Available quanity(unit) in a branch : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,440,250,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,480,MaxTextBox,25,
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("BUTTON","Add",
                         WS_VISIBLE|WS_CHILD,
                         248,620,100,20, /* X , Y, Width , height */
                         hwndP,(HMENU)73,NULL,NULL);
            levels.push(hwndP);
            break;
        }
        case 73:
        {
            char temps1[10][1000];
            int check = 1;
            for(int i=0; i<=5; i++)
            {
                int k = GetWindowText(edit[i],&temps1[i][0],1000);
                if(k<=0)
                {
                    MessageBox(hwndP,"You did not provide information for all the fields.",
                               "Could not update product info!",MB_OK);
                    HWND h = levels.top();
                    levels.pop();
                    EnableWindow(levels.top(),true);
                    DestroyWindow(h);
                    check = 0;
                    break;
                }
            }
            if(check==0) break;
            int k = MessageBox(hwndP,"Are you sure you want to add?",
                               "Add a product",MB_YESNO);
            if(k==7) break;
            string s;

            prodct = new Product("product/P"+to_string(Product::products+1)+".txt");
            prodct->productID = "P"+to_string(Product::products+1);
            prodct->name = temps1[0];
            prodct->category = temps1[1];
            s = temps1[2];
            prodct->unitCostPrice = stod(s);
            prodct->unit = temps1[3];

            s = temps1[4];
            double req = stod(s);
            s = temps1[5];
            double available = stod(s);

            a->addProduct(prodct,req,available);
            Product::products++;
            saveStaticValues();
            MessageBox(hwndP,"You added a product.",
                       "Product added!",MB_OK);
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);

            break;
        }
        case 71:
        {
            a->viewAllProducts();
            hwndP = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","All Products",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   650, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",("--- All Products ---\r\n\r\n\r\n"+output).c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,570,465,
                         hwndP,NULL,NULL,NULL);
            CreateWindow("STATIC","Search a product by Product ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,525,218,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",
                                   WS_BORDER |WS_VISIBLE|WS_CHILD,
                                   10,555,470,25, /* X , Y, Width , height */
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("BUTTON","Search",
                         WS_VISIBLE|WS_CHILD,
                         490,555,90,25, /* X , Y, Width , height */
                         hwndP,(HMENU)55,NULL,NULL);
            levels.push(hwndP);
            break;
        }
        case 55:
        {
            char temps[1000];
            GetWindowText(edit[1],&temps[0],1000);
            string prd = temps;
            file = "product/"+prd+".txt";
            string res;
            ifstream f(file);
            if(!f)
            {
                MessageBox(hwndP,"Search for a valid Product ID.",
                           "Info not found!",MB_OK);
                break;
            }
            if(prodct) delete prodct;
            prodct = new Product(file);
            prodct->getProductInfo();
            res = prodct->viewProductDetails();
            res+=prodct->viewProductDetailsInBranches();
            hwndP1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Product Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    650, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",res.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,570,300,
                         hwndP1,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update Product Price",
                         WS_VISIBLE|WS_CHILD,
                         210,420,200,25, /* X , Y, Width , height */
                         hwndP1,(HMENU)56,NULL,NULL);
            CreateWindow("BUTTON","Delete Product",
                         WS_VISIBLE|WS_CHILD,
                         210,470,200,25, /* X , Y, Width , height */
                         hwndP1,(HMENU)57,NULL,NULL);
            levels.push(hwndP1);
            break;
        }
        case 57:
        {
            int k3 = MessageBox(hwndP2,"Are you sure you want to delete?",
                                "Delete product",MB_YESNO);
            if(k3 == 7) break;
            MessageBox(hwndP1,("A product(Product ID : "+prodct->productID+") is deleted.").c_str(),
                       "Product deleted!",MB_OK);

            a->deleteProduct(prodct->productID,prodct->file_path);

            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            break;
        }
        case 56:
        {
            hwndP2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Change branch address",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    400, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",("Product ID : "+prodct->productID).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,25,570,25, /* X , Y, Width , height */
                         hwndP2,NULL,NULL,NULL);
            CreateWindow("STATIC","Unit cost price : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,65,140,25, /* X , Y, Width , height */
                         hwndP2,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT",to_string(prodct->unitCostPrice).c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,105,MaxTextBox,25,
                                   hwndP2,NULL,NULL,NULL);
            CreateWindow("STATIC","Unit : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,145,140,25, /* X , Y, Width , height */
                         hwndP2,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT",prodct->unit.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,185,MaxTextBox,25,
                                   hwndP2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         260,250,80,20, /* X , Y, Width , height */
                         hwndP2,(HMENU)58,NULL,NULL);
            levels.push(hwndP2);
            break;
        }
        case 58:
        {
            char temps1[1000];
            int k1 = GetWindowText(edit[4],&temps1[0],1000);
            char temps2[1000];
            int k2 = GetWindowText(edit[5],&temps2[0],1000);
            if(k1<=0||k2<=0)
            {
                MessageBox(hwndP2,"You did not provide information for all the fields.",
                           "Could not update product info!",MB_OK);
                HWND h = levels.top();
                levels.pop();
                EnableWindow(levels.top(),true);
                DestroyWindow(h);
                break;
            }
            int k3 = MessageBox(hwndP2,"Are you sure you want to update?",
                                "Product info update",MB_YESNO);
            if(k3==7) break;
            prodct->unitCostPrice = stod(temps1);
            prodct->unit = temps2;
            prodct->setProductInfo();
            MessageBox(hwndP2,"You updated product info.",
                       "Product info updated!",MB_OK);
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            break;

        }
        case 12:
        {
            a->viewAllBranches();
            hwndB = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","All Branches",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   650, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",("--- All Branches ---\r\n\r\n\r\n"+output).c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,570,465,
                         hwndB,NULL,NULL,NULL);
            CreateWindow("STATIC","Search a branch by Branch ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,525,218,25, /* X , Y, Width , height */
                         hwndB,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",
                                   WS_BORDER |WS_VISIBLE|WS_CHILD,
                                   10,555,470,25, /* X , Y, Width , height */
                                   hwndB,NULL,NULL,NULL);
            CreateWindow("BUTTON","Search",
                         WS_VISIBLE|WS_CHILD,
                         490,555,90,25, /* X , Y, Width , height */
                         hwndB,(HMENU)51,NULL,NULL);
            levels.push(hwndB);
            break;

        }
        case 51:
        {
            char temps[1000];
            GetWindowText(edit[1],&temps[0],1000);
            branch = temps;
            file = "branch/"+branch+"/"+branch+".txt";
            output="";
            if(a->searchBranch(file))
            {
                hwndB1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Branch Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                        XSCREEN, /* x */
                                        YSCREEN, /* y */
                                        600, /* width */
                                        650, /* height */
                                        NULL,NULL,temp.hInstance,NULL);
                CreateWindow("EDIT",output.c_str(),
                             WS_VISIBLE|WS_CHILD|ES_MULTILINE
                             |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                             10,10,570,490,
                             hwndB1,NULL,NULL,NULL);
                CreateWindow("BUTTON","Update branch address",
                             WS_VISIBLE|WS_CHILD,
                             210,550,200,25, /* X , Y, Width , height */
                             hwndB1,(HMENU)52,NULL,NULL);
                levels.push(hwndB1);
                break;
            }
            else
            {
                MessageBox(hwndB,"Search for a valid Branch ID.",
                           "Info not found!",MB_OK);
                break;
            }
        }
        case 52:
        {
            hwndB2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Change branch address",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN+100, /* y */
                                    600, /* width */
                                    300, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",("Branch ID : "+branch).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,25,570,25, /* X , Y, Width , height */
                         hwndB2,NULL,NULL,NULL);
            CreateWindow("STATIC","Branch Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,55,140,25, /* X , Y, Width , height */
                         hwndB2,NULL,NULL,NULL);

            if(b) delete b;
            b = new Branch(file);
            b->getBranchInfo();

            edit[2] = CreateWindow("EDIT",b->address.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,85,MaxTextBox,25,
                                   hwndB2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         260,150,80,20, /* X , Y, Width , height */
                         hwndB2,(HMENU)53,NULL,NULL);
            levels.push(hwndB2);
            break;
        }
        case 53:
        {
            char text[1005];
            int k = GetWindowText(edit[2],&text[0],1000);
            if(k<=0)
            {
                MessageBox(hwndB2,"You did not provide information.",
                           "Could not update address!",MB_OK);
                break;
            }

            b->address = text;
            b->setBranchInfo();

            MessageBox(hwndB2,"You updated branch address.",
                       "Address updated!",MB_OK);
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);

            break;
        }
        case 13:
        {
            hwndE = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Employee Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   700, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC","Search an employee by Employee ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,300,25, /* X , Y, Width , height */
                         hwndE,NULL,NULL,NULL);
            edit[7] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,80,450,25,
                                   hwndE,NULL,NULL,NULL);
            CreateWindow("BUTTON","Search",
                         WS_VISIBLE|WS_CHILD,
                         490,80,80,25, /* X , Y, Width , height */
                         hwndE,(HMENU)1001,NULL,NULL);
            CreateWindow("STATIC","View all employees by Branch ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,120,300,25, /* X , Y, Width , height */
                         hwndE,NULL,NULL,NULL);
            edit[8] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,160,450,25,
                                   hwndE,NULL,NULL,NULL);
            CreateWindow("BUTTON","View",
                         WS_VISIBLE|WS_CHILD,
                         490,160,80,25, /* X , Y, Width , height */
                         hwndE,(HMENU)1002,NULL,NULL);
            levels.push(hwndE);
            break;
        }
        case 1001:
        {
            char text[105];
            int k = GetWindowText(edit[7],&text[0],100);
            return_val = 0;
            if(k<=0)
            {
                MessageBox(hwndE,"Search for a valid Employee ID.",
                           "Info not found!",MB_OK);
                break;
            }
            string s,t = text;
            branch = "B";
            int i = 1;
            while(1)
            {
                if((t[i]>='0' && t[i]<='9') && i<t.size())
                {
                    branch+=t[i];
                    i++;
                }
                else break;
            }
            if(readFile) readFile.close();
            readFile.open("branch/"+branch+"/"+branch+".txt");
            if(!readFile)
            {
                MessageBox(hwndE,"Search for a valid Employee ID.",
                           "Info not found!",MB_OK);
                break;
            }
            readFile.close();
            file = "branch/"+branch+"/employees/"+t+".txt";
            readFile.open(file);
            if(!readFile)
            {
                file = "branch/"+branch+"/employees/manager/"+t+".txt";
                readFile.open(file);
                if(!readFile)
                {
                    file = "branch/"+branch+"/employees/cashiers/"+t+".txt";
                    readFile.open(file);
                    if(!readFile)
                    {
                        MessageBox(hwndE,"Search for a valid Employee ID.",
                                   "Info not found!",MB_OK);
                        break;
                    }
                    else
                    {
                        output="Cashier\r\n";
                        return_val = 2;
                    }
                }
                else
                {
                    output="Manager\r\n";
                    return_val = 1;
                }
            }
            else
            {
                output="";
                return_val = 3;
            }
            output+=a->viewEmployeeInfoByEmpFile(file);

            hwndE1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Employee Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",output.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,20,570,400,
                         hwndE1,NULL,NULL,NULL);
            s = "Update information of the employee "+ t ;
            CreateWindow("BUTTON",s.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         10,440,400,25, /* X , Y, Width , height */
                         hwndE1,(HMENU)1005,NULL,NULL);
            s = "Delete the employee "+ t ;
            CreateWindow("BUTTON",s.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         10,475,400,25, /* X , Y, Width , height */
                         hwndE1,(HMENU)1006,NULL,NULL);
            if(return_val!=3)
            {
                s = "Change password of the employee "+ t ;
                CreateWindow("BUTTON",s.c_str(),
                             WS_VISIBLE|WS_CHILD,
                             10,510,400,25, /* X , Y, Width , height */
                             hwndE1,(HMENU)1008,NULL,NULL);
            }
            levels.push(hwndE1);
            break;
        }
        case 1006:
        {
            char temps[1000];
            GetWindowText(edit[7],&temps[0],1000);
            string s=temps;
            s = "This action will delete the Employee "+s+".\nAre you sure you want to continue?";
            int k = MessageBox(hwndE1,s.c_str(),
                               "Delete the employee!",MB_YESNO);
            if(k==7) break;

            s = temps;

            if(e2) delete e2;
            if(return_val == 1) e2 = new Manager(file);
            if(return_val == 2) e2 = new Cashier(file);
            if(return_val == 3) e2 = new Employee(file);

            e2->getInfo();

            if(readFile) readFile.close();
            if(writeFile) writeFile.close();

            int res = remove(file.c_str());

            e2->file_path = "DeletedEmployeesHistory/D"+s+".txt";
            e2->setInfo();

            Branch br("branch/"+branch+"/"+branch+".txt");
            br.getBranchInfo();
            if(return_val == 1) br.deleteManager(s);
            if(return_val == 2) br.deleteCashier(s);
            if(return_val == 3) br.deleteOtherEmployee(s);

            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            s = "You deleted employee "+e2->EmpID+".";
            MessageBox(levels.top(),s.c_str(),
                       "Employee deleted!",MB_OK);
            break;
        }
        case 1005:
        {
            if(e2) delete e2;
            if(return_val == 1) e2 = new Manager(file);
            if(return_val == 2) e2 = new Cashier(file);
            if(return_val == 3) e2 = new Employee(file);

            e2->getInfo();

            string s = "Update Employee Info";
            hwndE2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window",s.c_str(),WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            s = ("Branch : "+branch);
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,5,MaxTextBox,20, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Employee ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT",e2->EmpID.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                   10,80,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,120,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[2] = CreateWindow("EDIT",e2->name.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,160,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,200,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT",e2->gender.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,240,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,280,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT",e2->contactNum.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,320,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,360,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT",e2->address.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,400,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Salary : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,440,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[6] = CreateWindow("EDIT",e2->salary.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,480,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            if(return_val!=3)
            {
                CreateWindow("STATIC","User ID : ",
                             WS_BORDER |WS_VISIBLE|WS_CHILD,
                             10,520,120,25, /* X , Y, Width , height */
                             hwndE2,NULL,NULL,NULL);
                CreateWindow("EDIT",e2->EmpID.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                             10,560,MaxTextBox,25,
                             hwndE2,NULL,NULL,NULL);
            }
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         248,600,100,20, /* X , Y, Width , height */
                         hwndE2,(HMENU)1010,NULL,NULL);
            e = e2;
            levels.push(hwndE2);
            break;
        }
        case 1008:
        {
            char temps[1000];
            GetWindowText(edit[7],&temps[0],1000);
            string s=temps;
            s = "This action will change the password of Employee "+s+" to default password : "+s+".\nDo you want to continue?";
            int k = MessageBox(hwndE1,s.c_str(),
                               "Change password to default password",MB_YESNO);
            if(k==7) break;

            s = temps;
            if(e2) delete e2;
            if(return_val == 1) e2 = new Manager(file);
            if(return_val == 2) e2 = new Cashier(file);

            e2->getInfo();

            e2->setPassword(s);

            e2->setInfo();

            MessageBox(hwndE1,("Password Updated.\nNew Password : "+e2->EmpID).c_str(),
                       "Password Updated!",MB_OK);
            break;

        }
        case 1010:
        {
            char tempstr[8][1000];
            int i,chk = 1;

            for(i=1; i<=6; i++)
            {
                int s = GetWindowText(edit[i],&tempstr[i][0],1000);
                if(s<=0)
                {
                    MessageBox(hwndE2,"You did not provide information for all the fields.\nEmployee's info remain same as before.",
                               "Could not update employee's info!",MB_OK);
                    HWND h = levels.top();
                    levels.pop();
                    EnableWindow(levels.top(),true);
                    DestroyWindow(h);
                    chk = 0;
                    break;
                }
            }
            if(chk == 0) break;
            int k = MessageBox(hwndE2,"Are you sure you want to update?","Employee Info",MB_YESNO);
            if(k==7)
            {
                HWND h = levels.top();
                levels.pop();
                EnableWindow(levels.top(),true);
                DestroyWindow(h);
                break;
            }
            MessageBox(hwndE2,"You updated the employee info.","Employee Info Updated!",MB_OK);
            e->EmpID = tempstr[1];
            e->name = tempstr[2];
            e->gender = tempstr[3];
            e->contactNum = tempstr[4];
            e->address = tempstr[5];
            e->salary = tempstr[6];
            if((m=dynamic_cast<Manager*>(e))||(c=dynamic_cast<Cashier*>(c)))
            {
                ifstream handle;
                string s;
                handle.open(file);
                getline(handle,s);
                e->setUserID(s);
                getline(handle,s);
                e->setPassword(s);
            }
            e->setInfo();
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            break;
        }
        case 1007:
        {
            char tempstr[7][1000];
            int i,chk = 1;

            for(i=1; i<=5; i++)
            {
                int s = GetWindowText(edit[i],&tempstr[i][0],1000);
                if(s<=0)
                {
                    chk = 0;
                    break;
                }
            }
            int s = GetWindowText(edit[12],&tempstr[6][0],1000);
            if(s<=0)
            {
                chk = 0;
            }
            if(chk == 0)
            {
                MessageBox(hwndE2,"You did not provide information for all the fields.\nEmployee's info remain same as before.",
                           "Could not update employee's info!",MB_OK);
                HWND h = levels.top();
                levels.pop();
                EnableWindow(levels.top(),true);
                DestroyWindow(h);
                chk = 0;
                break;
            }
            int k = MessageBox(hwndE2,"Are you sure you want to update?","Employee Info",MB_YESNO);
            if(k==7)
            {
                HWND h = levels.top();
                levels.pop();
                EnableWindow(levels.top(),true);
                DestroyWindow(h);
                break;
            }
            MessageBox(hwndE2,"You updated the employee info.","Employee Info Updated!",MB_OK);
            e->EmpID = tempstr[1];
            e->name = tempstr[2];
            e->gender = tempstr[3];
            e->contactNum = tempstr[4];
            e->address = tempstr[5];
            e->salary = tempstr[6];
            Manager *m1;
            Cashier *c1;
            if((m1=dynamic_cast<Manager*>(e))||(c1=dynamic_cast<Cashier*>(e)))
            {
                ifstream handle;
                string s;
                handle.open(file);
                getline(handle,s);
                e->setUserID(s);
                getline(handle,s);
                e->setPassword(s);
            }

            e->setInfo();

            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);

            break;
        }
        case 1002:
        {
            char text[100];
            GetWindowText(edit[8],&text[0],100);
            if(a->viewEmployeesByBranchID(text))
            {
                hwndE1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Employee Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                        XSCREEN, /* x */
                                        YSCREEN, /* y */
                                        600, /* width */
                                        700, /* height */
                                        NULL,NULL,temp.hInstance,NULL);
                branch = text;
                string s = "Employees of Branch : "+ branch;
                CreateWindow("STATIC",s.c_str(),
                             WS_BORDER |WS_VISIBLE|WS_CHILD,
                             10,10,570,25, /* X , Y, Width , height */
                             hwndE1,NULL,NULL,NULL);

                CreateWindow("EDIT",output.c_str(),
                             WS_VISIBLE|WS_CHILD|ES_MULTILINE
                             |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                             10,37,570,500,
                             hwndE1,NULL,NULL,NULL);
                s = "Update information of an employee of Branch "+ branch +" : ";
                CreateWindow("STATIC",s.c_str(),
                             WS_VISIBLE|WS_CHILD,
                             10,570,570,25, /* X , Y, Width , height */
                             hwndE1,NULL,NULL,NULL);
                CreateWindow("STATIC","Employee ID : ",
                             WS_VISIBLE|WS_CHILD,
                             10,590,100,25, /* X , Y, Width , height */
                             hwndE1,NULL,NULL,NULL);

                edit[9] = CreateWindow("EDIT","",
                                       WS_VISIBLE|WS_CHILD,
                                       110,590,470,25,
                                       hwndE1,NULL,NULL,NULL);
                s = "Update Info of the employee";
                CreateWindow("BUTTON",s.c_str(),
                             WS_VISIBLE|WS_CHILD,
                             110,620,200,23, /* X , Y, Width , height */
                             hwndE1,(HMENU)1003,NULL,NULL);
                s = "Add a new employee in Branch "+ branch;
                CreateWindow("BUTTON",s.c_str(),
                             WS_VISIBLE|WS_CHILD,
                             315,620,265,23, /* X , Y, Width , height */
                             hwndE1,(HMENU)1004,NULL,NULL);

                levels.push(hwndE1);
            }
            else
            {
                MessageBox(hwndE,"Search for a valid Branch ID.","Info not found!",MB_OK);
            }
            break;
        }
        case 1004:
        {
            string s = "Add Employee";
            hwndE2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window",s.c_str(),WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN+190, /* x */
                                    YSCREEN+100, /* y */
                                    300, /* width */
                                    400, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("BUTTON","Manager",
                         WS_VISIBLE|WS_CHILD,
                         95,50,100,25, /* X , Y, Width , height */
                         hwndE2,(HMENU)10041,NULL,NULL);
            CreateWindow("BUTTON","Cashier",
                         WS_VISIBLE|WS_CHILD,
                         95,90,100,25, /* X , Y, Width , height */
                         hwndE2,(HMENU)10042,NULL,NULL);
            CreateWindow("BUTTON","Others",
                         WS_VISIBLE|WS_CHILD,
                         95,130,100,25, /* X , Y, Width , height */
                         hwndE2,(HMENU)10043,NULL,NULL);
            levels.push(hwndE2);
            break;
        }
        case 10043:
        {
            string s = branch+"EMP"+to_string(Employee::employees+1);
            hwndE3 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Add employee",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",("Branch ID : "+branch).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         0,1,600,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Employee ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[0] = CreateWindow("EDIT",s.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                   10,80,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,120,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,160,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,200,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[2] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,240,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,280,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,320,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,360,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,400,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Salary : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,440,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,480,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("BUTTON","Add",
                         WS_VISIBLE|WS_CHILD,
                         248,620,100,20, /* X , Y, Width , height */
                         hwndE3,(HMENU)10046,NULL,NULL);
            levels.push(hwndE3);
            break;

        }
        case 10046:
        {
            string tempstr[10];
            int i,chk=1;
            for(i=0; i<6; i++)
            {
                char t[1000];
                int k = GetWindowText(edit[i],&t[0],1000);
                if(k<=0)
                {
                    MessageBox(hwndE3,"You did not provide information for all the fields.\nNew employee is not added.",
                               "Could not add employee!",MB_OK);
                    HWND h = levels.top();
                    levels.pop();
                    EnableWindow(levels.top(),true);
                    DestroyWindow(h);
                    chk = 0;
                    break;
                }
                tempstr[i] = t;
            }
            if(chk == 0) break;
            if(e) delete e;
            file = "branch/"+branch+"/employees/"+branch+"EMP"+to_string(Employee::employees+1)+".txt";
            e = new Employee(file);
            e->setInfo(
                tempstr[1],
                tempstr[2],
                tempstr[3],
                tempstr[4],
                tempstr[0],
                branch,
                tempstr[5]
            );
            MessageBox(hwndE3,("New employee "+branch+"EMP"+to_string(Employee::employees+1)+" is added.\n").c_str(),
                       "New employee added!",MB_OK);
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);

            Branch br("branch/" + branch + "/" + branch + ".txt");
            br.getBranchInfo();
            br.addOtherEmployee(branch+"EMP"+to_string(Employee::employees+1));

            Employee::employees++;
            saveStaticValues();

            break;

        }
        case 10041:
        {
            string s = branch+"EMP"+to_string(Employee::employees+1)+"M"+to_string(Manager::managers+1);
            hwndE3 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Add manager",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",("Branch ID : "+branch).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         0,1,600,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Employee ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[0] = CreateWindow("EDIT",s.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                   10,80,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,120,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,160,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,200,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[2] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,240,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,280,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,320,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,360,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,400,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Salary : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,440,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,480,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC",("User ID: "+s+"\nPassword : "+s).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,550,400,50, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            CreateWindow("BUTTON","Add",
                         WS_VISIBLE|WS_CHILD,
                         248,620,100,20, /* X , Y, Width , height */
                         hwndE3,(HMENU)10044,NULL,NULL);
            levels.push(hwndE3);
            break;
        }
        case 10044:
        {
            string tempstr[10];
            int i,chk=1;
            for(i=0; i<6; i++)
            {
                char t[1000];
                int k = GetWindowText(edit[i],&t[0],1000);
                if(k<=0)
                {
                    MessageBox(hwndE3,"You did not provide information for all the fields.\nNew employee is not added.",
                               "Could not add employee!",MB_OK);
                    HWND h = levels.top();
                    levels.pop();
                    EnableWindow(levels.top(),true);
                    DestroyWindow(h);
                    chk = 0;
                    break;
                }
                tempstr[i] = t;
            }
            if(chk == 0) break;
            if(m) delete m;
            file = "branch/"+branch+"/employees/manager/"+branch+"EMP"+to_string(Employee::employees+1)+"M"+to_string(Manager::managers+1)+".txt";
            m = new Manager(file);
            m->setInfo(branch+"EMP"+to_string(Employee::employees+1)+"M"+to_string(Manager::managers+1),
                       branch+"EMP"+to_string(Employee::employees+1)+"M"+to_string(Manager::managers+1),
                       tempstr[1],
                       tempstr[2],
                       tempstr[3],
                       tempstr[4],
                       tempstr[0],
                       branch,
                       tempstr[5]
                      );
            MessageBox(hwndE3,("New employee "+branch+"EMP"+to_string(Employee::employees+1)+"M"+to_string(Manager::managers+1)+" is added.\n"
                               "User ID : "+branch+"EMP"+to_string(Employee::employees+1)+"M"+to_string(Manager::managers+1)+
                               +"\nPassword : "+branch+"EMP"+to_string(Employee::employees+1)+"M"+to_string(Manager::managers+1)).c_str(),
                       "New employee added!",MB_OK);
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);

            Branch br("branch/" + branch + "/" + branch + ".txt");
            br.getBranchInfo();
            br.addManager(branch+"EMP"+to_string(Employee::employees+1)+"M"+to_string(Manager::managers+1));

            Manager::managers++;
            Employee::employees++;
            saveStaticValues();

            break;
        }
        case 10042:
        {
            string s = branch+"EMP"+to_string(Employee::employees+1)+"C"+to_string(Cashier::cashiers+1);
            hwndE3 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Add cashier",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",("Branch ID : "+branch).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         0,1,600,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Employee ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[0] = CreateWindow("EDIT",s.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                   10,80,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,120,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,160,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,200,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[2] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,240,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,280,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,320,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,360,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,400,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC","Salary : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,440,120,25, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,480,MaxTextBox,25,
                                   hwndE3,NULL,NULL,NULL);
            CreateWindow("STATIC",("User ID: "+s+"\nPassword : "+s).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,550,400,50, /* X , Y, Width , height */
                         hwndE3,NULL,NULL,NULL);
            CreateWindow("BUTTON","Add",
                         WS_VISIBLE|WS_CHILD,
                         248,620,100,20, /* X , Y, Width , height */
                         hwndE3,(HMENU)10045,NULL,NULL);
            levels.push(hwndE3);
            break;
        }
        case 10045:
        {
            string tempstr[10];
            int i,chk=1;
            for(i=0; i<6; i++)
            {
                char t[1000];
                int k = GetWindowText(edit[i],&t[0],1000);
                if(k<=0)
                {
                    MessageBox(hwndE3,"You did not provide information for all the fields.\nNew employee is not added.",
                               "Could not add employee!",MB_OK);
                    HWND h = levels.top();
                    levels.pop();
                    EnableWindow(levels.top(),true);
                    DestroyWindow(h);
                    chk = 0;
                    break;
                }
                tempstr[i] = t;
            }
            if(chk == 0) break;
            if(c) delete c;
            file = "branch/"+branch+"/employees/cashiers/"+branch+"EMP"+to_string(Employee::employees+1)+"C"+to_string(Cashier::cashiers+1)+".txt";
            c = new Cashier(file);
            c->setInfo(branch+"EMP"+to_string(Employee::employees+1)+"C"+to_string(Cashier::cashiers+1),
                       branch+"EMP"+to_string(Employee::employees+1)+"C"+to_string(Cashier::cashiers+1),
                       tempstr[1],
                       tempstr[2],
                       tempstr[3],
                       tempstr[4],
                       tempstr[0],
                       branch,
                       tempstr[5]
                      );
            MessageBox(hwndE3,("New employee "+branch+"EMP"+to_string(Employee::employees+1)+"C"+to_string(Cashier::cashiers+1)+" is added.\n"
                               "User ID : "+branch+"EMP"+to_string(Employee::employees+1)+"C"+to_string(Cashier::cashiers+1)+
                               +"\nPassword : "+branch+"EMP"+to_string(Employee::employees+1)+"C"+to_string(Cashier::cashiers+1)).c_str(),
                       "New employee added!",MB_OK);
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);

            Branch br("branch/" + branch + "/" + branch + ".txt");
            br.getBranchInfo();
            br.addCashier(branch+"EMP"+to_string(Employee::employees+1)+"C"+to_string(Cashier::cashiers+1));

            Cashier::cashiers++;
            Employee::employees++;
            saveStaticValues();
            break;
        }
        case 1003:
        {
            char text[50];
            ifstream handle;
            int u = 0;
            GetWindowText(edit[9],&text[0],50);
            string temps = text;
            string s = "Update Info of the ";

            file = "branch/"+branch+"/employees/"+temps+".txt";
            handle.open(file);
            if(handle)
            {
                e = new Employee(file);
                s+=("Employee "+temps);
                u = 1;
            }
            else
            {
                file = "branch/"+branch+"/employees/manager/"+temps+".txt";
                m = new Manager(file);
                handle.open(file);
                if(handle)
                {
                    e = new Manager(file);
                    s+=("Manager "+temps);
                    u = 2;
                }
                else
                {
                    file = "branch/"+branch+"/employees/cashiers/"+temps+".txt";
                    c = new Cashier(file);
                    handle.open(file);
                    if(handle)
                    {
                        e = new Cashier(file);
                        s+=("Cashier "+temps);
                        u = 3;
                    }
                    else
                    {
                        MessageBox(hwndE1,"Enter a valid Employee ID.","Employee not found!",MB_OK);
                        handle.close();
                        break;
                    }
                }
            }
            handle.close();

            e->getInfo();

            hwndE2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window",s.c_str(),WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC","Employee ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT",e->EmpID.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                   10,80,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,120,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[2] = CreateWindow("EDIT",e->name.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,160,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,200,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT",e->gender.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,240,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,280,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT",e->contactNum.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,320,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,360,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT",e->address.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,400,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","User ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,440,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            if(u!=1)
            {
                edit[6] = CreateWindow("EDIT",e->EmpID.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                       10,480,MaxTextBox,25,
                                       hwndE2,NULL,NULL,NULL);
            }
            else
            {
                edit[6] = CreateWindow("EDIT","N/A",WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                       10,480,MaxTextBox,25,
                                       hwndE2,NULL,NULL,NULL);
            }
            CreateWindow("STATIC","Salary : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,520,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[12] = CreateWindow("EDIT",e->salary.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    10,560,MaxTextBox,25,
                                    hwndE2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         248,620,100,20, /* X , Y, Width , height */
                         hwndE2,(HMENU)1007,NULL,NULL);
            levels.push(hwndE2);
            break;

        }
        case 21:
        {
            bool success = authorization(2,hwnd2);
            if(success)
            {
                hwnd5 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Manager",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                       XSCREEN, /* x */
                                       YSCREEN, /* y */
                                       600, /* width */
                                       700, /* height */
                                       NULL,NULL,temp.hInstance,NULL);
                string id = "Manager Logged In\nUser ID : "+current_user;
                CreateWindow("STATIC",id.c_str(),
                             WS_VISIBLE|WS_CHILD,
                             0,5,600,35, /* X , Y, Width , height */
                             hwnd5,NULL,NULL,NULL);
                CreateWindow("BUTTON","Branch Info",
                             WS_VISIBLE|WS_CHILD,
                             190,60,200,80, /* X , Y, Width , height */
                             hwnd5,(HMENU)22,NULL,NULL);

                CreateWindow("BUTTON","Branch Employee Info",
                             WS_VISIBLE|WS_CHILD,
                             190,160,200,80, /* X , Y, Width , height */
                             hwnd5,(HMENU)23,NULL,NULL);

                CreateWindow("BUTTON","Branch Product Info",
                             WS_VISIBLE|WS_CHILD,
                             190,260,200,80, /* X , Y, Width , height */
                             hwnd5,(HMENU)24,NULL,NULL);
                CreateWindow("BUTTON","About Me",
                             WS_VISIBLE|WS_CHILD,
                             190,460,200,80, /* X , Y, Width , height */
                             hwnd5,(HMENU)25,NULL,NULL);
                CreateWindow("BUTTON","Log Out",
                             WS_VISIBLE|WS_CHILD,
                             190,560,200,80, /* X , Y, Width , height */
                             hwnd5,(HMENU)26,NULL,NULL);
                CreateWindow("BUTTON","Customer Membership Info",
                             WS_VISIBLE|WS_CHILD,
                             190,360,200,80, /* X , Y, Width , height */
                             hwnd5,(HMENU)33,NULL,NULL);
                levels.push(hwnd5);
                m = new Manager("branch/"+branch+"/employees/manager/"+current_user+".txt");
                m->getInfo();
            }
            break;
        }
        case 24:
        {
            m->viewAllProducts();
            hwndP = CreateWindowEx(WS_EX_CLIENTEDGE,"Window",("All Products in Branch "+m->BranchID).c_str(),WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   700, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",("--- All Products in my branch ---\r\n\r\n\r\n"+output).c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,570,465,
                         hwndP,NULL,NULL,NULL);
            CreateWindow("BUTTON","Restock Products",
                         WS_VISIBLE|WS_CHILD,
                         200,490,200,30, /* X , Y, Width , height */
                         hwndP,(HMENU)79,NULL,NULL);
            CreateWindow("STATIC","Search a product by Product ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,555,218,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",
                                   WS_BORDER |WS_VISIBLE|WS_CHILD,
                                   10,585,470,25, /* X , Y, Width , height */
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("BUTTON","Search",
                         WS_VISIBLE|WS_CHILD,
                         490,585,90,25, /* X , Y, Width , height */
                         hwndP,(HMENU)65,NULL,NULL);
            levels.push(hwndP);
            break;
        }
        case 79:
        {
            int k = MessageBox(hwndP,"This action will restock your all branch products according to daily requirements.\nDo you want to continue?",
                               "Info not found!",MB_YESNO);
            if(k==7) break;
            string file2 = "branch/"+m->BranchID+"/"+m->BranchID+".txt";
            Branch b(file2);
            b.getBranchInfo();
            b.getProductInfoOfBranch();
            b.restockProducts();
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            MessageBox(levels.top(),"Products are restocked.",
                       "Products restocked!",MB_OK);
            break;
        }
        case 65:
        {
            char temps[1000];
            GetWindowText(edit[1],&temps[0],1000);
            string prd = temps;
            file = "product/"+prd+".txt";
            string res;
            ifstream f(file);
            if(!f)
            {
                MessageBox(hwndP,"Search for a valid Product ID.",
                           "Info not found!",MB_OK);
                break;
            }
            if(prodct) delete prodct;
            prodct = new Product(file);
            prodct->getProductInfo();
            res = prodct->viewProductDetails();
            string file2 = "branch/"+m->BranchID+"/"+m->BranchID+".txt";
            Branch b(file2);
            b.getBranchInfo();
            b.getProductInfoOfBranch();
            res = res + "Daily requirement of unit : " + to_string(b.products[prodct->productID].first)
                  + "\r\nAvailable quantity of unit : " + to_string(b.products[prodct->productID].second) + "\r\n\r\n";
            hwndP1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Product Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    650, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",res.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,570,300,
                         hwndP1,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update Daily Requirement of the Product",
                         WS_VISIBLE|WS_CHILD,
                         170,420,280,25, /* X , Y, Width , height */
                         hwndP1,(HMENU)66,NULL,NULL);
            CreateWindow("BUTTON","Update Available Quantity of the Product",
                         WS_VISIBLE|WS_CHILD,
                         170,470,280,25, /* X , Y, Width , height */
                         hwndP1,(HMENU)67,NULL,NULL);
            levels.push(hwndP1);
            break;
        }
        case 66:
        {
            hwndP2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Update Daily Requirement of the Product",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    400, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            string file2 = "branch/"+m->BranchID+"/"+m->BranchID+".txt";
            Branch b(file2);
            b.getBranchInfo();
            b.getProductInfoOfBranch();
            CreateWindow("STATIC",("Product ID : "+prodct->productID).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,25,570,25, /* X , Y, Width , height */
                         hwndP2,NULL,NULL,NULL);
            CreateWindow("STATIC",("Daily requirement in Branch "+m->BranchID+" : ").c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,65,250,25, /* X , Y, Width , height */
                         hwndP2,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT",to_string(b.products[prodct->productID].first).c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,105,MaxTextBox,25,
                                   hwndP2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         260,250,80,20, /* X , Y, Width , height */
                         hwndP2,(HMENU)68,NULL,NULL);
            return_val = 1;
            levels.push(hwndP2);
            break;
        }
        case 67:
        {
            hwndP2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Update Daily Requirement of the Product",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    400, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            string file2 = "branch/"+m->BranchID+"/"+m->BranchID+".txt";
            Branch b(file2);
            b.getBranchInfo();
            b.getProductInfoOfBranch();
            CreateWindow("STATIC",("Product ID : "+prodct->productID).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,25,570,25, /* X , Y, Width , height */
                         hwndP2,NULL,NULL,NULL);
            CreateWindow("STATIC",("Available Quantity in Branch "+m->BranchID+" : ").c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,65,250,25, /* X , Y, Width , height */
                         hwndP2,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT",to_string(b.products[prodct->productID].second).c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,105,MaxTextBox,25,
                                   hwndP2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         260,250,80,20, /* X , Y, Width , height */
                         hwndP2,(HMENU)68,NULL,NULL);
            return_val = 2;
            levels.push(hwndP2);
            break;
        }
        case 68:
        {
            char text[50];
            int k = GetWindowText(edit[4],&text[0],50);
            if(k<=0)
            {
                MessageBox(hwndP2,"You did not provide information.",
                           "Could not update product info!",MB_OK);
                break;
            }
            MessageBox(hwndP2,"You updated product info.",
                       "Product Info Updated!",MB_OK);
            string file2 = "branch/"+m->BranchID+"/"+m->BranchID+".txt", s = text;
            Branch b(file2);
            b.getBranchInfo();
            b.getProductInfoOfBranch();
            if(return_val == 1) b.products[prodct->productID].first = stod(s);
            if(return_val == 2) b.products[prodct->productID].second = stod(s);
            b.setProductInfoOfBranch();
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            break;

        }
        case 22:
        {
            hwndB = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Branch Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   600, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            string s = "My Branch "+ m->BranchID;
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,10,570,25, /* X , Y, Width , height */
                         hwndB,NULL,NULL,NULL);

            CreateWindow("EDIT",m->viewBranchDetails().c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,37,570,515,
                         hwndB,NULL,NULL,NULL);
            levels.push(hwndB);
            break;
        }
        case 23:
        {
            m->viewBranchEmployees();
            hwndE1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Employee Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            string s = "Employees of Branch : "+ m->BranchID;
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,10,570,25, /* X , Y, Width , height */
                         hwndE1,NULL,NULL,NULL);

            CreateWindow("EDIT",output.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,37,570,500,
                         hwndE1,NULL,NULL,NULL);
            s = "View and Update information of an employee of Branch "+m->BranchID +" : ";
            CreateWindow("STATIC",s.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         10,570,570,25, /* X , Y, Width , height */
                         hwndE1,NULL,NULL,NULL);
            CreateWindow("STATIC","Employee ID : ",
                         WS_VISIBLE|WS_CHILD,
                         10,590,100,25, /* X , Y, Width , height */
                         hwndE1,NULL,NULL,NULL);

            edit[9] = CreateWindow("EDIT","",
                                   WS_VISIBLE|WS_CHILD,
                                   110,590,470,25,
                                   hwndE1,NULL,NULL,NULL);
            s = "View and Update Info of the employee";
            CreateWindow("BUTTON",s.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         90,620,250,23, /* X , Y, Width , height */
                         hwndE1,(HMENU)150,NULL,NULL);
            s = "Change password of the employee";
            CreateWindow("BUTTON",s.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         345,620,235,23, /* X , Y, Width , height */
                         hwndE1,(HMENU)151,NULL,NULL);

            levels.push(hwndE1);
            break;
        }
        case 151:
        {
            char text[50];
            ifstream handle;
            GetWindowText(edit[9],&text[0],50);
            string temps = text;
            file = "branch/"+m->BranchID+"/employees/cashiers/"+temps+".txt";
            c = new Cashier(file);
            handle.open(file);
            if(handle)
            {
                e = new Cashier(file);
            }
            else
            {
                MessageBox(hwndE1,"Invalid Employee ID.\nThis employee may have no user ID and password.\nYou can only update password of cashiers in your branch.",
                           "Not a valid employee!",MB_OK);
                handle.close();
                break;
            }


            handle.close();

            string s = "This action will change the password of Employee "+temps+" to default password : "+temps+".\nDo you want to continue?";
            int k = MessageBox(hwndE1,s.c_str(),
                               "Change password to default password",MB_YESNO);
            if(k==7) break;

            if(e2) delete e2;
            e2 = new Manager(file);


            e2->getInfo();

            e2->setPassword(e2->EmpID);

            e2->setInfo();

            MessageBox(hwndE1,("Password Updated.\nNew Password : "+e2->EmpID).c_str(),
                       "Password Updated!",MB_OK);
            break;
        }
        case 150:
        {
            char text[50];
            ifstream handle;
            int u = 0;
            GetWindowText(edit[9],&text[0],50);
            string temps = text;
            string s = "Employee Info";

            file = "branch/"+m->BranchID+"/employees/"+temps+".txt";
            handle.open(file);
            if(handle)
            {
                e = new Employee(file);
                u = 1;
            }
            else
            {
                file = "branch/"+m->BranchID+"/employees/cashiers/"+temps+".txt";
                c = new Cashier(file);
                handle.open(file);
                if(handle)
                {
                    e = new Cashier(file);
                    u = 3;
                }
                else
                {
                    MessageBox(hwndE1,"Enter a valid Employee ID.","Employee not found!",MB_OK);
                    handle.close();
                    break;
                }
            }

            handle.close();

            e->getInfo();

            hwndE2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window",s.c_str(),WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    720, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            s = "--- EMPLOYEE INFO ---\r\n\r\nName : "+e->name +"\r\nGender : "+e->gender
                +"\r\nContact Number : "+e->contactNum
                +"\r\nAddress : "+e->address+"\r\nSalary : "+e->salary+"\r\nUser ID : ";
            if(u!=1)
            {
                s += e->EmpID;
            }
            else
            {
                s+= "N/A";
            }
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,10,570,135, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            s = "--- UPDATE INFO OF THE EMPLOYEE : "+e->EmpID+" ---";
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,170,570,20, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Employee ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,200,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT",e->EmpID.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                   140,200,440,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,240,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[2] = CreateWindow("EDIT",e->name.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,280,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,320,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT",e->gender.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,360,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,400,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT",e->contactNum.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,440,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,480,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT",e->address.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,520,MaxTextBox,25,
                                   hwndE2,NULL,NULL,NULL);
            CreateWindow("STATIC","Salary : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,560,120,25, /* X , Y, Width , height */
                         hwndE2,NULL,NULL,NULL);
            edit[12] = CreateWindow("EDIT",e->salary.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                    10,600,MaxTextBox,25,
                                    hwndE2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         248,650,100,20, /* X , Y, Width , height */
                         hwndE2,(HMENU)1007,NULL,NULL);
            levels.push(hwndE2);
            break;

        }
        case 31:
        {
            bool success = authorization(3,hwnd3);
            if(success)
            {
                hwnd6 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Cashier",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                       XSCREEN, /* x */
                                       YSCREEN, /* y */
                                       600, /* width */
                                       700, /* height */
                                       NULL,NULL,temp.hInstance,NULL);
                string id = "Cashier Logged In\nUser ID : "+current_user;
                CreateWindow("STATIC",id.c_str(),
                             WS_VISIBLE|WS_CHILD,
                             0,5,600,35, /* X , Y, Width , height */
                             hwnd6,NULL,NULL,NULL);
                CreateWindow("BUTTON","Branch Product Info",
                             WS_VISIBLE|WS_CHILD,
                             190,70,200,80, /* X , Y, Width , height */
                             hwnd6,(HMENU)32,NULL,NULL);

                CreateWindow("BUTTON","Customer Membership Info",
                             WS_VISIBLE|WS_CHILD,
                             190,190,200,80, /* X , Y, Width , height */
                             hwnd6,(HMENU)33,NULL,NULL);

                CreateWindow("BUTTON","Take Order",
                             WS_VISIBLE|WS_CHILD,
                             190,310,200,80, /* X , Y, Width , height */
                             hwnd6,(HMENU)34,NULL,NULL);

                CreateWindow("BUTTON","About Me",
                             WS_VISIBLE|WS_CHILD,
                             190,430,200,80, /* X , Y, Width , height */
                             hwnd6,(HMENU)35,NULL,NULL);

                CreateWindow("BUTTON","Log Out",
                             WS_VISIBLE|WS_CHILD,
                             190,550,200,80, /* X , Y, Width , height */
                             hwnd6,(HMENU)36,NULL,NULL);
                levels.push(hwnd6);
                c = new Cashier("branch/"+branch+"/employees/cashiers/"+current_user+".txt");
                c->getInfo();
            }
            break;
        }
        case 34:
        {
            c->viewAllProducts();
            orderlist.clear();
            hwndc = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Take Order",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   650, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            string s = "--- All Products in Branch : "+c->BranchID+" ---";
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD|SS_CENTER,
                         10,10,300,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            CreateWindow("EDIT",output.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,40,300,350,
                         hwndc,NULL,NULL,NULL);
            s = "-- Orders from the current customer --\r\n\r\n";
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD|SS_CENTER,
                         320,10,260,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            CreateWindow("EDIT",s.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         320,40,260,350,
                         hwndc,NULL,NULL,NULL);
            CreateWindow("BUTTON","Generate Bill",
                         WS_VISIBLE|WS_CHILD,
                         460,400,120,25, /* X , Y, Width , height */
                         hwndc,(HMENU)342,NULL,NULL);
            s = "--- Add(+) to the order list or delete(-) from the\norder list of the current customer ---";
            CreateWindow("STATIC",s.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         10,425,320,50, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            CreateWindow("STATIC","Product ID : ",
                         WS_VISIBLE|WS_CHILD,
                         10,475,100,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            edit[11] = CreateWindow("EDIT","",
                                    WS_VISIBLE|WS_CHILD,
                                    110,475,220,25,
                                    hwndc,NULL,NULL,NULL);
            CreateWindow("STATIC","Quantity(unit) : ",
                         WS_VISIBLE|WS_CHILD,
                         10,500,100,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            edit[12] = CreateWindow("EDIT","",
                                    WS_VISIBLE|WS_CHILD,
                                    110,500,220,25,
                                    hwndc,NULL,NULL,NULL);
            CreateWindow("BUTTON","Add",
                         WS_VISIBLE|WS_CHILD,
                         230,535,100,25, /* X , Y, Width , height */
                         hwndc,(HMENU)341,NULL,NULL);
            levels.push(hwndc);
            break;
        }
        case 341:
        {
            char pr[1000];
            char q[1000];
            string prid,qt,orders="";
            int k1 = GetWindowText(edit[11],&pr[0],1000);
            int k2 = GetWindowText(edit[12],&q[0],1000);

            if(k1<=0||k2<=0)
            {
                MessageBox(hwndc,"You did not provide information for all the fields.",
                           "Could not add product to the order list!",MB_OK);
                break;
            }
            c->viewAllProducts();
            prid = pr;
            qt = q;
            orderlist[prid] += stod(qt);
            for(it=orderlist.begin(); it!=orderlist.end(); it++)
            {
                orders+="Product ID : "+it->first+"\r\nQuantity(Unit) : "+to_string(it->second)+"\r\n\r\n";
            }
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            hwndc = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Take Order",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   650, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            string s = "--- All Products in Branch : "+c->BranchID+" ---";
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD|SS_CENTER,
                         10,10,300,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            CreateWindow("EDIT",output.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,40,300,350,
                         hwndc,NULL,NULL,NULL);
            s = "-- Orders from the current customer --\r\n\r\n"+orders;
            CreateWindow("STATIC",s.c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD|SS_CENTER,
                         320,10,260,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            CreateWindow("EDIT",s.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         320,40,260,350,
                         hwndc,NULL,NULL,NULL);
            CreateWindow("BUTTON","Generate Bill",
                         WS_VISIBLE|WS_CHILD,
                         460,400,120,25, /* X , Y, Width , height */
                         hwndc,(HMENU)342,NULL,NULL);
            s = "--- Add(+) to the order list or delete(-) from the\norder list of the current customer ---";
            CreateWindow("STATIC",s.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         10,425,320,50, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            CreateWindow("STATIC","Product ID : ",
                         WS_VISIBLE|WS_CHILD,
                         10,475,100,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            edit[11] = CreateWindow("EDIT","",
                                    WS_VISIBLE|WS_CHILD,
                                    110,475,220,25,
                                    hwndc,NULL,NULL,NULL);
            CreateWindow("STATIC","Quantity(unit) : ",
                         WS_VISIBLE|WS_CHILD,
                         10,500,100,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            edit[12] = CreateWindow("EDIT","",
                                    WS_VISIBLE|WS_CHILD,
                                    110,500,220,25,
                                    hwndc,NULL,NULL,NULL);
            CreateWindow("BUTTON","Add",
                         WS_VISIBLE|WS_CHILD,
                         230,535,100,25, /* X , Y, Width , height */
                         hwndc,(HMENU)341,NULL,NULL);
            levels.push(hwndc);
            break;
        }
        case 342:
        {
            hwndc2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Take Order",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN+50, /* x */
                                    YSCREEN+100, /* y */
                                    500, /* width */
                                    400, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC","SELECT CUSTOMER TYPE",
                         WS_BORDER |WS_VISIBLE|WS_CHILD|SS_CENTER,
                         100,90,300,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Member",
                         WS_VISIBLE|WS_CHILD,
                         190,145,120,30, /* X , Y, Width , height */
                         hwndc2,(HMENU)343,NULL,NULL);
            CreateWindow("BUTTON","Non-member",
                         WS_VISIBLE|WS_CHILD,
                         190,190,120,30, /* X , Y, Width , height */
                         hwndc2,(HMENU)344,NULL,NULL);
            levels.push(hwndc2);
            break;
        }
        case 343:
        {
            hwndP = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Generate Bill For A Member Customer",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN+50, /* x */
                                   YSCREEN+100, /* y */
                                   500, /* width */
                                   400, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC","Search a customer by Customer ID or Member ID :",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,70,340,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[13] = CreateWindow("EDIT","",
                                    WS_BORDER |WS_VISIBLE|WS_CHILD,
                                    10,105,470,25, /* X , Y, Width , height */
                                    hwndP,NULL,NULL,NULL);
            CreateWindow("BUTTON","Search",
                         WS_VISIBLE|WS_CHILD,
                         200,180,100,30, /* X , Y, Width , height */
                         hwndP,(HMENU)345,NULL,NULL);
            levels.push(hwndP);
            break;
        }
        case 345:
        {
            char tempstr[1000];
            int s = GetWindowText(edit[13],&tempstr[0],1000);
            string file = tempstr;
            file = "customer/"+file+".txt";
            ifstream f(file);
            if(!f)
            {
                MessageBox(hwndP,"Enter a valid Customer ID.","Customer not found!",MB_OK);
                SetWindowText(edit[13],0);
                f.close();
                break;
            }
            f.close();
            cust = new Customer(file);
            cust->getInfo();
            string res = cust->viewDetails();
            hwndP1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Customer Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    550, /* width */
                                    550, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",res.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,520,350,
                         hwndP1,NULL,NULL,NULL);
            CreateWindow("BUTTON","Generate Bill",
                         WS_VISIBLE|WS_CHILD,
                         185,400,200,30, /* X , Y, Width , height */
                         hwndP1,(HMENU)346,NULL,NULL);
            levels.push(hwndP1);
            break;
        }
        case 346:
        {
            int k = MessageBox(hwndP1,
                    "This action will update the available quantity of the branch products and generate a bill for the given order list.\nAre you sure you want to continue?",
                    "Generate Bill",MB_YESNO);
            if(k==7)
            {
                EnableWindow(levels.top(),true);
                break;
            }

            double total = c->GenerateBill(orderlist);
            output+="\r\n\r\nTotal Price = "+to_string(total)+" Tk. Only.";
            total = total - (total*cust->discount)/100;
            output+="\r\nTotal Price after discount = "+to_string(total)+" Tk. Only.";
            orderlist.clear();

            hwndP2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Bill",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN-50, /* x */
                                   YSCREEN+150, /* y */
                                   700, /* width */
                                   300, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",output.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         0,0,690,265,
                         hwndP2,NULL,NULL,NULL);
            levels.push(hwndP2);
            bill = true;
            member = true;
            break;
        }
        case 344:
        {
            int k = MessageBox(hwndc2,
                               "This action will update the available quantity of the branch products and generate a bill for the given order list.\nAre you sure you want to continue?",
                               "Generate Bill",MB_YESNO);
            if(k==7)
            {
                EnableWindow(levels.top(),true);
                break;
            }
            double total = c->GenerateBill(orderlist);
            orderlist.clear();
            output+="\r\n\r\nTotal Price = "+to_string(total)+" Tk. Only.";
            hwndP = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Bill",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN-50, /* x */
                                   YSCREEN+150, /* y */
                                   700, /* width */
                                   300, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",output.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         0,0,690,265,
                         hwndP,NULL,NULL,NULL);
            levels.push(hwndP);
            bill = true;
            member = false;
            break;
        }
        case 33:
        {
            string res = Customer::viewAllCustomers();
            hwndc = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Customer Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   700, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",("--- All Customers ---\r\n\r\n\r\n"+res).c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,570,465,
                         hwndc,NULL,NULL,NULL);
            CreateWindow("STATIC","Search a customer by Customer ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,535,250,25, /* X , Y, Width , height */
                         hwndc,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",
                                   WS_BORDER |WS_VISIBLE|WS_CHILD,
                                   10,565,470,25, /* X , Y, Width , height */
                                   hwndc,NULL,NULL,NULL);
            CreateWindow("BUTTON","Search",
                         WS_VISIBLE|WS_CHILD,
                         490,565,90,25, /* X , Y, Width , height */
                         hwndc,(HMENU)81,NULL,NULL);
            CreateWindow("BUTTON","Add A New Customer",
                         WS_VISIBLE|WS_CHILD,
                         210,620,200,25, /* X , Y, Width , height */
                         hwndc,(HMENU)82,NULL,NULL);
            levels.push(hwndc);
            break;
        }
        case 82:
        {
            hwndc2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Update Personal Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",("Customer ID or Member ID : CUST"+to_string(Customer::members+1)).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,10,570,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,50,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,90,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,130,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,170,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,210,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,250,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,290,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[6] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,330,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Membership Initiation Date : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,370,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[7] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,410,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Validity : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,450,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[8] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,490,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Discount : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,530,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[9] = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,570,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Add",
                         WS_VISIBLE|WS_CHILD,
                         248,620,100,20, /* X , Y, Width , height */
                         hwndc2,(HMENU)86,NULL,NULL);
            levels.push(hwndc2);
            break;
        }
        case 86:
        {
            char tempstr[12][1000];
            int i,chk = 1;
            for(i=3; i<=9; i++)
            {
                int s = GetWindowText(edit[i],&tempstr[i][0],1000);
                if(s<=0)
                {
                    MessageBox(hwndc2,"You did not provide information for all the fields.\n",
                               "Could not add customer!",MB_OK);
                    chk = 0;
                    break;
                }
            }
            if(chk == 0) break;
            int k = MessageBox(hwndc2,"Are you sure you want to add the customer?","Add a customer!",MB_YESNO);
            if(k==7) break;
            MessageBox(hwndc2,"You added a customer.","Customer Added!",MB_OK);
            cust = new Customer("customer/CUST"+to_string(Customer::members+1)+".txt");
            cust->name = tempstr[3];
            cust->address = tempstr[4];
            cust->gender = tempstr[5];
            cust->contactNum = tempstr[6];
            cust->membershipDate = tempstr[7];
            cust->validity = tempstr[8];
            string s = tempstr[9];
            cust->discount = stod(s);
            cust->memberID = "CUST"+to_string(Customer::members+1);
            cust->setInfo();
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            Customer::members++;
            saveStaticValues();
            break;
        }
        case 81:
        {
            char temps[1000];
            GetWindowText(edit[1],&temps[0],1000);
            string id = temps;
            file = "customer/"+id+".txt";
            string res;
            ifstream f(file);
            if(!f)
            {
                MessageBox(hwndc,"Search for a valid Customer ID.",
                           "Info not found!",MB_OK);
                break;
            }

            if(cust) delete cust;
            cust = new Customer(file);
            cust->getInfo();
            res = cust->viewDetails();

            hwndc1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Customer Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    550, /* width */
                                    550, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",res.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,520,350,
                         hwndc1,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update Customer Info",
                         WS_VISIBLE|WS_CHILD,
                         10,380,200,25, /* X , Y, Width , height */
                         hwndc1,(HMENU)83,NULL,NULL);
            CreateWindow("BUTTON","Delete Customer",
                         WS_VISIBLE|WS_CHILD,
                         10,420,200,25, /* X , Y, Width , height */
                         hwndc1,(HMENU)84,NULL,NULL);
            levels.push(hwndc1);
            break;
        }
        case 83:
        {
            hwndc2 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Update Personal Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    600, /* width */
                                    700, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",("Customer ID or Member ID : "+cust->memberID).c_str(),
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,10,570,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,50,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT",cust->name.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,90,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,130,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT",cust->gender.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,170,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,210,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT",cust->contactNum.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,250,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,290,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[6] = CreateWindow("EDIT",cust->address.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,330,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Membership Initiation Date : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,370,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[7] = CreateWindow("EDIT",cust->membershipDate.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,410,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Validity : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,450,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[8] = CreateWindow("EDIT",cust->validity.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,490,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("STATIC","Discount : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,530,200,25, /* X , Y, Width , height */
                         hwndc2,NULL,NULL,NULL);
            edit[9] = CreateWindow("EDIT",to_string(cust->discount).c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,570,MaxTextBox,25,
                                   hwndc2,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         248,620,100,20, /* X , Y, Width , height */
                         hwndc2,(HMENU)85,NULL,NULL);
            levels.push(hwndc2);
            break;
        }
        case 85:
        {
            char tempstr[12][1000];
            int i,chk = 1;
            for(i=3; i<=9; i++)
            {
                int s = GetWindowText(edit[i],&tempstr[i][0],1000);
                if(s<=0)
                {
                    MessageBox(hwndc2,"You did not provide information for all the fields.\nCustomer info remain same as before.",
                               "Could not update customer info!",MB_OK);
                    HWND h = levels.top();
                    levels.pop();
                    EnableWindow(levels.top(),true);
                    DestroyWindow(h);
                    chk = 0;
                    break;
                }
            }
            if(chk == 0) break;
            int k = MessageBox(hwndc2,"Are you sure you want to update customer info?","Update Customer info!",MB_YESNO);
            if(k==7) break;
            MessageBox(hwndc2,"You updated your customer info.","Customer Info Updated!",MB_OK);
            cust->name = tempstr[3];
            cust->address = tempstr[4];
            cust->gender = tempstr[5];
            cust->contactNum = tempstr[6];
            cust->membershipDate = tempstr[7];
            cust->validity = tempstr[8];
            string s = tempstr[9];
            cust->discount = stod(s);
            cust->setInfo();
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            break;
        }
        case 84:
        {
            int k = MessageBox(hwndc1,"Are you sure you want to delete the customer?",
                               "Customer Deletion!",MB_YESNO);
            if(k==7) break;
            remove(cust->file_path.c_str());
            MessageBox(hwndc1,("You deleted customer "+cust->memberID+".").c_str(),
                       "Customer Deleted!",MB_OK);
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            break;
        }
        case 32:
        {
            c->viewAllProducts();
            hwndP = CreateWindowEx(WS_EX_CLIENTEDGE,"Window",("All Products in Branch "+c->BranchID).c_str(),WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   650, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",("--- All Products in my branch ---\r\n\r\n\r\n"+output).c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,570,465,
                         hwndP,NULL,NULL,NULL);
            CreateWindow("STATIC","Search a product by Product ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,525,218,25, /* X , Y, Width , height */
                         hwndP,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT","",
                                   WS_BORDER |WS_VISIBLE|WS_CHILD,
                                   10,555,470,25, /* X , Y, Width , height */
                                   hwndP,NULL,NULL,NULL);
            CreateWindow("BUTTON","Search",
                         WS_VISIBLE|WS_CHILD,
                         490,555,90,25, /* X , Y, Width , height */
                         hwndP,(HMENU)80,NULL,NULL);
            levels.push(hwndP);
            break;
        }
        case 80:
        {
            char temps[1000];
            GetWindowText(edit[1],&temps[0],1000);
            string prd = temps;
            file = "product/"+prd+".txt";
            string res;
            ifstream f(file);
            if(!f)
            {
                MessageBox(hwndP,"Search for a valid Product ID.",
                           "Info not found!",MB_OK);
                break;
            }
            if(prodct) delete prodct;
            prodct = new Product(file);
            prodct->getProductInfo();
            res = prodct->viewProductDetails();
            string file2 = "branch/"+c->BranchID+"/"+c->BranchID+".txt";
            Branch b(file2);
            b.getBranchInfo();
            b.getProductInfoOfBranch();
            res = res + "Daily requirement : " + to_string(b.products[prodct->productID].first)
                  + "\r\nAvailable quantity : " + to_string(b.products[prodct->productID].second) + "\r\n\r\n";
            hwndP1 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Product Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                    XSCREEN, /* x */
                                    YSCREEN, /* y */
                                    500, /* width */
                                    400, /* height */
                                    NULL,NULL,temp.hInstance,NULL);
            CreateWindow("EDIT",res.c_str(),
                         WS_VISIBLE|WS_CHILD|ES_MULTILINE
                         |ES_READONLY|WS_HSCROLL|WS_VSCROLL,
                         10,10,470,340,
                         hwndP1,NULL,NULL,NULL);
            levels.push(hwndP1);
            break;
        }
        case 15:
        {
            string output = "MY PROFILE\n\n"
                            + a->viewDetails();
            hwnd7 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","About Me",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   500, /* width */
                                   600, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",output.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         0,5,500,200, /* X , Y, Width , height */
                         hwnd7,NULL,NULL,NULL);
            CreateWindow("BUTTON","Change Password",
                         WS_VISIBLE|WS_CHILD,
                         150,280,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)17,NULL,NULL);
            CreateWindow("BUTTON","Update Personal Info",
                         WS_VISIBLE|WS_CHILD,
                         150,320,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)18,NULL,NULL);
            levels.push(hwnd7);
            break;
        }
        case 18:
        {
            hwnd8 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Update Personal Info",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   600, /* width */
                                   700, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC","User ID : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            edit[1] = CreateWindow("EDIT",a->userID.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD|ES_READONLY,
                                   10,80,MaxTextBox,25,
                                   hwnd8,NULL,NULL,NULL);
            CreateWindow("STATIC","Name : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,120,120,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            edit[2] = CreateWindow("EDIT",a->name.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,160,MaxTextBox,25,
                                   hwnd8,NULL,NULL,NULL);
            CreateWindow("STATIC","Gender : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,200,120,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            edit[3] = CreateWindow("EDIT",a->gender.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,240,MaxTextBox,25,
                                   hwnd8,NULL,NULL,NULL);
            CreateWindow("STATIC","Contact Number : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,280,120,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            edit[4] = CreateWindow("EDIT",a->contactNum.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,320,MaxTextBox,25,
                                   hwnd8,NULL,NULL,NULL);
            CreateWindow("STATIC","Address : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,360,120,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            edit[5] = CreateWindow("EDIT",a->address.c_str(),WS_BORDER |WS_VISIBLE |WS_CHILD,
                                   10,400,MaxTextBox,25,
                                   hwnd8,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         248,550,100,20, /* X , Y, Width , height */
                         hwnd8,(HMENU)111,NULL,NULL);
            levels.push(hwnd8);
            break;
        }
        case 111:
        {
            char tempstr[7][1000];
            int i,chk = 1;
            for(i=1; i<=5; i++)
            {
                int s = GetWindowText(edit[i],&tempstr[i][0],1000);
                if(s<=0)
                {
                    MessageBox(hwnd8,"You did not provide information for all the fields.\nPersonal info remain same as before.",
                               "Could not update personal info!",MB_OK);
                    HWND h = levels.top();
                    levels.pop();
                    EnableWindow(levels.top(),true);
                    DestroyWindow(h);
                    chk = 0;
                    break;
                }
            }
            if(chk == 0) break;
            MessageBox(hwnd8,"You updated your personal info.","Personal Info Updated!",MB_OK);
            a->userID = tempstr[1];
            a->name = tempstr[2];
            a->gender = tempstr[3];
            a->contactNum = tempstr[4];
            a->address = tempstr[5];
            a->setInfo();
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            string output = "MY PROFILE\n\n"
                            + a->viewDetails();
            hwnd7 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","About Me",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   500, /* width */
                                   600, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",output.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         0,5,500,200, /* X , Y, Width , height */
                         hwnd7,NULL,NULL,NULL);
            CreateWindow("BUTTON","Change Password",
                         WS_VISIBLE|WS_CHILD,
                         150,280,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)17,NULL,NULL);
            CreateWindow("BUTTON","Update Personal Info",
                         WS_VISIBLE|WS_CHILD,
                         150,320,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)18,NULL,NULL);
            EnableWindow(levels.top(),false);
            levels.push(hwnd7);

            break;

        }
        case 25:
        {
            string output = "MY PROFILE\n\n"
                            + m->viewDetails();
            hwnd7 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","About Me",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   500, /* width */
                                   600, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",output.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         0,5,500,200, /* X , Y, Width , height */
                         hwnd7,NULL,NULL,NULL);
            CreateWindow("BUTTON","Change Password",
                         WS_VISIBLE|WS_CHILD,
                         150,280,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)27,NULL,NULL);
            CreateWindow("BUTTON","Update address",
                         WS_VISIBLE|WS_CHILD,
                         150,320,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)28,NULL,NULL);
            CreateWindow("BUTTON","Update Contact Number",
                         WS_VISIBLE|WS_CHILD,
                         150,360,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)29,NULL,NULL);
            levels.push(hwnd7);
            break;
        }
        case 35:
        {
            string output = "MY PROFILE\n\n"
                            + c->viewDetails();
            hwnd7 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","About Me",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   500, /* width */
                                   600, /* height */
                                   NULL,NULL,temp.hInstance,NULL);
            CreateWindow("STATIC",output.c_str(),
                         WS_VISIBLE|WS_CHILD,
                         0,5,500,200, /* X , Y, Width , height */
                         hwnd7,NULL,NULL,NULL);
            CreateWindow("BUTTON","Change Password",
                         WS_VISIBLE|WS_CHILD,
                         150,280,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)37,NULL,NULL);
            CreateWindow("BUTTON","Update address",
                         WS_VISIBLE|WS_CHILD,
                         150,320,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)38,NULL,NULL);
            CreateWindow("BUTTON","Update Contact Number",
                         WS_VISIBLE|WS_CHILD,
                         150,360,200,25, /* X , Y, Width , height */
                         hwnd7,(HMENU)39,NULL,NULL);
            levels.push(hwnd7);
            break;
        }
        case 28:
        {
            return_val = updatePersonalInfoWindow(m,"Address");
            break;
        }
        case 38:
        {
            return_val = updatePersonalInfoWindow(c,"Address");
            break;
        }
        case 29:
        {
            return_val = updatePersonalInfoWindow(m,"Contact Number");
            break;
        }
        case 39:
        {
            return_val = updatePersonalInfoWindow(c,"Contact Number");
            break;
        }
        case 211:
        {
            if(return_val == 1) updatePersonalInfo(m,"Address");
            else updatePersonalInfo(m,"Contact Number");
            break;
        }
        case 311:
        {
            if(return_val == 1) updatePersonalInfo(c,"Address");
            else updatePersonalInfo(c,"Contact Number");
            break;
        }
        case 17:
        {
            hwnd8 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Change Password",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   500, /* width */
                                   280, /* height */
                                   NULL,NULL,temp.hInstance,NULL);

            string temp = "Old Password : "+ a->password;

            CreateWindow("STATIC",temp.c_str(),
                         WS_BORDER|WS_VISIBLE|WS_CHILD,
                         10,10,470,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            CreateWindow("STATIC","New Password : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            textbox4 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    135,40,345,25,
                                    hwnd8,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         200,160,100,20, /* X , Y, Width , height */
                         hwnd8,(HMENU)110,NULL,NULL);
            levels.push(hwnd8);
            break;
        }
        case 27:
        {
            hwnd8 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Change Password",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   500, /* width */
                                   280, /* height */
                                   NULL,NULL,temp.hInstance,NULL);

            string temp = "Old Password : "+ m->password;

            CreateWindow("STATIC",temp.c_str(),
                         WS_BORDER|WS_VISIBLE|WS_CHILD,
                         10,10,470,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            CreateWindow("STATIC","New Password : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            textbox4 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    135,40,345,25,
                                    hwnd8,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         200,160,100,20, /* X , Y, Width , height */
                         hwnd8,(HMENU)210,NULL,NULL);
            levels.push(hwnd8);
            break;
        }
        case 37:
        {
            hwnd8 = CreateWindowEx(WS_EX_CLIENTEDGE,"Window","Change Password",WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX,
                                   XSCREEN, /* x */
                                   YSCREEN, /* y */
                                   500, /* width */
                                   280, /* height */
                                   NULL,NULL,temp.hInstance,NULL);

            string temp = "Old Password : "+ c->password;

            CreateWindow("STATIC",temp.c_str(),
                         WS_BORDER|WS_VISIBLE|WS_CHILD,
                         10,10,470,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            CreateWindow("STATIC","New Password : ",
                         WS_BORDER |WS_VISIBLE|WS_CHILD,
                         10,40,120,25, /* X , Y, Width , height */
                         hwnd8,NULL,NULL,NULL);
            textbox4 = CreateWindow("EDIT","",WS_BORDER |WS_VISIBLE |WS_CHILD,
                                    135,40,345,25,
                                    hwnd8,NULL,NULL,NULL);
            CreateWindow("BUTTON","Update",
                         WS_VISIBLE|WS_CHILD,
                         200,160,100,20, /* X , Y, Width , height */
                         hwnd8,(HMENU)310,NULL,NULL);
            levels.push(hwnd8);
            break;
        }
        case 110:
        {
            changePassword(a);
            break;
        }
        case 210:
        {
            changePassword(m);
            break;
        }
        case 310:
        {
            changePassword(c);
            break;
        }
        case 16:
        {
            int k = LogOut("Administrator");
            if(k==1) delete a;
            break;
        }
        case 26:
        {
            int k = LogOut("Manager");
            if(k==1) delete m;
            break;
        }
        case 36:
        {
            int k = LogOut("Cashier");
            if(k==1) delete c;
            break;
        }
        }
        break;
    case WM_CLOSE:
        if(bill)
        {
            bill = false;
            HWND h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            h = levels.top();
            levels.pop();
            EnableWindow(levels.top(),true);
            DestroyWindow(h);
            if(member)
            {
                h = levels.top();
                levels.pop();
                EnableWindow(levels.top(),true);
                DestroyWindow(h);
                h = levels.top();
                levels.pop();
                EnableWindow(levels.top(),true);
                DestroyWindow(h);
                member = false;
            }
        }
        else
        {
            if(levels.size()==1)
            {
                DestroyWindow(levels.top());
                levels.pop();
            }
            else if(levels.size()==3)
            {
                int k;
                if(current_user[0]=='A') k = LogOut("Administrator");
                else if(current_user.find("C")!=string::npos) k = LogOut("Cashier");
                else k = LogOut("Manager");
                readFile.close();
                writeFile.close();
            }
            else
            {
                HWND h = levels.top();
                levels.pop();
                EnableWindow(levels.top(),true);
                DestroyWindow(h);
            }
        }
        break;
    case WM_DESTROY:
        if(levels.size()==0) PostQuitMessage(0);/* send WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
