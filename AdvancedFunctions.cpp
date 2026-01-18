#include "AdvancedFunctions.h"
#include "Utils.h"
#include "Notifications.h"
#include "Config.h"

#include <iostream>
#include <iomanip>
#include <sstream>

void viewTimecard(std::vector<employee>& employees, int& employeeidx)
{

}

void editAvailability(std::vector<employee>& employees, int& employeeidx)
{

}

void schedEditor(std::vector<employee>& employees, int& employeeidx)
{
	std::cout << "1 - View schedule\n"
		<< "2 - View employee availability\n"
		<< "";
}

void viewEmployees(std::vector<employee>& employees, int& employeeidx)
{
	refresh(employeeidx, employees);
	displayEmployees(employees, employeeidx, false, true, true, true, true, false);
	std::cout << "\nPress Enter to return...";
	while (std::cin.get() != '\n');
}

void sendMessage(std::vector<employee>& employees, int& employeeidx, std::vector<notification>& notifications)
{
	while (true)
	{
		// Set input variables
		std::string input;
		int choice = -1;
		int lastEmployee = static_cast<int>(employees.size());
		int reminderOption = lastEmployee + 1; // Reminder is last for non-managers
		int announceOption = reminderOption + 1;	  // Announcement is last for managers
		int systemNotif = reminderOption + 2;	// System announcement is last for master access
		int code; // Message code
		int recipient; // Recipient index

		refresh(employeeidx, employees);
		// Display list
		displayEmployees(employees, employeeidx, false, true, false, false, false, true);
		std::cout << "\033[33m" << reminderOption << " - Create reminder\n\033[0m";
		// Announce (managers only)
		if (employees[employeeidx].getMgrStatus())
			std::cout << "\033[36m" << announceOption << " - Send announcement\n\033[0m";
		// System notif (master status only)
		if (employees[employeeidx].getMstrStatus())
			std::cout << "\033[35m" << systemNotif << " - Send SYSTEM notification\n\033[0m";

		std::cout << "Enter # (0 to cancel): ";
		std::getline(std::cin, input);

		// Check numeric
		try
		{
			choice = std::stoi(input);
		}
		catch (...)
		{
			std::cout << "Invalid\n";
			wait(2);
			continue; // Ask again
		}

		// Cancel
		if (choice == 0)
		{
			return;
		}

		// Allow system notification only for master status
		if (employees[employeeidx].getMstrStatus())
		{
			if (choice <= 0 || choice > systemNotif)
			{
				std::cout << "Invalid, choose a number between 1 and " << systemNotif << ".\n";
				wait(2);
				continue;
			}
		}
		// Allow announcement only for manager
		else if (employees[employeeidx].getMgrStatus())
		{
			if (choice <= 0 || choice > announceOption)
			{
				std::cout << "Invalid, choose a number between 1 and " << announceOption << ".\n";
				wait(2);
				continue;
			}
		}
		// Only allow individual send for non managers, add reminder as final option after employees
		else
		{
			if (choice <= 0 || choice > reminderOption)
			{
				std::cout << "Invalid, choose a number between 1 and " << reminderOption << "\n";
				wait(2);
				continue;
			}
		}
		std::string message;
		int recipientIdx = choice - 1;

		// Prevent self send
		if (recipientIdx == employeeidx)
		{
			std::cout << "You cannot send a message to yourself, select create reminder\n";
			wait(2);
			continue;
		}

		// Reminder
		if (choice == reminderOption)
		{
			std::cout << "Enter reminder (0 to cancel): ";
			std::getline(std::cin, message);
			if (message == "0")
				continue;
			code = 8;
			recipient = employees[employeeidx].getID();
		}
		// Announcement
		else if (choice == announceOption)
		{
			std::cout << "Enter announcement message (0 to cancel): ";
			std::getline(std::cin, message);
			if (message == "0")
				continue;
			code = 9;
			recipient = 0;
		}
		// System notif
		else if (choice == systemNotif)
		{
			std::cout << "Enter system message (0 to cancel): ";
			std::getline(std::cin, message);
			if (message == "0")
				continue;
			code = 0;
			recipient = 0;
		}
		// Message
		else
		{
			std::cout << "Enter message for " << employees[recipientIdx].getName() << " (0 to cancel): ";
			std::getline(std::cin, message);
			if (message == "0")
				continue;
			code = 1;
			recipient = employees[recipientIdx].getID();
		}

		// Send all
		if (code == 9 || code == 0)
		{
			for (int i = 0; i < employees.size(); i++)
			{
				// Bypass self send only for announcements
				if (code == 9)
				{
					if (i == employeeidx) continue;
				}
				recipient = employees[i].getID();
				newNotif(notifications, code, employees[employeeidx].getID(), recipient, message);
			}
		}
		// Send single
		else
		{
			newNotif(notifications, code, employees[employeeidx].getID(), recipient, message);
		}
		saveNotifs(employees, notifications);

		if (code == 1)
		{
			// Confirmation messages
			std::cout << "Message sent to " << employees[recipientIdx].getName() << "\n";
		}
		if (code == 0)
		{
			std::cout << "System notification created\n";
		}
		if (code == 9)
		{
			std::cout << "Announcement sent to all employees\n";
		}
		if (code == 8)
		{
			std::cout << "Reminder created\n";
		}
		wait(2);
		return;
	}
}

void financeDashboard()
{
	std::cout << "This function does not yet exist\n";
	wait(3);
	return;
}

