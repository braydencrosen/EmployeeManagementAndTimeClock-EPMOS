#include "Menus.h"
#include "Utils.h"
#include "Config.h"
#include "MgrFunctions.h"
#include "AdvancedFunctions.h"
#include "Notifications.h"
#include <iostream>
#include <limits>

char mainMenu(std::vector<employee>& employees, int& employeeidx)
{
	// Main menu
	while (true)
	{
		refresh(employeeidx, employees);
		int ubound = employees[employeeidx].getMgrStatus() ? 8 : 7;
		std::cout << "1 - Clock In\n"
			<< "2 - Clock Out\n"
			<< "3 - Start Meal\n"
			<< "4 - End Meal\n"
			<< "5 - Show Last Punch\n"
			<< "6 - View notifications\n";

		// Manager menu extension
		if (employees[employeeidx].getMgrStatus())
		{
			std::cout << "7 - View Clocked In\n"
				<< "8 - Advanced\n"
				<< "0 - LOGOUT\n";
		}
		else
		{
			std::cout << "7 - Advanced\n"
				<< "0 - LOGOUT\n";
		}

		char choice;

		std::cout << "-> ";
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (choice >= '0' && choice <= '0' + ubound)
			return choice;

		std::cout << "Invalid, try again\n";
		wait(1);
		refresh(employeeidx, employees);
	}
}

void advancedMenu(std::vector<employee>& employees, int& employeeidx, std::vector<notification>& notifications)
{
	int ubound = employees[employeeidx].getMgrStatus() ? 9 : 4;
	while (true)
	{
		refresh(employeeidx, employees);
		std::cout << "1 - View timecard\n"
			<< "2 - Edit availability\n"
			<< "3 - Send message\n";

		if (employees[employeeidx].getMgrStatus())
		{
			std::cout << "4 - Schedule editor\n"
				<< "5 - View all employees\n"
				<< "6 - Edit employees\n"
				<< "7 - Finance dashboard\n"
				<< "8 - Change punch status\n"
				<< "9 - Settings\n"
				<< "0 - Return\n";
		}
		else
		{
			std::cout << "4 - Settings\n"
				<< "0 - Return\n";
		}
		char choice;
		while (true)
		{
			std::cout << "-> ";
			std::cin >> choice;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (choice >= '0' && choice <= '0' + ubound)
				break;

			std::cout << "Invalid, try again\n";
		}

		// Advanced menu switch
		switch (choice)
		{
		case '1':
			// View timecard
			if (beta_mode)
				viewTimecard(employees, employeeidx);
			else
			{
				std::cout << "This function is temporarily unavailable\n"; wait(2);
			}
			break;

		case '2':
			// Edit availability
			if (beta_mode)
				editAvailability(employees, employeeidx);
			else
			{
				std::cout << "This function is temporarily unavailable\n"; wait(2);
			}
			break;

		case '3':
			sendMessage(employees, employeeidx, notifications);
			break;

		case '4':
			// Settings -- Sched editor for mgr
			if (beta_mode)
			{
				if (!employees[employeeidx].getMgrStatus())
					settings(employees, notifications, employeeidx);
				else
					schedEditor(employees, employeeidx);
			}
			else
			{
				std::cout << "This function is temporarily unavailable\n"; wait(2);
			}
			break;

		case '5':
			// View employees
			viewEmployees(employees, employeeidx);
			break;

		case '6':
			// Edit employees
			editInfo(employees, notifications, employeeidx);
			break;


		case '7':
			// Finance dashboard
			if (beta_mode)
				financeDashboard();
			else
			{
				std::cout << "This function is temporarily unavailable\n"; wait(2);
			}
			break;

		case '8':
			// Change punch status
			changePunchStautus(employees, notifications, employeeidx);
			break;

		case '9':
			// Settings
			if (beta_mode)
				settings(employees, notifications, employeeidx);
			else
			{
				std::cout << "This function is temporarily unavailable\n"; wait(2);
			}
			break;

		case '0':
			return;
			break;
		}
	}
}
