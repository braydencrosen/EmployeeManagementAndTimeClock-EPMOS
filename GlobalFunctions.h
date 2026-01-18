#pragma once
#include <vector>
#include "Employees.h"
#include "Utils.h"
#include "FileIO.h"

void clockIn(std::vector<employee>& employees, int& employeeidx);
void clockOut(std::vector<employee>& employees, int& employeeidx);
void startMeal(std::vector<employee>& employees, int& employeeidx);
void endMeal(std::vector<employee>& employees, int& employeeidx);