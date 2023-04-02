// CS Airline
// cs_airline.c
//
// This program was written by Sofia Nie (z5416810)
// on 13/04/2022
//
// Version 1.0.0 2022-04-02: Initial Release.
// Version 1.0.1 2022-04-06: Fixed style issue with help function.
//
// A booking system capable of managing a single flight with multiple stops

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manifest.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// The buffer length is used for reading a single line
#define MAX_STRING_LEN 100

#define COMMAND_PRINT_HELP "help"
#define COMMAND_PRINT_ROUTE "print"
#define COMMAND_SUBROUTE "subroute"
#define COMMAND_CHANGE_ORIGIN "change_origin"
#define COMMAND_BYPASS "bypass"
#define COMMAND_EMERGENCY "emergency"
#define COMMAND_CANCEL "cancel"
#define COMMAND_REVERSE "reverse"
#define COMMAND_ADD_PERSON "add_person"
#define COMMAND_PRINT_MANIFEST "print_manifest"
#define COMMAND_FLIGHT_STATS "stats"

// TODO: you may choose to add additional #defines here.
#define EMERGENCY_DEPARTURE 000
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Airports represent 'stops' along the flight path
// These are the 'nodes' of the linked list
struct airport {
    char code[MAX_STRING_LEN];
    int arrival_time;
    int departure_time;
    struct airport *next_airport;
    struct manifest *manifest;
};

// Root flight structure
// This stores the state of the current flight
// (i.e. the head and tail of the linked list)
struct flight {
    struct airport *origin_airport;
    struct airport *dest_airport;
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  FUNCTION PROTOTYPES  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Helper Functions
void remove_newline(char input[MAX_STRING_LEN]);
void do_print_help(void);

void interpret_line(
    char buffer[MAX_STRING_LEN],
    int *val0,
    int *val1,
    char word[MAX_STRING_LEN]
);

// Stage 1 Functions
struct flight *setup_flight(void);
void do_print_airports(struct flight *flight);
void print_one_airport(struct airport *ap);
struct airport *create_airport(
    char code[MAX_STRING_LEN],
    int arrival_time,
    int departure
);

// Stage 2 Functions
void do_subroute(struct flight *flight);
void do_change_origin(struct flight *flight);

// TODO: Your functions prototypes here
void do_bypass(struct flight *flight);
void do_emergency(struct flight *flight);
void do_cancel(struct flight *flight);
void do_reverse(struct flight *flight);
void do_add_person(struct flight *flight);
void do_print_manifest(struct flight *flight);
////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    // Stage 1.1
    // TODO: Complete the setup_flight function below
    struct flight *flight = setup_flight();

    // TODO: Fill out the while loop with relevant commands & functions
    // Move into and stay in operational mode until CTRL+D
    printf("Enter Command: ");
    char command_line[MAX_STRING_LEN];
    while (fgets(command_line, MAX_STRING_LEN, stdin) != NULL) {

        remove_newline(command_line);

        if (strcmp(COMMAND_PRINT_HELP, command_line) == 0) {
            // A help command we have implemented for you.
            do_print_help();
        } else if (strcmp(COMMAND_PRINT_ROUTE, command_line) == 0) {
            // Stage 1.2 - TODO: Complete this function below
            do_print_airports(flight);
        } else if (strcmp(COMMAND_SUBROUTE, command_line) == 0) {
            // Stage 2.1 - TODO: Complete this function below
            do_subroute(flight);
        } else if (strcmp(COMMAND_CHANGE_ORIGIN, command_line) == 0) {
            // Stage 2.2 - TODO: Complete this function below
            do_change_origin(flight);
        } else if (strcmp(COMMAND_BYPASS, command_line) == 0) {
            do_bypass(flight);
        } else if (strcmp(COMMAND_EMERGENCY, command_line) == 0) {
            do_emergency(flight);
        } else if (strcmp(COMMAND_CANCEL, command_line) == 0) {
            do_cancel(flight);
            flight = setup_flight();
        } else if (strcmp(COMMAND_REVERSE, command_line) == 0) {
            do_reverse(flight);
        } else if (strcmp(COMMAND_ADD_PERSON, command_line) == 0) {
            do_add_person(flight);
        } else if (strcmp(COMMAND_PRINT_MANIFEST, command_line) == 0) {
            do_print_manifest(flight);
        }
        // TODO: Add more conditions here for the remaining commands
            
        printf("Enter Command: ");
    }
    // Freeing all memory after CTRL+D is pressed
    struct airport *head = flight->origin_airport;
    struct airport *current = head;
    while (current != NULL) {
        current = current->next_airport;
        delete_manifest(head->manifest);
        free(head);
        head = current;
    }
    free(flight);
    printf("Goodbye!\n");

