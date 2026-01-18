#include "Admin.h"
#include "Config.h"
#include "FileIO.h"
#include "Utils.h"
#include "Employees.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <filesystem>

/*----------COMMAND-LIST----------
* config - Enter config menu
* beta false - Disable beta
* beta true - Enable beta
* maskinp false - Disable input masking
* maskinp true - Enable input masking
* setadmin - Set new admin password
* setpin - Set default manager pin
* block - Disable access during beta
* unblock - Enable access during beta
*
* sf - Show txt files
* rst - Restart
* help - Show all commands
*
* batch (b/) - Batch command preface
* b/ pay - Batch change pay
* b/ status - Batch change status
*
* set (s/) - Set command preface
* s/ status - Set status
* s/ pay - Set pay
* s/ pin - Set pin
* s/ id - Set id
* s/ name - Set name
*/

// Utility
void printFile(const std::filesystem::path& filename)
{
	std::ifstream file(filename);
	std::string line;

	std::cout << "==== " << filename.filename().string() << " ====\n";

	if (!file.is_open())
	{
		std::cout << "Could not open file.\n\n";
		return;
	}

	while (std::getline(file, line))
		std::cout << line << '\n';

	std::cout << "\n";
}
int getSetIdx(std::vector<employee>& employees)
{
	// Get set ID
	int id;
	int idx;
	while (true)
	{
		std::cout << "Enter ID (0 to cancel): ";
		std::cin >> id;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Invalid type
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "ID must be numeric" << std::endl;
			continue;
		}

		// Cancel
		if (id == 0)
		{
			return -1;
		}

		// Get index of ID
		idx = setIndex(id, employees);

		// Invalid
		if (idx == -1)
		{
			std::cout << "Personnel # not found, try again\n";
			continue;
		}

		// Valid
		else
		{
			return idx;
		}
	}
}

// Command functions
void showFiles()
{
	std::filesystem::path empFile = DATA_DIR / (beta_mode ? "BETA_employees.txt" : "employees.txt");
	std::filesystem::path punchFile = DATA_DIR / (beta_mode ? "BETA_punchRecords.txt" : "punchRecords.txt");
	std::filesystem::path notifFile = DATA_DIR / (beta_mode ? "BETA_notifications.txt" : "notifications.txt");
	std::filesystem::path configFile = DATA_DIR / "config.txt";

	std::cout << "\n";
	printFile(configFile);
	printFile(empFile);
	printFile(punchFile);
	printFile(notifFile);

	std::cout << "Press ENTER to return...";
	std::cin.get();
}

void help()
{
	// Tips
	std::cout << "\nCommands ARE case sensitive\n"
		<< "";
	// CONFIG
	std::cout << "\n\033[33mCONFIGURATION:\033[0m\n"
		<< "config - Enter configuration menu\n"
		<< "beta (true/false) - Toggle beta\n"
		<< "maskinp (true/false) - Toggle input masking\n"
		<< "setadmin - Set new admin password\n"
		<< "setpin - Set default manager pin\n"
		<< "block - Block system access\n"
		<< "unblock - Unblock system access\n"
		<< "help - Show all commands\n";

	// UTILITY
	std::cout << "\n\033[33mUTILITY:\033[0m\n"
		<< "sf - Show flies\n"
		<< "rst - Restart program\n";

	// BATCH
	std::cout << "\n\033[33mBATCH:\033[0m\n"
		<< "batch (b/) - Batch command preface\n"
		<< "b/ pay - Batch change pay\n"
		<< "b/ status - Batch change status\n";

	// SET
	std::cout << "\n\033[33mSET:\033[0m\n"
		<< "set status - Change status of individual employee\n"
		<< "set pay - Change pay of individual employee\n"
		<< "set pin - Change pin of individual employee\n"
		<< "set id - Change personnel # of individual employee\n"
		<< "set name - Change name of individual employee\n";

	std::cout << "\nPress ENTER to return...";
	std::cin.get();
}
void setAdminCode()
{
	std::cout << "Enter new admin password (0 to cancel): ";
	std::string code;
	getline(std::cin, code);
	if (code == "0")
	{
		std::cout << "Cancelling...\n";
		return;
	}

	admin_password = code;
	saveConfig();
	std::cout << "Admin password updated.\n";
}