void changePunchStautus(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx)
{
	int choiceidx;

	while (true)
	{
		// Display employee data
		refresh(employeeidx, employees);
		displayEmployees(employees, employeeidx, false, true, false, true, true, true);
		choiceidx = getChoiceIdx(employees, employeeidx);

		// Cancel
		if (choiceidx == -1)
			return;

		// Invalid
		else if (choiceidx == -2)
			continue;

		else
			break;
	}

	if (employees[choiceidx].getMgrStatus() && !employees[employeeidx].getMstrStatus())
	{
		std::cout << "You don't have permission to edit this employee's punches\n";
		wait(2);
		return;
	}
	char choice;
	int status = employees[choiceidx].getStatus();

	std::cout << "\nWould you like to:\n"
		<< "\n1 - Add clock in\n"
		<< "2 - Add clock out\n"
		<< "3 - Add start meal\n"
		<< "4 - Add end meal\n";

	while (true)
	{
		std::cout << "->";
		std::cin >> choice;
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid" << std::endl;
			wait(2);
			continue;
		}
		break;
	}

	std::string message;

	switch (choice)
	{
	case '1':
		// Clock in
		if (status == 0)
		{
			punch p{ employees[choiceidx].getID(), employees[choiceidx].getName(), "CLOCK_IN", getTime(), employees[employeeidx].getName(), true };
			savePunch(p);
			employees[choiceidx].setTimeStatus(1);

			// Create system notification
			message = "You have a recent punch edit, select view timecard to view details";
			newNotif(notifications, 0, employees[employeeidx].getID(), employees[choiceidx].getID(), message);
			saveNotifs(employees, notifications);

			std::cout << std::endl
				<< employees[choiceidx].getName() << ", was clocked in at " << getTime() << " by " << employees[employeeidx].getName() << std::endl;
		}
		else if (employees[choiceidx].getStatus() == 1)
		{
			std::cout << employees[choiceidx].getName() << " is already clocked in\n";
		}
		else
		{
			std::cout << employees[choiceidx].getName() << " is on meal\n";
		}
		break;

	case '2':
		// Clock out
		if (employees[choiceidx].getStatus() != 0)
		{
			punch p{ employees[choiceidx].getID(), employees[choiceidx].getName(), "CLOCK_OUT", getTime(), employees[employeeidx].getName(), true };
			savePunch(p);
			employees[choiceidx].setTimeStatus(0);

			// Create system notification
			message = "You have a recent punch edit, select view timecard to view details";
			newNotif(notifications, 0, employees[employeeidx].getID(), employees[choiceidx].getID(), message);
			saveNotifs(employees, notifications);

			std::cout << std::endl
				<< employees[choiceidx].getName() << ", was clocked out at " << getTime() << " by " << employees[employeeidx].getName() << std::endl;
		}
		else if (employees[choiceidx].getStatus() == 0)
		{
			std::cout << employees[choiceidx].getName() << " is not clocked in\n";
		}
		else
		{
			std::cout << employees[choiceidx].getName() << " is on a meal\n";
		}
		break;

	case '3':
		// Start meal
		if (employees[choiceidx].getStatus() == 1)
		{
			punch p{ employees[choiceidx].getID(), employees[choiceidx].getName(), "START_MEAL", getTime(), employees[employeeidx].getName(), true };
			savePunch(p);
			employees[choiceidx].setTimeStatus(2);

			// Create system notification
			message = "You have a recent punch edit, select view timecard to view details";
			newNotif(notifications, 0, employees[employeeidx].getID(), employees[choiceidx].getID(), message);
			saveNotifs(employees, notifications);

			std::cout << std::endl
				<< employees[choiceidx].getName() << ", placed on a meal at " << getTime() << " by " << employees[employeeidx].getName() << std::endl;
		}
		else if (employees[choiceidx].getStatus() == 0)
		{
			std::cout << employees[choiceidx].getName() << " is not clocked in\n";
		}
		else
		{
			std::cout << employees[choiceidx].getName() << " alreay on meal\n";
		}
		break;
	case '4':
		// End meal
		if (employees[choiceidx].getStatus() == 2)
		{
			punch p{ employees[choiceidx].getID(), employees[choiceidx].getName(), "END_MEAL", getTime(), employees[employeeidx].getName(), true };
			savePunch(p);
			employees[choiceidx].setTimeStatus(1);

			// Create system notification
			message = "You have a recent punch edit, select view timecard to view details";
			newNotif(notifications, 0, employees[employeeidx].getID(), employees[choiceidx].getID(), message);
			saveNotifs(employees, notifications);

			std::cout << std::endl
				<< employees[choiceidx].getName() << ", removed from meal at " << getTime() << " by " << employees[employeeidx].getName() << std::endl;
		}
		else if (employees[choiceidx].getStatus() == 1)
		{
			std::cout << employees[choiceidx].getName() << " is not on a meal\n";
		}
		else
		{
			std::cout << employees[choiceidx].getName() << " is not clocked in or on meal\n";
		}
		break;
	}
	saveEmployees(employees);
	wait(3);
}

void settings(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx)
{
	while (true)
	{
		refresh(employeeidx, employees);

		char choice;
		std::cout << "\033[33m--SETTINGS--\033[0m\n"
			<< "5 - Change PIN\n"
			<< "0 - Return\n";
		std::cout << "->";
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (choice)
		{
			// Change pin
		case '5':
			changePin(employees, employeeidx);
			// Return
		case '0':
			return;
			// Invalid
		default:
			std::cout << "Invalid, try again\n";
			wait(2);
			continue;

		}
	}
}
