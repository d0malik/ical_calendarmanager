/**
 *        Name: Daniel Domalik
 *          ID: 0933553
 */

#include "CalendarParser.h"
#include "CalendarHelper.h"

// ************* A4 functionality - MUST be implemented ***************
char* findOrganizer(char* fileName, int eventNum) {
    Calendar* cal;
    char * cal_string;
    char * code = printError(createCalendar(fileName, &cal));
    if (strcmp(code, "OK") == 0) {
        if (validateCalendar(cal) == OK) {
            ListIterator event_iter = createIterator((List*)cal->events);
            Event* event;
            int i = 0;
            while((event = nextElement(&event_iter)) != NULL) {
                if (i == eventNum) { break; }
                i++;
            }
            ListIterator prop_iter = createIterator((List*)event->properties);
            Property* prop;
            while((prop = nextElement(&prop_iter)) != NULL) {
                if (strcmp(prop->propName, "ORGANIZER") == 0) {
                    cal_string = prop->propDescr;
                }
            }
        }else{
            cal_string = "";
        }
    }else{
        cal_string = "";
    }
    free(code);

    return (cal_string);
}

char* findLocation(char* fileName, int eventNum) {
    Calendar* cal;
    char * cal_string;
    char * code = printError(createCalendar(fileName, &cal));
    if (strcmp(code, "OK") == 0) {
        if (validateCalendar(cal) == OK) {
            ListIterator event_iter = createIterator((List*)cal->events);
            Event* event;
            int i = 0;
            while((event = nextElement(&event_iter)) != NULL) {
                if (i == eventNum) { break; }
                i++;
            }
            ListIterator prop_iter = createIterator((List*)event->properties);
            Property* prop;
            while((prop = nextElement(&prop_iter)) != NULL) {
                if (strcmp(prop->propName, "LOCATION") == 0) {
                    cal_string = prop->propDescr;
                }
            }
        }else{
            cal_string = "";
        }
    }else{
        cal_string = "";
    }
    free(code);

    return (cal_string);
}
// ********************************************************************

// ************* A3 functionality - MUST be implemented ***************
char* fileToCalToJSON(char* fileName) {
    Calendar* cal;
    char * cal_string;
    char * code = printError(createCalendar(fileName, &cal));
    if (strcmp(code, "OK") == 0) {
        if (validateCalendar(cal) == OK) {
            cal_string = calendarToJSON(cal);
            deleteCalendar(cal);
        }else{
            cal_string = "{\"version\":\"\",\"prodID\":\"invalid file\",\"numProps\":\"\",\"numEvents\":\"\"}";
        }
    }else{
        cal_string = "{\"version\":\"\",\"prodID\":\"invalid file\",\"numProps\":\"\",\"numEvents\":\"\"}";
    }
    free(code);

    return (cal_string);
}

char* fileToEventsToJSON(char* fileName) {
    Calendar* cal;
    char * cal_string;
    char * code = printError(createCalendar(fileName, &cal));
    if (strcmp(code, "OK") == 0) {
        if (validateCalendar(cal) == OK) {
            cal_string = eventListToJSON(cal->events);
            deleteCalendar(cal);
        }else{
            cal_string = "";
        }
    }else{
        cal_string = "";
    }
    free(code);

    return (cal_string);
}

// {"action":action,"trigger":trigger,"duration":duration,"repeat":"repeat","numProps":3,"props":[props]}
char* alarmToJSON(const Alarm* alarm) {
    if (alarm == NULL) { return "{}"; }

    char* duration = '\0';
    char* repeat = '\0';
    int props = 2;
    char* jsonList = '\0';
    jsonList = (char*)malloc(sizeof(char) * strlen("{}") + 1);
    strcpy(jsonList, "");
    int flag = 0;

    ListIterator prop_iter = createIterator(alarm->properties);
    Property* prop;
    while((prop = nextElement(&prop_iter)) != NULL) {
        if (strcmp(prop->propName, "DURATION") == 0) {
            duration = prop->propDescr;
        }else if (strcmp(prop->propName, "REPEAT") == 0) {
            repeat = prop->propDescr;
        }else{
            char * propJSON = propToJSON(prop);
            if (flag == 0) {
                free(jsonList);
                jsonList = (char*)malloc(sizeof(char) * strlen(propJSON) + 4);
                strcpy(jsonList, propJSON);
                jsonList[strlen(jsonList)] = '\0';
                flag = 1;
            }else{
                strcat(jsonList, ",");
                jsonList = (char*)realloc(jsonList, ((sizeof(char) * strlen(jsonList) + 4) + (sizeof(char) * strlen(propJSON) + 4)));
                strcat(jsonList, propJSON);
                jsonList[strlen(jsonList)] = '\0';
            }
        free(propJSON);
        }
        props++;
    }

    int newLen = (sizeof(char) * strlen(duration) + 1) + (sizeof(char) * strlen(repeat) + 1) + (sizeof(char) * strlen(jsonList) + 1) + 
                (((sizeof(char) * strlen("\"\"")) + 1) * 4) + ((sizeof(char) * 128) + 1) + (sizeof(int) * 2);
    char* json = malloc(newLen);
    snprintf(json, newLen,
            "{\"action\":\"%s\",\"trigger\":\"%s\",\"duration\":\"%s\",\"repeat\":\"%s\",\"numProps\":%d,\"addProps\":[%s]}",
            alarm->action, alarm->trigger, duration == NULL ? "" : duration, repeat == NULL ? "" : repeat, props, jsonList);
    return json;
}

// [{"action":action,"trigger":trigger,"duration":duration,"repeat":"repeat","numProps":3},
//  {"action":action,"trigger":trigger,"duration":duration,"repeat":"repeat","numProps":3},
//  {"action":action,"trigger":trigger,"duration":duration,"repeat":"repeat","numProps":3}]
char* alarmListToJSON(const List* alarmList) {
    if ((List*)alarmList == NULL) { return "[]"; }

    char* jsonList;
    int flag = 0;
    ListIterator alarm_iter = createIterator((List*)alarmList);
    Alarm* alarm;
    while((alarm = nextElement(&alarm_iter)) != NULL) {
        char * alarmJSON = alarmToJSON(alarm);
        if (flag == 0) {
            jsonList = (char*)malloc(sizeof(char) * strlen(alarmJSON) + 8);
            strcpy(jsonList, alarmJSON);
            jsonList[strlen(jsonList)] = '\0';
            flag = 1;
        }else{
            jsonList = (char*)realloc(jsonList, ((sizeof(char) * strlen(jsonList) + 8) + (sizeof(char) * strlen(alarmJSON) + 8)));
            strcat(jsonList, alarmJSON);
            jsonList[strlen(jsonList)] = '\0';
        }
        strcat(jsonList, ",");
        free(alarmJSON);
    }

    jsonList[strlen(jsonList) - 1] = '\0';
    char* json = malloc((sizeof(char) * strlen(jsonList) + 8));
    snprintf(json, (sizeof(char) * strlen(jsonList) + 8), "[%s]", jsonList);
    free(jsonList);
    return json;
}

char* fileToAlarmsToJSON(char* fileName, int eventNum) {
    Calendar* cal;
    char * cal_string;
    char * code = printError(createCalendar(fileName, &cal));
    if (strcmp(code, "OK") == 0) {
        if (validateCalendar(cal) == OK) {
            ListIterator event_iter = createIterator((List*)cal->events);
            Event* event;
            int i = 0;
            while((event = nextElement(&event_iter)) != NULL) {
                if (i == eventNum) { break; }
                i++;
            }
            if (event->alarms != NULL) {
                cal_string = alarmListToJSON(event->alarms);
            }
            deleteCalendar(cal);
        }else{
            cal_string = "";
        }
    }else{
        cal_string = "";
    }
    free(code);

    return (cal_string);
}

// {"propName":"val","propDescr":"val"}
char* propToJSON(const Property* prop) {
    if (prop == NULL) { return "{}"; }

    int newLen = (sizeof(char) * strlen(prop->propName) + 1) + (sizeof(char) * strlen(prop->propDescr) + 1) + 
                (((sizeof(char) * strlen("\"\"")) + 1) * 4) + ((sizeof(char) * 32) + 1) + (sizeof(int) * 2);
    char* json = malloc(newLen);
    snprintf(json, newLen, "{\"propName\":\"%s\",\"propDescr\":\"%s\"}", prop->propName, prop->propDescr);
    return json;
}

// [{"propName": "val", "propDescr":"val"},
//  {"propName": "val", "propDescr":"val"},
//  {"propName": "val", "propDescr":"val"}]
char* propListToJSON(const List* propList) {
    if ((List*)propList == NULL) { return "[]"; }

    char* jsonList;
    int flag = 0;
    ListIterator prop_iter = createIterator((List*)propList);
    Property* prop;
    while((prop = nextElement(&prop_iter)) != NULL) {
        char * propJSON = propToJSON(prop);
        if (flag == 0) {
            jsonList = (char*)malloc(sizeof(char) * strlen(propJSON) + 4);
            strcpy(jsonList, propJSON);
            jsonList[strlen(jsonList)] = '\0';
            flag = 1;
        }else{
            jsonList = (char*)realloc(jsonList, ((sizeof(char) * strlen(jsonList) + 4) + (sizeof(char) * strlen(propJSON) + 4)));
            strcat(jsonList, propJSON);
            jsonList[strlen(jsonList)] = '\0';
        }
        strcat(jsonList, ",");
        free(propJSON);
    }

    jsonList[strlen(jsonList) - 1] = '\0';
    char* json = malloc((sizeof(char) * strlen(jsonList) + 4));
    snprintf(json, (sizeof(char) * strlen(jsonList) + 4), "[%s]", jsonList);
    free(jsonList);
    return json;
}

char* fileToPropsToJSON(char* fileName, int eventNum) {
    Calendar* cal;
    char * cal_string;
    char * code = printError(createCalendar(fileName, &cal));
    if (strcmp(code, "OK") == 0) {
        if (validateCalendar(cal) == OK) {
            ListIterator event_iter = createIterator((List*)cal->events);
            Event* event;
            int i = 0;
            while((event = nextElement(&event_iter)) != NULL) {
                if (i == eventNum) { break; }
                i++;
            }
            if (event->alarms != NULL) {
                cal_string = propListToJSON(event->properties);
            }
            deleteCalendar(cal);
        }else{
            cal_string = "";
        }
    }else{
        cal_string = "";
    }
    free(code);

    return (cal_string);
}