// Batch commands
void batchPay(std::vector<employee>& employees, std::vector<notification>& notifications)
{
	std::cout << "Enter new pay rate for all employees: ";
	double newPay;
	std::cin >> newPay;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Prepare system notification variables
	std::stringstream ss;
	std::string message;

	for (int i = 0; i < employees.size(); i++)
	{
		message = '\n';
		ss.str("");
		// Create system message
		ss << "Your pay has been changed from $"
			<< std::fixed << std::setprecision(2) << employees[i].getPay()
			<< " to $" << std::fixed << std::setprecision(2) << newPay;

		message = ss.str();

		// Set new pay and create notification
		employees[i].setPay(newPay);
		newNotif(notifications, 0, 0, employees[i].getID(), message);
	}
	saveEmployees(employees);
	saveNotifs(employees, notifications);
}
void batchStatus(std::vector<employee>& employees, std::vector<notification>& notifications)
{
	bool mgr = false;
	bool mstr = false;
	bool assc = true;
	bool exclusion = true;
	bool pinset = false;

	int exclusionID;
	int exclIdx;
	char choice;

	std::string statusMessage;

	std::filesystem::path empFile = DATA_DIR / (beta_mode ? "BETA_employees.txt" : "employees.txt");
	printFile(empFile);

	// Get manager choice
	while (true)
	{
		std::cout << "Mgr (y/n): ";
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (choice == 'y' || choice == '1')
		{
			mgr = true;
			break;
		}
		else if (choice == 'n' || choice == '0')
		{
			mgr = false;
			break;
		}
		else
		{
			std::cout << "Unknown, try again...\n";
			continue;
		}
	}

	// Get master choice
	if (mgr)
	{
		while (true)
		{
			std::cout << "Mstr (y/n): ";
			std::cin >> choice;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (choice == 'y' || choice == '1')
			{
				mstr = true;
				break;
			}
			else if (choice == 'n' || choice == '0')
			{
				mstr = false;
				break;
			}
			else
			{
				std::cout << "Unknown, try again...\n";
				continue;
			}
		}
	}

	// Ask for exclusion to batch edit
	std::cout << "Exclusion (y/n) ";
	while (true)
	{
		std::cout << "-> ";
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (choice == 'y' || choice == '1')
		{
			std::cout << "Enter exclusion ID: ";
			std::cin >> exclusionID;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			exclIdx = setIndex(exclusionID, employees);
			exclusion = true;
			break;
		}
		else if (choice == 'n' || choice == '0')
		{
			exclusion = false;
			break;
		}
		else
		{
			std::cout << "Unknown, try again\n";
			continue;
		}
	}

	// Batch edit
	for (int i = 0; i < employees.size(); i++)
	{
		// Skip exclusion
		if (exclusion && exclIdx == i)
			continue;

		// Handle pin
		if (mgr && employees[i].getMgrPin() == 0)
		{
			employees[i].setPin(mgrPin);
			pinset = true;
		}

		// Set manager
		if (mgr && !mstr)
		{
			employees[i].setPermissions(0);
			statusMessage = "Your status has been changed to manager.";
		}

		// Set master
		else if (mgr && mstr)
		{
			employees[i].setPermissions(1);
			statusMessage = "Your status has been changed to manager *(MASTER ACCESS)*.";
		}


		// Set associate
		else
		{
			employees[i].setPermissions(2);
			employees[i].setPin(0);
			statusMessage = "Your status has been changed to associate.";
		}

		// Create notification
		newNotif(notifications, 0, 0, employees[i].getID(), statusMessage);
	}
	if (pinset)
	{
		std::cout << "One or more users has been converted to manager without an existing pin. The default pin is : " << mgrPin << "\n";
		wait(6);
	}
	saveEmployees(employees);
	saveNotifs(employees, notifications);
}

