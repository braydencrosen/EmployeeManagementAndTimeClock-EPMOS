#include "FileIO.h"
#include "Employees.h"
#include "Config.h"
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

// Employee management
void loadEmployees(std::vector<employee>& employees)
{
	std::filesystem::path employeeFile = DATA_DIR / (beta_mode ? "BETA_employees.txt" : "employees.txt");
	
	std::ifstream file(employeeFile);
	if (!file.is_open())
		return;

	employees.clear();
	std::string line;

	while (getline(file, line))
	{
		// Skip empty lines
		if (line.empty())
			continue;

		// Skip over file header
		if (line.starts_with("NAME|"))

		// Count pipes to ensure correct format (6 pipes = 9 fields)
		if (count(line.begin(), line.end(), '|') != 6)
			continue;

		try
		{
			// Locate all pipe positions
			std::vector<size_t> p;
			size_t pos = 0;
			while ((pos = line.find('|', pos)) != std::string::npos)
			{
				p.push_back(pos);
				pos++;
			}

			// Parse fields
			std::string name = line.substr(0, p[0]);
			int id = stoi(line.substr(p[0] + 1, p[1] - p[0] - 1));
			double pay = stod(line.substr(p[1] + 1, p[2] - p[1] - 1));
			bool isMgr = stoi(line.substr(p[2] + 1, p[3] - p[2] - 1));
			int pin = stoi(line.substr(p[3] + 1, p[4] - p[3] - 1));
			bool isMaster = stoi(line.substr(p[4] + 1, p[5] - p[4] - 1));
			int timeStatus = stoi(line.substr(p[5] + 1));

			// Construct employee
			employees.emplace_back(
				name,
				id,
				pay,
				isMgr,
				pin,
				isMaster,
				timeStatus
			);

		}
		catch (...)
		{
			// Skip malformed lines safely
			continue;
		}
	}

	file.close();
}
void saveEmployees(const std::vector<employee>& employees)
{
	std::filesystem::path employeeFile = DATA_DIR / (beta_mode ? "BETA_employees.txt" : "employees.txt");

	std::ofstream file(employeeFile);
	// Write header first
	file << "NAME|ID|PAY|IS_MANAGER|PIN|IS_MSTR|STATUS\n";

	for (const auto& e : employees)
	{
		file << e.getName() << "|"
			<< e.getID() << "|"
			<< e.getPay() << "|"
			<< e.getMgrStatus() << "|"
			<< e.getMgrPin() << "|"
			<< e.getMstrStatus() << "|"
			<< e.getStatus() << "\n";
	}
}

// Punch file management
void savePunch(const punch& p)
{
	std::filesystem::path punchFile = DATA_DIR / (beta_mode ? "BETA_punchRecords.txt" : "punchRecords.txt");

	std::ofstream file(punchFile, std::ios::app);
	file << p.employeeID << "--" << p.name << "--" << p.type << "--" << p.timestamp;

	bool addMgrName = p.externally;

	if (addMgrName)
	{
		file << "--SET-BY--" << p.setby;
	}
	file << "\n";
}

punch getLastPunch(int employeeID)
{
	std::filesystem::path punchFile = DATA_DIR / (beta_mode ? "BETA_punchRecords.txt" : "punchRecords.txt");

	std::ifstream file(punchFile);
	std::string line;
	punch last = { 0, "", "", "" };

	while (getline(file, line))
	{
		size_t p1 = line.find("--");
		size_t p2 = line.find("--", p1 + 2);
		size_t p3 = line.find("--", p2 + 2);

		if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos)
			continue;

		int idFromFile = stoi(line.substr(0, p1));

		if (idFromFile == employeeID)
		{
			last.employeeID = idFromFile;
			last.name = line.substr(p1 + 2, p2 - (p1 + 2));
			last.type = line.substr(p2 + 2, p3 - (p2 + 2));
			last.timestamp = line.substr(p3 + 2);
		}
	}

	return last;
}