char* createCalFromJSON(char* fileName, char* cal_info, char* uid, char* dtstart, char* _dtstart, char* dtstamp, char* _dtstamp, char* summary) {
    Calendar* cal = JSONtoCalendar(cal_info);
    Event* event = (Event*)malloc(sizeof(Event));
    if (event == NULL) {
        return "OTHER_ERROR";
    }
    event->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    event->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
    strcpy(event->UID, uid);
    DateTime* startdt = (DateTime*)malloc(sizeof(DateTime));
    strncpy(startdt->date, dtstart, 9); 
    strncpy(startdt->time, _dtstart, 7);
    DateTime* stampdt = (DateTime*)malloc(sizeof(DateTime));
    strncpy(stampdt->date, dtstamp, 9); 
    strncpy(stampdt->time, _dtstamp, 7);
    event->startDateTime = *startdt;
    event->creationDateTime = *stampdt;
    addEvent(cal, event);
    free(event);
    free(startdt);
    free(stampdt);
    char* code = printError(writeCalendar(fileName, cal));
    return code;
}

char* addEventFromJSON(char* fileName, char* uid, char* dtstart, char* _dtstart, char* dtstamp, char* _dtstamp, char* summary) {
    if (strlen(summary) > 199) {
        return "ERROR";
    }

    Calendar* cal;
    char* temp_code = printError(createCalendar(fileName, &cal));
    if (strcmp(temp_code, "OK") != 0) {
        return temp_code;
    }

    ListIterator iter = createIterator(cal->events);
    Event* event1;
    while((event1 = nextElement(&iter)) != NULL) {
        if (strcmp(event1->UID, uid) == 0) {
            return "UID";
        }
    }

    Event* event = (Event*)malloc(sizeof(Event));
    if (event == NULL) {
        return "OTHER_ERROR";
    }
    event->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    event->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
    strcpy(event->UID, uid);
    DateTime* startdt = (DateTime*)malloc(sizeof(DateTime));
    strncpy(startdt->date, dtstart, 9); 
    strncpy(startdt->time, _dtstart, 7);
    DateTime* stampdt = (DateTime*)malloc(sizeof(DateTime));
    strncpy(stampdt->date, dtstamp, 9); 
    strncpy(stampdt->time, _dtstamp, 7);
    event->startDateTime = *startdt;
    event->creationDateTime = *stampdt;
    if (strcmp(summary, "") != 0) {
        Property* prop = malloc(sizeof(*prop) + (sizeof(char [strlen("SUMMARY")]) + 1) + (sizeof(char [strlen(summary)]) + 1));
        strcpy(prop->propName, "SUMMARY");
        strcpy(prop->propDescr, summary);
        insertBack(event->properties, copyProperty(prop));
        free(prop);
    }
    addEvent(cal, event);
    char* code = printError(writeCalendar(fileName, cal));
    return code;
}
// ********************************************************************

// ************* A2 functionality - MUST be implemented ***************
ICalErrorCode writeCalendar(char* fileName, const Calendar* obj) {
    if (obj == NULL) {
        return WRITE_ERROR; // INV_CAL (null Calendar object)
    }else if (fileName == NULL) {
        return WRITE_ERROR; // INV_SMTHG (null fileName string)
    }else if (strcmp(fileName, "") == 0) {
        return WRITE_ERROR;
    }

    FILE *out;
    out = fopen(fileName, "w+");
    if (out == NULL) {
        return WRITE_ERROR;
    }else{
        fprintf(out, "BEGIN:VCALENDAR\r\n");
        fprintf(out, "VERSION:%f\r\n", obj->version);
        fprintf(out, "PRODID:%s\r\n", obj->prodID);

        // PROPERTIES
        if (obj->properties != NULL) {
            ListIterator iter = createIterator(obj->properties);
            Property* prop;
            while((prop = nextElement(&iter)) != NULL){
                fprintf(out, "%s:%s\r\n", prop->propName, prop->propDescr);
            }
        }

        // EVENTS
        if (obj->events != NULL) {
            ListIterator event_iter = createIterator(obj->events);
            Event *event;
            while ((event = nextElement(&event_iter)) != NULL) {
                fprintf(out, "BEGIN:VEVENT\r\n");

                char * event_create = printDate(&event->creationDateTime);
                char * event_start = printDate(&event->startDateTime);
                fprintf(out, "UID:%s\r\n", event->UID);
                fprintf(out, "DTSTAMP:%s\r\n", event_create);
                fprintf(out, "DTSTART:%s\r\n", event_start);
                free(event_create);
                free(event_start);
                
                ListIterator event_prop_iter = createIterator(event->properties);
                Property* event_prop;
                while((event_prop = nextElement(&event_prop_iter)) != NULL) {
                    int flag = 0;
                    for (int i = 0; i < 10; i++) { if (strlen(event_prop->propDescr) <= i) { break; } if (event_prop->propDescr[i] == '=') { flag = 1; break; } }
                    if (flag == 1) {
                        fprintf(out, "%s;%s\r\n", event_prop->propName, event_prop->propDescr);
                    }else{
                        fprintf(out, "%s:%s\r\n", event_prop->propName, event_prop->propDescr);
                    }
                }

                ListIterator event_alarm_iter = createIterator(event->alarms);
                Alarm* alarm;
                while((alarm = nextElement(&event_alarm_iter)) != NULL) {
                    fprintf(out, "BEGIN:VALARM\r\n");

                    int action_flag = 0;
                    for (int i = 0; i < 10; i++) { if (strlen(alarm->action) <= i) { break; } if (alarm->action[i] == '=') { action_flag = 1; break; } }
                    if (action_flag == 1) {
                        fprintf(out, "ACTION;%s\r\n", alarm->action);
                    }else{
                        fprintf(out, "ACTION:%s\r\n", alarm->action);
                    }

                    int trigger_flag = 0;
                    for (int i = 0; i < 10; i++) { if (strlen(alarm->trigger) <= i) { break; } if (alarm->trigger[i] == '=') { trigger_flag = 1; break; } }
                    if (trigger_flag == 1) {
                        fprintf(out, "TRIGGER;%s\r\n", alarm->trigger);
                    }else{
                        fprintf(out, "TRIGGER:%s\r\n", alarm->trigger);
                    }

                    ListIterator alarm_prop_iter = createIterator(alarm->properties);
                    Property* alarm_prop;
                    while((alarm_prop = nextElement(&alarm_prop_iter)) != NULL) {
                        int alarm_prop_flag = 0;
                        for (int i = 0; i < 10; i++) { if (strlen(alarm_prop->propDescr) <= i) { break; } if (alarm_prop->propDescr[i] == '=') { alarm_prop_flag = 1; break; } }
                        if (alarm_prop_flag == 1) {
                            fprintf(out, "%s;%s\r\n", alarm_prop->propName, alarm_prop->propDescr);
                        }else{
                            fprintf(out, "%s:%s\r\n", alarm_prop->propName, alarm_prop->propDescr);
                        }
                    }
                    fprintf(out, "END:VALARM\r\n");
                }

                fprintf(out, "END:VEVENT\r\n");
            }
        }

        fprintf(out, "END:VCALENDAR\r\n");
    }

    fclose(out);
    return OK;
}