// Set commands
void setPay(std::vector<employee>& employees, std::vector<notification>& notifications, int& idx)
{
	// Prepare system notification variables
	std::stringstream ss;
	std::string message;

	double newPay;
	std::cout << "Enter new pay: $";
	std::cin >> newPay;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	std::cout << employees[idx].getName() << "'s pay changed from $" << employees[idx].getPay() << " to $" << newPay << std::endl;

	// Create notification
	message = '\n';
	ss.str("");
	// Create system message
	ss << "Your pay has been changed from $"
		<< std::fixed << std::setprecision(2) << employees[idx].getPay()
		<< " to $" << std::fixed << std::setprecision(2) << newPay;

	message = ss.str();
	newNotif(notifications, 0, 0, employees[idx].getID(), message);

	employees[idx].setPay(newPay);
	saveEmployees(employees);
	saveNotifs(employees, notifications);
}
void setStatus(std::vector<employee>& employees, std::vector<notification>& notifications, int& idx)
{
	bool mgr = false;
	bool mstr = false;
	bool assc = true;
	bool pinset = false;

	std::string statusMessage;

	char choice;

	// Get manager choice
	while (true)
	{
		std::cout << "Mgr (y/n): ";
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (choice == 'y' || choice == '1')
		{
			mgr = true;
			break;
		}
		else if (choice == 'n' || choice == '0')
		{
			mgr = false;
			break;
		}
		else
		{
			std::cout << "Unknown, try again...\n";
			continue;
		}
	}

	// Get master choice
	if (mgr)
	{
		while (true)
		{
			std::cout << "Mstr (y/n): ";
			std::cin >> choice;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (choice == 'y' || choice == '1')
			{
				mstr = true;
				break;
			}
			else if (choice == 'n' || choice == '0')
			{
				mstr = false;
				break;
			}
			else
			{
				std::cout << "Unknown, try again...\n";
				continue;
			}
		}
	}

	// Handle pin
	if (mgr && employees[idx].getMgrPin() == 0)
	{
		employees[idx].setPin(mgrPin);
		pinset = true;
	}

	// Set manager
	if (mgr && !mstr)
	{
		employees[idx].setPermissions(0);
		statusMessage = "Your status has been changed to manager.";
	}

	// Set master
	else if (mgr && mstr)
	{
		employees[idx].setPermissions(1);
		statusMessage = "Your status has been changed to manager *(MASTER ACCESS)*.";
	}

	// Set associate
	else
	{
		employees[idx].setPermissions(2);
		employees[idx].setPin(0);
		statusMessage = "Your status has been changed to associate.";
	}

	// Create notification
	newNotif(notifications, 0, 0, employees[idx].getID(), statusMessage);

	if (pinset)
	{
		std::cout << employees[idx].getName() << " has been converted to manager wihtout an existing pin. The default pin is : " << mgrPin << "\n";
		wait(5);
	}
	saveNotifs(employees, notifications);
	saveEmployees(employees);
}
void setPin(std::vector<employee>& employees, std::vector<notification>& notifications, int& idx)
{
	int pin;
	std::string message;

	// Do not allow pin set for non managers
	if (!employees[idx].getMgrStatus())
	{
		std::cout << "This employee must be a manager to have a pin\n";
		wait(2);
		return;
	}

	while (true)
	{
		std::cout << "Enter new pin: ";
		std::cin >> pin;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Invalid type
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Pin must be numeric" << std::endl;
			continue;
		}

		// Invalid size
		if (pin < 1000 || pin > 9999)
		{
			std::cout << "Pin must be 4 digits and positive\n";
			continue;
		}
		// Valid
		else
		{
			break;
		}
	}
	employees[idx].setPin(pin);

	// Create system notification
	message = "Your manager pin was changed.";
	newNotif(notifications, 0, 0, employees[idx].getID(), message);

	std::cout << "Pin change complete\n";

	saveEmployees(employees);
	saveNotifs(employees, notifications);
}
void setID(std::vector<employee>& employees, std::vector<notification>& notifications, int& idx)
{
	int id;
	std::string message;

	while (true)
	{
		std::cout << "Enter new 7-digit ID (0 to cancel): ";
		std::cin >> id;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Invalid type
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Personnel # must be numeric" << std::endl;
			continue;
		}

		// Cancel
		if (id == 0)
		{
			return;
		}

		// Invalid size
		if (id < 1000000 || id > 9999999)
		{
			std::cout << "Personnel # must be 7 digits and positive\n";
			continue;
		}
		// Valid
		else
		{
			break;
		}
	}

	message = "Your ID has been changed\n";

	employees[idx].setID(id);
	newNotif(notifications, 0, 0, employees[idx].getID(), message);

	saveNotifs(employees, notifications);
	saveEmployees(employees);
}
void setName(std::vector<employee>& employees, std::vector<notification>& notifications, int& idx)
{
	std::string name;

	std::cout << "Enter new name: ";
	getline(std::cin, name);

	employees[idx].setName(name);

	// Create system notification
	std::string message = "Your user name has been changed\n";
	newNotif(notifications, 0, 0, employees[idx].getID(), message);

	saveNotifs(employees, notifications);
	saveEmployees(employees);
}

