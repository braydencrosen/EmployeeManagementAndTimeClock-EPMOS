# Employee Management & Time Clock (EPMOS)

A terminal-based Employee Management and Time Clock system written in C++. This project simulates core features of a small workforce system with employee records, punch in/out tracking, manager/admin controls, notifications, and configurable application behavior.

> Designed as a learning project to practice C++ OOP, modular architecture, and file I/O using plain text files for persistent data between runs. 

> This project is an enhanced version of my older employeeTimeManagementSystem repository

---

## Features

* **Employee Management**

  * Create, edit, and persist employee records
  * Attributes include name, ID, pay, permissions, and clock status (in / out / meal)

* **Time Clock System**

  * Punch in / punch out / meal tracking
  * Punch records saved to file
  * Retrieve last punch per employee

* **Roles & Permissions**

  * Associate, Manager, and Manager (MSTR / master)
  * PIN-based verification for restricted actions

* **Notifications**

  * Send and store notifications between users
  * Persisted notification storage

* **Configuration**

  * Configurable settings loaded/saved from file
  * Beta mode support
  * Feature and access toggles


 * **Clean separation by responsibility:**

    * `Employees.*` – employee model
    * `FileIO.*` – persistence
    * `Menus.*` – UI flows
    * `Admin.*`, `MgrFunctions.*`, `AdvancedFunctions.*` – role-specific actions
    * `Utils.*` – helpers

---
## Work In Progress

Some features show in the advanced menu that are either partially built, or do not exist yet. 

These features are automatically bypassed, displaying: `This function is temporarily unavailable`

* View timecard
* Edit availability
* Schedule editor
* Finance dashboard
* Settings

**If BETA MODE is enabled, the bypass is disabled**

---

## Structure

```
.
├── main.cpp
├── Config.h / Config.cpp
├── Employees.h / Employees.cpp
├── FileIO.h / FileIO.cpp
├── Menus.h / Menus.cpp
├── Admin.h / Admin.cpp
├── MgrFunctions.h / MgrFunctions.cpp
├── AdvancedFunctions.h / AdvancedFunctions.cpp
├── Notifications.h / Notifications.cpp
├── Schedules.h / Schedules.cpp
├── Utils.h / Utils.cpp
├── GlobalFunctions.h / GlobalFunctions.cpp
```

### Data Files

Created at runtime:

* `employees.txt`
* `config.txt`

Created as needed:

* `punchRecords.txt`
* `notifications.txt`
* Optional `BETA_*.txt` versions when beta mode and beta files are enabled (these can be set from the admin menu)

---

## Setup

### Prerequisites

* **C++17** or newer
* Any **C++17**-compatible compiler:

  * **g++ (MinGW / WSL)**
  * **clang++ (macOS/Linux)**
  * **MSVC (Visual Studio – optional, not required)**

  **This program has recently been optimized for ***Mac*** and ***Linux***, errors may occur.**

---

## Getting Started

* Launch the program (a set of employees will automatically be created in a file called "employees.txt")
* Log in to the test employee (ID: `1111111`)
* Use the main menu to:

  * Clock in/out
  * View status
  * Access manager features (PIN: `1111`)
  * View notifications
  * View or edit employee data (based on permissions)

Most actions are menu-driven through terminal prompts.

---

## Configuration

The application supports persistent configuration through the `Config.*` system, including:

* `beta_mode` – switches file prefixes to `BETA_*`
* `useBetaFiles` - Toggles use of seperate **Beta** files for employees, notifications, and punches (Beta mode must also be true)
* `admin_access` – enables/disables admin tools
* `mgrPin` – manager verification PIN
* `block_access` – lock down entire system for testing (admin override still works)
* `maskInput` – hides sensitive input (like PINs)

Configuration is automatically loaded and saved at runtime and after changes are made

**If no configuration file is found at compilation, you will automatically be redirected to the config menu.**

---

## Permissions

The table below summarizes access levels by role. This reflects the intended design of the system and can be adjusted as features evolve. 

Note that `Test User` has **master access**

| Action / Capability                 | Associate    | Manager    | Manager (MSTR) |
| ----------------------------------- | ----------   | ---------- | -------------- |
| Clock in / Clock out                | ✅          | ✅          | ✅              |
| Start / End meal                    | ✅          | ✅          | ✅              |
| View last punch                     | ✅          | ✅          | ✅              |
| View own notifications              | ✅          | ✅          | ✅              |
| Send notifications                  | ⚠️ Limited  | ⚠️ Limited  | ✅              |
| View all employees                  | ❌          | ✅          | ✅              |
| Access manager functions            | ❌          | ✅          | ✅              |
| Create punch edit                   | ❌          | ⚠️ Limited  | ✅              |
| Edit employee records               | ❌          | ⚠️ Limited  | ✅              |
| Punch Edit                          | ❌          | ⚠️ Limited  | ✅              |
| Change permissions / roles          | ❌          | ⚠️ Limited  | ✅              |

### Overview
* **Manager MSTR** should be treated as a supervisor with complete feature control
* Managers without master status cannot view the pay of another manager.
* Managers without master status cannot edit information of another manager.
* Managers cannot create a punch edit for another manager.

#### Notifications
* All employees can send a message and create a reminder
* Managers can send announcements
* Only managers with master access can create system notifications, as they do not display a senderID

---

## Admin Panel

The Admin Panel provides a powerful command-line interface for advanced control over the system, including mode switching, batch operations, and system configuration that is not associated with any user profile in the system.

### Accessing the Admin Panel

1. From the **login screen**, enter the default admin access code: `9`

2. When prompted, enter the default admin password: `admin`


> The admin access code and password can be changed from the **Config Menu**.

### What You Can Do in the Admin Panel

- Switch application modes (e.g., beta mode)
- Run batch commands
- Access configuration menu
- Access advanced system tools

### Help Command

To view an organized list of all available admin commands and their descriptions, type: `help`

---

## Roadmap / Ideas

Potential future improvements:

* Improve terminal UI with less flicker and better layout (possibly using curses)
* Cross-platform screen handling
* Encrypted storage for sensitive data
* JSON or SQLite persistence instead of text files
* GUI version (e.g., Qt or WinForms frontend)

---

## Author

**Brayden Crosen** – Computer Science student at Santa Fe College (Gainesville, FL)

**This project was simply to help me learn hands-on. You may use, modify, and learn from it freely.**
