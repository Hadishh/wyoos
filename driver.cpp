#include "driver.h"

Driver:: Driver(){}
Driver:: ~Driver(){}
void Driver:: Activate(){}
int Driver:: Reset()
{
    return 0;
}
void Driver:: Deactivate() {} 

     
DriverManager:: DriverManager(){
    numDriver = 0;
}
void DriverManager:: AddDriver(Driver* newDriver){
    drivers[numDriver++] = newDriver;
}

void DriverManager::ActivateAll(){
    for (int i = 0; i < numDriver; ++i)
        drivers[i]->Activate();
}