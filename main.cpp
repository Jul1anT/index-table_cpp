#include <iostream>
#include <string>
#include <iomanip>
#include "BPlusTree.h"

// Main program to demonstrate B+ Tree usage with employee records
// Usage: Insert n employees, then perform q queries

int main() {
    // Create B+ Tree indexed by employee id_edad
    BPlusTree<int, Employee> tree;

    int n;  // Number of employees to insert
    std::cin >> n;

    // Read and insert n employees
    for (int i = 0; i < n; i++) {
        Employee emp;
        std::cin >> emp.id_edad >> emp.nombre >> emp.salario >> emp.cargo;
        tree.insert(emp.id_edad, emp);
    }

    int q;  // Number of queries
    std::cin >> q;

    // Process q queries
    for (int i = 0; i < q; i++) {
        int queryType, id;
        std::cin >> queryType >> id;

        if (queryType == 1) {
            // Search query: find employee by id_edad
            auto result = tree.search(id);
            if (result.has_value()) {
                const Employee& emp = result.value();
                // Output all 4 columns: id_edad, nombre, salario, cargo
                std::cout << emp.id_edad << " " 
                          << emp.nombre << " "
                          << std::fixed << std::setprecision(2) << emp.salario << " "
                          << emp.cargo << "\n";
            } else {
                std::cout << "No encontrado\n";
            }
        }
    }

    return 0;
}
