#include <Clignotant.h>
#include <ClickButton.h>


/* 

*/

Clignotant clignoDroit(10,2);

//===============================================================

void setup()
{

  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  clignoDroit.button.debounceTime   = 20;   // Debounce timer in ms
  clignoDroit.button.multiclickTime = 250;  // Time limit for multi clicks
  clignoDroit.button.longClickTime  = 1000; // time until "held-down clicks" register

  // Setup initial blinkers state
  clignoDroit.isBlinking=0;

  // Initialize timing counters
  clignoDroit.startedTimeBlinking = 0;
}


//===============================================================

void loop()
{
  // Level 0 ---- Clignotants
  DetermineTurningRIght();

  // Give Orders to system
  
  
  // Will blinking if on, else will stay Off.
  Blink(clignoDroit.isBlinking);

    
  // Then execute orders
  digitalWrite(clignoDroit.pin,clignoDroit.state);
}

//===============================================================


void Blink(Clignotant cligno, byte active)
{
    //If it shall blink, do the maths to blink //Blink half a second
    if (active) cligno.state = 1 - ((millis() - cligno.startedTimeBlinking)/500)%2;
    
    //Else, shut off AND save start time for next toggle
    else {cligno.startedTimeBlinking = millis();    cligno.state = 0; }
}

//===============================================================

void DetermineTurningRIght()
{
      // Update button state
  clignoDroit.button.Update();

  // Save click codes in Function, as click codes are reset at next Update()
  if (clignoDroit.button.clicks != 0) clignoDroit.blinkerLastClick = clignoDroit.button.clicks;
  

  // // blink if clicked (once)
  if(clignoDroit.button.clicks == 1) clignoDroit.isBlinking = !clignoDroit.isBlinking;
      
}
