#include "Notifications.h"
#include "Utils.h"
#include "Employees.h"
#include "Config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// Notification file management
void loadNotifs(std::vector<employee>& employees, std::vector<notification>& notifications)
{
	std::filesystem::path notifFile = DATA_DIR / ((beta_mode && useBetaFiles) ? "BETA_notifications.txt" : "notifications.txt");

	std::ifstream file(notifFile);
	if (!file.is_open()) return;

	notifications.clear();

	std::string line;

	// Skip header lines
	while (std::getline(file, line))
	{
		if (line.find("CODE |") != std::string::npos)
			break;
	}

	// Read notifications
	while (std::getline(file, line))
	{
		if (line.empty()) continue;

		notification n;
		std::stringstream ss(line);
		std::string temp;

		// CODE
		std::getline(ss, temp, '|');
		n.code = std::stoi(temp);

		// SENDER_ID
		std::getline(ss, temp, '|');
		n.senderID = std::stoi(temp);

		// RECEIVER_ID
		std::getline(ss, temp, '|');
		n.receiverID = std::stoi(temp);

		// TIMESTAMP
		std::getline(ss, temp, '|');
		n.timestamp = temp;

		// MESSAGE (rest of line)
		std::getline(ss, n.message);

		// Trim leading spaces
		auto trim = [](std::string& s)
			{
				s.erase(0, s.find_first_not_of(" \t"));
				s.erase(s.find_last_not_of(" \t") + 1);
			};

		trim(n.timestamp);
		trim(n.message);

		notifications.push_back(n);
	}
}

void saveNotifs(std::vector<employee>& employees, std::vector<notification>& notifications)
{
	std::filesystem::path notifFile = DATA_DIR / ((beta_mode && useBetaFiles) ? "BETA_notifications.txt" : "notifications.txt");

	std::ofstream file(notifFile);
	if (!file)
	{
		std::cerr << "Error: could not open notifications.txt for writing\n";
		return;
	}

	// Write description
	file << "CODES:\n"
		<< "0 - System\n"
		<< "1 - Message\n"
		<< "8 - Reminder\n"
		<< "9 - Announcement\n"
		<< "\n"
		<< "CODE | SENDER_ID | RECEIVER_ID | TIMESTAMP | MESSAGE\n";

	// Write notifications
	for (const auto& n : notifications)
	{
		file << n.code << " | "
			<< n.senderID << " | "
			<< n.receiverID << " | "
			<< n.timestamp << " | "
			<< n.message << '\n';
	}

	// file.close(); // optional, RAII will handle it
}

void newNotif(std::vector<notification>& notifications, int code, int sendid, int recid, std::string& message)
{
	std::string time = getTime();
	notification n;
	n.code = code;
	n.senderID = sendid;
	n.receiverID = recid;
	n.timestamp = time;
	n.message = message;

	notifications.push_back(n);
}