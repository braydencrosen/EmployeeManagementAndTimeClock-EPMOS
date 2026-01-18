#include "Schedules.h"
#include "Employees.h"
#include <vector>
#include <fstream>
#include <iostream>

void saveAvailability(std::vector<employee>& employees)
{
    std::ofstream file("availability.txt");
    if (!file) {
        std::cerr << "Error opening file for writing.\n";
        return;
    }

    /*for (const auto& a : availability)
    {
        file << 
    }*/
}


void loadAvailability(std::vector<employee>& employees)
{

}
