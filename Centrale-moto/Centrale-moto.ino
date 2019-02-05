#include <ClickButton.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <ACS712.h>

#define COMMODOS 1 // Are we in commodos (for turns) or clickbutton setup ?

// Indexes of animations players
#define anim_TURNSIGNALS 0
#define anim_FRONTBEAM 1         // ON / OFF
#define anim_TAIL 2              // Three states: LOW / HIGH / OFF
#define anim_MESURECOURANT 3              // ON / OFF

// =================== MAPPING ===================
//                   ____________                
//                   \          /                
//                  a_HIBEAM_pin (7)             
//                  a_LOBEAM_pin (6)             
//       np_FL_pin (2)   \__/   np_FR_pin (3)    
//            \/________[_][_]________\/        
//                  a_INSTR_pin(9)               
//                        !!                    
//    b_LEFT_pin (14)-----------b_RIGHT_pin (17) 
//      b_BEAM_pin (15) !----! b_HORN_pin (16)   
//                        !!                    
//                        !!                    
//       a_HORN_pin(8)   oooo                   
//                      o°°°°o                  
//                      o°°°°o   b_BRAKE (18)    
//                       o°°o                   
//       np_RL_pin (4)    oo      np_RL_pin (5)  
//            /\__________!!__________/\        
//                        !!                    
//                       [OO]                   
//                  np_TAIL_pin(10)               
//                                              



//  A0   A1   A2   A3   A4   A5
//  14   15   16   17   18   19
//  in------------------------>
//                              
//                              
//  n/a  n/a  OUT-------------------------------------------------------->
//   0    1    2    3    4    5    6    7    8    9    10    11    12    13
//                             


#define BUTTON_LEFT 14    // Clickbutton / SWITCH
#define BUTTON_BEAM 15    // clickbutton
#define BUTTON_HORN 16    // clickbutton
#define BUTTON_RIGHT 17   // Clickbutton / SWITCH
#define CONTACT_BRAKE 18  // Contact

#define a_FRONTLEFT 2   // Neopixel
#define a_FRONTRIGHT 3  // Neopixel
#define a_REARLEFT 4    // Neopixel
#define a_REARRIGHT 5   // Neopixel
#define a_LOWBEAM 6     // SS-Relay
#define a_HIGHBEAM 7    // SS-Relay
#define a_HORN 8        // SS-Relay
#define a_INSTRLIGHTS 9 // SS-Relay
#define a_TAIL 10       // Neopixel

//#define SENSOR_CURRENT 19 // Current sensor

// Animator parameters
const uint16_t AnimCount = 4; //max 7 anims runnin at the same time (7 equipments)
NeoPixelAnimator animations(AnimCount); // , NEO_MILLISECONDS ; NeoPixel animation management object

// Neopixel parameters
const uint16_t TurnSignalPixelCount = 12; // common number of pixels for turn signals
const uint16_t TailPixelCount = 12; // common number of pixels for turn signals
const uint16_t TailStripTilt = 0; // Offset index to apply to tail strip in case level animation is done

// Turn signals parameters
const float MaxLightness = 0.4f; // max lightness at the head of the tail (0.5f is full bright)
const float TurningAnimationTime = 1000; // duration of animation to loop (in ms)
uint16_t TurningAnimationCurrentlyRunning = 0; // 0=off ; 1=left ; 2=right ; 3=warnings
byte f_WARNINGS_READY = 0;
RgbColor TurnSignalsColor(50, 4, 0);  //  = RgbColor(255,30,0) --- TODO ----


// Buttons & inputs (constructors)
ClickButton b_LEFT_cB(BUTTON_LEFT, LOW, CLICKBTN_PULLUP);
ClickButton b_BEAM_cB(BUTTON_BEAM, LOW, CLICKBTN_PULLUP);
ClickButton b_HORN_cB(BUTTON_HORN, LOW, CLICKBTN_PULLUP);
ClickButton b_RIGHT_cB(BUTTON_RIGHT, LOW, CLICKBTN_PULLUP);

// Actuators and outputs (constructors)
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripLeftFront(TurnSignalPixelCount, a_FRONTLEFT);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripLeftRear(TurnSignalPixelCount, a_REARLEFT);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripRightFront(TurnSignalPixelCount, a_FRONTRIGHT);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripRightRear(TurnSignalPixelCount, a_REARRIGHT);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> stripTail(TailPixelCount, a_TAIL);

