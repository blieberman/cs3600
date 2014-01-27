/*
 * CS3600, Spring 2014
 * C Bootcamp, Homework 2, Problem 2
 * (c) 2012 Alan Mislove
 *
 * In this problem, your goal is to learn about C structures.  You will
 * be mananging employee records.  Each employee record consists of 
 * the following values:
 *
 *  - A name string (less than 49 chars long)
 *  - A salary (integer, positive, less than $4B)
 *  - A employee ID (integer, positive, less than 4B)
 *  - A location string (less than 49 chars long)
 *
 * You are guaranteed that there will be no more than 10 employees. You
 * should support 10, and exactly 10, employees.
 *
 * You should do the following:
 * 
 *  - Define an appropriate structure to represent each employee. Call
 *    this structure "struct employee" (surprise!).
 * 
 *  - Define a global location to store the employee records (hint: array).
 *    Be sure to have a way to keep track of which employee records 
 *    are in-use and which are empty.
 *
 *  - Fill in the add_employee, remove_employee, get_salary functions
 *
 * Do not touch anything inside the main() function.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int add_employee(char *name, unsigned int id, unsigned int salary, char *location);
int remove_employee(unsigned int id);
int get_salary(unsigned int id);

// Do not touch this function
int main(int argc, char **argv) {
  int i;
  for (i=1; i<argc; i++) {
    char *arg = argv[i];
    switch (arg[0]) {
      case 'a': {
        char *name = (char*) malloc(100);
        unsigned int salary;
        unsigned int id;
        char *location = (char*) malloc(100);

        sscanf(arg, "a %[A-Z-a-z] %u %u %[A-Z-a-z]", name, &id, &salary, location);

        int result = add_employee(name, id, salary, location);
        if (result == -1) printf("Err: Employee name already exists.\n");
        if (result == -2) printf("Err: Employee id already exists.\n");
        if (result == -3) printf("Err: Too many employees!\n");
        }
        break;
      case 'r': {
        unsigned int id;

        sscanf(arg, "r %u", &id);

        int result = remove_employee(id);
        if (result == -1) printf("Err: Employee %d not found.\n", id);
        }
        break;
      case 's': {
        unsigned int id;

        sscanf(arg, "s %u", &id);

        int result = get_salary(id);
						
        if (result == -1) printf("Err: Employee %d not found.\n", id);
        else printf("Salary of %d: %d\n", id, result);
        }
        break;
      default: {
        printf("Error: Unknown command '%c'.\n", arg[0]);
        return -1;
        }
    }
  }

  return 0;
}

//Definition of employee structure
#define MAX_STRING_LEN 49
typedef struct {
  char name[MAX_STRING_LEN];
  unsigned int salary;
  unsigned int id;
  char location[MAX_STRING_LEN];
	int isInit;
} employee;

//Definition of global storage location for employees
#define MAX_NUM_EMPLOYEES 10
employee employees[MAX_NUM_EMPLOYEES];
employee employeesTemp[MAX_NUM_EMPLOYEES];

void init_records(){ //initialize emptyInit
  for(int i = 0; i < MAX_NUM_EMPLOYEES; i++) {
	  strcpy(employees[i].name, "\0");
	  employees[i].salary = 0;
	  employees[i].id = 0;
	  strcpy(employees[i].name, "\0");
		employees[i].isInit = 1;
		
	  strcpy(employeesTemp[i].name, "\0");
	  employeesTemp[i].salary = 0;
	  employeesTemp[i].id = 0;
	  strcpy(employeesTemp[i].name, "\0");
		employeesTemp[i].isInit = 1;
	}
}

/**
 * This function should add an employee to the list.  The passed-in arguments
 * describe the employee.  You should add the employee and return one of the following
 * values:
 *   0 - success
 *  -1 - if an employee with an identical name already exists
 *  -2 - if an employee with an identical ID aleady exists
 *  -3 - if there is no more space
 */
int index = 0;

int add_employee(char *name, unsigned int id, unsigned int salary, char *location) {
	if(employees[0].isInit == 0) {
		init_records();
	}
	for (int x = 0; x < MAX_NUM_EMPLOYEES; x++) { 
		//check current array for these conds before adding new employee
  	if (employees[x].name == name) {
  		return -1;
  	}
		if (employees[x].id == id) {
			return -2;
		}
		else if (index > MAX_NUM_EMPLOYEES) {
			return -3;
		}
	}
	//add employee to array
  strcpy(employees[index].name, name);
  employees[index].id = id;
  employees[index].salary = salary;
  strcpy(employees[index].location, location);
	
	index++;
		
	return 0;
}

/**
 * This function should remove an existing employee from the list, based on the
 * provided ID.  You should return one of the following values:
 *  0 - success
 * -1 - if employee was not found
 */
int remove_employee(unsigned int id) {
	int k = 0;
	
	for (int x = 0; x < MAX_NUM_EMPLOYEES; x++) {
		if (employees[x].id == id) { //if employee id is found in array remove it...
			break;
		}
		else {
			return -1;
		}
	}
	
	for (int i=0; i < MAX_NUM_EMPLOYEES; i++) { //store in temp struct
		if (employees[i].id != id) {
			employeesTemp[k] = employees[i];
			k++;
		}
	}
	
	memcpy (employees, employeesTemp, sizeof (employees)); //copy from temp struct
	return 0;
}

/**
 * This function should look up the salary of the employee assigned the given ID,
 * and return the corresponding value. You should return one of the following values:
 * >= 0 - success, the salary value
 *   -1 - if employee was not found
 */
int get_salary(unsigned int id) {
	
	if (employees[0].id == id) {
		return employees[0].salary;
	}
	else if (employees[1].id == id) {
		return employees[1].salary;
	}
	else if (employees[2].id == id) {
		return employees[2].salary;
	}
	else if (employees[3].id == id) {
		return employees[3].salary;
	}
	else if (employees[3].id == id) {
		return employees[3].salary;
	}
	else if (employees[4].id == id) {
		return employees[4].salary;
	}
	else if (employees[5].id == id) {
		return employees[5].salary;
	}
	else if (employees[6].id == id) {
		return employees[6].salary;
	}
	else if (employees[7].id == id) {
		return employees[7].salary;
	}
	else if (employees[8].id == id) {
		return employees[8].salary;
	}
	else if (employees[9].id == id) {
		return employees[9].salary;
	}
	else {
		return -1;
	}
}