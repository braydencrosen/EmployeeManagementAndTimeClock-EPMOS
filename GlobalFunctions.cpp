#include "GlobalFunctions.h"
#include <iostream>

void clockIn(std::vector<employee>& employees, int& employeeidx)
{
	// Prevent clock in if on clock
	if (employees[employeeidx].getStatus() == 0)
	{
		// Create p struct and pass to .txt file
		punch p{ employees[employeeidx].getID(), employees[employeeidx].getName(), "CLOCK_IN", getTime() };
		savePunch(p);
		employees[employeeidx].setTimeStatus(1);
		std::cout << std::endl
			<< employees[employeeidx].getName() << ", you are now clocked in at " << getTime() << std::endl;
		saveEmployees(employees);
	}
	else if (employees[employeeidx].getStatus() == 2)
	{
		std::cout << "\nYou are on a meal break, select end meal" << std::endl;
	}
	else
	{
		std::cout << "\nYou are already clocked in" << std::endl;
	}
}

void clockOut(std::vector<employee>& employees, int& employeeidx)
{
	// Prevent clock out if on meal or not on clock
	if (employees[employeeidx].getStatus() == 1)
	{
		punch p{ employees[employeeidx].getID(), employees[employeeidx].getName(), "CLOCK_OUT", getTime() };
		savePunch(p);
		employees[employeeidx].setTimeStatus(0);
		std::cout << std::endl
			<< employees[employeeidx].getName() << ", you are now clocked out at " << getTime() << std::endl;
		saveEmployees(employees);
	}
	else
	{
		std::cout << "\nYou are not clocked in" << std::endl;
	}
}

void startMeal(std::vector<employee>& employees, int& employeeidx)
{
	// Prevent start meal if not clocked in or already on meal
	if (employees[employeeidx].getStatus() == 1)
	{
		punch p{ employees[employeeidx].getID(), employees[employeeidx].getName(), "START_MEAL", getTime() };
		savePunch(p);
		employees[employeeidx].setTimeStatus(2);
		std::cout << std::endl
			<< employees[employeeidx].getName() << ", start meal saved at " << getTime() << std::endl;
		saveEmployees(employees);
	}
	else
	{
		std::cout << "\nYou are not clocked in" << std::endl;
	}
}

void endMeal(std::vector<employee>& employees, int& employeeidx)
{
	// Prevent end meal if not on a meal or not clocked in
	if (employees[employeeidx].getStatus() == 2)
	{
		punch p{ employees[employeeidx].getID(), employees[employeeidx].getName(), "END_MEAL", getTime() };
		savePunch(p);
		employees[employeeidx].setTimeStatus(1);
		std::cout << std::endl
			<< employees[employeeidx].getName() << ", end meal saved at " << getTime() << std::endl;
		saveEmployees(employees);
	}
	else
	{
		std::cout << "\nYou are not on a meal" << std::endl;
	}
}