// Beam section
byte f_BEAM_ON;
byte f_BEAM_HB;

// Current sensor section
//ACS712 capteurCourant(ACS712_05B, SENSOR_CURRENT);
//float mesureCourant;
//int compteur=0;
// int numberOfSamples=20;
// int samplingTime=5;
// float zeroOffset = 0.0391;

// Debug section
char buf [64];

void setup(){

    // Debug serial
    Serial.begin(115200);

    // Setup output relay pins
    SetupOutputPin(a_LOWBEAM);
    SetupOutputPin(a_HIGHBEAM);
    SetupOutputPin(a_HORN);
    SetupOutputPin(a_INSTRLIGHTS);

    // Setup NeopixelsBuses
    stripLeftFront.Begin();
    stripLeftRear.Begin();
    stripRightFront.Begin();
    stripRightRear.Begin();
    stripTail.Begin();
    stripLeftFront.Show();
    stripLeftRear.Show();
    stripRightFront.Show();
    stripRightRear.Show();
    stripTail.Show();

    // Setup Clickbuttons
    b_LEFT_cB.debounceTime   = 20;   // Debounce timer in ms
    b_LEFT_cB.multiclickTime = 180;  // Time limit for multi clicks
    b_LEFT_cB.longClickTime  = 1000; // time until "held-down clicks" register

    b_BEAM_cB.debounceTime   = 20;   // Debounce timer in ms
    b_BEAM_cB.multiclickTime = 180;  // Time limit for multi clicks
    b_BEAM_cB.longClickTime  = 1000; // time until "held-down clicks" register

    b_HORN_cB.debounceTime   = 20;   // Debounce timer in ms
    b_HORN_cB.multiclickTime = 180;  // Time limit for multi clicks
    b_HORN_cB.longClickTime  = 1000; // time until "held-down clicks" register

    b_RIGHT_cB.debounceTime   = 20;   // Debounce timer in ms
    b_RIGHT_cB.multiclickTime = 180;  // Time limit for multi clicks
    b_RIGHT_cB.longClickTime  = 1000; // time until "held-down clicks" register

    // Setup beam toggles:
    f_BEAM_ON = HIGH;
    f_BEAM_HB = LOW;
    digitalWrite(a_HIGHBEAM, LOW);
    digitalWrite(a_LOWBEAM, HIGH);

    // Setup Instrument lights
    digitalWrite(a_INSTRLIGHTS, f_BEAM_ON);

    // Setup Horn
    digitalWrite(a_HORN, LOW);

    // Setup current sensor
    //delay(100);
    //capteurCourant.calibrate();
}

void loop(){

    UpdateClickButtonsBeforeReading();

    // Where the magic happens :D

#if COMMODOS == 1
    if (b_LEFT_cB.depressed && b_RIGHT_cB.depressed){ //ERROR --> shorting on one of the buttons
        Serial.println("Error, one of the switch is shorted to ground");
        animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, NotTurningAnimation); // TODO: Add error anim
    } else if (b_LEFT_cB.depressed){ // Right turn
        if (TurningAnimationCurrentlyRunning != 1)
            animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, TurningLeftAnimation);
    } else if (b_RIGHT_cB.depressed) { // Left turn
        if (TurningAnimationCurrentlyRunning != 2)
            animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, TurningRightAnimation);
    } else if (b_BEAM_cB.clicks == -2) { //Start warnings
        if (TurningAnimationCurrentlyRunning != 3){
            animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, WarningAnimation);
        } else {
            animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, NotTurningAnimation);
        }
    }
    else if (TurningAnimationCurrentlyRunning != 3){ // Not turning && not warning
        animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, NotTurningAnimation);
    }

#else

    //------Clickbutton case - not used in handlebar V1-----------
    if (b_LEFT_cB.clicks == 1){ // Left button pressed once
        if (TurningAnimationCurrentlyRunning != 1)
            animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, TurningLeftAnimation);
        else {
            animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, NotTurningAnimation);
        }
    }

    if (b_RIGHT_cB.clicks == 1){ // Right button pressed once
        if (TurningAnimationCurrentlyRunning != 2) {
            animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, TurningRightAnimation);
        } else {
            animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, NotTurningAnimation);
        }
    }

    //-------Clickbutton case - not used here, using double long click on BEAM------
    if (b_RIGHT_cB.clicks == -1 || b_LEFT_cB.clicks == -1){ // Long click on turn buttons
      if (f_WARNINGS_READY){ // Toggle Warnings ON/OFF depending on running or not!
            // reset armed indicator
            f_WARNINGS_READY = 0;
            // toggle on/off
            if (TurningAnimationCurrentlyRunning != 3){
                animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, WarningAnimation);
            } else {
                animations.StartAnimation(anim_TURNSIGNALS, TurningAnimationTime, NotTurningAnimation);
            }
        } 
      else { // Arm warning indicators
            f_WARNINGS_READY = 1;
      }
    }
