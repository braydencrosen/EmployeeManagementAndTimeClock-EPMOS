#pragma once
#include "Employees.h"

// Availability

struct availability
{
    int mon, tues, wed, thurs, fri, sat, sun;
};


void saveAvailability(std::vector<employee>& employees);
void loadAvailability(std::vector<employee>& employees);