    return 0;
}

// Stage 1.1
// Creates and initialises a flight by asking the user for input.
// Parameters:
//     None
// Returns:
//     Pointer to the malloc'd flight
struct flight *setup_flight(void) {
    // Create a new, empty flight
    struct flight *new_flight = malloc(sizeof(struct flight));
    new_flight->origin_airport = NULL;
    new_flight->dest_airport = NULL;

    // Read number of stops using fgets
    printf("How many stops will be enroute? ");
    char input_line[MAX_STRING_LEN] = {0};
    fgets(input_line, MAX_STRING_LEN, stdin);

    // Convert the string to a number
    int num_stops = 0;
    num_stops = atoi(input_line);

    // TODO: For each airport Use fgets to read a line and then use the
    //       provided interpret_line() function to extract information
    int i = 0;
    while (i < num_stops) {
        char raw_input_line[MAX_STRING_LEN];
        fgets(raw_input_line, MAX_STRING_LEN, stdin);
        int arrival_time = 0;
        int departure_time = 0;
        char airport_code[MAX_STRING_LEN];
        
        interpret_line(raw_input_line, &arrival_time, 
        &departure_time, airport_code);
        // Checking if airport will be invalid 
        if (departure_time < arrival_time) {
            printf("Departure time cannot be before the arrival time!\n");
        } else {
            // Creating new airport
            struct airport *new_airport = create_airport(airport_code,
            arrival_time, departure_time); 
            new_airport->manifest = create_manifest();
            
            // Declaring the head of the linked list
            if (new_flight->origin_airport == NULL) { 
                new_flight->origin_airport = new_airport;
                new_flight->dest_airport = new_airport;
            } else {
                // Traversing and adding new airport to end of list while
                // updating destination airport and error checking
                struct airport *current = new_flight->origin_airport;
                while (current->next_airport != NULL) {
                    current = current->next_airport;
                }     
                if (new_airport->arrival_time < current->departure_time) {
                    printf("New arrival time cannot be ");
                    printf("before the previous departure time\n");
                    delete_manifest(new_airport->manifest);
                    free(new_airport);
                } else {
                    current->next_airport = new_airport;
                    new_flight->dest_airport = new_airport;
                } 
            }
        }
        i++;
    }
    
    return new_flight;
    
}

// Stage 1.2
// Given a pointer to a flight struct, prints all the info about the route
// Parameters:
//     flight = the flight which contains the flight route to print
// Returns:
//     None
void do_print_airports(struct flight *flight) {

    // TODO: Loop through the flight route and call 
    //       the provided print_one_airport function
    struct airport *current = flight->origin_airport;
    printf("ROUTE:\n");
    while (current != NULL) {
        print_one_airport(current);
        current = current->next_airport;
    }
}

// Stage 1.2
// PROVIDED FUNCTION - DO NOT CHANGE
// Given a pointer to an airport struct,
// prints all the info about the airport
// Parameters:
//     ap = the airport to print
// Returns:
//     None
void print_one_airport(struct airport *ap) {
    printf(
        "%8s:    %04d -> %04d\n",
       ap->code,
       ap->arrival_time,
       ap->departure_time
    );
}

// Stage 1.1
// Given the information about a new airport,
// Uses `malloc` to create memory for it and returns a pointer to
// that memory.
// Parameters:
//     code            = the airport ICAO code
//     arrival_time    = the arrival time of the new airport
//     departure_time  = the departure time of the new airport
// Returns:
//     A pointer to the malloc'd struct airport
struct airport *create_airport(
    char code[MAX_STRING_LEN],
    int arrival_time,
    int departure_time)
{
    // Malloc new struct
    struct airport *new_airport = malloc(sizeof(struct airport));

    // initialise airport fields
    strcpy(new_airport->code, code);
    new_airport->arrival_time = arrival_time;
    new_airport->departure_time = departure_time;
    new_airport->next_airport = NULL;
    new_airport->manifest = NULL;
    return new_airport;
}