ICalErrorCode validateCalendar(const Calendar* obj) {
    if (obj == NULL) {
        return INV_CAL;
    }else if (!obj->version) {
        return INV_CAL;
    }else if (obj->prodID == NULL) {
        return INV_CAL;
    }else if (strcmp(obj->prodID, "") == 0) {
        return INV_CAL;
    }else if (strlen(obj->prodID) > 999) {
        return INV_CAL;
    }else if (obj->properties == NULL) {
        return INV_CAL;
    }else{
        ListIterator iter = createIterator(obj->properties);
        Property* prop;
        while((prop = nextElement(&iter)) != NULL) {
            if (strlen(prop->propName) > 199) {
                return INV_CAL;
            }
            if (strcmp(prop->propName, "CALSCALE") == 0 ||
                strcmp(prop->propName, "METHOD") == 0) { }
            else{
                return INV_CAL;    
            }
            if (prop->propDescr == NULL) {
                return INV_CAL;
            }else if (strcmp(prop->propDescr, "") == 0) {
                return INV_CAL;
            }
        }
    }

    if (obj->events == NULL) {
        return INV_CAL;
    }else{
        ListIterator event_iter = createIterator(obj->events);
        Event *event;
        while ((event = nextElement(&event_iter)) != NULL) {
            if (event->UID == NULL) {
                return INV_EVENT;
            }else if (strcmp(event->UID, "") == 0) {
                return INV_EVENT;
            }else if (strlen(event->UID) > 999) {
                return INV_EVENT;
            }else if (&event->creationDateTime == NULL) {
                return INV_EVENT;
            }else if (&event->startDateTime == NULL) {
                return INV_EVENT;
            }

            char * event_create = printDate(&event->creationDateTime);
            char * event_start = printDate(&event->startDateTime);
            char * token = strtok(event_create, "T");
            if (strlen(token) != 8) {
                return INV_EVENT; // inv date?
            }else{
                token = strtok(NULL, "T");
                if (token[strlen(token) - 1] == 'Z') {
                    token[strlen(token) - 1] = '\0';
                }
                if (strlen(token) != 6) {
                    return INV_EVENT; // inv date?
                }
            }
            token = strtok(event_start, "T");
            if (strlen(token) != 8) {
                return INV_EVENT; // inv date?
            }else{
                token = strtok(NULL, "T");
                if (token[strlen(token) - 1] == 'Z') {
                    token[strlen(token) - 1] = '\0';
                }
                if (strlen(token) != 6) {
                    return INV_EVENT; // inv date?
                }
            }
            free(event_create);
            free(event_start);

            if (event->properties == NULL) {
                return INV_EVENT;
            }else{
                ListIterator event_prop_iter = createIterator(event->properties);
                Property* event_prop;
                int dtend, duration;
                dtend = duration = 0;
                while((event_prop = nextElement(&event_prop_iter)) != NULL) {
                    if (strlen(event_prop->propName) > 199) {
                        return INV_EVENT;
                    }
                    if (strcmp(event_prop->propName, "DTEND") == 0) {
                        if (duration == 1) {
                            return INV_EVENT;
                        }
                        dtend = 1;
                    }else if (strcmp(event_prop->propName, "DURATION") == 0) {
                        if (dtend == 1) {
                            return INV_EVENT;
                        }
                        duration = 1;
                    }else{
                        if ((strcmp(event_prop->propName, "CLASS")         == 0) ||
                            (strcmp(event_prop->propName, "CREATED")       == 0) ||
                            (strcmp(event_prop->propName, "DESCRIPTION")   == 0) ||
                            (strcmp(event_prop->propName, "GEO")           == 0) ||
                            (strcmp(event_prop->propName, "LAST-MODIFIED") == 0) ||
                            (strcmp(event_prop->propName, "LOCATION")      == 0) ||
                            (strcmp(event_prop->propName, "ORGANIZER")     == 0) ||
                            (strcmp(event_prop->propName, "PRIORITY")      == 0) ||
                            (strcmp(event_prop->propName, "SEQUENCE")      == 0) ||
                            (strcmp(event_prop->propName, "STATUS")        == 0) ||
                            (strcmp(event_prop->propName, "SUMMARY")       == 0) ||
                            (strcmp(event_prop->propName, "TRANSP")        == 0) ||
                            (strcmp(event_prop->propName, "URL")           == 0) ||
                            (strcmp(event_prop->propName, "RECURID")       == 0) ||
                            (strcmp(event_prop->propName, "RRULE")         == 0)) {
                            int c = 0;
                            ListIterator itr = createIterator(event->properties);
                            Property* data = nextElement(&itr);
                            while (data != NULL) {
                                if (strcmp(data->propName, event_prop->propName) == 0) { c++; }
                                data = nextElement(&itr);
                            }
                            if (c > 1) { return INV_EVENT; }
                        }else{
                            if ((strcmp(event_prop->propName, "ATTACH")     == 0) ||
                                (strcmp(event_prop->propName, "ATTENDEE")   == 0) ||
                                (strcmp(event_prop->propName, "CATEGORIES") == 0) ||
                                (strcmp(event_prop->propName, "COMMENT")    == 0) ||
                                (strcmp(event_prop->propName, "CONTACT")    == 0) ||
                                (strcmp(event_prop->propName, "EXDATE")     == 0) ||
                                (strcmp(event_prop->propName, "RSTATUS")    == 0) ||
                                (strcmp(event_prop->propName, "RELATED")    == 0) ||
                                (strcmp(event_prop->propName, "RESOURCES")  == 0) ||
                                (strcmp(event_prop->propName, "RDATE")      == 0)) { }
                            else{
                                return INV_EVENT;
                            }
                        }
                    }

                    if (event_prop->propDescr == NULL) {
                        return INV_EVENT;
                    }else if (strcmp(event_prop->propDescr, "") == 0) {
                        return INV_EVENT;
                    }
                }
            }

            if (event->alarms == NULL) {
                return INV_EVENT;
            }else{
                ListIterator event_alarm_iter = createIterator(event->alarms);
                Alarm* alarm;
                while((alarm = nextElement(&event_alarm_iter)) != NULL) {
                    if (alarm->action == NULL) {
                        return INV_ALARM;
                    }else if (strcmp(alarm->action, "") == 0) {
                        return INV_ALARM;
                    }else if (strcmp(alarm->action, "AUDIO") != 0) {
                        return INV_ALARM;
                    }else if (alarm->trigger == NULL) {
                        return INV_ALARM;
                    }else if (strcmp(alarm->trigger, "") == 0) {
                        return INV_ALARM;
                    }

                    if (alarm->properties == NULL) {
                        return INV_ALARM;
                    }else{
                        ListIterator alarm_prop_iter = createIterator(alarm->properties);
                        Property* alarm_prop;
                        int duration, repeat;
                        duration = repeat = 0;
                        while((alarm_prop = nextElement(&alarm_prop_iter)) != NULL) {
                            if (strlen(alarm_prop->propName) > 199) {
                                return INV_ALARM;
                            }
                            if (strcmp(alarm_prop->propName, "REPEAT") == 0) {
                                repeat++;
                            }else if (strcmp(alarm_prop->propName, "DURATION") == 0) {
                                duration++;
                            }else{
                                if ((strcmp(alarm_prop->propName, "ATTACH") == 0)) {
                                    int c = 0;
                                    ListIterator itr = createIterator(alarm->properties);
                                    Property* data = nextElement(&itr);
                                    while (data != NULL) {
                                        if (strcmp(data->propName, alarm_prop->propName) == 0) { c++; }
                                        data = nextElement(&itr);
                                    }
                                    if (c > 1) { return INV_ALARM; }
                                }else{
                                    return INV_ALARM;
                                }
                            }

                            if (alarm_prop->propDescr == NULL) {
                                return INV_ALARM;
                            }else if (strcmp(alarm_prop->propDescr, "") == 0) {
                                return INV_ALARM;
                            }
                        }

                        if (duration > 1 || repeat > 1) {
                            return INV_ALARM;
                        }else{
                            if ((duration == 1 && repeat == 0) || (duration == 0 && repeat == 1)) {
                                return INV_ALARM;
                            }
                        }
                    }
                }
            }
        }
    }

    return OK;
}

// {"date":"19540203","time":"123012","isUTC":false}
char* dtToJSON(DateTime prop) {
    int newLen = (sizeof(char) * 9) + (sizeof(char) * 7) + (sizeof(int)) + ((sizeof(char) * 32) + 1);
    char* json = malloc(newLen);
    snprintf(json, newLen, "{\"date\":\"%s\",\"time\":\"%s\",\"isUTC\":%s}", (&prop)->date, (&prop)->time, (&prop)->UTC == 1 ? "true" : "false");
    return json;
}

// {"startDT":DTval,"numProps":propVal,"numAlarms":almVal,"summary":"sumVal"}
char* eventToJSON(const Event* event) {
    if (event == NULL) { return "{}"; }

    char* summary = '\0';

    int props = 3;
    ListIterator prop_iter = createIterator(event->properties);
    Property* prop;
    while((prop = nextElement(&prop_iter)) != NULL) {
        if (strcmp(prop->propName, "SUMMARY") == 0) {
            summary = prop->propDescr;
        }
        props++;
    }

    int alarms = 0;
    ListIterator alarms_iter = createIterator(event->alarms);
    Alarm* alarm;
    while((alarm = nextElement(&alarms_iter)) != NULL) { alarms++; }

    char* start_date = dtToJSON(event->startDateTime);
    int newLen;
    if (summary) {
        newLen = ((sizeof(char) * strlen(start_date)) + 1) + ((sizeof(char) * strlen(summary)) + 1) + ((sizeof(char) * 64) + 1) + (sizeof(int) * 2);
    }else{
        newLen = ((sizeof(char) * strlen(start_date)) + 1) + ((sizeof(char) * strlen("\"\"")) + 1) + ((sizeof(char) * 64) + 1) + (sizeof(int) * 2);
    }
    char* json = malloc(newLen);
    snprintf(json, newLen,
            "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\"}",
            start_date, props, alarms, summary == NULL ? "" : summary);
    free(start_date);
    return json;
}

//[{"startDT":{"date":"19540203","time":"123012","isUTC":true},"numProps":3,"numAlarms":0,"summary":""},
//{"startDT":{"date":"19540203","time":"123012","isUTC":true},
//"numProps":4,"numAlarms":2,"summary":"Do taxes"}]
char* eventListToJSON(const List* eventList) {
    if ((List*)eventList == NULL) { return "[]"; }

    char* jsonList;
    int flag = 0;
    ListIterator event_iter = createIterator((List*)eventList);
    Event* event;
    while((event = nextElement(&event_iter)) != NULL) {
        char * eventJSON = eventToJSON(event);
        if (flag == 0) {
            jsonList = (char*)malloc(sizeof(char) * strlen(eventJSON) + 4);
            strcpy(jsonList, eventJSON);
            jsonList[strlen(jsonList)] = '\0';
            flag = 1;
        }else{
            jsonList = (char*)realloc(jsonList, ((sizeof(char) * strlen(jsonList) + 4) + (sizeof(char) * strlen(eventJSON) + 4)));
            strcat(jsonList, eventJSON);
            jsonList[strlen(jsonList)] = '\0';
        }
        strcat(jsonList, ",");
        free(eventJSON);
    }

    jsonList[strlen(jsonList) - 1] = '\0';
    char* json = malloc((sizeof(char) * strlen(jsonList) + 4));
    snprintf(json, (sizeof(char) * strlen(jsonList) + 4), "[%s]", jsonList);
    free(jsonList);
    return json;
}

char* calendarToJSON(const Calendar* cal) {
    if ((Calendar*)cal == NULL) { return "{}"; }

    ListIterator event_iter = createIterator(cal->events);
    Event* event;
    int events = 0;
    while((event = nextElement(&event_iter)) != NULL) { events++; }

    if (events == 0) { return "{}"; }

    ListIterator prop_iter = createIterator(cal->properties);
    Property* prop;
    int props = 2;
    while((prop = nextElement(&prop_iter)) != NULL) {
        props++;
    }

    int newLen = (sizeof(char) * strlen(cal->prodID) + 1) + (sizeof(char) * 64) + (sizeof(int) * 2);
    char* json = malloc(newLen);
    snprintf(json, newLen,
            "{\"version\":%d,\"prodID\":\"%s\",\"numProps\":%d,\"numEvents\":%d}",
            ((int)cal->version), cal->prodID, props, events);
    return json;
}

