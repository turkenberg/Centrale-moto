// Clignotant.h
#ifndef Clignotant_H
#define Clignotant_H

class Clignotant
{
public:
    //Constructor
    Clignotant(int p, int b);
    
    // the blinkers and its state (On/off)
    int pin;
    int state = 0;

    // the  Command and clickbutton
    int buttonPin;
    
    // the state (Toggle)
    bool isBlinking;

    // Arbitrary blinker function 
    int blinkerLastClick = 0;

    // Timing misc.
    unsigned long startedTimeBlinking = 0;
};

#endif