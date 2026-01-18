#pragma once
#include <string>
#include <vector>
#include "Employees.h"

struct notification
{
	int code;
	int senderID;
	int receiverID;
	std::string message;
	std::string timestamp;
};

void loadNotifs(std::vector<employee>& employees, std::vector<notification>& notifications);
void saveNotifs(std::vector<employee>& employees, std::vector<notification>& notifications);

void newNotif(std::vector<notification>& notifications, int code, int sendid, int recid, std::string& message);

void displayNotifs(std::vector<notification>& notifications, std::vector<employee>& employees);
