#pragma once

#include <vector>
#include <string>

// Forward declarations to avoid heavy includes in headers
class employee;
struct notification;

// Admin panel entry point
void adminPanel(std::vector<employee>& employees, std::vector<notification>& notifications, int idx);

// Admin command handler
bool adminCommand(const std::string& command, std::vector<employee>& employees, std::vector<notification>& notifications, int idx);

// Utility
void showFiles();