#endif

    // Beam function
    if (b_BEAM_cB.clicks == 1) f_BEAM_HB = !f_BEAM_HB;
    if (b_BEAM_cB.clicks == -1) f_BEAM_ON =!f_BEAM_ON;

    if (f_BEAM_ON){
        digitalWrite(a_HIGHBEAM, f_BEAM_HB);
        digitalWrite(a_LOWBEAM, !f_BEAM_HB);
    } else {
        digitalWrite(a_HIGHBEAM, LOW);
        digitalWrite(a_LOWBEAM, LOW);
    }

    // Instruments lights with beams
    digitalWrite(a_INSTRLIGHTS, f_BEAM_ON);

    // Horn
    digitalWrite(a_HORN, b_HORN_cB.depressed);

    animations.UpdateAnimations();
    ShowAllStrips();

}


// ======== HELPERS & OTHERS ============

void ShowAllStrips(){ // Show strips (only the ones that are dirty)
    if (stripLeftFront.IsDirty())    stripLeftFront.Show();
    if (stripLeftRear.IsDirty())     stripLeftRear.Show();
    if (stripRightFront.IsDirty())   stripRightFront.Show();
    if (stripRightRear.IsDirty())    stripRightRear.Show();
    if (stripTail.IsDirty())         stripTail.Show();
}

void SetupOutputPin(uint16_t pinNumber){
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, LOW);
}

// void SetupInputPinPullup(int pinNumber){
//   pinMode(pinNumber, INPUT);
//   digitalWrite(pinNumber, 0);
// }

void UpdateClickButtonsBeforeReading(){
    b_LEFT_cB.Update();
    b_BEAM_cB.Update();
    b_HORN_cB.Update();
    b_RIGHT_cB.Update();
}

#pragma region animations


void NotTurningAnimation(const AnimationParam& _param){
    TurningAnimationCurrentlyRunning = 0;
    stripLeftFront.ClearTo(RgbColor(0,0,0));
    stripRightFront.ClearTo(RgbColor(0,0,0));
    stripLeftRear.ClearTo(RgbColor(0,0,0));
    stripRightRear.ClearTo(RgbColor(0,0,0));
}

void TurningLeftAnimation(const AnimationParam& _param){

    TurningAnimationCurrentlyRunning = 1; // This is the LEFT anim

    int ScaledProgress = static_cast<int>(_param.progress * 36) % 12; // looping 12-based index (as three-fold animation)

    if (_param.state == AnimationState_Started){           // animation started ; draw all

        // Turning off right side!
        stripRightFront.ClearTo(RgbColor(0,0,0));
        stripRightRear.ClearTo(RgbColor(0,0,0));

        // Lighting on left side
        stripLeftFront.ClearTo(TurnSignalsColor);
        stripLeftRear.ClearTo(TurnSignalsColor);      

    } else if (_param.state == AnimationState_Completed) { // animation finished ; restart

        animations.RestartAnimation(_param.index);

    } else {    // animation progression

        if (_param.progress < 0.333333f) { // scaledprogress is between 0 (excluded) and 12 (excluded too)
            
            for(int i = 0; i <= ScaledProgress; i++)
            {
                stripLeftFront.SetPixelColor(i, RgbColor(0,0,0));
                stripLeftRear.SetPixelColor(11-i, RgbColor(0,0,0));
            }
            
        } else if (_param.progress < 0.666666f) {

            stripLeftFront.ClearTo(RgbColor(0,0,0));
            stripLeftRear.ClearTo(RgbColor(0,0,0));

        } else if (_param.progress < 0.999999f) {
            
            for(int i = 0; i <= ScaledProgress; i++)
            {
                stripLeftFront.SetPixelColor(i, TurnSignalsColor);
                stripLeftRear.SetPixelColor(11-i, TurnSignalsColor);
            }

        }
    }
}

