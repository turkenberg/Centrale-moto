#include <Clignotant.h>
#include <ClickButton.h>


/* 

*/

// Right blinker
Clignotant clignoDroit(10,2);
ClickButton buttonUpRight(clignoDroit.pin,LOW ,HIGH);

// Left blinker
Clignotant clignoGauche(11,3);
ClickButton buttonUpLeft(clignoGauche.pin,LOW ,HIGH);

//===============================================================

void setup()
{

  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  buttonUpRight.debounceTime   = 20;   // Debounce timer in ms
  buttonUpRight.multiclickTime = 250;  // Time limit for multi clicks
  buttonUpRight.longClickTime  = 1000; // time until "held-down clicks" register

    // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  buttonUpLeft.debounceTime   = 20;   // Debounce timer in ms
  buttonUpLeft.multiclickTime = 250;  // Time limit for multi clicks
  buttonUpLeft.longClickTime  = 1000; // time until "held-down clicks" register

  // Setup initial blinkers state
  clignoDroit.isBlinking=0;
  clignoGauche.isBlinking=0;

  // Initialize timing counters
  clignoDroit.startedTimeBlinking = 0;
  clignoGauche.startedTimeBlinking = 0;
}


//===============================================================

void loop()
{
  // Level 0 ---- Clignotants
  DetermineTurning(buttonUpRight, clignoDroit);
  DetermineTurning(buttonUpLeft, clignoGauche);

  // Get level 0 orders.
  Blink(clignoDroit);
  Blink(clignoGauche);

  // Seek for level 1 orders (warning). If exists, will overwrite level 0
  DetermineWarnings(clignoDroit,clignoGauche);
  
  // Then execute orders
  digitalWrite(clignoDroit.pin,clignoDroit.state);
  digitalWrite(clignoGauche.pin,clignoGauche.state);
}

//===============================================================


void DetermineWarnings(Clignotant clignoR, Clignotant clignoL)
{
    //If both are in long click state, access and OVERRIDE
    if (clignoR.blinkerLastClick ==-1 && clignoL.blinkerLastClick ==-1)
    {
      clignoR.state = 1 - ((millis() - clignoR.startedTimeBlinking)/500)%2;
      clignoL.state = clignoR.state;
    }
    //Else, let blinkers behave as they were previously entitled to
}

//===============================================================


void Blink(Clignotant cligno)
{
    //If it shall blink, do the maths to blink //Blink half a second
    if (cligno.isBlinking) cligno.state = 1 - ((millis() - cligno.startedTimeBlinking)/500)%2;
    
    //Else, shut off AND save start time for next toggle
    else {cligno.startedTimeBlinking = millis();    cligno.state = 0; }
}

//===============================================================

void DetermineTurning(ClickButton but, Clignotant cligno)
{
      // Update button state
  but.Update();

  // Save click codes in Function, as click codes are reset at next Update()
  if (but.clicks != 0) cligno.blinkerLastClick = but.clicks;
  

  // // Toggle Blinking if button is clicked once
  if(but.clicks == 1) cligno.isBlinking = !cligno.isBlinking;
      
}
