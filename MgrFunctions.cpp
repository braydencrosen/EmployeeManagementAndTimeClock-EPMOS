#include "MgrFunctions.h"
#include "Notifications.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>

void editInfo(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx)
{
	while (true)
	{
		refresh(employeeidx, employees);
		displayEmployees(employees, employeeidx, false, true, true, true, false, true);
		std::cout << "\n";
		std::cout << "Would you like to:\n"
			<< "1 - Add\n"
			<< "2 - Change pay\n"
			<< "3 - Change Status\n"
			<< "4 - \033[33mLock\033[0m\n"
			<< "5 - \033[33mUnlock\033[0m\n"
			<< "6 - \033[31mTERMINATE\033[0m\n"
			<< "0 - Return\n"
			<< "->";
		char choice;
		int choiceidx;

		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Get index if not adding or canceling
		if (choice != '1' && choice != '0')
		{
			choiceidx = getChoiceIdx(employees, employeeidx);
			// Invalid
			if (choiceidx == -2)
				continue;
			// Cancel
			if (choiceidx == -1)
				return;
		}

		switch (choice)
		{
			// Add employee
		case '1':
			addEmployee(employees, employeeidx);
			break;


			// Change pay
		case '2':
			changePay(employees, notifications, employeeidx, choiceidx);
			break;

			// Change status
		case '3':
			changeStatus(employees, notifications, employeeidx, choiceidx);
			break;

			// Lock employee
		case '4':
			lockEmployee(employees, employeeidx, true, choiceidx);
			break;

			// Unlock employee
		case '5':
			lockEmployee(employees, employeeidx, false, choiceidx);
			break;

			// Remove employee
		case '6':
			terminateEmployee(employees, employeeidx, choiceidx);
			break;

			// Exit
		case '0':
			return;
			break;

			// Invalid
		default:
			std::cout << "Unknown, try again" << std::endl;
			break;
		}
	}
}

void viewClockedIn(std::vector<employee>& employees, int& employeeidx)
{
	bool clockedIn = false;
	// Show clocked in employees
	std::cout << "\n--Clocked In--" << std::endl;
	for (int i = 0; i < employees.size(); i++)
	{
		if (employees[i].getStatus() == 1)
		{
			std::cout << std::left << std::setw(20) << employees[i].getName();

			// Display manager status
			if (employees[i].getMgrStatus())
			{
				std::cout << "MGR";
			}
			if (employees[i].getMstrStatus())
			{
				std::cout << "*";
			}
			std::cout << "\n";
			clockedIn = true;
		}
	}
	if (!clockedIn)
	{
		std::cout << "\nNo employees are clocked in" << std::endl;
	}

	bool onMeal = false;
	// Check if employees are on meal
	for (int i = 0; i < employees.size(); i++)
	{
		if (employees[i].getStatus() == 2)
			onMeal = true;
	}

	if (onMeal)
	{
		std::cout << "\n--On Meal--" << std::endl;
		for (int i = 0; i < employees.size(); i++)
		{
			if (employees[i].getStatus() == 2)
			{
				std::cout << std::left << std::setw(20) << employees[i].getName();

				// Display manager status
				if (employees[i].getMgrStatus())
				{
					std::cout << "MGR";
				}
				if (employees[i].getMstrStatus())
				{
					std::cout << "*";
				}
				std::cout << "\n";
			}
		}
	}
	std::cout << "\nPress Enter to return...";
	while (std::cin.get() != '\n');

}
