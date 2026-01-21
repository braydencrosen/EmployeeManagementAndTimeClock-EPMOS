#pragma once

#include "Employees.h"
#include "Notifications.h"

#include <filesystem>

extern std::filesystem::path DATA_DIR;

extern const std::string VERSION;

// Program modes
extern bool beta_mode;
extern bool useBetaFiles;
extern int admin_access;
extern int mgrPin;
extern bool block_access;

// Admin rules
extern std::string admin_password;

extern bool maskInput;

void configMenu();
bool checkConfig();

// Config file management
void saveConfig();
void loadConfig();