// {"version":2,"prodID":"-//hacksw/handcal//NONSGML v1.0//EN"}
Calendar* JSONtoCalendar(const char* str) {
    if (str == NULL) { return NULL; }
    if (strcmp(str, "") == 0) { return NULL; }

    Calendar* cal = malloc(sizeof(Calendar));
    float version = 0.0;

    for (int i = 0; i < strlen(str); i++) {
        char* temp = malloc(sizeof(char) * strlen(str) + 1);
        if (temp == NULL) { return NULL; }
        strcpy(temp, "");

        int old = 0;
        if (str[i] == '"') {
            i++;
            old = i;
            while (str[i] != '"') {
                i++;
            }
        }

        if (old != 0 && old != i) {
            int x, y;
            x = old;
            y = 0;

            while (x != i) {
                temp[y] = str[x];
                x++;
                y++;
            }
            temp[y] = '\0';

            if (strcmp(temp, "version") == 0) {
                i += 2;
                old = i;
                while (str[i] != ',') { i++; }
                char* temp_version = malloc(sizeof(char) * i + 1);
                if (temp_version == NULL) { free(temp); return NULL; }

                i = old;
                int j = 0;
                while (str[i] != ',') {
                    temp_version[j] = str[i];
                    i++;
                    j++;
                }
                temp_version[j] = '\0';
                strcat(temp_version, ".0\0");
                version = atof(temp_version);
                free(temp_version);
            }

            if (strcmp(temp, "prodID") == 0) {
                i += 2;
                old = i;
                while (str[i + 1] != '"') { i++; }
                char* temp_prodID = malloc(sizeof(char) * i + 1);
                if (temp_prodID == NULL) { free(temp); return NULL; }

                i = old;
                int j = 0;
                while (str[i + 1] != '"') {
                    temp_prodID[j] = str[i + 1];
                    i++;
                    j++;
                }
                i++;
                temp_prodID[j] = '\0';
                strcpy(cal->prodID, temp_prodID);
                free(temp_prodID);
            }
        }

        free(temp);
    }

    cal->version = version;

    cal->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
    cal->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);

    return cal;
}

Event* JSONtoEvent(const char* str) {
    if (str == NULL) { return NULL; }
    if (strcmp(str, "") == 0) { return NULL; }

    Event* event = malloc(sizeof(Event));

    for (int i = 0; i < strlen(str); i++) {
        char* temp = malloc(sizeof(char) * strlen(str) + 1);
        if (temp == NULL) { return NULL; }
        strcpy(temp, "");

        int old = 0;
        if (str[i] == '"') {
            i++;
            old = i;
            while (str[i] != '"') {
                i++;
            }
        }

        if (old != 0 && old != i) {
            int x, y;
            x = old;
            y = 0;

            while (x != i) {
                temp[y] = str[x];
                x++;
                y++;
            }
            temp[y] = '\0';

            if (strcmp(temp, "UID") == 0) {
                i += 2;
                old = i;
                while (str[i + 1] != '"') { i++; }
                char* temp_UID = malloc(sizeof(char) * i + 1);
                if (temp_UID == NULL) { free(temp); return NULL; }

                i = old;
                int j = 0;
                while (str[i + 1] != '"') {
                    temp_UID[j] = str[i + 1];
                    i++;
                    j++;
                }
                i++;
                temp_UID[j] = '\0';
                strcpy(event->UID, temp_UID);
                free(temp_UID);
            }
        }

        free(temp);
    }

    event->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
    event->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);

    return event;
}

void addEvent(Calendar* cal, Event* toBeAdded) {
    if (cal == NULL) { return; }
    if (toBeAdded == NULL) { return; }
    insertBack(cal->events, copyEvent(toBeAdded));
}
// ********************************************************************



