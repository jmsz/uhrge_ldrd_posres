//The ADL includes:
#include "../../adl3/include/ADL.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//*****************************************************************************************

int main()
{
int number_of_events = 1000;
double mean_rate = 0.002; //2 Mcps in ns
double random_numbers[number_of_events];
double time_to_next_event [number_of_events];
double event_times [number_of_events +1];
event_times[0] = 0.0;

srand(time(NULL)); //time(NULL)

for(int i = 0; i < number_of_events; i++){
double r = ((double)rand() / (double)RAND_MAX) ; // + event_times[0]; // want number 0 to 1
//if(r = 0){int r = rand() % (100) + event_times[0]; }
random_numbers[i] = r;
std::cout<<"rand: "<<r<<std::endl;
time_to_next_event[i] =(log(1 - random_numbers[i]) / ((-1) * mean_rate));
//std::cout<<"delta t: "<<time_to_next_event[i]<<std::endl;

}

double cumsum = 0;
for(int i = 0; i < number_of_events; i++){
    cumsum = cumsum + time_to_next_event[i];
    event_times[i+1] = cumsum;
}

for(int k = 0; k < number_of_events; k++){
std::cout<<time_to_next_event[k]<<std::endl;
}

return 0;
}