// TODO
void do_subroute(struct flight *flight) {
    // Reading in airport code using fgets and 
    // looping through list until airport with the same code has been found
    printf("Enter airport code: ");
    char airport_code[MAX_STRING_LEN] = {0};
    fgets(airport_code, MAX_STRING_LEN, stdin);
    remove_newline(airport_code);
    struct airport *current = flight->origin_airport;
    while (strcmp(current->code,airport_code) != 0) {
        current = current->next_airport;
        if (current == NULL) {
            printf("Desired subroute destination does not exist!\n");
            return;
        }
    }
    // Calculating how long it will take from origin to specified airport
    int end = 0;
    int start = 0;
    int hours = 0;
    int mins = 0;
    int minute_diff = 0;
    
    end = current->arrival_time;
    start = flight->origin_airport->departure_time;
    hours = (end / 100) - (start / 100);
    mins = (end % 100) - (start % 100);
    minute_diff = hours * 60 + mins;

    if (minute_diff < 0) {
        minute_diff *= -1;
    }

    hours = minute_diff / 60;
    mins = minute_diff % 60;
    
    printf("Time for subroute: %d hrs & %d mins\n", hours, mins);
    return;
}

// TODO
void do_change_origin(struct flight *flight) {
    // Read in new head node information using fgets
    printf("Enter new origin info: ");
    char raw_input_line[MAX_STRING_LEN];
    fgets(raw_input_line, MAX_STRING_LEN, stdin);
    int arrival_time = 0;
    int departure_time = 0;
    char new_airport_code[MAX_STRING_LEN];
    
    interpret_line(raw_input_line, &arrival_time, 
    &departure_time, new_airport_code);
    
    // Error and code uniqueness checking before creating new airport
    if (departure_time < arrival_time) {
        printf("Departure time cannot be before the arrival time!\n");
        return;
    } 
    if (departure_time > (flight->origin_airport->arrival_time)) {
        printf("Departure of new origin cannot be after the ");
        printf("arrival time of the next airport!\n");
        return;
    }
    struct airport *current = flight->origin_airport;
    int uniqueness_check = 0;
   
    while (current != NULL) {
        if (strcmp(current->code,new_airport_code) == 0) {
            uniqueness_check++;
        } else {     
        }
        current = current->next_airport; 
    } 
    if (uniqueness_check > 0) {
        printf("New airport code is not unique!\n");
        return;
    } else if (uniqueness_check == 0) {
        printf("Added: %s\n", new_airport_code);
        // Creating airport and updating head of linked list
        struct airport *new_origin = create_airport(new_airport_code,
        arrival_time, departure_time);
        
        new_origin->next_airport = flight->origin_airport;
        flight->origin_airport = new_origin;
    }
    return;
}
void do_bypass(struct flight *flight) {
    // Reading in code using fgets
    printf("Enter airport code: ");
    char airport_code[MAX_STRING_LEN] = {0};
    fgets(airport_code, MAX_STRING_LEN, stdin);
    remove_newline(airport_code);
    struct airport *current = flight->origin_airport;
    // Using strcmp and loop to find airport that matches given code 
    // and freeing memory of the airport
    if (strcmp(current->code,airport_code) == 0) {
        flight->origin_airport = current->next_airport;
        delete_manifest(current->manifest);
        free(current);  
        return;      
    } 
    while ((current->next_airport!= NULL) && 
        (strcmp(current->next_airport->code,airport_code) != 0)) {
        current = current->next_airport;     
    }
    // Error message if inputted airport code is not in the flight route
    if (current->next_airport == NULL) {
        printf("No airport of that code exists!\n");
        return;
    }
    // Updating pointer to relink airport route
    if (current->next_airport->next_airport == NULL) {
        delete_manifest(current->next_airport->manifest);
        free(current->next_airport);
        current->next_airport = NULL;
        flight->dest_airport = current;
    } else {
        struct airport *new_next_airport = current->next_airport->next_airport;
        delete_manifest(current->next_airport->manifest);
        free(current->next_airport);
        current->next_airport = new_next_airport;
    }
    return;
}

