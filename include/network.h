#pragma once
#include <Arduino.h>

class Task;

String network_uid();
String network_hostname();
bool network_connected();

/*
  Tasks to be enabled/disabled when network goes up or down
*/
void network_addtask(Task& task);
