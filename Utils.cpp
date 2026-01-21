#include "Utils.h"
#include "FileIO.h"
#include "Config.h"
#include "Admin.h"
#include "Notifications.h"
#include "GlobalFunctions.h"
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>
#define NOMINMAX

// Only include for windows sytems
#ifdef _WIN32
#include "Windows.h"
#endif

// Only include for apple systems
#ifdef __APPLE__
#include <termios.h>
#include <unistd.h>
#endif

#include <iomanip>
#include <fstream>
#pragma warning(disable : 4996)

// Windows
#include "Utils.h"
#include "FileIO.h"
#include "Config.h"
#include "Admin.h"
#include "Notifications.h"
#include "GlobalFunctions.h"
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>

#define NOMINMAX

// Platform specific includes
#ifdef _WIN32
#include <Windows.h>
#else // macOS/Linux
#include <termios.h>
#include <unistd.h>
#endif

#pragma warning(disable : 4996)

// Cross platform terminal echo handling
void disableEcho() {
#ifdef _WIN32
    if (maskInput) {
        HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(h, &mode);
        SetConsoleMode(h, mode & ~ENABLE_ECHO_INPUT);
    }
#else
    if (maskInput) {
        struct termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        tty.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }
#endif
}

void enableEcho() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_ECHO_INPUT);
#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

// Cross platform terminal clearing
void clear() {
#if defined(_WIN32) || defined(_WIN64)
    system("CLS"); // Windows
#else
    system("clear"); // macOS/Linux
#endif
}

// Cross platform time handling
std::string getTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[40];
    strftime(buffer, sizeof(buffer), "%D %H:%M:%S", ltm);
    return std::string(buffer);
}

void wait(int sec) {
    std::this_thread::sleep_for(std::chrono::seconds(sec));
}

void printHeader(int& employeeidx, std::vector<employee>& employees)
{
	std::cout << std::endl;
	std::cout << "Employee Management OS " << VERSION << std::endl;
	std::cout << getTime() << std::endl;

	if (beta_mode)
	{
		std::cout << "~~BETA~~\n";
	}

	if (employeeidx > -1)
	{
		if (employees[employeeidx].getMstrStatus())
			std::cout << "**";

		std::cout << employees[employeeidx].getName();

		if (employees[employeeidx].getMstrStatus())
			std::cout << "**";

		std::cout << "\n";
	}

}
void refresh(int& employeeidx, std::vector<employee>& employees)
{
	clear();
	printHeader(employeeidx, employees);
}

