#include <ClickButton.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>


// Either COMMODOS or CLICKBUTTONS
//#define MODE COMMODOS

// Either NEOPIXELS or BULBS
#define TURNSIGNAL NEOPIXELS


#pragma region HORN
// ++++++++++++++COMMANDES++++++++++
//---HORN-----
const uint16_t b_HORN_pin = 5;
byte b_HORN_read;

// ++++++++++++++FONCTIONS++++++++++
// -none-

// ++++++++++++++ACTUATORS++++++++++
//---HORN-----
const uint16_t a_HORN_pin = 13;
byte a_HORN_do;
#pragma endregion



#pragma region INSTRUMENTLIGHT
// ++++++++++++++COMMANDES++++++++++
// -none-

// ++++++++++++++FONCTIONS++++++++++
// -none-

// ++++++++++++++ACTUATORS++++++++++
//---INSTR-----
const uint16_t a_INSTR_pin = 8;
byte a_INSTR_do;

#pragma endregion



#pragma region FRONTBEAM
// ++++++++++++++COMMANDES++++++++++
//---BEAM-----
const uint16_t b_BEAM_pin = 4;
ClickButton b_BEAM_cB(b_BEAM_pin, LOW, CLICKBTN_PULLUP);

// ++++++++++++++FONCTIONS++++++++++
//---BEAM---
byte f_BEAM_ON;
byte f_BEAM_HB;

// ++++++++++++++ACTUATORS++++++++++
//---BEAM-----
const uint16_t a_BEAM_pin = 9;
int a_BEAM_do;                // 'int' coz' do is analog
#pragma endregion



#pragma region TAILLIGHT
// ++++++++++++++COMMANDES++++++++++
//---TAIL-----
// const uint16_t a_INSTR_pin = 6;
// byte a_INSTR_do;
//---BRAKE-----
const uint16_t b_BRAKE_pin = 2;
byte b_BRAKE_read;

// ++++++++++++++FONCTIONS++++++++++
// -none-

// ++++++++++++++ACTUATORS++++++++++
//---BRAKE-----
const uint16_t a_BRAKE_pin = 10;
byte a_BRAKE_do;

#pragma endregion



#pragma region TURNSIGNAL
// ++++++++++++++COMMANDES++++++++++
//---LEFT-----
const uint16_t b_LEFT_pin = 7;
ClickButton b_LEFT_cB(b_LEFT_pin, LOW, CLICKBTN_PULLUP);

//---RIGHT-----
const uint16_t b_RIGHT_pin = 6;
ClickButton b_RIGHT_cB(b_RIGHT_pin, LOW, CLICKBTN_PULLUP);

// ++++++++++++++FONCTIONS++++++++++
//---TURN---
byte f_TURN_ONELONGCLICK;
byte f_TURN_WARNINGS;
byte f_TURN_LEFT;
byte f_TURN_BLKNGON;
byte f_v;
unsigned long timeStartedBlinking;

// ++++++++++++++ACTUATORS++++++++++
#if TURNSIGNAL == BULBS
//---LEFT-----
const uint16_t a_LEFT_pin = 12;
byte a_LEFT_do;
byte aff_LEFT_do;

//---RIGHT-----
const uint16_t a_RIGHT_pin = 11;
byte a_RIGHT_do;
byte aff_RIGHT_do;

#elif TURNSIGNAL == NEOPIXELS

const uint16_t PixelCount = 36; // common number of pixels (real is 12 but 36 allows for cooler animation)
const uint16_t AnimCount = 1; //Three anims, but ONE channel, for now! There will pbly be a second one on the tail light
const uint16_t TailLength = 12; // length of the tail, must be shorter than PixelCount
const float MaxLightness = 0.4f; // max lightness at the head of the tail (0.5f is full bright)

const uint16_t a_LEFTFRONT_pin = 2;  // data pin for neopixel
const uint16_t a_RIGHTFRONT_pin = 3;  // data pin for neopixel
//const uint16_t a_LEFTREAR_pin = 11;  // data pin for neopixel
//const uint16_t a_RIGHTREAR_pin = 12;  // data pin for neopixel

//NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripLeftFront(PixelCount, a_LEFTFRONT_pin);   // new strip
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripRightFront(PixelCount, a_RIGHTFRONT_pin); // new strip
//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripLeftRear(PixelCount, a_LEFTREAR_pin);   // new strip
//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripRightRear(PixelCount, a_RIGHTREAR_pin); // new strip

NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object

#endif

#pragma endregion

//===============================================================


void setup(){

  //Serial.begin(9600);

  //#if MODE == CLICKBUTTONS
  setupBrakeClickButtons();
  setupHornClickButtons();
  setupBeamClickButtons();
  setupTurnClickButtons();
  //#elif MODE == COMMODOS


  //#endif
}

void loop(){

  BrakeFunction();
  HornFunction();
  BeamFunction();
  TurnFunction();

  UpdateBrake();
  UpdateHorn();
  UpdateBeam();
  UpdateTurn();

}
#pragma region SETUPS
void setupTurnClickButtons(){
  // button(s) (pull_up)
  b_LEFT_cB.debounceTime   = 20;   // Debounce timer in ms
  b_LEFT_cB.multiclickTime = 180;  // Time limit for multi clicks
  b_LEFT_cB.longClickTime  = 1000; // time until "held-down clicks" register

  b_RIGHT_cB.debounceTime   = 20;   // Debounce timer in ms
  b_RIGHT_cB.multiclickTime = 180;  // Time limit for multi clicks
  b_RIGHT_cB.longClickTime  = 1000; // time until "held-down clicks" register

  // Functions
  f_TURN_WARNINGS=0;
  f_TURN_LEFT=0;
  f_TURN_BLKNGON=0;
  timeStartedBlinking=0;

  #if TURNSIGNAL == BULBS
  // actuator(s)
  pinMode(a_LEFT_pin, OUTPUT);
  a_LEFT_do = 0;
  digitalWrite(a_LEFT_pin, a_LEFT_do);
  aff_LEFT_do = 0;
  //           -----
  pinMode(a_RIGHT_pin, OUTPUT);
  a_RIGHT_do = 0;
  analogWrite(a_RIGHT_pin, a_RIGHT_do);
  aff_RIGHT_do = 0;

  #elif TURNSIGNAL == NEOPIXELS
  stripLeftFront.Begin();
  stripRightFront.Begin();
  //stripLeftRear.Begin();
  //stripRightRear.Begin();

  stripLeftFront.Show();
  stripRightFront.Show();
  //stripLeftRear.Show();
  //stripRightRear.Show();

  a_LEFT_do = 0;
  a_RIGHT_do = 0;
  #endif
}

void setupBeamClickButtons(){
  // button(s) (pull_up)
  b_BEAM_cB.debounceTime   = 20;   // Debounce timer in ms
  b_BEAM_cB.multiclickTime = 100;  // Time limit for multi clicks
  b_BEAM_cB.longClickTime  = 1000; // time until "held-down clicks" register

  // Functions
  f_BEAM_ON = 1;
  f_BEAM_HB = 0;

  // actuator(s)
  pinMode(a_BEAM_pin, OUTPUT);
  a_BEAM_do = 0;
  analogWrite(a_BEAM_pin, a_BEAM_do);
  //           -----
  pinMode(a_INSTR_pin, OUTPUT);
  a_INSTR_do = 0;
  analogWrite(a_INSTR_pin, a_INSTR_do);
}

void setupBrakeClickButtons(){
  // button(s) (pull_up)
  pinMode(b_BRAKE_pin, INPUT);
  b_BRAKE_read = HIGH;
  digitalWrite(b_BRAKE_pin,b_BRAKE_read);
  

  // actuator(s)
  pinMode(a_BRAKE_pin, OUTPUT);
  a_BRAKE_do = LOW;
  digitalWrite(a_BRAKE_pin,a_BRAKE_do);
  
}

void setupHornClickButtons(){
  // button(s) (pull_up)
  pinMode(b_HORN_pin, INPUT);
  b_HORN_read = HIGH;
  digitalWrite(b_HORN_pin, b_HORN_read);
  

  // actuator(s)
  pinMode(a_HORN_pin, OUTPUT);
  a_HORN_do = LOW;
  digitalWrite(a_HORN_pin, a_HORN_do);
  
}
#pragma endregion



#pragma region FUNCTIONS

void TurnFunction(){
      
  // Update button state
  b_LEFT_cB.Update();
  b_RIGHT_cB.Update();
  
  // LEFT : deduce command from inputs
  if (b_LEFT_cB.clicks == 1) {
      TurnHandler(1);
  }

  // RIGHT : deduce command from inputs
  else if (b_RIGHT_cB.clicks == 1) {
      TurnHandler(0);
  }

  // BOTH : any did a long click
  if (b_RIGHT_cB.clicks == -1 || b_LEFT_cB.clicks == -1){
      if (f_TURN_ONELONGCLICK){ // Toggle Warnings!
            f_TURN_WARNINGS = !f_TURN_WARNINGS;
            f_TURN_ONELONGCLICK = 0;
            f_TURN_BLKNGON = 0;
      } 
      else {
            f_TURN_ONELONGCLICK = 1;
      }
  }
      


  // Now we have all commands, find what to do
  if (f_TURN_BLKNGON){
        if (f_TURN_LEFT)
        {
            a_LEFT_do=1;
            a_RIGHT_do=0;
        } 
        else
        {
            a_LEFT_do=0;
            a_RIGHT_do=1;
        }
  } else if (f_TURN_WARNINGS) {
  a_LEFT_do=1;
  a_RIGHT_do=1;

  } else {
  a_LEFT_do=0;
  a_RIGHT_do=0;
  }
}

void BeamFunction(){
      
  // Update button state
  b_BEAM_cB.Update();
  
  // toggle state on click
  if (b_BEAM_cB.clicks == 1) f_BEAM_HB = !f_BEAM_HB;

  // toggle on/off long click
  if (b_BEAM_cB.clicks == -1) f_BEAM_ON = !f_BEAM_ON;

  // THEN if ON: do HB or LB ; else ; turn off
  if (f_BEAM_ON){
        a_INSTR_do = 1;
        if (f_BEAM_HB) a_BEAM_do = 255;
        else a_BEAM_do = 40;
  } else {
        a_INSTR_do = 0;
        a_BEAM_do = 0;
  }

}

void BrakeFunction(){
      
  // Read input
  b_BRAKE_read = digitalRead(b_BRAKE_pin);

  // Apply rules
  if (b_BRAKE_read == 0)
  {
    a_BRAKE_do=HIGH;
  }
  else
  {
    a_BRAKE_do=LOW;
  }

}

void HornFunction(){
      
  // Read input
  b_HORN_read = digitalRead(b_HORN_pin);

  // Apply rules
  if (b_HORN_read == 0)
  {
    a_HORN_do=HIGH;
  }
  else
  {
    a_HORN_do=LOW;
  }

}
#pragma endregion



#pragma region UPDATES

void UpdateTurn(){

#if TURNSIGNAL == BULBS
  // Update blink clock
  f_v = 1 - ((millis()- timeStartedBlinking)/500)%2; // - timeStartedBlinking)

  // Blink accordingly
  if (a_LEFT_do == a_RIGHT_do) {
        if (!a_LEFT_do){ // equals and LOW
              aff_LEFT_do = 0;
              aff_RIGHT_do = 0;
        } else { // equals ans HIGH
              aff_LEFT_do = f_v;
              aff_RIGHT_do = f_v;
        }
  } else if (a_LEFT_do) { // Different L HIGH R LOW
        aff_LEFT_do = f_v;
        aff_RIGHT_do = 0;
  } else { //diff. %
        aff_LEFT_do = 0;
        aff_RIGHT_do = f_v;
  }


  digitalWrite(a_LEFT_pin, aff_LEFT_do);
  digitalWrite(a_RIGHT_pin, aff_RIGHT_do);

#elif TURNSIGNAL == NEOPIXELS
  if (a_LEFT_do == a_RIGHT_do) {
        if (!a_LEFT_do){ // equals and LOW
            animations.StartAnimation(0, 30, OffAnimUpdate);
        } else { // equals ans HIGH
            //anim warning
        }
  } else if (a_LEFT_do) { // Different L HIGH R LOW
        //anim left
  } else { //diff. %
        //anim right
  }

  animations.UpdateAnimations();

  stripLeftFront.Show();
  stripRightFront.Show();
  //StripShallUpdate(stripLeftRear);
  //StripShallUpdate(stripRightRear);

#endif
}



