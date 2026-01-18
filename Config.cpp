#include "Config.h"
#include "Utils.h"
#include "Admin.h"

#include <iostream>
#include <fstream>
#include <filesystem>

// CURRENT VERSION
const std::string VERSION = "1.0.0";

//================DEFAULT CONFIGURATION================//
bool defBeta = false;
int defAdminAccess = 9;
std::string defAdminPassword = "admin";
bool defMaskInput = false;
int defMgrPin = 1111;
bool defBlockAccess = false;
//====================================================//

// Set path for project txt source files
std::filesystem::path DATA_DIR;

// Configuration variables
int mgrPin;
bool beta_mode;
int admin_access;
std::string admin_password;
bool maskInput;
bool block_access;



void configMenu()
{
	clear();
	std::cout << "\033[31mCONFIGURATION MODE\033[0m\n";

	char choice;
	std::string password;

	std::cout << "1 - Enter config menu\n"
		<< "2 - Apply default settings\n"
		<< "-> ";
	while (true)
	{
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Manual config
		if (choice == '1')
		{
			char configset;
			int numset;
			std::string password;

			// Set beta
			while (true)
			{
				std::cout << "Beta (y/n): ";
				std::cin >> configset;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (configset == 'y' || configset == '1') { beta_mode = true; configset = '\0'; break; }
				else if (configset == 'n' || configset == '0') { beta_mode = false; configset = '\0'; break; }
				else { std::cout << "Unknown\n"; continue; }
			}
			// Set admin access
			while (true)
			{
				std::cout << "Admin access code (from login screen): ";
				std::cin >> numset;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (std::cin.fail())
				{
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Must be numeric" << std::endl;
					continue;
				}
				if (numset < 0 || numset > 999999) { std::cout << "Invalid size (code must be positive and 1 - 6 digits)\n"; continue; }
				else { admin_access = numset; numset = 0; break; }
			}
			// Set admin password
			{
				std::cout << "Admin password: ";
				std::cin >> password;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				admin_password = password;
			}
			// Set input mask
			while (true)
			{
				std::cout << "Input mask (y/n): ";
				std::cin >> configset;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (configset == 'y' || configset == '1') { maskInput = true; configset = '\0'; break; }
				else if (configset == 'n' || configset == '0') { maskInput = false; configset = '\0'; break; }
				else { std::cout << "Unknown\n"; continue; }
			}
			// Set default mgr pin
			while (true)
			{
				std::cout << "Default MGR pin: ";
				std::cin >> numset;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (std::cin.fail())
				{
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Must be numeric" << std::endl;
					continue;
				}
				if (numset < 1000 || numset > 9999) { std::cout << "Invalid size (pin must be positive and 4 digits)\n"; continue; }
				else { mgrPin = numset; numset = 0; break; }
			}
			// Set system block
			while (true)
			{
				std::cout << "Block system (y/n): ";
				std::cin >> configset;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (configset == 'y' || configset == '1') { block_access = true; configset = '\0'; break; }
				else if (configset == 'n' || configset == '0') { block_access = false; configset = '\0'; break; }
				else { std::cout << "Unknown\n"; continue; }
			}
		}
		// Apply default config
		else if (choice == '2')
		{
			std::cout << "Applying default settings...\n";
			wait(1);
			// Set beta
			beta_mode = defBeta;
			std::cout << "Beta - " << (defBeta ? "TRUE" : "FALSE") << std::endl;
			wait(1);
			// Set admin access
			admin_access = defAdminAccess;
			std::cout << "Admin access - " << defAdminAccess << std::endl;
			wait(1);
			// Set admin password
			admin_password = defAdminPassword;
			std::cout << "Admin password - " << defAdminPassword << std::endl;
			wait(1);
			// Set input mask
			maskInput = defMaskInput;
			std::cout << "Mask input - " << (defMaskInput ? "TRUE" : "FALSE") << std::endl;
			wait(1);
			// Set default mgr pin
			mgrPin = defMgrPin;
			std::cout << "Default MGR pin - " << defMgrPin << "\n";
			wait(1);
			// Set system block
			block_access = defBlockAccess;
			std::cout << "System block - " << (defBlockAccess ? "TRUE" : "FALSE") << std::endl;
			wait(1);

		}
		else
		{
			std::cout << "Unknown, try again\n";
			continue;
		}
		/// SAVE
		std::cout << "Saving configuration settings and restarting...\n";
		saveConfig();
		wait(2);
		return;
	}
}
bool checkConfig()
{
	std::ifstream file((DATA_DIR / "config.txt"), std::ios::ate); // open at end

	if (!file.is_open())
		return true; // does not exist

	return file.tellg() == 0; // empty if size is 0
}
// File management
void saveConfig()
{
	std::ofstream file(DATA_DIR / "config.txt");

	if (!file.is_open())
		return;

	file << "BETA | " << beta_mode << "\n\n";
	file << "ADMIN_ACCESS | " << admin_access << "\n";
	file << "ADMIN_CODE | " << admin_password << "\n";
	file << "MASK_INPUT | " << maskInput << "\n";
	file << "DEFAULT_MGR_PIN | " << mgrPin << "\n";
	file << "BLOCK_ACCESS | " << block_access << "\n";

	file.close();
}
void loadConfig()
{
	std::ifstream file(DATA_DIR / "config.txt");
	if (!file.is_open())
		return;

	std::string line;

	while (std::getline(file, line))
	{
		// Skip empty lines
		if (line.empty())
			continue;

		// Find separator
		size_t sep = line.find('|');
		if (sep == std::string::npos)
			continue;

		std::string key = line.substr(0, sep);
		std::string value = line.substr(sep + 1);

		// Trim spaces
		auto trim = [](std::string& s)
			{
				s.erase(0, s.find_first_not_of(" \t"));
				s.erase(s.find_last_not_of(" \t") + 1);
			};

		trim(key);
		trim(value);

		// Match keys
		if (key == "BETA")
			beta_mode = (value == "1");
		else if (key == "ADMIN_ACCESS")
			admin_access = std::stoi(value);
		else if (key == "ADMIN_CODE")
			admin_password = value;
		else if (key == "MASK_INPUT")
			maskInput = (value == "1");
		else if (key == "DEFAULT_MGR_PIN")
			mgrPin = std::stoi(value);
		else if (key == "BLOCK_ACCESS")
			block_access = (value == "1");
	}

	file.close();
}