int getChoiceIdx(std::vector<employee>& employees, int& employeeidx)
{
	// return -1 cancels, return -2 refreshes and asks again
	int choice;
	int last = static_cast<int>(employees.size());
	int idx;
	std::string input;
	std::cout << "Enter # (0 to cancel): ";
	std::getline(std::cin, input);

	try
	{
		choice = std::stoi(input);
	}
	catch (...)
	{
		std::cout << "Invalid\n";
		wait(2);
		return -2;
	}

	if (choice == 0)
	{
		return -1;
	}

	// Check input
	if (choice <= 0 || choice > last)
	{
		std::cout << "Invalid, choose a number between 1 and " << last << "\n";
		wait(2);
		return -2;
	}

	std::string message;
	int recipientIdx = choice - 1;

	// Prevent self select
	if (recipientIdx == employeeidx)
	{
		std::cout << "You cannot select yourself\n";
		wait(2);
		return -2;
	}

	idx = choice - 1;

	return idx;
}
bool checkIDInput(std::vector<employee>& employees, int& id)
{
	// Check size
	if (id < 1000000 || id > 9999999)
	{
		std::cout << "Your personnel # must be 7 digits" << std::endl;
		wait(2);
		return false;
	}

	for (int i = 0; i < employees.size(); i++)
	{
		if (employees[i].getID() == id)
			return true;
	}

	std::cout << "Personnel # not found" << std::endl;
	wait(2);
	return false;
}
int getID(std::vector<employee>& employees, std::vector<notification>& notifications)
{
	while (true)
	{
		int temp = -1;
		refresh(temp, employees);

		disableEcho();
		std::cout << "Enter your personnel #: ";
		int id;
		std::cin >> id;
		enableEcho();

		// Check for cin faiure
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Your ID must be numeric" << std::endl;
			wait(2);
			continue;
		}

		if (id == admin_access)
		{
			adminPanel(employees, notifications, -1);
			return -1;
		}

		if (checkIDInput(employees, id))
			return id;
	}
	return 0;
}
int setIndex(int input, std::vector<employee>& employees)
{
	for (int i = 0; i < employees.size(); i++)
		if (input == employees[i].getID())
			return i;
	return -1;
}
bool notifScreen(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx, bool loggedin)
{
	refresh(employeeidx, employees);

	int currentid = employees[employeeidx].getID();
	bool notifFound = false;
	bool remindFound = false;

	// Input variable
	char choice;

	// Lock message
	if (employees[employeeidx].getStatus() == -2)
	{
		std::cout << "\033[33mYour profile has been locked, you cannot login at this time\n"
			<< "Press ENTER to acknowledge...\033[0m";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cin.get();
		return false;
	}

	// Termination message
	if (employees[employeeidx].getStatus() == -1)
	{
		std::cout << "\033[31mYou have been terminated, upon acknowledgement, your profile will be deleted\n"
			<< "1 - Acknowledge\n"
			<< "2 - Logout\n"
			<< "-> ";
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (choice == '1')
		{
			std::cout << "Deleting your profile..\033[0m\n";
			employees.erase(employees.begin() + employeeidx);
			saveEmployees(employees);
			wait(2);
			return false;
		}
		else if (choice == '2')
		{
			std::cout << "Logging you out\033[0m\n";
			wait(2);
			return false;
		}

		std::cout << "Invalid, try again\n";
		wait(1);
	}

	// First pass: find reminders
	for (size_t i = 0; i < notifications.size(); ++i)
	{
		if (notifications[i].receiverID == currentid && notifications[i].code == 8)
		{
			remindFound = true;
			break;
		}
	}

	// Display reminders
	if (remindFound)
	{
		std::cout << "\n---- REMINDER ----\n";
		for (size_t i = 0; i < notifications.size(); ++i)
		{
			if (notifications[i].receiverID == currentid && notifications[i].code == 8)
			{
				std::cout << "\033[33m - " << notifications[i].message << "\033[0m\n";
			}
		}

		while (true)
		{
			std::cout << "1 - Acknowledge\n2 - Ignore\n->";
			std::cin >> choice;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (choice == '1')
			{
				// Erase all reminders safely
				for (int i = static_cast<int>(notifications.size()) - 1; i >= 0; --i)
				{
					if (notifications[i].receiverID == currentid && notifications[i].code == 8)
						notifications.erase(notifications.begin() + i);
				}
				saveNotifs(employees, notifications);
				break;
			}
			else if (choice == '2')
			{
				break;
			}
			else
			{
				std::cout << "Invalid, try again\n";
			}
		}
	}

	// Second pass: notifications (non-reminders)
	notifFound = false;
	for (size_t i = 0; i < notifications.size(); ++i)
	{
		if (notifications[i].receiverID == currentid && notifications[i].code != 8)
		{
			notifFound = true;
			break;
		}
	}

	// No notifications found - if called from menu display message
	if (!notifFound && !loggedin)
		return true;
	if (!notifFound && loggedin)
	{
		refresh(employeeidx, employees);
		std::cout << "\nYou have no new notifications!\n";
		wait(2);
		return true;
	}

	refresh(employeeidx, employees);

	// Display notifications independently
	for (size_t i = 0; i < notifications.size(); )
	{
		if (notifications[i].receiverID != currentid || notifications[i].code == 8)
		{
			++i;
			continue;
		}

		int senderIdx = setIndex(notifications[i].senderID, employees);

		std::cout << "\n---- NOTIFICATION ----\n";

		if (notifications[i].code == 0)
		{
			std::cout << "\033[4;37;45mSYSTEM\033[0m - "
				<< "\033[35m" << notifications[i].message << "\033[0m\n";
		}
		else if (notifications[i].code == 1)
		{
			std::cout
				<< "\033[4;30;43mMESSAGE\033[0m "
				<< "\033[37;40mfrom " << employees[senderIdx].getName() << ":\033[0m "
				<< "\033[33;40m" << notifications[i].message << "\033[0m\n";
		}
		else if (notifications[i].code == 9)
		{
			std::cout
				<< "\033[4;30;46mAnnouncement\033[0m "
				<< "\033[37;40mfrom " << employees[senderIdx].getName() << ":\033[0m "
				<< "\033[36;40m" << notifications[i].message << "\033[0m\n";
		}

		while (true)
		{
			std::cout << "\n1 - Acknowledge\n"
				<< "2 - Ignore\n";
			// Allow reply for message notifications
			if (notifications[i].code == 1)
				std::cout << "3 - Reply\n";

			std::cout << "->";
			std::cin >> choice;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			// Acknowledge
			if (choice == '1')
			{
				notifications.erase(notifications.begin() + i);
				saveNotifs(employees, notifications);
				break; // do not increment i, next notification shifts into i
			}
			// Ignore
			else if (choice == '2')
			{
				++i; // move to next notification
				break;
			}
			// Reply (only accessible for messages)
			else if (choice == '3' && notifications[i].code == 1)
			{
				std::string initmessage;
				std::string message;
				std::cout << "Enter reply for " << employees[senderIdx].getName() << ":";
				std::getline(std::cin, initmessage);

				message = "Reply -> " + initmessage;

				newNotif(notifications, 1, employees[employeeidx].getID(), employees[senderIdx].getID(), message);
				notifications.erase(notifications.begin() + i);  // do not increment i
				saveNotifs(employees, notifications);
				break;
			}
			else
			{
				std::cout << "Invalid, try again\n";
			}
		}

		refresh(employeeidx, employees);
	}

	return true;
}


