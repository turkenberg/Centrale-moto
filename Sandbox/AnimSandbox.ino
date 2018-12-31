#pragma region stuff
// NeoPixelFunLoop
// This example will move a trail of light around a series of pixels.  
// A ring formation of pixels looks best.  
// The trail will have a slowly fading tail.
// 
// This will demonstrate the use of the RotateRight method.
//

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <ClickButton.h>


const uint16_t PixelCount = 36; // make sure to set this to the number of pixels in your strip
const uint16_t PixelPin = 12;  // make sure to set this to the correct pin, ignored for Esp8266
const uint16_t AnimCount = 1; // we only need one
const uint16_t TailLength = 12; // length of the tail, must be shorter than PixelCount
const float MaxLightness = 0.4f; // max lightness at the head of the tail (0.5f is full bright)

ClickButton testButton(2, LOW, CLICKBTN_PULLUP);

NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
// for esp8266 omit the pin
//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount);

NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object

#pragma endregion

RgbColor color(124,15,0);
byte hasStarted = 0;
char buf [64];
// ============================================
// if (param.state == AnimationState_Started)
// if (param.state == AnimationState_Completed)
// strip.RotateLeft(1);

void LoopAnimUpdate(const AnimationParam& param){
        

    if (!hasStarted){
        for(int i = 0; i < TailLength; i++)
        {
            strip.SetPixelColor(i,color);
        }
        hasStarted = 1;
    }
    else if (param.state == AnimationState_Completed){
        animations.RestartAnimation(param.index);
        strip.RotateRight(1);
    }

    sprintf (buf, "Animation progress is %d \r\n", (float) param.progress*10000);
    Serial.println(buf);
}
// =======================================================
void setup(){
    // button(s) (pull_up)
    testButton.debounceTime   = 20;   // Debounce timer in ms
    testButton.multiclickTime = 180;  // Time limit for multi clicks
    testButton.longClickTime  = 1000; // time until "held-down clicks" register
    
    Serial.begin(9600);

    strip.Begin();
    strip.Show();}
void loop(){
    testButton.Update();
    if (testButton.clicks == 1)     // we use the index 0 animation to time how often we rotate all the pixels
      {
      if (animations.IsAnimating())  {
        animations.StopAnimation(0);
        hasStarted = 0;
        strip.ClearTo(RgbColor(0,0,0));
      }

      else
        animations.StartAnimation(0, 25, LoopAnimUpdate);
      }

    // this is all that is needed to keep it running
    // and avoiding using delay() is always a good thing for
    // any timing related routines
    animations.UpdateAnimations();
    strip.Show();}


