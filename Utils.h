#pragma once
#include "Employees.h"
#include "Notifications.h"
#include <vector>

// Windows
void disableEcho();
void enableEcho();


// Session util functions
std::string getTime();
void clear();
bool notifScreen(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx, bool loggedin);
void refresh(int& employeeidx, std::vector<employee>& employees);
void wait(int sec);
void printHeader(int& employeeidx, std::vector<employee>& employees);
int getChoiceIdx(std::vector<employee>& employees, int& employeeidx);
bool checkIDInput(std::vector<employee>& employees, int& id);
int getID(std::vector<employee>& employees, std::vector<notification>& notifications);
int setIndex(int input, std::vector<employee>& employees);

// Settings util functions
void changePin(std::vector<employee>& employees, int& employeeidx);

// MGR util functions
bool verifyPin(std::vector<employee>& employees, int& employeeidx);
void displayEmployees(std::vector<employee>& employees, int& employeeidx, bool id, bool name, bool pay, bool status, bool punchstatus, bool identifiers);
void addEmployee(std::vector<employee>& employees, int& employeeidx);
void terminateEmployee(std::vector<employee>& employees, int& employeeidx, int& choiceidx);
void lockEmployee(std::vector <employee>& employees, int& employeeidx, bool lock, int& choiceidx);
void changePay(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx, int& choiceidx);
int statusChangeCheck(std::vector<employee>& employees, int& employeeidx, int errMessage);
void createMgrPin(std::vector<employee>& employees, int& employeeidx, int idx);
void changeStatus(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx, int& choiceidx);

// ADMIN
void adminPanel(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx, const int& admin_password);