void do_emergency(struct flight *flight) {
    printf("How long until emergency: ");
    // Using fgets to read in hours  
    char emergency_hour[MAX_STRING_LEN]; 
    fgets(emergency_hour, MAX_STRING_LEN, stdin); 
    remove_newline(emergency_hour);
    // Using atoi to convert string to number and multiplying by 100 to 
    // convert to 24 hour format
    int num_emergency_hour = atoi(emergency_hour)*100;
    int emergency_dep_time = EMERGENCY_DEPARTURE;
    // Modifying head airport's departure time and removing rest of airports
    if (num_emergency_hour == 0) {
        flight->origin_airport->departure_time = emergency_dep_time;
        struct airport *current = flight->origin_airport->next_airport;
        while (current != NULL) {
            struct airport *previous = current;
            current = current->next_airport;
            delete_manifest(previous->manifest);
            free(previous);
        }
        flight->origin_airport->next_airport = NULL;
        return;
    }   
    struct airport *current = flight->origin_airport;
    struct airport *origin = flight->origin_airport;
    // Calculating emergency airport's arrival time
    int emergency_arr_time = origin->departure_time + num_emergency_hour;
    if (emergency_arr_time > flight->dest_airport->arrival_time) {
        printf("Flight was safe!\n");
        return;
    }
    while (current->next_airport->arrival_time < emergency_arr_time) {
        current = current->next_airport;
    } 
    // Modifying existing airport to have details of the emergency airport
    char emergency_code[MAX_STRING_LEN] = "ALT0";
    remove_newline(emergency_code);
    current->next_airport->arrival_time = emergency_arr_time;
    strcpy(current->next_airport->code, emergency_code);
    current->next_airport->departure_time = emergency_dep_time;
    flight->dest_airport = current->next_airport;
    // Freeing up memory for all airports after emergency airport
    struct airport *scan = current->next_airport->next_airport;
    while (scan != NULL) {
        struct airport *previous = scan;
        scan = scan->next_airport;
        delete_manifest(previous->manifest);
        free(previous);
    }
    current->next_airport->next_airport = NULL;
    return;
}

void do_cancel (struct flight *flight) {
    struct airport *head = flight->origin_airport;
    struct airport *current = head;
    // Freeing up memory of all airports
    while (current != NULL) {
        current = current->next_airport;
        delete_manifest(head->manifest);
        free(head);
        head = current;
    }
    // Freeing the flight struct
    free(flight);
    printf("Flight cancelled. Now accepting a new flight:\n");
   
    return;
} 


void do_reverse(struct flight *flight) {
    struct airport *previous = NULL;   
    struct airport *current = flight->origin_airport;   
    struct airport *new_dest = flight->origin_airport;
    // Traverse the list
    while (current != NULL) {
        struct airport *next = current->next_airport;
        current->next_airport = previous;    
        previous = current;
        current = next;
    }
    // Fixing pointer to point to new origin and new destination airport
    flight->origin_airport = previous;
    flight->dest_airport = new_dest;
    return;
}

void do_add_person(struct flight *flight) {
    printf("Enter the airport code: ");
    // Using fgets to read in code, name and weight  
    char airport_code[MAX_STRING_LEN]; 
    fgets(airport_code, MAX_STRING_LEN, stdin); 
    remove_newline(airport_code);  
    
    printf("Enter the passenger name: ");
    char name[MAX_STRING_LEN]; 
    fgets(name, MAX_STRING_LEN, stdin); 
    remove_newline(name); 
    
    printf("Enter the passenger weight: ");
    char weight[MAX_STRING_LEN]; 
    fgets(weight, MAX_STRING_LEN, stdin); 
    remove_newline(airport_code); 
    double weight_num = atof(weight);
    
    struct airport *current = flight->origin_airport;
    int code_check = 0;
    // Traversing until same airport code has been found   
    while ((current != NULL) && (code_check == 0)) {
        if (strcmp(current->code,airport_code) == 0) {
            code_check++;
        } else {
            current = current->next_airport;     
        }   
    } 
    // Adding person to the airport's manifest or printing error message
    // if airport with the same code is not found 
    if (code_check > 0) {
        add_person(current->manifest, name, weight_num);
        return;
    } else if (code_check == 0) {
        printf("No airport of that code exists!\n");  
    }
    return;
}

