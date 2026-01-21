#include "Utils.h"
#include "Config.h"
#include "FileIO.h"
#include "Employees.h"
#include "Menus.h"
#include "GlobalFunctions.h"
#include "MgrFunctions.h"
#include "AdvancedFunctions.h"
#include "Notifications.h"
#pragma warning(disable : 4996)

#include <iostream>
#include <filesystem>

//MAIN 
int main()
{
	std::filesystem::path DATA_DIR = std::filesystem::path(__FILE__).parent_path() / "Data";

	// Create folder if it doesn't exist
	if (!std::filesystem::exists(DATA_DIR))
	{
		std::filesystem::create_directory(DATA_DIR);
	}

	std::filesystem::path filePath = DATA_DIR / "config.txt";

	if (std::filesystem::exists(filePath)) {
		std::cout << "File exists: " << filePath << '\n';
	}
	else {
		std::cout << "File NOT found at: " << filePath << '\n';
	}


	// Define variables to store file data between data loads
	std::vector<employee> employees;
	std::vector<notification> notifications;
	int employeeidx = -1;

	// Load file data to variables
	loadConfig();
	loadEmployees(employees);
	loadNotifs(employees, notifications);
	refresh(employeeidx, employees);

	if (employees.empty())
	{
		/// Load test employee data if file is empty or does not exist
		employees.push_back(employee("Test User", 1111111, 20.00, true, 1111, true, 1));
		employees.push_back(employee("Alex Martinez", 2039485, 15.25, false, 0, false, 1));
		employees.push_back(employee("Samantha Lee", 4012346, 16.10, true, 2864, false, 2));
		employees.push_back(employee("Jordan Patel", 1964273, 15.75, false, 0, false, 2));
		employees.push_back(employee("Chris Donovan", 4012348, 17.00, false, 0, false, 1));

		saveEmployees(employees);
	}

	//// Master Session
	while (true)
	{
		// Load (or reload) file data to session
		loadConfig();
		loadEmployees(employees);
		loadNotifs(employees, notifications);

		// Configuration redirect if no file found
		if (checkConfig())
		{
			std::cout << "\n\033[31mNo configuration file found. Redirecting to config setup...\033[0m\n";
			wait(8);
			configMenu();
		}

		// BLOCKED
		if (block_access)
		{
			int input;
			while (true)
			{
				clear();
				std::cout << "\033[33mSystem is currently disabled. Please try again later\033[0m\n";
				disableEcho();
				std::cin >> input;
				enableEcho();
				if (std::cin.fail())
				{
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
				if (input == admin_access) { break; }
			}
		}


		// Display login screen and set index to ID
		int id = getID(employees, notifications);

		// Restart master session if admin panel accessed
		if (id == -1) { continue; }

		// Set index to logged in employee
		employeeidx = setIndex(id, employees);

		bool run = true;
		// Check for notifications, terminate session if user is locked or terminated
		if (!notifScreen(employees, notifications, employeeidx, false)) { run = false; }

		// Session
		while (run)
		{
			punch last;

			// Main menu
			switch (mainMenu(employees, employeeidx))
			{
			case '1':
				// Clock In
				clockIn(employees, employeeidx);
				wait(2);
				run = false;
				break;

			case '2':
				// Clock Out
				clockOut(employees, employeeidx);
				wait(2);
				run = false;
				break;

			case '3':
				// Start Meal
				startMeal(employees, employeeidx);
				wait(2);
				run = false;
				break;

			case '4':
				// End Meal
				endMeal(employees, employeeidx);
				wait(2);
				run = false;
				break;

			case '5':
			{
				// Show Last Punch
				last = getLastPunch(employees[employeeidx].getID());
				if (last.employeeID == 0)
					std::cout << "No punches found\n";
				else
					std::cout << "\nLast punch: " << last.type
					<< " at " << last.timestamp << std::endl;
				wait(3);
				break;
			}
			case '6':
			{
				notifScreen(employees, notifications, employeeidx, true);
				break;
			}
			case '7':
				// Advanced -- MGR View clocked in (no pin verification)
				if (employees[employeeidx].getMgrStatus())
					viewClockedIn(employees, employeeidx);
				else
					advancedMenu(employees, employeeidx, notifications);
				break;

			case '8':
				// Advanced menu
				if (verifyPin(employees, employeeidx))
					advancedMenu(employees, employeeidx, notifications);
				break;
			case '0':
				//Logout
				refresh(employeeidx, employees);
				run = false;
				break;
			}
		}
	}
}