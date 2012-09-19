/* 
 Simon: the game
 
 The circuit:
 * UPDATE THIS LATER
 
 created 2 October 2012
 by Macklin Underdown
 PAT 451
 
 Debouncing code and theory taken from:
 http://www.arduino.cc/en/Tutorial/Debounce
 */



#include "pitches.h"        // add pitches header file to reference note frequencies

const int gameSize = 20;          // number of steps
const int numButtons = 4;

const int speakerPin = 9;   // initialize speaker pin

int buttonPin[] = {         // initialize button pins
  2, 3, 4, 5};
int ledPin[] = {            // initialize LED pins
  10, 11, 12, 13};
int buttonState[numButtons];
int buttonPrevious[numButtons];     // for debouncing purposes
int buttonPushCounter[numButtons];

long time[numButtons] = {
  0, 0, 0, 0};         // the last time the output pin was toggled
long debounce = 5;   // the debounce time, increase if the output flickers

int ledState[numButtons];

const int i_to_a = 97;

char simonInputArray[gameSize];
char simonMasterArray[gameSize];
int inputCounter = 0;

int buttonTone[numButtons] = {
  NOTE_C3, NOTE_E3, NOTE_G3, NOTE_C4};

void setup() {
  // open serial communication
  Serial.begin(9600);
  // initialize button pins, led pins, button states,  and other variable arrays
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(buttonPin[i], HIGH);
    buttonPrevious[i] = LOW;
    ledState[i] = LOW;
  }
  // the sum of all analog inputs creates a truly random seed every time
  randomSeed(analogRead(A0+A1+A2+A3+A4+A5));
  for (int i = 0; i < gameSize; i++) {
    // use that random seed to create a unique Simon array that will be checked against the input button presses
    int randomNum = random(numButtons);
    char masterChar = char(randomNum + i_to_a);
    simonMasterArray[i] = masterChar;
  }
  // prints out master array for debugging purposes
  Serial.println(simonMasterArray);
}

void loop() {
  for (int i = 0; i < numButtons; i++) {
    checkButton(i);
  }
}

void checkButton(int i) {
  // a button's state is constantly read into an array
  buttonState[i] = digitalRead(buttonPin[i]);
  // some software debouncing to check if the button was actually pressed
  // if the button is truly pressed
  if (buttonState[i] == HIGH && buttonPrevious[i] == LOW && millis() - time[i] > debounce) {
    // turn on the corresponding LED
    digitalWrite(ledPin[i], HIGH);
    // play a tone
    tone(speakerPin, buttonTone[i], 200);
    // wait
    delay(200);
    // turn off the corresponding LED
    digitalWrite(ledPin[i], LOW);
    // ensure the corresponding tones turns off
    noTone(speakerPin);
    // boolean flag to write text to the serial monitor once and not each time through the loop
    boolean buttonWriteText = true;
    if (buttonWriteText) {
      // anything that needs to happen only once happens here
      char inputChar = char(i + i_to_a);
      simonInputArray[inputCounter] = inputChar;
      Serial.print("MASTER:");
      Serial.print("\t");
      Serial.println(simonMasterArray);
      Serial.print("INPUT:");
      Serial.print("\t");
      Serial.println(simonInputArray);
      Serial.print("Index of array: ");
      Serial.println(inputCounter++);
      Serial.print("Input character: ");
      Serial.println(inputChar);
      if (simonInputArray[i] == simonMasterArray[i]) {
        Serial.println("MATCH");
      }
      else {
        Serial.println("INCORRECT");
      }
      Serial.println();
      // exit the if statement
      !buttonWriteText;
    }
    // set time of button to current time of program for debounce checking later
    time[i] = millis();    
  }
  // update the previous button state for button debouncing purposes
  buttonPrevious[i] = buttonState[i];
}



