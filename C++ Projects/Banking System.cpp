#include <iostream>
#include <fstream>
using namespace std;

class onlineBank{
protected:
    string Name,uname,Address,uaddress,Account_type,uaccount_type,ID_NO,uid_no,Password,upassword;
    int Account_no;
};

class Bank:public onlineBank{
public:
    string getName(){
    return Name;
    }
    void setName(string name){
    Name = name;
    }
    string getAddress(){
    return Address;
    }
    void setAddress(string address){
    Address = address;
    }
    string getAccount_type(){
    return Account_type;
    }
    void setAccount_type(string account_type){
    Account_type = account_type;
    }
    string getID_NO(){
    return ID_NO;
    }
    void setID_NO(string id_no){
    ID_NO = id_no;
    }
    int getAccount_no(){
    return Account_no;
    }
    string getPassword(){
    return Password;
    }
    void setPassword(string password){
    Password = password;
    }

    void mainMenu();
    void createCustPrifile();
    void userLogin();

};

void Bank::userLogin(){
    string Nm,Ps;
    cout << "=========================User-Login========================="<<endl;
    cout << "Enter your Username :";
    cin >> Nm;
    cout << "Enter Your Password :";
    cin >> Ps;
    ifstream readU("Account_Details.txt");
    while(readU>>uname>>upassword>>uaddress>>uaccount_type>>uid_no){
    if(Nm == uname){
        cout << "Welcome to the login Page "<<uname <<endl;
    }else{
        cout << "Invalid login details....try again"<<endl;
        userLogin();
    }
}}

void Bank::createCustPrifile(){
    string name,address,account_type,id_no,password;

    cout << "Create customer profile"<<endl;
    cout << "Enter Name :";
    cin >> name;
    setName(name);
    cout << "Create a password for User :";
    cin >> password;
    setPassword(password);
    cout << "Enter Address :";
    cin >> address;
    setAddress(address);
    cout << "Enter Account Type :";
    cin >> account_type;
    setAccount_type(account_type);
    cout << "Enter ID Number :";
    cin >> id_no;
    setID_NO(id_no);
 ofstream writeF("Account_Details.txt",ios::app);
 writeF<<getName()<<' '<<getPassword()<<' '<<getAddress()<<' '<<getAccount_type()<<' '<<getID_NO();

    cout << "*********Account created successfully**********"<<endl;
ifstream readF("Account_Details.txt");
while(readF>>uname>>upassword>>uaddress>>uaccount_type>>uid_no){

    cout << "Name :"<<uname <<endl;
    cout << "Address :"<<uaddress <<endl;
    cout << "Account type :"<<uaccount_type <<endl;
    cout << "ID Number :"<<uid_no <<endl;

    mainMenu();

}

}

void Bank::mainMenu(){
    int option;
    string uname,pword;
    cout << "=================Main-Menu==================="<<endl;
    cout << "******Welcome to Capitec Banking system*****"<<endl;
    cout << "1. Employee Login"<<endl;
    cout << "2. Customer Login"<<endl;
    cout << "Select Your Option :";
    cin >> option;
    switch(option){
case 1:
    cout << "******Welcome to Employee Login-Page*****"<<endl;
    cout << "Please Log in"<<endl;
    cout << "Enter Username :";
    cin >> uname;
    cout << "Enter Password :";
    cin >> pword;
    if(uname == "admin" && pword == "12345"){
        cout << "******Welcome to Admin Page*****"<<endl;
        createCustPrifile();
    }else{
        cout << "Wrong Password or username.....exiting"<<endl;
        mainMenu();
    }
    break;
case 2:
    userLogin();
    }
}


int main()
{
    Bank bank;
    bank.mainMenu();
    return 0;
}
