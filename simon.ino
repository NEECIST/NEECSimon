//---------------------------
// Simon Console - NEEC 2021
//---------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_SSD1306 display = Adafruit_SSD1306();

const int buttons[4] = {6, 5, 4, 3};
const int leds[4]    = {10, 9, 8 , 7};

//const int sda = A4;
//const int sdl = A5;

int buttonStates[4] = {0, 0, 0, 0};

int sequence[200];
int sequenceCurLen = 0;
int sequenceCurInput = 0;


int state = 3;
int pressed = 0;

int delayLed = 500;

int points = 0;
int level = 1;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int i = 0; i < 4; i++){
    pinMode(leds[i], OUTPUT);
    pinMode(buttons[i], INPUT_PULLUP);
  };

  digitalWrite(leds[0], HIGH);
  digitalWrite(leds[1], HIGH);
  digitalWrite(leds[2], HIGH);
  digitalWrite(leds[3], HIGH);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  updateOLED();
}

void loop() {
  for(int i = 0; i < 4; i++){
    buttonStates[i] = (digitalRead(buttons[i]) == LOW) ? 1 : 0; 
    Serial.print(digitalRead(buttons[i]));
  }
  Serial.println("");
  //Serial.print(level);
  //Serial.print("\t");
  //Serial.println(points);
  
  switch(state){
    //Mostrar sequence
    case 0:
      generateNext();
      state = 1;
      playSequence();
      break;
    case 1:
      waitAnswear();
      break;
    case 2:
      state = 3;
      wrongAnswear();
      break;
    case 3:
      nextGame();
      break;   
    }
}

void generateNext(){
  randomSeed(analogRead(A0));
  sequenceCurLen = sequenceCurLen + 1;
  sequence[sequenceCurLen] = random(4);
}

void playSequence(){
  delay(150);
  for(int i = 0; i < sequenceCurLen; i++){
    digitalWrite(leds[sequence[i]], HIGH);
    delay(delayLed);
    digitalWrite(leds[sequence[i]], LOW);
    delay(delayLed);
  }
}
void waitAnswear() {
  int sum = buttonStates[0] + buttonStates[1] + buttonStates[2] + buttonStates[3];
  if(sum != 0 && pressed == 1){
    return;
  }else if(sum == 0){
    pressed = 0;
    return;
  }else if(sum == 1){
    pressed = 1;
    for(int i = 0; i < 4; i++){
      if(buttonStates[i] == 1){
        digitalWrite(leds[i], HIGH);
        delay(delayLed);
        digitalWrite(leds[i], LOW);
        if(sequence[sequenceCurInput] == i){
          sequenceCurInput = sequenceCurInput + 1;
          if(sequenceCurInput == sequenceCurLen){
            sequenceCurInput = 0;
            state = 0;
            points++;
            level++;  
            updateOLED();
            return;            
          }else{
            state = 1;
            points++;
            updateOLED();
            return;
          }          
        }else{
          //Wrong Answear
          state = 2;
          return;
        }
      }
    }    
  }else{
    state = 2;
    return;
  }
}

void wrongAnswear(){
  for(int i  = 0; i < 3; i++){
    digitalWrite(leds[0], HIGH);
    digitalWrite(leds[1], HIGH);
    digitalWrite(leds[2], HIGH);
    digitalWrite(leds[3], HIGH);
    delay(delayLed/2);
    digitalWrite(leds[0], LOW);
    digitalWrite(leds[1], LOW);
    digitalWrite(leds[2], LOW);
    digitalWrite(leds[3], LOW);
    delay(delayLed/2);
  }
  digitalWrite(leds[0], HIGH);
  digitalWrite(leds[1], HIGH);
  digitalWrite(leds[2], HIGH);
  digitalWrite(leds[3], HIGH);

  points = 0;
  level = 0;
  updateOLED();
}
void nextGame(){
  int sum = buttonStates[0] + buttonStates[1] + buttonStates[2] + buttonStates[3];
  if(sum == 4){
    digitalWrite(leds[0], LOW);
    digitalWrite(leds[1], LOW);
    digitalWrite(leds[2], LOW);
    digitalWrite(leds[3], LOW);
    sequenceCurLen = 0;
    sequenceCurInput = 0;
    delay(2000);
    state = 0;
  }
}

void updateOLED(){
  display.clearDisplay();
  
  // Display static text
  display.setCursor(32, 0);
  display.println("NEECSimon");
  display.display(); 

  display.setCursor(12,10);
  // Display static text
  display.print("Nivel: ");
  display.println(level);
  display.display();

  display.setCursor(12,20);
  // Display static text
  display.print("Pontos:");
  display.println(points);
  display.display();
}