void do_print_manifest(struct flight *flight) {
    printf("Enter the airport code: ");
    // Using fgets to read in code  
    char airport_code[MAX_STRING_LEN]; 
    fgets(airport_code, MAX_STRING_LEN, stdin); 
    remove_newline(airport_code);  
    
    struct airport *current = flight->origin_airport;
    int code_check = 0;
    // Traversing until same airport code has been found 
    while ((current != NULL) && (code_check == 0)) {
        if (strcmp(current->code,airport_code) == 0) {
            code_check++;
        } else {
            current = current->next_airport;     
        }     
    } 
    // Prints out information of person in that airport manifest or error
    // message when no airport of that code has been found
    if (code_check > 0) {
        print_manifest(current->manifest);
        return;
    } else if (code_check == 0) {
        printf("No airport of that code exists!\n");  
    }
    return;
      
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  HELPER FUNCTIONS  //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Helper Function
// You likely do not need to change this function.
//
// Given a raw string will remove and first newline it sees.
// The newline character wil be replaced with a null terminator ('\0')
// Parameters:
//     flight  = the flight to move people along of
// Returns:
//     None
void remove_newline(char input[MAX_STRING_LEN]) {

    // Find the newline or end of string
    int index = 0;
    while (input[index] != '\n' && input[index] != '\0') {
        index++;
    }
    // Goto the last position in the array and replace with '\0'
    // Note: will have no effect if already at null terminator
    input[index] = '\0';
}


// Helper Function
// You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
//
// Given a raw string in the following foramt: [integer0] [integer1] [string]
// This function will extract the relevant values into the given pointer variables.
// The function will also remove any newline characters.
//
// For example, if given: "0135 0545 YSSY"
// The function will put the integer values
//     135 into the val1 pointer
//     545 into the val2 pointer
// And will copy a null terminated string
//     "YSSY" into the 'word' array
//
// If you are interested, `strtok` is a function which takes a string,
// and splits it up into before and after a "token" (the second argument)
//
// Parameters:
//     buffer  = A null terminated string in the following format
//               [integer0] [integer1] [string]
//     val0    = A pointer to where [integer0] should be stored
//     val1    = A pointer to where [integer1] should be stored
//     word    = An array for the [string] to be copied into
// Returns:
//     None
void interpret_line(
    char buffer[MAX_STRING_LEN],
    int *val0,
    int *val1,
    char word[MAX_STRING_LEN]
) {

    // Remove extra newline
    remove_newline(buffer);

    // Extract value 1 as int
    char *val0_str = strtok(buffer, " ");
    if (val0_str != NULL) {
        *val0 = atoi(val0_str);
    } else {
        *val0 = -1;
    }

    // Extract value 2 as int
    char *val1_str = strtok(NULL, " ");
    if (val1_str != NULL) {
        *val1 = atoi(val1_str);
    } else {
        *val1 = -1;
    }

    char *word_str = strtok(NULL, " ");
    if (word_str != NULL) {
        // Extract word
        strcpy(word, word_str);
    }

    if (val0_str == NULL || val1_str == NULL || word_str == NULL) {
        // If any of these are null, there were not enough words.
        printf("Could not properly interpret line: %s.\n", buffer);
    }

}

void do_print_help(void) {
    printf("+-----------------+-------------------------------------------+\n");
    printf("|Command Name     | How to Use                                |\n");
    printf("+=================+===========================================+\n");
    printf("|PRINT ROUTE      | Enter command: print                      |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|SUBROUTE         | Enter command: subroute                   |\n");
    printf("|                 | Enter airport code: [AIRPORT CODE]        |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|CHANGE ORIGIN    | Enter command: change_origin              |\n");
    printf("|                 | Enter new origin info: [ARRIVAL TIME]...  |\n");
    printf("|                 |     ... [DEPARTURE TIME] [AIRPORT CODE]   |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|BYPASS           | Enter command: bypass                     |\n");
    printf("|                 | Enter airport code: [AIRPORT CODE]        |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|EMERGENCY        | Enter command: emergency                  |\n");
    printf("|                 | How long until emergency: [TIME IN HOURS] |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|CANCEL           | Enter command: cancel                     |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|REVERSE          | Enter command: reverse                    |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|                 | Enter command: add_person                 |\n");
    printf("|ADD PERSON       | Enter the airport code: [AIRPORT CODE]    |\n");
    printf("|                 | Enter the passenger name: [NAME]          |\n");
    printf("|                 | Enter the passenger weight: [WEIGHT]      |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|PRINT MANIFEST   | Enter command: print_manifest             |\n");
    printf("|                 | Enter the airport code: [AIRPORT CODE]    |\n");
    printf("+-----------------+-------------------------------------------+\n");
    printf("|FLIGHT STATISTICS| Enter command: stats                      |\n");
    printf("+-----------------+-------------------------------------------+\n");
}
