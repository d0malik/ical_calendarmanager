#ifndef CALENDARHELPER_H
#define CALENDARHELPER_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "CalendarParser.h"

char* fileToCalToJSON(char* fileName);
char* fileToEventsToJSON(char* fileName);
char* alarmToJSON(const Alarm* alarm);
char* alarmListToJSON(const List* alarmList);
char* fileToAlarmsToJSON(char* fileName, int eventNum);
char* propToJSON(const Property* prop);
char* propListToJSON(const List* propList);
char* fileToPropsToJSON(char* fileName, int eventNum);
char* createCalFromJSON(char* fileName, char* cal_info, char* uid, char* dtstart, char* _dtstart, char* dtstamp, char* _dtstamp, char* summary);
char* addEventFromJSON(char* fileName, char* uid, char* dtstart, char* _dtstart, char* dtstamp, char* _dtstamp, char* summary);

Property* copyProperty(Property* toBeCopied);
Event* copyEvent(Event* toBeCopied);
Alarm* copyAlarm(Alarm* toBeCopied);
DateTime copyDate(DateTime* toBeCopied);

#endif	
