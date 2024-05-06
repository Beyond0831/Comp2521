
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"
#include "FlightDb.h"
#include "AVLTree.h"

struct flightDb {
	Tree byFlightNumber;
	Tree byDepartureAirport;
	Tree byTime;
};
////////////////////////////////////////////////////////////////////////
// Comparison functions

int comparebyTime(Record r1, Record r2) {
	int DayCmp = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
	if (DayCmp != 0) {
		return DayCmp;
	}
	int HourCmp = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
	if (HourCmp != 0) {
		return HourCmp;
	}
	int MinuteCmp = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
	if (MinuteCmp != 0) {
		return MinuteCmp;
	}
	int byFlightNumber = strcmp(RecordGetFlightNumber(r1), RecordGetFlightNumber(r2));
	return byFlightNumber;
}

int comparebyFlightNumber(Record r1, Record r2) {
	int FlightCmp = strcmp(RecordGetFlightNumber(r1), RecordGetFlightNumber(r2));
	if (FlightCmp != 0) {
		return FlightCmp;
	}
	int DayCmp = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
	if (DayCmp != 0) {
		return DayCmp;
	}
	int HourCmp = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
	if (HourCmp != 0) {
		return HourCmp;
	}
	int MinuteCmp = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
	return MinuteCmp;
}

int comparebyDepartureAirport(Record r1, Record r2) {
	int DepartureCmp = strcmp(RecordGetDepartureAirport(r1), RecordGetDepartureAirport(r2));
	if (DepartureCmp != 0) {
		return DepartureCmp;
	}
	int DayCmp = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
	if (DayCmp != 0) {
		return DayCmp;
	}
	int HourCmp = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
	if (HourCmp != 0) {
		return HourCmp;
	}
	int MinuteCmp = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
	if (MinuteCmp != 0) {
		return MinuteCmp;
	}
	int FlightCmp = strcmp(RecordGetFlightNumber(r1), RecordGetFlightNumber(r2));
	return FlightCmp;
}



/**
 * Creates a new flight DB. 
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
FlightDb DbNew(void) {
	FlightDb db = malloc(sizeof(*db));
	if (db == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}
	db->byFlightNumber = TreeNew(comparebyFlightNumber);
	db->byDepartureAirport = TreeNew(comparebyDepartureAirport);
	db->byTime = TreeNew(comparebyTime);
	return db;
}

/**
 * Frees all memory allocated to the given flight DB
 */
void 	DbFree(FlightDb db) {
	TreeFree(db->byFlightNumber, false);
	TreeFree(db->byDepartureAirport, false);
    TreeFree(db->byTime, true);
    free(db);
}

/**
 * Inserts  a  flight  record  into the given DB if there is not already
 * record with the same flight number, departure airport, day, hour  and
 * minute.
 * If  inserted successfully, this function takes ownership of the given 
 * record (so the caller should not modify or free it). 
 * Returns true if the record was successfully inserted,  and  false  if
 * the  DB  already  contained  a  record  with  the same flight number,
 * departure airport, day, hour and minute.
 * The time complexity of this function must be O(log n).
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
bool     DbInsertRecord(FlightDb db, Record r) {
	if (TreeInsert(db->byFlightNumber, r)) {
		TreeInsert(db->byTime, r);
		TreeInsert(db->byDepartureAirport, r);
	    return true;
    } else {
        return false;
    }
}

/**
 * Searches  for  all  records with the given flight number, and returns
 * them all in a list in increasing order of  (day, hour, min).  Returns
 * an empty list if there are no such records. 
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindByFlightNumber(FlightDb db, char *flightNumber) {
	Record dummy_lower = RecordNew(flightNumber, "", "", 0, 0, 0, '\0');
	Record dummy_upper = RecordNew(flightNumber, "", "", 6, 23, 59, '\0');
	List l = TreeSearchBetween(db->byFlightNumber, dummy_lower, dummy_upper);
	RecordFree(dummy_lower);
    RecordFree(dummy_upper);
    return l;
}

/**
 * Searches  for all records with the given departure airport and day of
 * week (0 to 6), and returns them all in a list in increasing order  of
 * (hour, min, flight number).
 * Returns an empty list if there are no such records.
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindByDepartureAirportDay(FlightDb db, char *departureAirport,
                                     int day) {
	Record dummy_lower = RecordNew("", departureAirport, "", day, 0, 0, '\0');
	Record dummy_upper = RecordNew("", departureAirport, "", day, 23, 59, '\0');
	List l = TreeSearchBetween(db->byDepartureAirport, dummy_lower, dummy_upper);
	RecordFree(dummy_lower);
    RecordFree(dummy_upper);
    return l;
}


/**
 * Searches  for  all  records  between  (day1, hour1, min1)  and (day2,
 * hour2, min2), and returns them all in a list in increasing  order  of
 * (day, hour, min, flight number).
 * Returns an empty list if there are no such records.
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindBetweenTimes(FlightDb db, 
                            int day1, int hour1, int min1, 
                            int day2, int hour2, int min2) {
	Record dummy_lower = RecordNew("", "", "", day1, hour1, min1, '\0');
	Record dummy_upper = RecordNew("zzzzzzzz", "", "", day2, hour2, min2, '\0');
	List l = TreeSearchBetween(db->byTime, dummy_lower, dummy_upper);
	RecordFree(dummy_lower);
    RecordFree(dummy_upper);
    return l;
}

/**
 * Searches  for  and  returns  the  earliest next flight from the given
 * departure airport, on or after the given (day, hour, min), or NULL if
 * there is no such flight.
 * The returned record must not be freed or modified. 
 * The time complexity of this function must be O(log n).
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
Record   DbFindNextFlight(FlightDb db, char *departureAirport, 
                          int day, int hour, int min) {
	Record dummy = RecordNew("", departureAirport, "", day, hour, min, '\0');			  
	Record rec = TreeNext(db->byDepartureAirport, dummy);
	if (rec == NULL) {
		Record NextWeek = RecordNew("", departureAirport, "", 0, 0, 0, '\0');	
		rec = TreeNext(db->byDepartureAirport, NextWeek);
		RecordFree(NextWeek);

	}
	RecordFree(dummy);
	return rec;
}