// ************* A1 functionality - MUST be implemented ***************
ICalErrorCode createCalendar(char* fileName, Calendar** obj) {
    // CALENDAR
    Calendar* temp_cal = malloc(sizeof(Calendar));
    // CALENDAR Property List
    (temp_cal)->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    // CALENDAR Event List
    (temp_cal)->events = initializeList(&printEvent, &deleteEvent, &compareEvents);

    FILE *in;
    // INV_FILE Checks
    if (!fileName) {
        free(temp_cal->properties);
        free(temp_cal->events);
        free(temp_cal);
        return INV_FILE;
    }else{
        char ext[64];
        char * token;
        strcpy(ext, fileName);
        token = strtok(ext, ".");
        token = strtok(NULL, ".");
        if (token == NULL) {
            free(temp_cal->properties);
            free(temp_cal->events);
            free(temp_cal);
            return INV_FILE;
        }
        if (strcmp(token, "ics") != 0) {
            free(temp_cal->properties);
            free(temp_cal->events);
            free(temp_cal);
            return INV_FILE;
        }else{
            in = fopen(fileName, "r");
            if (in == NULL) {
                free(temp_cal->properties);
                free(temp_cal->events);
                free(temp_cal);
                return INV_FILE;
            }else{
                char buffer[255];
                buffer[0] = '\0';
                while (fgets(buffer, sizeof(buffer), in));
                if (strlen(buffer) > 0) {
                    if (buffer[strlen(buffer) - 2] == '\r' && buffer[strlen(buffer) - 1] == '\n') {
                        rewind(in);
                        (temp_cal)->version = -1.0;
                        strcpy((temp_cal)->prodID, "");
                    }else{
                        free(temp_cal->properties);
                        free(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_FILE;
                    }
                }else{
                    free(temp_cal->properties);
                    free(temp_cal->events);
                    free(temp_cal);
                    fclose(in);
                    return INV_FILE;
                }
            }
        }
    }

    char buffer[2048]; // buffer for file reading
    char fold[2048];   // folded lines

    // // // // // // // // // // // //
    // CALENDAR Variable Declaration
    int cal_ver_check, cal_prodid_check, cal_open_check, cal_close_check, cal_calscale_check, cal_method_check, cal_eve_check;
    cal_ver_check = cal_prodid_check = cal_open_check = cal_close_check = cal_calscale_check = cal_method_check = cal_eve_check = 0;
    // EVENT Variable Declaration
    int eve_open_check, eve_close_check, eve_dt_check, eve_uid_check, eve_dtstart_check, eve_dtend_duration_check;
    eve_open_check = eve_close_check = eve_dt_check = eve_uid_check = eve_dtstart_check = eve_dtend_duration_check = 0;
    // CALENDAR Start
    while (fgets(buffer, sizeof(buffer), in)) {
        char * folded = malloc(sizeof(char) * strlen(buffer) + 1);
        strcpy(folded, buffer);
        folded[strlen(folded) - 1] = '\0';
        long offset = ftell(in);
        while (fgets(fold, sizeof(fold), in)) {
            if (fold[0] == ' ' || fold[0] == '\t' || fold[0] == ';') {
                if (fold[0] != ';') {
                    int newLen = strlen(folded) + strlen(fold) + 1;
                    folded = (char*)realloc(folded, newLen);
                    char * trim = malloc((sizeof(char) * strlen(fold)) + 1);
                    strncpy(trim, fold + 1, strlen(fold) + 1);
                    trim[strlen(trim) - 1] = '\0';
                    folded[strlen(folded) - 1] = '\0';
                    strcat(folded + strlen(folded), trim);
                    folded[strlen(folded)] = '\0';
                    free(trim);
                }
            }else{ break; }
        }
        fseek(in, offset, SEEK_SET);

        if (strcmp(folded, "BEGIN:VCALENDAR\r") == 0) { cal_open_check = 1; }
        if (strcmp(folded, "END:VCALENDAR\r") == 0) { cal_close_check = 1; }

        char cal_prop_name[255];
        char * cal_token;
        
        // PRODUCT ID PROP
        strncpy(cal_prop_name, folded, 6);
        cal_prop_name[6] = '\0';
        if (strcmp(cal_prop_name, "PRODID") == 0) {
            cal_token = strtok(folded, ":");
            cal_token = strtok(NULL, "");
            cal_token[strlen(cal_token) - 1] = '\0';
            if (strcmp(cal_token, "") == 0 || cal_token[0] == '\r' || cal_token[0] == '\n') {
                free(folded);
                freeList(temp_cal->properties);
                freeList(temp_cal->events);
                free(temp_cal);
                fclose(in);
                return INV_PRODID;
            }else{
                if (cal_prodid_check == 1) {
                    free(folded);
                    freeList(temp_cal->properties);
                    freeList(temp_cal->events);
                    free(temp_cal);
                    fclose(in);
                    return DUP_PRODID;
                }else{
                    cal_prodid_check = 1;
                    strcpy((temp_cal)->prodID, cal_token);
                }
            }
        }

        // VERSION PROP
        strncpy(cal_prop_name, folded, 7);
        cal_prop_name[7] = '\0';
        if (strcmp(cal_prop_name, "VERSION") == 0) {
            cal_token = strtok(folded, ":");
            cal_token = strtok(NULL, "");
            cal_token[strlen(cal_token) - 1] = '\0';
            if (strcmp(cal_token, "") == 0 || cal_token[0] == '\r' || cal_token[0] == '\n') {
                free(folded);
                freeList(temp_cal->properties);
                freeList(temp_cal->events);
                free(temp_cal);
                fclose(in);
                return INV_VER;
            }else{
                if (cal_ver_check == 1) {
                    free(folded);
                    freeList(temp_cal->properties);
                    freeList(temp_cal->events);
                    free(temp_cal);
                    fclose(in);
                    return DUP_VER;
                }else{
                    if (atof(cal_token) == 0.0) {
                        free(folded);
                        free(temp_cal->properties);
                        free(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_VER;
                    }else{
                        cal_ver_check = 1;
                        (temp_cal)->version = atof(cal_token);
                    }
                }
            }
        }

        // CALSCALE PROP
        strncpy(cal_prop_name, folded, 8);
        cal_prop_name[8] = '\0';
        if (strcmp(cal_prop_name, "CALSCALE") == 0) {
            cal_token = strtok(folded, ":");
            cal_token = strtok(NULL, "");
            cal_token[strlen(cal_token) - 1] = '\0';
            if (strcmp(cal_token, "") == 0 || cal_token[0] == '\r' || cal_token[0] == '\n') {
                free(folded);
                freeList(temp_cal->properties);
                freeList(temp_cal->events);
                free(temp_cal);
                fclose(in);
                return INV_CAL;
            }else{
                if (cal_calscale_check == 1) {
                    free(folded);
                    freeList(temp_cal->properties);
                    freeList(temp_cal->events);
                    free(temp_cal);
                    fclose(in);
                    return INV_CAL;
                }else{
                    cal_calscale_check = 1;
                    Property* cal_prop = malloc(sizeof(*cal_prop) + sizeof(char [strlen(cal_token)]) + 1);
                    if (cal_prop == NULL) { free(folded); free(temp_cal); fclose(in); return OTHER_ERROR; }
                    strcpy(cal_prop->propName, "CALSCALE");
                    strcpy(cal_prop->propDescr, cal_token);
                    insertBack((temp_cal)->properties, copyProperty(cal_prop));
                    free(cal_prop);
                }
            }
        }

        // METHOD PROP
        strncpy(cal_prop_name, folded, 6);
        cal_prop_name[6] = '\0';
        if (strcmp(cal_prop_name, "METHOD") == 0) {
            cal_token = strtok(folded, ":");
            cal_token = strtok(NULL, "");
            cal_token[strlen(cal_token) - 1] = '\0';
            if (strcmp(cal_token, "") == 0 || cal_token[0] == '\r' || cal_token[0] == '\n') {
                free(folded);
                freeList(temp_cal->properties);
                freeList(temp_cal->events);
                free(temp_cal);
                fclose(in);
                return INV_CAL;
            }else{
                if (cal_method_check == 1) {
                    free(folded);
                    freeList(temp_cal->properties);
                    freeList(temp_cal->events);
                    free(temp_cal);
                    fclose(in);
                    return INV_CAL;
                }else{
                    cal_method_check = 1;
                    Property* cal_prop = malloc(sizeof(*cal_prop) + sizeof(char [strlen(cal_token)]) + 1);
                    if (cal_prop == NULL) { free(folded); free(temp_cal); fclose(in); return OTHER_ERROR; }
                    strcpy(cal_prop->propName, "METHOD");
                    strcpy(cal_prop->propDescr, cal_token);
                    insertBack((temp_cal)->properties, copyProperty(cal_prop));
                    free(cal_prop);
                }
            }
        }

        // EVENT TAG
        if (strcmp(folded, "BEGIN:VEVENT\r") == 0) {
            cal_eve_check = 1;
            eve_open_check = 1;
            
            Event* event = malloc(sizeof(Event));
            if (event == NULL) { free(folded); free(temp_cal); fclose(in); return OTHER_ERROR; }
            event->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
            event->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);

            while (fgets(buffer, sizeof(buffer), in)) {
                for (int i = 1; i < 12; i++) { if (buffer[0] != ' ') { if (buffer[i] == ';') { buffer[i] = ':'; } }else{ break; } }

                char * e_folded = malloc(sizeof(char) * strlen(buffer) + 1);
                strcpy(e_folded, buffer);
                e_folded[strlen(e_folded) - 1] = '\0';
                long offset = ftell(in);
                while (fgets(fold, sizeof(fold), in)) {
                    if (fold[0] == ' ' || fold[0] == '\t' || fold[0] == ';') {
                        if (fold[0] != ';') {
                            int newLen = strlen(e_folded) + strlen(fold) + 1;
                            e_folded = (char*)realloc(e_folded, newLen);
                            char * trim = malloc((sizeof(char) * strlen(fold)) + 1);
                            strncpy(trim, fold + 1, strlen(fold) + 1);
                            trim[strlen(trim) - 1] = '\0';
                            e_folded[strlen(e_folded) - 1] = '\0';
                            strcat(e_folded + strlen(e_folded), trim);
                            e_folded[strlen(e_folded)] = '\0';
                            free(trim);
                        }
                    }else{ break; }
                }
                fseek(in, offset, SEEK_SET);

                // CALENDAR End Tag
                if (strcmp(e_folded, "END:VCALENDAR\r") == 0) {
                    if (eve_open_check == 1) {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_EVENT;
                    }
                }

                if (strcmp(e_folded, "BEGIN:VEVENT\r") == 0) {
                    if (eve_open_check == 1) {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_EVENT;
                    }else{
                        eve_open_check = 1;
                    }
                }

                if (strcmp(e_folded, "END:VEVENT\r") == 0) {
                    if (eve_open_check == 0) {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_EVENT;
                    }else{
                        eve_close_check = 1;
                        if (eve_dt_check == 0 || eve_uid_check == 0) {
                            freeList(event->alarms);
                            freeList(event->properties);
                            free(event);
                            free(e_folded);
                            free(folded);
                            freeList(temp_cal->properties);
                            freeList(temp_cal->events);
                            free(temp_cal);
                            fclose(in);
                            return INV_EVENT;
                        }else{
                            if (eve_dtstart_check == 0) {
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                free(e_folded);
                                free(folded);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                fclose(in);
                                return INV_EVENT;
                            }else{
                                eve_open_check = eve_close_check = 0;
                                eve_dt_check = eve_uid_check = eve_dtstart_check = eve_dtend_duration_check = 0;
                                // INSERT INTO Calendar's Event List
                                insertBack((temp_cal)->events, copyEvent(event));
                                // printf("EVENT ADDED\n");
                                free(e_folded);
                                break;
                            }
                        }
                    }
                }

                // EVENT PROP
                char eve_prop_name[255];
                char * eve_token;

                // DTSTAMP PROP
                strncpy(eve_prop_name, e_folded, 7);
                eve_prop_name[7] = '\0';
                if (strcmp(eve_prop_name, "DTSTAMP") == 0) {
                    eve_token = strtok(e_folded, ":");
                    eve_token = strtok(NULL, "");
                    eve_token[strlen(eve_token) - 1] = '\0';
                    if (strcmp(eve_token, "") == 0 || eve_token[0] == '\r' || eve_token[0] == '\n') {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_DT;
                    }else{
                        if (eve_dt_check == 1) {
                            freeList(event->alarms);
                            freeList(event->properties);
                            free(event);
                            free(e_folded);
                            free(folded);
                            freeList(temp_cal->properties);
                            freeList(temp_cal->events);
                            free(temp_cal);
                            fclose(in);
                            return INV_DT;
                        }else{
                            eve_dt_check = 1;
                            DateTime* dt = malloc(sizeof(DateTime));
                            if (eve_token[strlen(eve_token) - 1] == 'Z') { dt->UTC = true; }
                            else                                         { dt->UTC = false; }
                            eve_token = strtok(eve_token, "T");
                            if (strlen(eve_token) != 8) {
                                free(dt);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                free(e_folded);
                                free(folded);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                fclose(in);
                                return INV_DT;
                            }else{
                                strcpy(dt->date, eve_token);
                            }
                            eve_token = strtok(NULL, "Z");
                            if (strlen(eve_token) != 6) {
                                free(dt);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                free(e_folded);
                                free(folded);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                fclose(in);
                                return INV_DT;
                            }else{
                                strcpy(dt->time, eve_token);
                            }
                            event->creationDateTime = *dt;
                            free(dt);
                        }
                    }
                }

                // UID PROP
                strncpy(eve_prop_name, e_folded, 3);
                eve_prop_name[3] = '\0';
                if (strcmp(eve_prop_name, "UID") == 0) {
                    eve_token = strtok(e_folded, ":");
                    eve_token = strtok(NULL, "");
                    eve_token[strlen(eve_token) - 1] = '\0';
                    if (strcmp(eve_token, "") == 0 || eve_token[0] == '\r' || eve_token[0] == '\n') {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_EVENT;
                    }else{
                        if (eve_uid_check == 1) {
                            freeList(event->alarms);
                            freeList(event->properties);
                            free(event);
                            free(e_folded);
                            free(folded);
                            freeList(temp_cal->properties);
                            freeList(temp_cal->events);
                            free(temp_cal);
                            fclose(in);
                            return INV_EVENT;
                        }else{
                            eve_uid_check = 1;                            
                            strcpy(event->UID, eve_token);
                        }
                    }
                }

                // DTSTART PROP
                strncpy(eve_prop_name, e_folded, 7);
                eve_prop_name[7] = '\0';
                if (strcmp(eve_prop_name, "DTSTART") == 0) {
                    eve_token = strtok(e_folded, ":");
                    eve_token = strtok(NULL, "");
                    eve_token[strlen(eve_token) - 1] = '\0';
                    if (strcmp(eve_token, "") == 0 || eve_token[0] == '\r' || eve_token[0] == '\n') {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_DT;
                    }else{
                        if (eve_dtstart_check == 1) {
                            freeList(event->alarms);
                            freeList(event->properties);
                            free(event);
                            free(e_folded);
                            free(folded);
                            freeList(temp_cal->properties);
                            freeList(temp_cal->events);
                            free(temp_cal);
                            fclose(in);
                            return INV_DT;
                        }else{
                            if(eve_token[0] == 'T' && eve_token[1] == 'Z' && eve_token[2] == 'I' && eve_token[3] == 'D') {
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                free(e_folded);
                                free(folded);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                fclose(in);
                                return INV_DT;
                            }

                            eve_dtstart_check = 1;
                            DateTime* dt = malloc(sizeof(DateTime));
                            if (eve_token[strlen(eve_token) - 1] == 'Z') { dt->UTC = true; }
                            else                                         { dt->UTC = false; }
                            eve_token = strtok(eve_token, "T");
                            strcpy(dt->date, eve_token);
                            eve_token = strtok(NULL, "Z");
                            strcpy(dt->time, eve_token);
                            event->startDateTime = *dt;
                            free(dt);
                        }
                    }
                }

                // (EVENT) OPTIONAL + MUST NOT OCCUR > 1
                // CLASS, CREATED, DESCRIPTION, GEO, LAST-MOD, LOCATION, ORGANIZER, PRIORITY, SEQ, STATUS, SUMMARY, TRANSP, URL, RECURID
                strncpy(eve_prop_name, e_folded, 255);
                eve_prop_name[255] = '\0';
                eve_token = strtok(eve_prop_name, ":");
                if ((strcmp(eve_token, "CLASS")            == 0) ||
                    (strcmp(eve_token, "CREATED")          == 0) ||
                    (strcmp(eve_token, "DESCRIPTION")      == 0) ||
                    (strcmp(eve_token, "GEO")              == 0) ||
                    (strcmp(eve_token, "LAST-MOD")         == 0) ||
                    (strcmp(eve_token, "LAST-MODIFIED")    == 0) ||
                    (strcmp(eve_token, "LOCATION")         == 0) ||
                    (strcmp(eve_token, "ORGANIZER")        == 0) ||
                    (strcmp(eve_token, "PRIORITY")         == 0) ||
                    (strcmp(eve_token, "SEQ")              == 0) ||
                    (strcmp(eve_token, "SEQUENCE")         == 0) ||
                    (strcmp(eve_token, "STATUS")           == 0) ||
                    (strcmp(eve_token, "SUMMARY")          == 0) ||
                    (strcmp(eve_token, "TRANSP")           == 0) ||
                    (strcmp(eve_token, "URL")              == 0) ||
                    (strcmp(eve_token, "RECURID")          == 0)) {
                    eve_token = strtok(e_folded, ":");
                    char * temp = strtok(NULL, "");
                    temp[strlen(temp) - 1] = '\0';
                    if (strcmp(temp, "") == 0 || temp[0] == '\r' || temp[0] == '\n') {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_EVENT;
                    }else{
                        int found = 0;
                        ListIterator itr = createIterator(event->properties);
                        Property* data = nextElement(&itr);
                        while (data != NULL) {
                            if (strcmp(data->propName, eve_token) == 0) { found = 1; }
                            data = nextElement(&itr);
                        }
                        if (found == 0) {
                            Property* eve_prop = malloc(sizeof(*eve_prop) + sizeof(char [strlen(temp)]) + 1);
                            strcpy(eve_prop->propName, eve_token);
                            strcpy(eve_prop->propDescr, temp);
                            insertBack(event->properties, copyProperty(eve_prop));
                            free(eve_prop);
                        }else{
                            freeList(event->alarms);
                            freeList(event->properties);
                            free(event);
                            free(e_folded);
                            free(folded);
                            freeList(temp_cal->properties);
                            freeList(temp_cal->events);
                            free(temp_cal);
                            fclose(in);
                            return INV_EVENT;
                        }
                    }
                }

                // DTEND/DURATION
                strncpy(eve_prop_name, e_folded, 255);
                eve_prop_name[255] = '\0';
                eve_token = strtok(eve_prop_name, ":");
                if ((strcmp(eve_token, "DTEND")    == 0) ||
                    (strcmp(eve_token, "DURATION") == 0)) {
                    eve_token = strtok(e_folded, ":");
                    char * temp = strtok(NULL, "");
                    temp[strlen(temp) - 1] = '\0';
                    if (strcmp(temp, "") == 0 || temp[0] == '\r' || temp[0] == '\n') {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_EVENT;
                    }else{
                        if (eve_dtend_duration_check == 1) {
                            freeList(event->alarms);
                            freeList(event->properties);
                            free(event);
                            free(e_folded);
                            free(folded);
                            freeList(temp_cal->properties);
                            freeList(temp_cal->events);
                            free(temp_cal);
                            fclose(in);
                            return INV_EVENT;
                        }else{
                            eve_dtend_duration_check = 1;

                            if(temp[0] == 'T' && temp[1] == 'Z' && temp[2] == 'I' && temp[3] == 'D') {
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                free(folded);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                fclose(in);
                                return INV_DT;
                            }

                            Property* eve_prop = malloc(sizeof(*eve_prop) + sizeof(char [strlen(temp)]) + 1);
                            strcpy(eve_prop->propName, eve_token);
                            strcpy(eve_prop->propDescr, temp);
                            insertBack(event->properties, copyProperty(eve_prop));
                            free(eve_prop);
                        }
                    }
                }


                // ATTACH, ATTENDEE, CATEGORIES, COMMENT, CONTACT, EXDATE, RSTATUS, RELATED, RESOURCES, RDATE, X-PROP, IANA-PROP **RRULE**
                strncpy(eve_prop_name, e_folded, 255);
                eve_prop_name[255] = '\0';
                eve_token = strtok(eve_prop_name, ":");
                if ((strcmp(eve_token, "ATTACH")     == 0) ||
                    (strcmp(eve_token, "ATTENDEE")   == 0) ||
                    (strcmp(eve_token, "CATEGORIES") == 0) ||
                    (strcmp(eve_token, "COMMENT")    == 0) ||
                    (strcmp(eve_token, "CONTACT")    == 0) ||
                    (strcmp(eve_token, "EXDATE")     == 0) ||
                    (strcmp(eve_token, "RSTATUS")    == 0) ||
                    (strcmp(eve_token, "RELATED")    == 0) ||
                    (strcmp(eve_token, "RESOURCES")  == 0) ||
                    (strcmp(eve_token, "RDATE")      == 0) ||
                    (strcmp(eve_token, "X-PROP")     == 0) ||
                    (strcmp(eve_token, "IANA-PROP")  == 0) ||
                    (strcmp(eve_token, "RRULE")      == 0)) {
                    eve_token = strtok(e_folded, ":");
                    char * temp = strtok(NULL, "");
                    temp[strlen(temp) - 1] = '\0';
                    if (strcmp(temp, "") == 0 || temp[0] == '\r' || temp[0] == '\n') {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_EVENT;
                    }else{                      
                        Property* eve_prop = malloc(sizeof(*eve_prop) + sizeof(char [strlen(temp)]) + 1);
                        strcpy(eve_prop->propName, eve_token);
                        strcpy(eve_prop->propDescr, temp);
                        insertBack(event->properties, copyProperty(eve_prop));
                        free(eve_prop);
                    }
                }

                // ALARM Variable Declaration
                int ala_open_check, ala_close_check, ala_action_check, ala_trigger_check, ala_duration_check, ala_repeat_check, ala_attach_check;
                ala_open_check = ala_close_check = ala_action_check = ala_trigger_check = ala_duration_check = ala_repeat_check = ala_attach_check = 0;
                // ALARM PROP Start
                if (strcmp(e_folded, "END:VALARM\r") == 0) {
                    if (ala_open_check == 0) {
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        free(e_folded);
                        free(folded);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        fclose(in);
                        return INV_ALARM;
                    }
                }
                if (strcmp(e_folded, "BEGIN:VALARM\r") == 0) {
                    Alarm* alarm = malloc(sizeof(Alarm));
                    alarm->trigger = '\0';
                    List* a_p_list = initializeList(&printProperty, &deleteProperty, &compareProperties);
                    alarm->properties = a_p_list;
                    ala_open_check = 1;
                    while (fgets(buffer, sizeof(buffer), in)) {
                        for (int i = 1; i < 12; i++) { if (buffer[0] != ' ') { if (buffer[i] == ';') { buffer[i] = ':'; } }else{ break; } }

                        char * alarm_folded = malloc(sizeof(char) * strlen(buffer) + 1);
                        strcpy(alarm_folded, buffer);
                        alarm_folded[strlen(alarm_folded) - 1] = '\0';
                        long offset = ftell(in);
                        while (fgets(fold, sizeof(fold), in)) {
                            if (fold[0] == ' ' || fold[0] == '\t' || fold[0] == ';') {
                                if (fold[0] != ';') {
                                    int newLen = strlen(alarm_folded) + strlen(fold) + 1;
                                    alarm_folded = (char*)realloc(alarm_folded, newLen);
                                    char * trim = malloc((sizeof(char) * strlen(fold)) + 1);
                                    strncpy(trim, fold + 1, strlen(fold) + 1);
                                    trim[strlen(trim) - 1] = '\0';
                                    alarm_folded[strlen(alarm_folded) - 1] = '\0';
                                    strcat(alarm_folded + strlen(alarm_folded), trim);
                                    alarm_folded[strlen(alarm_folded)] = '\0';
                                    free(trim);
                                }
                            }else{ break; }
                        }
                        fseek(in, offset, SEEK_SET);

                        if (strcmp(alarm_folded, "BEGIN:VALARM\r") == 0) {
                            if (ala_open_check == 1) {
                                freeList(alarm->properties);
                                free(alarm->trigger);
                                free(alarm);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                free(alarm_folded);
                                free(e_folded);
                                free(folded);
                                fclose(in);
                                return INV_ALARM;
                            }else{
                                ala_open_check = 1;
                            }
                        }

                        if (strcmp(alarm_folded, "END:VALARM\r") == 0) {
                            ala_close_check = 1;
                            if ((ala_duration_check == 0 && ala_repeat_check == 0) ||
                                (ala_duration_check == 1 && ala_repeat_check == 1)) {
                                if (ala_action_check == 1 && ala_trigger_check == 1) {
                                    // INSERT INTO Event's Alarm List
                                    // printf("ALARM ADDED\n");
                                    insertBack(event->alarms, copyAlarm(alarm));
                                    free(alarm_folded);
                                    break;
                                }else{
                                    freeList(alarm->properties);
                                    free(alarm->trigger);
                                    free(alarm);
                                    freeList(event->alarms);
                                    freeList(event->properties);
                                    free(event);
                                    freeList(temp_cal->properties);
                                    freeList(temp_cal->events);
                                    free(temp_cal);
                                    free(alarm_folded);
                                    free(e_folded);
                                    free(folded);
                                    fclose(in);
                                    return INV_ALARM;
                                }
                            }else{
                                freeList(alarm->properties);
                                free(alarm->trigger);
                                free(alarm);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                free(alarm_folded);
                                free(e_folded);
                                free(folded);
                                fclose(in);
                                return INV_ALARM;
                            }
                        }

                        // EVENT PROP
                        char ala_prop_name[255];
                        char * ala_token;

                        strncpy(ala_prop_name, alarm_folded, 255);
                        ala_prop_name[255] = '\0';
                        ala_token = strtok(ala_prop_name, ":");
                        if (strcmp(ala_prop_name, "TRIGGER") == 0) {
                            ala_token = strtok(alarm_folded, ":");
                            ala_token = strtok(NULL, "");
                            ala_token[strlen(ala_token) - 1] = '\0';
                            if (strcmp(ala_token, "") == 0 || ala_token[0] == '\r' || ala_token[0] == '\n') {
                                freeList(alarm->properties);
                                free(alarm->trigger);
                                free(alarm);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                free(alarm_folded);
                                free(e_folded);
                                free(folded);
                                fclose(in);
                                return INV_ALARM;
                            }else{
                                if (ala_trigger_check == 1) {
                                    freeList(alarm->properties);
                                    free(alarm->trigger);
                                    free(alarm);
                                    freeList(event->alarms);
                                    freeList(event->properties);
                                    free(event);
                                    freeList(temp_cal->properties);
                                    freeList(temp_cal->events);
                                    free(temp_cal);
                                    free(alarm_folded);
                                    free(e_folded);
                                    free(folded);
                                    fclose(in);
                                    return INV_ALARM;
                                }else{
                                    ala_trigger_check = 1;
                                    // alarm->trigger = ala_token;
                                    alarm->trigger = malloc(sizeof(char) * strlen(ala_token) + 1);
                                    strcpy(alarm->trigger, ala_token);
                                }
                            }
                        }

                        strncpy(ala_prop_name, alarm_folded, 255);
                        ala_prop_name[255] = '\0';
                        ala_token = strtok(ala_prop_name, ":");
                        if (strcmp(ala_prop_name, "ACTION") == 0) {
                            ala_token = strtok(alarm_folded, ":");
                            ala_token = strtok(NULL, "");
                            ala_token[strlen(ala_token) - 1] = '\0';
                            if (strcmp(ala_token, "") == 0 || ala_token[0] == '\r' || ala_token[0] == '\n') {
                                freeList(alarm->properties);
                                free(alarm->trigger);
                                free(alarm);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                free(alarm_folded);
                                free(e_folded);
                                free(folded);
                                fclose(in);
                                return INV_ALARM;
                            }else{
                                if (ala_action_check == 1) {
                                    freeList(alarm->properties);
                                    free(alarm->trigger);
                                    free(alarm);
                                    freeList(event->alarms);
                                    freeList(event->properties);
                                    free(event);
                                    freeList(temp_cal->properties);
                                    freeList(temp_cal->events);
                                    free(temp_cal);
                                    free(alarm_folded);
                                    free(e_folded);
                                    free(folded);
                                    fclose(in);
                                    return INV_ALARM;
                                }else{
                                    ala_action_check = 1;
                                    strcpy(alarm->action, ala_token);
                                }
                            }
                        }

                        strncpy(ala_prop_name, alarm_folded, 255);
                        ala_prop_name[255] = '\0';
                        ala_token = strtok(ala_prop_name, ":");
                        if (strcmp(ala_prop_name, "DURATION") == 0) {
                            ala_token = strtok(alarm_folded, ":");
                            ala_token = strtok(NULL, "");
                            ala_token[strlen(ala_token) - 1] = '\0';
                            if (strcmp(ala_token, "") == 0 || ala_token[0] == '\r' || ala_token[0] == '\n') {
                                freeList(alarm->properties);
                                free(alarm->trigger);
                                free(alarm);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                free(alarm_folded);
                                free(e_folded);
                                free(folded);
                                fclose(in);
                                return INV_ALARM;
                            }else{
                                if (ala_duration_check == 1) {
                                    freeList(alarm->properties);
                                    free(alarm->trigger);
                                    free(alarm);
                                    freeList(event->alarms);
                                    freeList(event->properties);
                                    free(event);
                                    freeList(temp_cal->properties);
                                    freeList(temp_cal->events);
                                    free(temp_cal);
                                    free(alarm_folded);
                                    free(e_folded);
                                    free(folded);
                                    fclose(in);
                                    return INV_ALARM;
                                }else{
                                    ala_duration_check = 1;
                                    Property* ala_prop = malloc(sizeof(*ala_prop) + sizeof(char [strlen(ala_token)]) + 1);
                                    strcpy(ala_prop->propName, "DURATION");
                                    strcpy(ala_prop->propDescr, ala_token);
                                    insertBack(alarm->properties, copyProperty(ala_prop));
                                    free(ala_prop);
                                }
                            }
                        }

                        strncpy(ala_prop_name, alarm_folded, 255);
                        ala_prop_name[255] = '\0';
                        ala_token = strtok(ala_prop_name, ":");
                        if (strcmp(ala_prop_name, "REPEAT") == 0) {
                            ala_token = strtok(alarm_folded, ":");
                            ala_token = strtok(NULL, "");
                            ala_token[strlen(ala_token) - 1] = '\0';
                            if (strcmp(ala_token, "") == 0 || ala_token[0] == '\r' || ala_token[0] == '\n') {
                                freeList(alarm->properties);
                                free(alarm->trigger);
                                free(alarm);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                free(alarm_folded);
                                free(e_folded);
                                free(folded);
                                fclose(in);
                                return INV_ALARM;
                            }else{
                                if (ala_repeat_check == 1) {
                                    freeList(alarm->properties);
                                    free(alarm->trigger);
                                    free(alarm);
                                    freeList(event->alarms);
                                    freeList(event->properties);
                                    free(event);
                                    freeList(temp_cal->properties);
                                    freeList(temp_cal->events);
                                    free(temp_cal);
                                    free(alarm_folded);
                                    free(e_folded);
                                    free(folded);
                                    fclose(in);
                                    return INV_ALARM;
                                }else{
                                    ala_repeat_check = 1;
                                    Property* ala_prop = malloc(sizeof(*ala_prop) + sizeof(char [strlen(ala_token)]) + 1);
                                    strcpy(ala_prop->propName, "REPEAT");
                                    strcpy(ala_prop->propDescr, ala_token);
                                    insertBack(alarm->properties, copyProperty(ala_prop));
                                    free(ala_prop);
                                }
                            }
                        }

                        strncpy(ala_prop_name, alarm_folded, 255);
                        ala_prop_name[255] = '\0';
                        ala_token = strtok(ala_prop_name, ":");
                        if (strcmp(ala_prop_name, "ATTACH") == 0) {
                            ala_token = strtok(alarm_folded, ":");
                            ala_token = strtok(NULL, "");
                            ala_token[strlen(ala_token) - 1] = '\0';
                            if (strcmp(ala_token, "") == 0 || ala_token[0] == '\r' || ala_token[0] == '\n') {
                                freeList(alarm->properties);
                                free(alarm->trigger);
                                free(alarm);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                free(alarm_folded);
                                free(e_folded);
                                free(folded);
                                fclose(in);
                                return INV_ALARM;
                            }else{
                                if (ala_attach_check == 1) {
                                    freeList(alarm->properties);
                                    free(alarm->trigger);
                                    free(alarm);
                                    freeList(event->alarms);
                                    freeList(event->properties);
                                    free(event);
                                    freeList(temp_cal->properties);
                                    freeList(temp_cal->events);
                                    free(temp_cal);
                                    free(alarm_folded);
                                    free(e_folded);
                                    free(folded);
                                    fclose(in);
                                    return INV_ALARM;
                                }else{
                                    ala_attach_check = 1;
                                    Property* ala_prop = malloc(sizeof(*ala_prop) + sizeof(char [strlen(ala_token)]) + 1);
                                    strcpy(ala_prop->propName, "ATTACH");
                                    strcpy(ala_prop->propDescr, ala_token);
                                    insertBack(alarm->properties, copyProperty(ala_prop));
                                    free(ala_prop);
                                }
                            }
                        }

                        strncpy(ala_prop_name, alarm_folded, 255);
                        ala_prop_name[255] = '\0';
                        ala_token = strtok(ala_prop_name, ":");
                        if ((strcmp(ala_token, "X-PROP")     == 0) ||
                            (strcmp(ala_token, "IANA-PROP")  == 0)) {
                            ala_token = strtok(alarm_folded, ":");
                            char * temp = strtok(NULL, "");
                            temp[strlen(temp) - 1] = '\0';  
                            if (strcmp(temp, "") == 0 || temp[0] == '\r' || temp[0] == '\n') {
                                freeList(alarm->properties);
                                free(alarm->trigger);
                                free(alarm);
                                freeList(event->alarms);
                                freeList(event->properties);
                                free(event);
                                freeList(temp_cal->properties);
                                freeList(temp_cal->events);
                                free(temp_cal);
                                free(alarm_folded);
                                free(e_folded);
                                free(folded);
                                fclose(in);
                                return INV_ALARM;
                            }else{                      
                                Property* ala_prop = malloc(sizeof(*ala_prop) + sizeof(char [strlen(temp)]) + 1);
                                strcpy(ala_prop->propName, ala_token);
                                strcpy(ala_prop->propDescr, temp);
                                insertBack(alarm->properties, copyProperty(ala_prop));
                                free(ala_prop);
                            }
                        }
                        

                        free(alarm_folded);
                    }

                    if (ala_close_check == 0 || (ala_trigger_check == 0 || ala_action_check == 0)) {
                        freeList(alarm->properties);
                        free(alarm->trigger);
                        free(alarm);
                        freeList(event->alarms);
                        freeList(event->properties);
                        free(event);
                        freeList(temp_cal->properties);
                        freeList(temp_cal->events);
                        free(temp_cal);
                        free(e_folded);
                        free(folded);
                        fclose(in);
                        return INV_ALARM;
                    }
                    
                    free(alarm->properties);
                    free(alarm->trigger);
                    free(alarm);
                }

                if (ala_open_check == 1 && ala_close_check == 0) {
                    freeList(event->alarms);
                    freeList(event->properties);
                    free(event);
                    freeList(temp_cal->properties);
                    freeList(temp_cal->events);
                    free(temp_cal);
                    free(e_folded);
                    free(folded);
                    fclose(in);
                    return INV_ALARM;
                }
                // ALARM PROP End
                if(e_folded[0] == ':') {
                    freeList(event->alarms);
                    freeList(event->properties);
                    free(event);
                    free(e_folded);
                    free(folded);
                    freeList(temp_cal->properties);
                    freeList(temp_cal->events);
                    free(temp_cal);
                    fclose(in);
                    return INV_EVENT;
                }
                free(e_folded);
            }

            free(event->properties);
            free(event->alarms);
            free(event);
        }
        // EVENT Error
        if (eve_open_check == 1 && eve_close_check == 0) {
            free(folded);
            freeList(temp_cal->properties);
            freeList(temp_cal->events);
            free(temp_cal);
            fclose(in);
            return INV_EVENT;
        }

        // EVENT End Tag
        if (strcmp(folded, "END:VEVENT\r") == 0) {
            free(folded);
            freeList(temp_cal->properties);
            freeList(temp_cal->events);
            free(temp_cal);
            fclose(in);
            return INV_EVENT;
        }

        // IANA-/X- PROP
        strncpy(cal_prop_name, folded, 255);
        cal_prop_name[255] = '\0';
        cal_token = strtok(cal_prop_name, ":");
        if ((strcmp(cal_token, "X-PROP")    == 0) ||
            (strcmp(cal_token, "IANA-PROP") == 0)) {
            cal_token = strtok(folded, ":");
            char * temp = strtok(NULL, "");
            temp[strlen(temp) - 1] = '\0';
            if (strcmp(temp, "") == 0 || temp[0] == '\r' || temp[0] == '\n') {
                free(folded);
                freeList(temp_cal->properties);
                freeList(temp_cal->events);
                free(temp_cal);
                fclose(in);
                return INV_CAL;
            }else{
                Property* cal_prop = malloc(sizeof(*cal_prop) + sizeof(char [strlen(temp)]) + 1);
                if (cal_prop == NULL) { free(folded); free(temp_cal); fclose(in); return OTHER_ERROR; }
                strcpy(cal_prop->propName, cal_token);
                strcpy(cal_prop->propDescr, temp);
                insertBack((temp_cal)->properties, copyProperty(cal_prop));
                free(cal_prop);
            }
        }

        free(folded);
    }
    // CALENDAR Error
    if (cal_open_check == 1 && cal_close_check == 1) {
        if (((temp_cal)->version) < 0.0 || strcmp((temp_cal)->prodID, "") == 0) {
            freeList(temp_cal->properties);
            freeList(temp_cal->events);
            free(temp_cal);
            fclose(in);
            return INV_CAL;
        }else{
            if (cal_eve_check == 0) {
                freeList(temp_cal->properties);
                freeList(temp_cal->events);
                free(temp_cal);
                fclose(in);
                return INV_CAL;
            }
        }
    }else{
        freeList(temp_cal->properties);
        freeList(temp_cal->events);
        free(temp_cal);
        fclose(in);
        return INV_CAL;
    }
    // CALENDAR End
    // // // // // // // // // // // //

    *obj = temp_cal;
    
    fclose(in);
    return OK;
}

void deleteCalendar(Calendar* obj) {
    if (obj != NULL) {
        if (obj->events)
            freeList(obj->events);
        
        if (obj->properties)
            freeList(obj->properties);
        
        free(obj);
    }
}

char* printCalendar(const Calendar* obj) {
    if (obj == NULL) {
        return "";
    }

    int newLen = ((sizeof(char) * strlen(obj->prodID) + 1) + (sizeof(char) * 32) + 1);
    char * cal_string = malloc(newLen);
    if (cal_string == NULL) {
        return "MEMORY ERROR";
    }
    *cal_string = '\0';
    if (obj != NULL) {
        if (obj->prodID != NULL && obj->properties != NULL && obj->events != NULL) {
            printf("\nPRINTING CALENDAR ...\n");
            snprintf(cal_string, newLen, "ProdID:%s\nVersion:%.1f", obj->prodID, obj->version);

            char* prop_string = toString(obj->properties);
            newLen = strlen(prop_string) + strlen(cal_string) + 1;
            cal_string = (char*)realloc(cal_string, newLen);
            strcat(cal_string, prop_string);
            free(prop_string);

            char* event_string = toString(obj->events);
            newLen = strlen(event_string) + strlen(cal_string) + 1;
            cal_string = (char*)realloc(cal_string, newLen);
            strcat(cal_string, event_string);
            free(event_string);
        }
    }

    return cal_string;
}

char* printError(ICalErrorCode err) {
    char * code = malloc(sizeof(char) * 16);
    if (code == NULL) {
        return "OTHER_ERROR";
    }
    switch(err) {
        case OK:
            strcpy(code, "OK\0");
            return code;
            break;

        case INV_FILE:
            strcpy(code, "INV_FILE\0");
            return code;
            break;

        case INV_CAL:
            strcpy(code, "INV_CAL\0");
            return code;
            break;

        case INV_VER:
            strcpy(code, "INV_VER\0");
            return code;
            break;

        case DUP_VER:
            strcpy(code, "DUP_VER\0");
            return code;
            break;

        case INV_PRODID:
            strcpy(code, "INV_PRODID\0");
            return code;
            break;

        case DUP_PRODID:
            strcpy(code, "DUP_PRODID\0");
            return code;
            break;
        
        case INV_EVENT:
            strcpy(code, "INV_EVENT\0");
            return code;
            break;

        case INV_DT:
            strcpy(code, "INV_DT\0");
            return code;
            break;

        case INV_ALARM:
            strcpy(code, "INV_ALARM\0");
            return code;
            break;

        case WRITE_ERROR:
            strcpy(code, "WRITE_ERROR\0");
            return code;
            break;

        case OTHER_ERROR:
            strcpy(code, "OTHER_ERROR\0");
            return code;
            break;

        default:
            strcpy(code, "OTHER_ERROR\0");
            return code;
            break;
    }
}
// ********************************************************************



// ************* List helper functions - MUST be implemented ***************
char* printEvent(void* toBePrinted) {
    char* event_string = malloc(sizeof(char) * sizeof(((Event*)toBePrinted)->UID) + 256);

    strcpy(event_string, "UID:\0");
    strcat(event_string, ((Event*)toBePrinted)->UID);
    event_string[strlen(event_string)] = '\0';
    strcat(event_string, "\n\0");

    char* create_date = malloc((sizeof(char) * (16 + 28)));
    char* c_date = printDate( &((Event*)toBePrinted)->creationDateTime );
    strcpy(create_date, "DTSTAMP:\0");
    strcat(create_date, c_date);

    char* start_date = malloc((sizeof(char) * (16 + 28)));
    char* s_date = printDate( &((Event*)toBePrinted)->startDateTime );
    strcpy(start_date, "DTSTART:\0");
    strcat(start_date, s_date);
    
    strcat(event_string, create_date);
    event_string[strlen(event_string)] = '\0';
    strcat(event_string, "\n\0");
    strcat(event_string, start_date);
    event_string[strlen(event_string)] = '\0';

    free(c_date);
    free(s_date);
    free(create_date);
    free(start_date);

    char * event_property_string = toString(((Event*)toBePrinted)->properties);
    strcat(event_string, event_property_string);
    free(event_property_string);

    char * event_alarm_string = toString(((Event*)toBePrinted)->alarms);
    strcat(event_string, event_alarm_string);
    free(event_alarm_string);

    return event_string;
}
void deleteEvent(void* toBeDeleted) {
    if (toBeDeleted == NULL) { return; }
    freeList( ((Event*)toBeDeleted)->properties );
    freeList( ((Event*)toBeDeleted)->alarms );
    free( ((Event*)toBeDeleted) );
}
int compareEvents(const void* first, const void* second) {
    bool same = false;
    
    Event* one = (Event*)malloc(sizeof(Event));
    Event* two = (Event*)malloc(sizeof(Event));

    one = (Event*)first;
    two = (Event*)second;

    void *p1 = &one->creationDateTime;
    void *p2 = &two->creationDateTime;

    if (strcmp(one->UID, two->UID) == 0) {
        if (compareDates(p1, p2) == 1) {
            same = true;
        }
    }

    free(p1);
    free(p2);
    free(one);
    free(two);

    return same;
}

char* printAlarm(void* toBePrinted) {
    char * alarm_string = malloc(sizeof(char) * 256);
    strcpy(alarm_string, "ACTION:\0");
    strcat(alarm_string, ((Alarm*)toBePrinted)->action);
    alarm_string[strlen(alarm_string)] = '\0';
    strcat(alarm_string, "\n\0");

    strcat(alarm_string, "TRIGGER:\0");
    strcat(alarm_string, ((Alarm*)toBePrinted)->trigger);
    alarm_string[strlen(alarm_string)] = '\0';

    char * alarm_property_string = toString(((Alarm*)toBePrinted)->properties);
    strcat(alarm_string, alarm_property_string);
    free(alarm_property_string);
    
    return alarm_string;
}
void deleteAlarm(void* toBeDeleted) {
    if (toBeDeleted == NULL) { return; }
    free( ((Alarm*)toBeDeleted)->trigger );
    freeList( ((Alarm*)toBeDeleted)->properties );
    free( ((Alarm*)toBeDeleted) );
}
int compareAlarms(const void* first, const void* second) {
    bool same = false;
    
    Alarm* one = (Alarm*)malloc(sizeof(Alarm));
    Alarm* two = (Alarm*)malloc(sizeof(Alarm));

    one = (Alarm*)first;
    two = (Alarm*)second;

    if (strcmp(one->action, two->action) == 0) {
        if (strcmp(one->trigger, two->trigger)) {
            same = true;
        }
    }

    free(one);
    free(two);

    return same;
}

char* printProperty(void* toBePrinted) {
    char * property_string = malloc((sizeof(char) * strlen(((Property*)toBePrinted)->propName)) + 8);

    strcpy(property_string, ((Property*)toBePrinted)->propName);
    property_string[strlen(property_string)] = '\0';
    strcat(property_string, ":\0");
    int newLen = (sizeof(char) * ((strlen(property_string) + 1)) + (sizeof(char) * strlen(((Property*)toBePrinted)->propDescr) + 1));
    property_string = realloc(property_string, newLen);
    strcat(property_string, ((Property*)toBePrinted)->propDescr);
    
    return property_string;
}
void deleteProperty(void* toBeDeleted) {
    if (toBeDeleted == NULL) { return; }
    free( ((Property*)toBeDeleted) );
}
int compareProperties(const void* first, const void* second) {
    bool same = false;
    
    Property* one = (Property*)first;
    Property* two = (Property*)second;

    void *p1 = &one->propName;
    void *p2 = &two->propName;

    if (strcmp(p1, p2) == 0) {
        p1 = &one->propDescr;
        p2 = &two->propDescr;
        if (strcmp(p1, p2) == 0) {
            same = true;
        }
    }

    return same;
}

char* printDate(void* toBePrinted) {
    char * date = malloc(sizeof(char) * (9 + 7 + 1 + 10 + 1));

    strcpy(date, ((DateTime*)toBePrinted)->date );
    strcat(date, "T");
    strcat(date, ((DateTime*)toBePrinted)->time );
    if ( ((DateTime*)toBePrinted)->UTC == true) {
        strcat(date, "Z\0");
    }

    return date;
}
void deleteDate(void* toBeDeleted) {
    if (toBeDeleted == NULL) { return; }
    free( (DateTime*)toBeDeleted );
}
int compareDates(const void* first, const void* second) {
    bool same = false;
    
    DateTime* one = (DateTime*)malloc(sizeof(DateTime));
    DateTime* two = (DateTime*)malloc(sizeof(DateTime));

    one = (DateTime*)first;
    two = (DateTime*)second;

    if (strcmp(one->date, two->date) == 0) {
        if (strcmp(one->time, two->time) == 0) {
            if (one->UTC == two->UTC) {
                same = true;
            }
        }
    }

    free(one);
    free(two);

    return same;
}
// *************************************************************************