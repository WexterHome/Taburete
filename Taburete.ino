#include <FastLED.h>
#include "LowPower.h"

#define NUM_EXT_LEDS 29   //********************
#define NUM_LFACE_LEDS 3  //********************
#define NUM_CFACE_LEDS 2  //********************
#define NUM_RFACE_LEDS 3  //********************
#define PIN_EXT_LEDS 9   //********************
#define PIN_LFACE_LEDS 10 //********************
#define PIN_CFACE_LEDS 11 //********************
#define PIN_RFACE_LEDS 12 //********************
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define MAX_BRIGHTNESS 200 //********************

const int BUTTON = 3;
const int threshold = 500;
volatile int mode = 1;
volatile int lastTime = 0;

int brightness = MAX_BRIGHTNESS;
bool brightness_direction = false;


CRGB ext_leds[NUM_EXT_LEDS];
CRGB lface_leds[NUM_LFACE_LEDS];
CRGB cface_leds[NUM_CFACE_LEDS];
CRGB rface_leds[NUM_RFACE_LEDS];

//ColorPalette Variables
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


void setup(){
  Serial.begin(9600);

  //Configuración de las tiras LEDs
  FastLED.addLeds<LED_TYPE, PIN_EXT_LEDS, COLOR_ORDER>(ext_leds, NUM_EXT_LEDS);
  FastLED.addLeds<LED_TYPE, PIN_LFACE_LEDS, COLOR_ORDER>(lface_leds, NUM_LFACE_LEDS);
  FastLED.addLeds<LED_TYPE, PIN_CFACE_LEDS, COLOR_ORDER>(cface_leds, NUM_CFACE_LEDS);
  FastLED.addLeds<LED_TYPE, PIN_RFACE_LEDS, COLOR_ORDER>(rface_leds, NUM_RFACE_LEDS);
  //Configuración del brillo
  FastLED.setBrightness(MAX_BRIGHTNESS);
  //Configuración efecto ColorPalette
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  pinMode(BUTTON, INPUT_PULLUP);
  //Interrupción para usar el botón
  attachInterrupt(digitalPinToInterrupt(BUTTON), modesController, RISING);
}


void loop(){
  switch(mode){
    case 1:
      allLedsOn();
      delay(20);
      break;
    
    case 2:
      onlyFaceLedsOn();
      LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_ON, TIMER1_ON, TIMER0_ON, 
                SPI_OFF, USART0_ON, TWI_OFF);
      break;

    case 3:
      onlyEyesLedsOn();
      delay(200);
      break;

    case 4:
      safeEnergy();
      LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_ON, TIMER1_ON, TIMER0_ON, 
                SPI_OFF, USART0_ON, TWI_OFF);
      break;
  }
  
  FastLED.show();
}

void allLedsOn(){
  //Brillo al máximo
  brightness = MAX_BRIGHTNESS;
  FastLED.setBrightness(brightness);
  
  //Efecto ColorPalette - LEDs externos
  ChangePalettePeriodically();
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; // motion speed 
  FillLEDsFromPaletteColors( startIndex);

  //Cara de la pantera
  //Encendemos los leds de la cara color amarillo - Puedes cambiar el color
  for(int i=0; i<NUM_LFACE_LEDS; i+=2){
    lface_leds[i].setRGB(0,0,255);  //*******************
    rface_leds[i].setRGB(0,0,255);  //*******************
  }

  for(int i=0; i<NUM_CFACE_LEDS; i++){
    cface_leds[i].setRGB(0,0,255);  //*******************
  }

  //Ojos de color rojo - Puedes cambiar el color
  lface_leds[1].setRGB(0,0,255);  //***************** 
  rface_leds[1].setRGB(0,0,255);  //*****************
}

void onlyFaceLedsOn(){
  //Brillo al máximo
  brightness = MAX_BRIGHTNESS;
  FastLED.setBrightness(brightness);
  //Apagamos los LEDs externos
  for(int i = 0; i < NUM_EXT_LEDS; i++){
    ext_leds[i].setRGB(0,0,0);
  }

  //Encendemos los leds de la cara color amarillo - Puedes cambiar el color
  for(int i=0; i<NUM_LFACE_LEDS; i+=2){
    lface_leds[i].setRGB(255,255,255);  //*******************
    rface_leds[i].setRGB(255,255,255);  //*******************
  }

  for(int i=0; i<NUM_CFACE_LEDS; i++){
    cface_leds[i].setRGB(255,255,255);  //*******************
  }

  //Ojos de color rojo - Puedes cambiar el color
  lface_leds[1].setRGB(0,0,255);  //***************** 
  rface_leds[1].setRGB(0,0,255);  //*****************
}

void onlyEyesLedsOn(){
  //Apagamos los LEDs externos
  for(int i = 0; i < NUM_EXT_LEDS; i++){
    ext_leds[i].setRGB(0,0,0);
  }

  //Apagamos los leds de la cara
  for(int i=0; i<NUM_LFACE_LEDS; i+=2){
    lface_leds[i].setRGB(0,0,0);
    rface_leds[i].setRGB(0,0,0);
  }

  for(int i=0; i<NUM_CFACE_LEDS; i++){
    cface_leds[i].setRGB(0,0,0);
  }

  //Encendemos los ojos de color rojo - Puedes cambiar el color
  lface_leds[1].setRGB(0, 0, 255);  //********************
  rface_leds[1].setRGB(0, 0, 255);  //********************

  //Modificamos el brillo de los ojos
  if(brightness_direction == false){
    if(brightness > 0){
      brightness -=1;
    }
    else{
      brightness_direction = true;
    }
  }

  else{
    if(brightness < MAX_BRIGHTNESS){
      brightness +=1;
    }
    else{
      brightness_direction = false;
    }
  }
  FastLED.setBrightness(brightness);
}

void safeEnergy(){
  //Brillo al máximo
  brightness = MAX_BRIGHTNESS;
  FastLED.setBrightness(brightness);
  
  for(int i = 0; i < NUM_EXT_LEDS; i++){
    ext_leds[i].setRGB(0,0,0);
  }

  //Apagamos los leds de la cara
  for(int i=0; i<NUM_LFACE_LEDS; i+=2){
    lface_leds[i].setRGB(0,0,0);
    rface_leds[i].setRGB(0,0,0);
  }

  for(int i=0; i<NUM_CFACE_LEDS; i++){
    cface_leds[i].setRGB(0,0,0);
  }

  //Encendemos los ojos de color rojo - Puedes cambiar el color
  lface_leds[1].setRGB(0, 0, 255 );  //********************
  rface_leds[1].setRGB(0, 0, 255);  //********************
}

void modesController(){
  if((millis()-lastTime) > threshold){
    lastTime = millis();
    if(mode > 3){
      mode = 1;
    }
    else{
      mode++;
    }
  }
}


//FUNCIONES EFECTO COLORPALETTE
void FillLEDsFromPaletteColors( uint8_t colorIndex){
  
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_EXT_LEDS; ++i) {
        ext_leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void ChangePalettePeriodically(){
  
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

void SetupTotallyRandomPalette(){
  
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

void SetupBlackAndWhiteStripedPalette(){
  
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

void SetupPurpleAndGreenPalette(){
  
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};
