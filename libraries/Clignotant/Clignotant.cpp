// Clignotant.cpp
#include "Clignotant.h"

// Basic constructor with common parameters for all blinkers
Clignotant::Clignotant(int p, int b){
    pin = p;
    buttonPin = b;
    state = 0;
    isBlinking = false;
    blinkerLastClick = 0;
    startedTimeBlinking = 0;
}
