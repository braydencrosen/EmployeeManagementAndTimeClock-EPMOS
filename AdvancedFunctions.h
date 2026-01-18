#pragma once
#include "Employees.h"
#include "FileIO.h"
#include "Notifications.h"
#include "Config.h"

void viewTimecard(std::vector<employee>& employees, int& employeeidx);
void editAvailability(std::vector<employee>& employees, int& employeeidx);
void schedEditor(std::vector<employee>& employees, int& employeeidx);
void viewEmployees(std::vector<employee>& employees, int& employeeidx);
void sendMessage(std::vector<employee>& employees, int& employeeidx, std::vector<notification>& notifications);
void financeDashboard();
void changePunchStautus(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx);
void settings(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx);