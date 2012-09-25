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
#include <SoftwareSerial.h>         // include Software Serial library

const int totalGameLength = 20;     // number of steps
const int numButtons = 4;           // number of buttons (and also LEDs)

SoftwareSerial mySerial(7,8);       // we will use "software serial" Rx pin 7, Tx pin 8
const int speakerPin = 9;           // initialize speaker pin

const int buttonPin[] = {           // initialize button pins
2, 3, 4, 5};
const int ledPin[] = {              // initialize LED pins
10, 11, 12, 13};
int buttonState[numButtons];
int buttonPrevious[numButtons];     // for debouncing purposes
int buttonPushCounter[numButtons];

long time[numButtons];              // the last time the output pin was toggled
long debounce = 5;                  // the debounce time; using capacitors as well for hardware debouncing

int ledState[numButtons];

int simonInputArray[totalGameLength];      // array to determine
int simonMasterArray[totalGameLength];     // array that input will be checked against
int inputCounter;                          // checks to see if input matches master sequence and increments by one each time a level is passed
int currentGameLength;                     // the current level

boolean is_button_pressed[numButtons];
boolean perform_button_action[numButtons];

int buttonTone[numButtons] = {
NOTE_C3, NOTE_E3, NOTE_G3, NOTE_C4};

boolean game_init = true;           // options and reinitializations that happen before the game begins
boolean game_start = false;         //
boolean play_intro_melody = true;
boolean sequence_playback = false;  // what plays back master Simon sequence; false until game is started

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };

void setup() {
  // open serial communication (both for serial monitor and LCD)
  Serial.begin(9600);
  mySerial.begin(9600);
  // initialize button pins, led pins, button states, and other variable arrays
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(buttonPin[i], HIGH);
    buttonPrevious[i] = HIGH;
    ledState[i] = LOW;
    time[i] = 0;
    perform_button_action[i] = false;
    is_button_pressed[i] = false;
  }
  inputCounter = 0;
  currentGameLength = 3;
  // the sum of all analog inputs creates a truly random seed for the Simon sequence
  randomSeed(analogRead(A0+A1+A2+A3+A4+A5));
  // assign a random value to each number in the Simon sequence
  for (int i = 0; i < totalGameLength; i++) {
    simonMasterArray[i] = random(numButtons);
    Serial.print("Index:");
    Serial.print("\t");
    Serial.print(i);
    Serial.print("\t");
    Serial.println(simonMasterArray[i]);
  }
}

void loop() {
  if (sequence_playback) {

  }
  // loop through all buttons and LEDs at once
  for (int i = 0; i < numButtons; i++) {
    // a button's state is constantly read into an array
    buttonState[i] = digitalRead(buttonPin[i]);
    // if the button is truly pressed (debouncing)
    if (buttonState[i] == HIGH && buttonPrevious[i] == LOW && millis() - time[i] > debounce) {
      // set this check variable to true in order to only perform once action upon a button press
      is_button_pressed[i] = true;
      if (is_button_pressed[i]) {
        // each time a button is pressed, its value is added to the input array
        simonInputArray[inputCounter] = i;
        // this checks to see if the input value matches the master array
        // if they match
        if (simonInputArray[inputCounter] == simonMasterArray[inputCounter]) {
          // print match
          Serial.println("MATCH");
          Serial.print("MASTER:");
          Serial.print("\t");
          Serial.println(simonMasterArray[inputCounter]);
          Serial.print("INPUT:");
          Serial.print("\t");
          Serial.println(simonInputArray[inputCounter]);
          Serial.print("Index of array: ");
          Serial.println(inputCounter++);
          Serial.print("Input character: ");
          Serial.println(i);
          Serial.println();
          digitalWrite(ledPin[i], HIGH);
          tone(speakerPin, buttonTone[i]);
          delay(200);
          digitalWrite(ledPin[i], LOW);
          noTone(speakerPin);
          // need if statement to check inputCounter to level variable
          // if they equal each other, set inputCounter back to 0 and turn level variable up one
        }
        else {
          // print incorrect
          Serial.println("INCORRECT! GAME OVER!");
          // loop that plays back a low tone and flashes all LEDs to signify an incorrect input
          for (int j = 0; j < numButtons; j++) {
            digitalWrite(ledPin[j], HIGH);
            tone(speakerPin, NOTE_FS1);
            delay(100);
            digitalWrite(ledPin[j], LOW);
            noTone(speakerPin);
          }
          // game is over, so go back to the start and reinitialize variables
        }
        // exit the if statement
        !is_button_pressed[i];
      }
      time[i] = millis();
    }
    // update the previous button state for button debouncing purposes
    buttonPrevious[i] = buttonState[i];
  }
}

void introMelody() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(speakerPin, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speakerPin);
  }
}