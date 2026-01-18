#pragma once
#include <vector>
#include <ctime>
#include <string>
#include "Employees.h"

struct punch
{
	int employeeID;
	std::string name;
	std::string type;
	std::string timestamp;
	std::string setby;
	bool externally = false;
};

void loadEmployees(std::vector<employee>& employees);
void saveEmployees(const std::vector<employee>& employees);

void savePunch(const punch& p);
punch getLastPunch(int employeeID);
