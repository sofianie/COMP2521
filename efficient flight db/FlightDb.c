// Implementation of the Flight DB ADT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FlightDb.h"
#include "List.h"
#include "Tree.h"


struct flightDb {
    Tree byFlightNo;
    Tree byDepAirDay;
    Tree byDepTime; 
};

// Compares two records by flight number
// and then departure information (day, hour, min and airport).
// Returns:
// * - A negative number if the first record is less than the second
// * - Zero if the records are equal
// * - A positive number if the first record is greater than the second
int compareByFlightNo (Record r1, Record r2) {
    char *f1 = RecordGetFlightNumber(r1);
    char *f2 = RecordGetFlightNumber(r2);
    int flightcmp = strcmp(f1, f2);

    if (flightcmp != 0) {
        return flightcmp;
    }

    int depdaycmp = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
    if (depdaycmp != 0) {
        return depdaycmp;
    }

    int dephrcmp = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
    if (dephrcmp != 0) {
        return dephrcmp;
    } 

    int depmincmp = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
    if (depmincmp != 0) {
        return depmincmp;
    } else {
        return strcmp(RecordGetDepartureAirport(r1),RecordGetDepartureAirport(r2));
    }
}

// Compares two records by departure airport
// and then rest of departure information (day, hour and min)
// and finally, compare flight number.
// Returns:
// * - A negative number if the first record is less than the second
// * - Zero if the records are equal
// * - A positive number if the first record is greater than the second
int compareByDepAirDay (Record r1, Record r2) {
    char *depair1 = RecordGetDepartureAirport(r1);
    char *depair2 = RecordGetDepartureAirport(r2);
    int depaircmp = strcmp(depair1, depair2);

    if (depaircmp != 0) {
        return depaircmp;
    }

    int depdaycmp = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
    if (depdaycmp != 0) {
        return depdaycmp;
    }

    int dephrcmp = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
    if (dephrcmp != 0) {
        return dephrcmp;
    }

    int depmincmp = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
    if (depmincmp != 0) {
        return depmincmp;
    } else {
        char *f1 = RecordGetFlightNumber(r1);
        char *f2 = RecordGetFlightNumber(r2);
        int flightcmp = strcmp(f1, f2);
        return flightcmp;
    }
}
// Compares two records by departure information (day, hour and min)
// and then flight number before finally comparing departure airports
// Returns:
// * - A negative number if the first record is less than the second
// * - Zero if the records are equal
// * - A positive number if the first record is greater than the second
int compareByDepTime (Record r1, Record r2) {
    int depdaycmp = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
    if (depdaycmp != 0) {
        return depdaycmp;
    }

    int dephrcmp = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
    if (dephrcmp != 0) {
        return dephrcmp;
    }

    int depmincmp = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
    if (depmincmp != 0) {
        return depmincmp;
    }
    char *f1 = RecordGetFlightNumber(r1);
    char *f2 = RecordGetFlightNumber(r2);
    int flightcmp = strcmp(f1, f2);

    if (flightcmp != 0) {
        return flightcmp;
    } else {
        char *depair1 = RecordGetDepartureAirport(r1);
        char *depair2 = RecordGetDepartureAirport(r2);
        int depaircmp = strcmp(depair1, depair2);
        return depaircmp;
    }
}

// Heavily based off of code from StudentDb.c in lab04
FlightDb DbNew(void) {
    FlightDb db = malloc(sizeof(*db));
    if (db == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    db->byFlightNo = TreeNew(compareByFlightNo);
    db->byDepAirDay = TreeNew(compareByDepAirDay); 
    db->byDepTime = TreeNew(compareByDepTime);
    return db;
}

// Heavily based off of code from StudentDb.c in lab04
void DbFree(FlightDb db) {
    TreeFree(db->byFlightNo, false);
    TreeFree(db->byDepTime, false);
    TreeFree(db->byDepAirDay, true);
    free(db);
}

bool DbInsertRecord(FlightDb db, Record r) {
    if (TreeInsert(db->byFlightNo, r)) {
        TreeInsert(db->byDepAirDay, r);
        TreeInsert(db->byDepTime, r);
        return true;
    } else {
        return false;
    }
}

// Finding functions were based off of code from StudentDb.c in lab04
List DbFindByFlightNumber(FlightDb db, char *flightNumber) {
    Record dummy1 = RecordNew(flightNumber, "0", "0", 0, 0, 0, 0);
    Record dummy2 = RecordNew(flightNumber, "ZZZ", "ZZZ", 6, 23, 59, 1140);
    List l = TreeSearchBetween(db->byFlightNo, dummy1, dummy2);
    RecordFree(dummy1);
    RecordFree(dummy2);
    return l;
}

List DbFindByDepartureAirportDay(FlightDb db, char *departureAirport,
                                 int day) {
    Record dummy1 = RecordNew("0", departureAirport, "0", day, 0, 0, 0);
    Record dummy2 = RecordNew("ZZZZZ", departureAirport, "ZZZ", day, 23, 59, 1140);
    List l = TreeSearchBetween(db->byDepAirDay, dummy1, dummy2);
    RecordFree(dummy1);
    RecordFree(dummy2);
    return l;
}

List DbFindBetweenTimes(FlightDb db, 
                        int day1, int hour1, int min1, 
                        int day2, int hour2, int min2) {
    if (day1 > day2) {
        Record dummy1 = RecordNew("0", "0", "0", day1, hour1, min1, 0);
        Record dummy2 = RecordNew("ZZZZZ", "ZZZ", "ZZZ", 6, 23, 59, 1140);
        List l = TreeSearchBetween(db->byDepTime, dummy1, dummy2);

        Record dummy3 = RecordNew("ZZZZZ", "ZZZ", "ZZZ", 0, 0, 0, 1140);
        Record dummy4 = RecordNew("ZZZZZ", "ZZZ", "ZZZ", day2, hour2, min2, 1140);
        List add = TreeSearchBetween(db->byDepTime, dummy3, dummy4);
        ListExtend(l, add);
        ListFree(add);
        RecordFree(dummy1);
        RecordFree(dummy2);
        RecordFree(dummy3);
        RecordFree(dummy4);
        return l;
    } else {
        Record dummy1 = RecordNew("0", "0", "0", day1, hour1, min1, 0);
        Record dummy2 = RecordNew("ZZZZZ", "ZZZ", "ZZZ", day2, hour2, min2, 1140);
        List l = TreeSearchBetween(db->byDepTime, dummy1, dummy2);
        RecordFree(dummy1);
        RecordFree(dummy2);
        return l;
    }
}

Record DbFindNextFlight(FlightDb db, char *flightNumber, 
                        int day, int hour, int min) {
    Record dummy = RecordNew(flightNumber, "0", "0", day, hour, min, 0);
    Record rec = TreeNext(db->byFlightNo, dummy);
    int flightcmp = strcmp(RecordGetFlightNumber(rec), flightNumber);

    // Wrapping around the week
    if (flightcmp != 0) {
        Record dummy1 = RecordNew(flightNumber, "0", "0", 0, 0, 0, 0);
        Record r1 = TreeNext(db->byFlightNo, dummy1);
        if (strcmp(RecordGetFlightNumber(r1), flightNumber) != 0) {
            RecordFree(dummy1);
            RecordFree(dummy);
            return NULL;
        }
        RecordFree(dummy1);
        RecordFree(dummy);
        return r1;
    }
    RecordFree(dummy);
    return rec;
}
