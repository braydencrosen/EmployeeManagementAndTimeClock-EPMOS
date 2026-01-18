#pragma once
#include "Employees.h"
#include "Config.h"
#include "Notifications.h"
#include <string>

char mainMenu(std::vector<employee>& employees, int& employeeidx);
void advancedMenu(std::vector<employee>& employees, int& employeeidx, std::vector<notification>& notifications);
