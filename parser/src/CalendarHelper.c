/**
 *        Name: Daniel Domalik
 *          ID: 0933553
 */

#include "CalendarHelper.h"

Property* copyProperty(Property* toBeCopied) {
    Property* tmpProp = (Property*)malloc(sizeof(Property) + sizeof(char [strlen(toBeCopied->propDescr)]) + 1);
    
	if (tmpProp == NULL){
		return NULL;
	}

	strcpy(tmpProp->propDescr, toBeCopied->propDescr);
    strcpy(tmpProp->propName, toBeCopied->propName);
	
	return tmpProp;
}

Event* copyEvent(Event* toBeCopied) {
    if ((Event*)toBeCopied == NULL) { return NULL; }

    Event* tmpEvnt = (Event*)malloc(sizeof(Event));

    if (tmpEvnt == NULL) {
        return NULL;
    }

    tmpEvnt->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    memcpy(tmpEvnt->properties, toBeCopied->properties, sizeof(Event*));

    tmpEvnt->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
    memcpy(tmpEvnt->alarms, toBeCopied->alarms, sizeof(Alarm*));
    
    tmpEvnt->creationDateTime = toBeCopied->creationDateTime;
    tmpEvnt->startDateTime = toBeCopied->startDateTime;

    strcpy(tmpEvnt->UID, toBeCopied->UID);

    return tmpEvnt;
}

Alarm* copyAlarm(Alarm* toBeCopied) {
    Alarm* tmpAlarm = (Alarm*)malloc(sizeof(Alarm));

    if (tmpAlarm == NULL) {
        return NULL;
    }

    tmpAlarm->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    memcpy(tmpAlarm->properties, toBeCopied->properties, sizeof(Alarm*));
    
    strcpy(tmpAlarm->action, toBeCopied->action);

    tmpAlarm->trigger = malloc(sizeof(char) * strlen(toBeCopied->trigger) + 1);
    strcpy(tmpAlarm->trigger, toBeCopied->trigger);

    return tmpAlarm;
}

DateTime copyDate(DateTime* toBeCopied) {
    DateTime* tmpDt = malloc(sizeof(DateTime));

    strcpy(tmpDt->date, toBeCopied->date);
    strcpy(tmpDt->time, toBeCopied->time);
    tmpDt->UTC = toBeCopied->UTC;

    DateTime date = *tmpDt;

    return date;
}