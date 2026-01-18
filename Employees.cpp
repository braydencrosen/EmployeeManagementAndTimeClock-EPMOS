#include "Employees.h"

using namespace std;

// Constructor
employee::employee(string empName, int empID, double empPay, bool managerStatus, int pin, bool mststatus, int status)
{
    name = empName;
    employeeID = empID;
    pay = empPay;
    isManager = managerStatus;
    managerPin = pin;
    masterStatus = mststatus;
    timeStatus = status;
    
}

// Getter functions
string employee::getName() const { return name; }
int employee::getID() const { return employeeID; }
double employee::getPay() const { return pay; }
bool employee::getMgrStatus() const { return isManager; }
int employee::getMgrPin() const { return managerPin; }
bool employee::getMstrStatus() const { return masterStatus; }
int employee::getStatus() const { return timeStatus; }

// Setter functions
void employee::setTimeStatus(int set) { timeStatus = set; }
void employee::setPay(double newPay) { pay = newPay; }
void employee::setPin(int pin) { managerPin = pin; }
void employee::setID(int id) { employeeID = id; }
void employee::setName(std::string newName) { name = newName; }
void employee::setPermissions(int status)
{   
    // Manager
    if (status == 0)
    {
        isManager = true;
        masterStatus = false;
    }
    // Master
    else if (status == 1)
    {
        isManager = true;
        masterStatus = true;
    }
    // Associate
    else
    {
        isManager = false;
        masterStatus = false;
    }
}