void UpdateBeam(){
  // write the do
  analogWrite(a_BEAM_pin,a_BEAM_do);
  digitalWrite(a_INSTR_pin, a_INSTR_do);
}

void UpdateBrake(){
  // write the do
  digitalWrite(a_BRAKE_pin,a_BRAKE_do);
}

void UpdateHorn(){
  // write the do
  digitalWrite(a_HORN_pin,a_HORN_do);
}
#pragma endregion



#pragma region HELPERS

void TurnHandler(byte _leftButton){
  if (_leftButton){
       // we are blinking - command is left
      if (f_TURN_BLKNGON)
      {
            // it was blinking left, toggle off
            if (f_TURN_LEFT)
            {     
                  f_TURN_BLKNGON = 0;
            }
            else // it was blinking right, shut down
            {
                  f_TURN_LEFT = 1;
                  timeStartedBlinking = millis(); // new blink cycle
            }
            
      }
      else // We were not blinking, toggle new blink cycle
      {
            f_TURN_WARNINGS = 0; // in case we were warning
            f_TURN_BLKNGON = 1;
            f_TURN_LEFT = 1;
            timeStartedBlinking = millis(); // new blink cycle
      }
  }
  else
  {
       // we are blinking - command is right
      if (f_TURN_BLKNGON)
      {
            // it was blinking left, switch side
            if (f_TURN_LEFT)
            {     
                  f_TURN_LEFT = 0;
                  timeStartedBlinking = millis(); // new blink cycle
            }
            else // it was blinking right, shut down
            {
                  f_TURN_BLKNGON = 0;
            }
            
      }
      else // We were not blinking, toggle new blink cycle
      {
            f_TURN_WARNINGS = 0; // in case we were warning
            f_TURN_BLKNGON = 1;
            f_TURN_LEFT = 0;
            timeStartedBlinking = millis(); // new blink cycle
      }
  }
}

void LeftAnimUpdate(const AnimationParam param)
{
    if (param.state == AnimationState_Started)
    {
      //DrawTailPixels(stripRightFront);
    }
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    else if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);

        // rotate the complete strip one pixel to the right on every update
        stripLeftFront.RotateLeft(1);
        //stripLeftRear.RotateRight(1)  
    }
}

void RightAnimUpdate(const AnimationParam param)
{
    if (param.state == AnimationState_Started)
    {
      DrawTailPixels(stripRightFront);
    }
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    else if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);

        // rotate the complete strip one pixel to the right on every update
        stripRightFront.RotateRight(1);
        //stripRightRear.RotateLeft(1);
    }
}

void WarningAnimUpdate(const AnimationParam param)
{
    if (param.state == AnimationState_Started)
    {
      RgbColor color = RgbColor(255, 40, 0);

      for (uint16_t index = 0; index < stripRightFront.PixelCount() && index <= TailLength; index++)
      {
          stripRightFront.SetPixelColor(index, color);
      }
    }
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    else if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);

        // rotate the complete strip one pixel to the right on every update
        stripRightFront.RotateRight(1);
        stripLeftFront.RotateLeft(1);
        //stripRightRear.RotateRight(1);
        //stripLeftRear.RotateRight(1);
    }
}

void OffAnimUpdate(const AnimationParam param)
{
  stripLeftFront.Clear();
  stripRightFront.Clear();
}

#pragma endregion