// Handle commands
void set(std::vector<employee>& employees, std::vector<notification>& notifications)
{
	std::string cmd;
	int idx;

	std::filesystem::path empFile = DATA_DIR / (beta_mode ? "BETA_employees.txt" : "employees.txt");
	printFile(empFile);
	std::cout << "SET (0 to cancel) -> ";
	getline(std::cin, cmd);

	// Set Pay
	if (cmd == "pay")
	{
		idx = getSetIdx(employees);
		// Cancel
		if (idx == -1) { return; }
		setPay(employees, notifications, idx);
		wait(2);
		return;
	}
	// Set status
	if (cmd == "status")
	{
		idx = getSetIdx(employees);
		// Cancel
		if (idx == -1) { return; }
		setStatus(employees, notifications, idx);
		wait(2);
		return;
	}
	// Set pin
	if (cmd == "pin")
	{
		idx = getSetIdx(employees);
		// Cancel
		if (idx == -1) { return; }
		setPin(employees, notifications, idx);
		wait(2);
		return;
	}
	// Set ID
	if (cmd == "id" || cmd == "ID")
	{
		idx = getSetIdx(employees);
		// Cancel
		if (idx == -1) { return; }
		setID(employees, notifications, idx);
		std::cout << "ID change complete\n";
		wait(2);
		return;
	}
	// Set name
	if (cmd == "name")
	{
		idx = getSetIdx(employees);
		// Cancel
		if (idx == -1) { return; }
		setName(employees, notifications, idx);
		std::cout << "Name change complete\n";
		wait(2);
		return;
	}
}
void batch(std::vector<employee>& employees, std::vector<notification>& notifications)
{
	std::cout << "BATCH (0 to cancel) -> ";

	std::string cmd;
	getline(std::cin, cmd);

	// Batch pay
	if (cmd == "pay")
	{
		batchPay(employees, notifications);
		std::cout << "All employee pay rates updated.\n";
		wait(2);
		return;
	}
	// Batch status
	else if (cmd == "status")
	{
		batchStatus(employees, notifications);
		saveEmployees(employees);
		std::cout << "Batch status change success\n";
		wait(2);
		return;
	}
	// Cancel
	else if (cmd == "0")
	{
		return;
	}
	else
	{
		std::cout << "Unknown batch command.\n";
		wait(2);
	}
}
bool adminCommand(const std::string& command, std::vector<employee>& employees, std::vector<notification>& notifications, int idx)
{
	// Beta commands
	if (command == "beta true")
	{
		if (!beta_mode)
		{
			beta_mode = true;
			std::cout << "Enabling beta....restarting\n";
			saveConfig();
			wait(2);
			return true;
		}
		else
		{
			std::cout << "Beta mode is already enabled.\n";
			wait(2);
			return false;
		}
	}
	else if (command == "beta false")
	{
		if (beta_mode)
		{
			beta_mode = false;
			std::cout << "Disabling beta....restarting\n";
			saveConfig();
			wait(2);
			return true;
		}
		else
		{
			std::cout << "Beta mode is not currently enabled.\n";
			wait(2);
			return false;
		}
	}

	// Show files
	else if (command == "sf") // Show files
	{
		showFiles();
		return false;
	}

	// Restart
	else if (command == "rst")
	{
		std::cout << "Restarting, please wait...\n";
		wait(3);
		return true;
	}

	// Input masking
	else if (command == "maskinp true")
	{
		if (!maskInput)
		{
			maskInput = true;
			std::cout << "Input masking enabled...restarting\n";
			saveConfig();
			wait(2);
			return true;
		}
		else
		{
			std::cout << "Input masking is already enabled\n";
			wait(2);
			return false;
		}
	}
	else if (command == "maskinp false")
	{
		if (maskInput)
		{
			maskInput = false;
			std::cout << "Input masking disabled...restarting\n";
			saveConfig();
			wait(2);
			return true;
		}
		else
		{
			std::cout << "Input masking is already disabled\n";
			wait(2);
			return false;
		}
	}

	// Default mgr pin
	else if (command == "setpin")
	{
		int pin;
		std::cout << "Enter default MGR pin: ";
		std::cin >> pin;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (pin < 999 || pin > 9999)
		{
			std::cout << "Invalid pin.";
			wait(2);
			return false;
		}

		mgrPin = pin;
		std::cout << "Default pin changed\n";
		saveConfig();
		wait(2);
		return false;
	}

	else if (command == "config")
	{
		configMenu();
		wait(2);
		return true;
	}
	// Block system
	else if (command == "block")
	{
		if (!block_access)
		{
			std::cout << "System block enabled. Enter admin access to bypass.\n";
			block_access = true;
			saveConfig();
			wait(3);
			return true;
		}
		else
		{
			std::cout << "System is already blocked\n";
			wait(2);
			return false;
		}
	}
	else if (command == "unblock")
	{
		if (block_access)
		{
			std::cout << "System block disabled.\n";
			block_access = false;
			saveConfig();
			wait(2);
			return true;
		}
		else
		{
			std::cout << "System is not currently blocked\n";
			wait(2);
			return false;
		}
	}

	// Help
	else if (command == "help")
	{
		help();
		return false;
	}

	// Set admin code
	else if (command == "setadmin")
	{
		setAdminCode();
		wait(2);
		return false;
	}

	// Call batch
	else if (command == "batch" || command == "b/")
	{
		batch(employees, notifications);
		return false;
	}
	// Call set
	else if (command == "set" || command == "s/")
	{
		set(employees, notifications);
		return false;
	}
	// Unknown
	else
	{
		std::cout << "Unknown command.\n";
		wait(2);
		return false;
	}
}

// Main panel
void adminPanel(std::vector<employee>& employees, std::vector<notification>& notifications, int idx)
{
	std::string pin;

	std::cout << "Enter administrator password (0 to cancel): ";
	disableEcho();
	std::cin >> pin;
	enableEcho();

	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Incorrect\n";
		wait(2);
		return;
	}

	if (pin == "0")
		return;

	if (pin != admin_password)
	{
		std::cout << "Incorrect\n";
		wait(2);
		return;
	}

	// Clear leftover newline
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// ADMIN MODE
	std::string command;
	while (true)
	{
		refresh(idx, employees);
		std::cout << "\033[1;33;45m\n--ADMIN PANEL--\033[0m\n\n";

		std::cout << "Enter command (0 to return): ";
		std::getline(std::cin, command);

		if (command == "0")
			return;

		// Admin command returns true to restart
		if (adminCommand(command, employees, notifications, idx))
			return;
	}
}