void TurningRightAnimation(const AnimationParam& _param){

    TurningAnimationCurrentlyRunning = 2; // This is the LEFT anim 

    int ScaledProgress = static_cast<int>(_param.progress * 36) % 12; // looping 12-based index (as three-fold animation)

    if (_param.state == AnimationState_Started){           // animation started ; draw all

        // Turning off left side!
        stripLeftFront.ClearTo(RgbColor(0,0,0));
        stripLeftRear.ClearTo(RgbColor(0,0,0));

        // Lighting on right side
        stripRightFront.ClearTo(TurnSignalsColor);
        stripRightRear.ClearTo(TurnSignalsColor);         

    } else if (_param.state == AnimationState_Completed) { // animation finished ; restart

        animations.RestartAnimation(_param.index);

    } else {    // animation progression

        if (_param.progress < 0.333333f) { // scaledprogress is between 0 (excluded) and 12 (excluded too)
            for(int i = 0; i <= ScaledProgress; i++)
            {
                stripRightFront.SetPixelColor(11-i, RgbColor(0,0,0));
                stripRightRear.SetPixelColor(i, RgbColor(0,0,0));
            }
            
        } else if (_param.progress < 0.666666f) {

            stripRightFront.ClearTo(RgbColor(0,0,0));
            stripRightRear.ClearTo(RgbColor(0,0,0));

        } else if (_param.progress < 0.999999f) {

            for(int i = 0; i <= ScaledProgress; i++)
            {
                stripRightFront.SetPixelColor(11-i, TurnSignalsColor);
                stripRightRear.SetPixelColor(i, TurnSignalsColor);
            }

        }
    }
}

void WarningAnimation(const AnimationParam& _param){

    TurningAnimationCurrentlyRunning = 3; // This is the WARNING anim 

    int ScaledProgress = static_cast<int>(_param.progress * 36) % 12; // looping 12-based index (as three-fold animation)

    if (_param.state == AnimationState_Started){           // animation started ; draw all

        // Lighting on everyone
        stripRightFront.ClearTo(TurnSignalsColor);
        stripRightRear.ClearTo(TurnSignalsColor);
        stripLeftFront.ClearTo(TurnSignalsColor);
        stripLeftRear.ClearTo(TurnSignalsColor);         

    } else if (_param.state == AnimationState_Completed) { // animation finished ; restart

        animations.RestartAnimation(_param.index);

    } else {    // animation progression

        if (_param.progress < 0.333333f) { // scaledprogress is between 0 (excluded) and 12 (excluded too)
            for(int i = 0; i <= ScaledProgress; i++)
            {
                stripRightFront.SetPixelColor(11-i, RgbColor(0,0,0));
                stripRightRear.SetPixelColor(i, RgbColor(0,0,0));
                stripLeftFront.SetPixelColor(i, RgbColor(0,0,0));
                stripLeftRear.SetPixelColor(11-i, RgbColor(0,0,0));
            }
            
        } else if (_param.progress < 0.666666f) {

            stripRightFront.ClearTo(RgbColor(0,0,0));
            stripRightRear.ClearTo(RgbColor(0,0,0));
            stripLeftFront.ClearTo(RgbColor(0,0,0));
            stripLeftRear.ClearTo(RgbColor(0,0,0));

        } else if (_param.progress < 0.999999f) {

            for(int i = 0; i <= ScaledProgress; i++)
            {
                stripRightFront.SetPixelColor(11-i, TurnSignalsColor);
                stripRightRear.SetPixelColor(i, TurnSignalsColor);
                stripLeftFront.SetPixelColor(i, TurnSignalsColor);
                stripLeftRear.SetPixelColor(11-i, TurnSignalsColor);
            }

        }
    }
}

/* void MeasuringCurrentAnimation(const AnimationParam& _param){

    // Remaining issue: zero-ing at startup ;
    // Arduino idle current is approximately 39 mA
    
    if (_param.state == AnimationState_Completed) {
        if (compteur < numberOfSamples){
            mesureCourant += capteurCourant.getCurrentDC() + zeroOffset;
            ++compteur;
            animations.RestartAnimation(anim_MESURECOURANT);
        } else {
            mesureCourant /= compteur*10;
            Serial.println(mesureCourant*1000,0);
            compteur = 0;
            mesureCourant = 0;
            animations.RestartAnimation(anim_MESURECOURANT);
        }
    }
}
*/
#pragma endregion

// TODO :  add brake neopixels (or led plates) + Do animations