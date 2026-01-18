#pragma once
#include <string>
#include <vector>

class employee
{
private:
	std::string name;
	int employeeID;
	double pay;
	bool isManager;
	int managerPin;
	bool masterStatus;
	int timeStatus; // 0 (off clock) | 1 (on clock) | 2 (on meal) | -2 (locked) | -1 (terminated)

public:
	// CONSTRUCTOR
	employee(std::string empName, int empID, double empPay, bool managerStatus, int pin, bool mststatus, int status);

	// Getter functions
	std::string getName() const;
	int getID() const;
	double getPay() const;
	bool getMgrStatus() const;
	int getMgrPin() const;
	bool getMstrStatus() const;
	int getStatus() const;

	// Setter functions
	void setTimeStatus(int set);
	void setPay(double newPay);
	void setPin(int pin);
	void setID(int id);
	void setName(std::string newName);
	void setPermissions(int status);
};
