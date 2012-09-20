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

#include "pitches.h"                // add pitches header file to reference note frequencies

const int gameSize = 20;            // number of steps
const int numButtons = 4;           // number of buttons (and also LEDs)

const int speakerPin = 9;           // initialize speaker pin

const int buttonPin[] = {                 // initialize button pins
  2, 3, 4, 5};
const int ledPin[] = {                    // initialize LED pins
  10, 11, 12, 13};
int buttonState[numButtons];
int buttonPrevious[numButtons];     // for debouncing purposes
int buttonPushCounter[numButtons];

long time[numButtons];              // the last time the output pin was toggled
long debounce = 5;                  // the debounce time, increase if the output flickers

int ledState[numButtons];

int simonInputArray[gameSize];      // array to determine
int simonMasterArray[gameSize];     // array that input will be checked against
int inputCounter = 0;

int currentStep;


int buttonTone[numButtons] = {
  NOTE_C3, NOTE_E3, NOTE_G3, NOTE_C4};

boolean game_init = true;

void setup() {
  // open serial communication
  Serial.begin(9600);
  // initialize button pins, led pins, button states, and other variable arrays
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(buttonPin[i], HIGH);
    buttonPrevious[i] = LOW;
    ledState[i] = LOW;
    time[i] = 0;
  }
}

void loop() {
  // initialzing game setup
  gameSetup();
  for (int i = 0; i < numButtons; i++) {
    checkButton(i);
  }
}

void gameSetup() {
  // this will only be run once at the beginning of each game
  if (game_init) {
    // the sum of all analog inputs creates a truly random seed for the Simon sequence
    randomSeed(analogRead(A0+A1+A2+A3+A4+A5));
    // assign a random value to each number in the Simon sequence
    for (int i = 0; i < gameSize; i++) {
      simonMasterArray[i] = random(numButtons);
      Serial.print(simonMasterArray[i]);
    }
  }
  // don't come back to gameSetup() until this flag is flipped
  game_init = false;
}

int checkButton(int i) {

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
    // tone(speakerPin, buttonTone[i], 200);
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
      simonInputArray[inputCounter] = i;
      Serial.print("MASTER:");
      Serial.print("\t");
      Serial.println(simonMasterArray[i]);
      Serial.print("INPUT:");
      Serial.print("\t");
      Serial.println(simonInputArray[i]);
      Serial.print("Index of array: ");
      Serial.println(inputCounter++);
      Serial.print("Input character: ");
      Serial.println(i);
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