// Settings util functions
void changePin(std::vector<employee>& employees, int& employeeidx)
{
	refresh(employeeidx, employees);

	std::cout << "Your current pin is: " << employees[employeeidx].getMgrPin() << std::endl;
	int newPin;

	while (true)
	{
		std::cout << "\nEnter new 4 digit pin (0 to cancel): ";
		std::cin >> newPin;

		if (newPin == 0)
		{
			return;
		}

		if (newPin < 999 || newPin > 9999)
		{
			std::cout << "Your pin must be 4 digits\n";
			continue;
		}
		break;
	}
	employees[employeeidx].setPin(newPin);
	saveEmployees(employees);
}

// MGR util functions
bool verifyPin(std::vector<employee>& employees, int& employeeidx)
{
	int pin;
	while (true)
	{
		std::cout << "Enter manager pin (0 to cancel): ";
		disableEcho();
		std::cin >> pin;
		enableEcho();

		// Check numeric
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Your manager pin must be numeric" << std::endl;
			continue;
		}
		// Check size
		if ((pin < 1000 || pin > 9999) && pin != 0)
		{
			std::cout << "Your pin must be 4 digits" << std::endl;
			continue;
		}
		if (pin == 0)
		{
			return false;
		}
		// Check match
		if (employees[employeeidx].getMgrPin() == pin)
		{
			return true;
		}
		else
		{
			std::cout << "Incorrect, returning to main menu" << std::endl;
			wait(2);
			return false;
		}
	}
}
void displayEmployees(std::vector<employee>& employees, int& employeeidx, bool id, bool name, bool pay, bool status, bool punchstatus, bool identifiers)
{
	std::cout << "\nEMPLOYEES:\n";

	for (size_t i = 0; i < employees.size(); i++)
	{
		std::cout << std::left;

		if (identifiers)
		{
			int ident = i + 1;
			std::cout << ident << " - ";
		}

		// ID
		if (id)
		{
			if (!employees[employeeidx].getMstrStatus() &&
				employees[i].getMgrStatus() &&
				employees[i].getID() != employees[employeeidx].getID())
			{
				std::cout << std::setw(9) << "*******";
			}
			else
			{
				std::cout << std::setw(9) << employees[i].getID();
			}
		}
		// Name
		if (name)
		{
			std::cout << std::setw(20) << employees[i].getName();
		}
		// Pay
		if (pay)
		{
			std::ostringstream payStr;

			if (!employees[employeeidx].getMstrStatus() &&
				employees[i].getMstrStatus())
			{
				payStr << "$****";
			}
			else
			{
				payStr << "$"
					<< std::fixed << std::setprecision(2)
					<< employees[i].getPay();
			}

			std::cout << std::setw(12) << payStr.str();
		}
		// Status
		if (status)
		{
			std::string statusStr;

			if (employees[i].getStatus() == -1)
			{
				statusStr = "TERMINATED";
			}
			else if (employees[i].getStatus() == -2)
			{
				statusStr = "LOCKED";
			}
			else if (employees[i].getMgrStatus())
			{
				statusStr = employees[i].getMstrStatus() ? "MGR*" : "MGR";
			}
			else
			{
				statusStr = "";
			}

			std::cout << std::setw(10) << statusStr;
		}
		// Punch status
		if (punchstatus)
		{
			std::string punchStr;

			if (employees[i].getStatus() == 1)
				punchStr = "ON CLOCK";
			else if (employees[i].getStatus() == 2)
				punchStr = "ON MEAL";
			else
				punchStr = "\0";

			std::cout << std::setw(10) << punchStr;
		}

		std::cout << '\n';
	}
}
void addEmployee(std::vector<employee>& employees, int& employeeidx)
{
	std::string name;
	int id;
	double pay;
	int mgrInput;
	bool isManager = false;
	int mgrpin = 0;
	int mstInput;
	bool isMaster;

	// Name
	std::cout << "Enter name: ";
	std::getline(std::cin, name);

	// ID
	while (true)
	{
		std::cout << "Enter personnel #: ";
		std::cin >> id;

		if (std::cin.fail() || id < 1000000 || id > 9999999)
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "ID must be a 7-digit number\n";
			continue;
		}

		// Check duplicate ID
		bool exists = false;
		for (int i = 0; i < employees.size(); i++)
		{
			if (employees[i].getID() == id)
			{
				std::cout << "ID already exists" << std::endl;
				exists = true;
				break;
			}
		}

		if (!exists)
			break;
	}

	// Pay
	while (true)
	{
		std::cout << "Enter pay: ";
		std::cin >> pay;

		if (std::cin.fail() || pay < 0)
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Pay must be a positive number\n";
			continue;
		}
		break;
	}

	// Manager status (only accessible to employees with master access)
	if (employees[employeeidx].getMgrStatus())
	{
		while (true)
		{
			std::cout << "Enter 0 for associate or 1 for manager: ";
			std::cin >> mgrInput;

			if (std::cin.fail() || (mgrInput != 0 && mgrInput != 1))
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Enter only 0 or 1\n";
				continue;
			}

			isManager = (mgrInput == 1);
			break;
		}
	}

	// Master status (only accessible to employees with master access)
	if (employees[employeeidx].getMstrStatus() && isManager)
	{
		while (true)
		{
			std::cout << "Enter 0 to continue or 1 to grant master access: ";
			std::cin >> mstInput;

			if (std::cin.fail() || (mstInput != 0 && mstInput != 1))
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Enter only 0 or 1\n";
				continue;
			}

			isMaster = (mstInput == 1);
			break;
		}
	}
	else
	{
		isMaster = false;
	}

	// Manager pin (only if manager)
	if (isManager)
	{
		while (true)
		{
			std::cout << "Enter 4-digit manager pin: ";
			std::cin >> mgrpin;

			if (std::cin.fail() || mgrpin < 1000 || mgrpin > 9999)
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Pin must be 4 digits\n";
				continue;
			}
			break;
		}
	}

	employees.push_back(employee(name, id, pay, isManager, mgrpin, isMaster, 0));
	saveEmployees(employees);

	std::cout << "\nEmployee added successfully.\n";
	wait(2);
}
void terminateEmployee(std::vector<employee>& employees, int& employeeidx, int& choiceidx)
{
	// Check permission
	if (employees[choiceidx].getMgrStatus() && !employees[employeeidx].getMstrStatus())
	{
		std::cout << "You don't have permission to terminate this employee\n";
		wait(2);
		return;
	}
	// Already terminated
	if (employees[choiceidx].getStatus() == -1)
	{
		std::cout << employees[choiceidx].getName() << " has already been terminated\n";
		wait(2);
		return;
	}
	// Terminate
	else
	{
		std::cout << "\n\033[31mTermination message has been sent to " << employees[choiceidx].getName() << ". Upon acknowledgemt, their user profile will be deleted\033[0m\n";

		// Force off clock
		if (employees[choiceidx].getStatus() != 0)
		{
			punch p{ employees[choiceidx].getID(), employees[choiceidx].getName(), "CLOCK_OUT", getTime(), employees[employeeidx].getName(), true };
			savePunch(p);
		}

		employees[choiceidx].setTimeStatus(-1);
		saveEmployees(employees);
		wait(6);
		return;
	}

}
void lockEmployee(std::vector <employee>& employees, int& employeeidx, bool lock, int& choiceidx)
{
	int id;
	int idx = choiceidx;

	////LOCK
	if (lock)
	{
		// Check permission
		if (lock && employees[choiceidx].getMgrStatus() && !employees[employeeidx].getMstrStatus())
		{
			std::cout << "You do not have permission to lock this employee\n";
			wait(2);
			return;
		}
		// Send lock notif to user's profile (prevent lock if employee has been terminated)
		if (lock && employees[idx].getStatus() != -2 && employees[idx].getStatus() != -1)
		{
			std::cout << "\n\033[33mLock notification has been sent to " << employees[idx].getName() << "\033[0m\n";

			// Force off clock
			if (employees[choiceidx].getStatus() != 0)
			{
				punch p{ employees[choiceidx].getID(), employees[choiceidx].getName(), "CLOCK_OUT", getTime(), employees[employeeidx].getName(), true };
				savePunch(p);
			}

			employees[idx].setTimeStatus(-2);
			saveEmployees(employees);
			wait(4);
			return;
		}
		// Cannot double lock
		else if (employees[idx].getStatus() == -2)
		{
			std::cout << employees[idx].getName() << " is already locked\n";
			wait(2);
			return;
		}
		// Cannot lock terminated employee
		else if (employees[idx].getStatus() == -1)
		{
			std::cout << employees[idx].getName() << " is awaiting termination and cannot be locked\n";
			wait(2);
			return;
		}
	}
	////UNLOCK
	else
	{
		// Check permission
		if (!lock && employees[choiceidx].getMgrStatus() && !employees[employeeidx].getMstrStatus())
		{
			std::cout << "You do not have permission to unlock this employee\n";
			wait(2);
			return;
		}
		if (employees[idx].getStatus() == -2)
		{
			std::cout << employees[idx].getName() << " has been unlocked\n";
			employees[idx].setTimeStatus(0);
			saveEmployees(employees);
			wait(2);
			return;
		}
		// Prevent unlock if awaiting termination
		else if (employees[idx].getStatus() == -1)
		{
			std::cout << employees[idx].getName() << " is awaiting termination and cannot be unlocked\n";
			wait(2);
			return;
		}
		// Not locked
		else
		{
			std::cout << employees[idx].getName() << " is not locked\n";
			wait(2);
			return;
		}
	}
}
void changePay(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx, int& choiceidx)
{
	if (employees[choiceidx].getMgrStatus() && !employees[employeeidx].getMstrStatus())
	{
		std::cout << "You do not have permission to change this employee's pay\n";
		wait(2);
		return;
	}

	double newPay;
	std::cout << "Enter new pay: ";
	std::cin >> newPay;

	if (std::cin.fail() || newPay < 0)
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Pay must be a positive number.\n";
		wait(2);
		return;
	}

	std::cout << "\nPay updated: "
		<< employees[choiceidx].getName()
		<< " ($" << std::fixed << std::setprecision(2)
		<< employees[choiceidx].getPay()
		<< " to $" << newPay << ")\n";

	// Create system notification
	std::stringstream ss;
	ss << "Your pay has been changed from $"
		<< std::fixed << std::setprecision(2) << employees[choiceidx].getPay()
		<< " to $" << std::fixed << std::setprecision(2) << newPay;

	std::string message = ss.str();
	newNotif(notifications, 0, employees[employeeidx].getID(), employees[choiceidx].getID(), message);
	saveNotifs(employees, notifications);

	employees[choiceidx].setPay(newPay);
	saveEmployees(employees);
	wait(3);
}
int statusChangeCheck(std::vector<employee>& employees, int& employeeidx, int errMessage)
{
	int id;
	int idx = -1;
	std::cout << "Enter personnel # (0 to cancel): ";
	std::cin >> id;

	// Check id
	if ((std::cin.fail() || id < 1000000 || id > 9999999) && id != 0)
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "ID must be a 7-digit number\n";
		wait(2);
		return -1;
	}

	if (id == 0)
		return -1;

	// Status change error message
	if (id == employees[employeeidx].getID() && errMessage == 0)
	{
		std::cout << "You cannot change your own status\n";
		wait(2);
		return -1;
	}
	// Time change error message
	if (id == employees[employeeidx].getID() && errMessage == 1)
	{
		std::cout << "You must use the main menu to edit your own punch\n";
		wait(2);
		return -1;
	}

	// Find employee
	for (int i = 0; i < employees.size(); i++)
	{
		if (employees[i].getID() == id)
		{
			idx = i;
			break;
		}
	}

	if (idx == -1)
	{
		std::cout << "Personnel # not found\n";
		wait(2);
		return -1;
		;
	}

	// Cannot change master access status without having master access
	if (employees[idx].getMgrStatus() && !employees[employeeidx].getMstrStatus())
	{
		std::cout << "Personnel # not found\n";
		wait(2);
		return -1;
	}

	return idx;
}
void createMgrPin(std::vector<employee>& employees, int& employeeidx, int idx)
{
	int pin;
	// Create manager pin if one doesn't already exist
	if (employees[idx].getMgrPin() == 0)
	{
		while (true)
		{
			std::cout << "\nCreate manager pin: ";
			std::cin >> pin;
			// Check pin
			if (std::cin.fail() || pin < 1000 || pin > 9999)
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Pin must be a 4-digit number\n";
				continue;
			}
			employees[idx].setPin(pin);
			break;
		}
	}
}
void changeStatus(std::vector<employee>& employees, std::vector<notification>& notifications, int& employeeidx, int& choiceidx)
{
	char choice;

	// Prevent status change to managers if non master status
	if (employees[choiceidx].getMgrStatus() && !employees[employeeidx].getMstrStatus())
	{
		std::cout << "You do not have permission to change this employee's status\n";
		wait(2);
		return;
	}

	std::cout << "\nWould you like to:\n"
		<< "1 - Promote to manager\n";
	// Only show master status option for master status employees
	if (employees[employeeidx].getMstrStatus())
	{
		std::cout << "2 - Demote to associate\n"
			<< "3 - Grant master access\n"
			<< "4 - Remove master access\n";
	}
	std::cout << "0 - Return\n";
	while (true)
	{
		std::cout << "-> ";
		std::cin >> choice;
		std::string message;
		switch (choice)
		{
			// Promote to manager (Manager OR Master)
		case '1':
			if (employees[choiceidx].getMgrStatus())
			{
				std::cout << "\nEmployee is already a manager\n";
				wait(2);
				return;
			}

			createMgrPin(employees, employeeidx, choiceidx);

			employees[choiceidx].setPermissions(0);
			std::cout << "\nEmployee promoted to manager\n";
			// Create system notification
			message = "You have been promoted to manager";
			newNotif(notifications, 0, employees[employeeidx].getID(), employees[choiceidx].getID(), message);
			saveNotifs(employees, notifications);

			saveEmployees(employees);
			wait(2);
			return;
			break;

			// Demote to associate (MASTER ONLY)
		case '2':
			if (!employees[employeeidx].getMstrStatus())
			{
				std::cout << "Invalid choice.\n";
				wait(2);
				continue;
			}

			if (!employees[choiceidx].getMgrStatus())
			{
				std::cout << "\nThis employee is already an associate\n";
				wait(2);
				return;
			}

			employees[choiceidx].setPermissions(-1);
			std::cout << "\nEmployee demoted to associate.\n";
			employees[choiceidx].setPin(0);

			// Create system notification
			message = "You have been demoted to associate";
			newNotif(notifications, 0, employees[employeeidx].getID(), employees[choiceidx].getID(), message);
			saveNotifs(employees, notifications);

			saveEmployees(employees);
			wait(2);
			return;
			break;

			// Grant master (MASTER ONLY)
		case '3':
			if (!employees[employeeidx].getMstrStatus())
			{
				std::cout << "Invalid choice.\n";
				wait(2);
				continue;
			}

			if (employees[choiceidx].getMstrStatus())
			{
				std::cout << "\nEmployee already has master access\n";
				wait(2);
				return;
			}
			// Create manager pin if one does not already exist
			createMgrPin(employees, employeeidx, choiceidx);

			employees[choiceidx].setPermissions(1);
			std::cout << "\nMaster access granted.\n";
			// Create system notification
			message = "You have been granted master access";
			newNotif(notifications, 0, employees[choiceidx].getID(), employees[choiceidx].getID(), message);
			saveNotifs(employees, notifications);

			saveEmployees(employees);
			wait(2);
			return;
			break;

			// Remove master access (MASTER ONLY)
		case '4':
			if (!employees[employeeidx].getMstrStatus())
			{
				std::cout << "Invalid choice.\n";
				wait(2);
				continue;
			}

			if (!employees[choiceidx].getMstrStatus())
			{
				std::cout << "\nThis employee does not have master access\n";
				wait(2);
				return;
			}
			std::cout << "\nEmployee no longer has master access\n";
			employees[choiceidx].setPermissions(0);

			// Create system notification
			message = "You no longer have master access";
			newNotif(notifications, 0, employees[employeeidx].getID(), employees[choiceidx].getID(), message);
			saveNotifs(employees, notifications);

			saveEmployees(employees);
			wait(2);
			return;
			break;

			// Return
		case '0':
			return;
			break;

		default:
			std::cout << "Invalid choice.\n";
			continue;
			break;
		}
	}
}

