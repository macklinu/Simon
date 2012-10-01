/*
Simon: the game

created 2 October 2012
by Macklin Underdown
PAT 451
*/

// headers
#include "pitches.h"                      	// add pitches header file to reference note frequencies
#include <SoftwareSerial.h>               	// include Software Serial library

// pins
SoftwareSerial mySerial(7,8);             	// LCD "software serial" Rx pin 7, Tx pin 8
const int speakerPin = 9;                 	// initialize speaker pin
const int buttonPin[] = {                 	// initialize button pins
2, 3, 4, 5};
const int ledPin[] = {                    	// initialize LED pins
10, 11, 12, 13};

// constants
const int totalGameLength = 20;           	// number of steps (number of levels in game = this - 3)
const int numButtons = 4;                 	// number of buttons (and also LEDs and tones)

// game states
boolean game_start;
boolean gametime;
boolean one_time_setup;
boolean write_intro_text;
boolean game_over;
boolean game_over_text;
int gameSpeed;								// the speed of Simon playback will increase as the game goes on to increase difficulty

// buttons
int buttonState[numButtons];				// the current state of a button
int buttonPrevious[numButtons];           	// for debouncing purposes
long buttonDebounceTime[numButtons];      	// the last time the button was toggled
long debounceTime = 5;                    	// the debounce time
int maxButtonReadLength;					// currently not implemented
boolean is_button_pressed[numButtons];
boolean perform_button_action[numButtons];	
boolean button_actually_pressed[numButtons];

// LEDs
// maybe get rid of this?
int ledState[numButtons];					// the current state of the LED

// Simon + input
int simonInputArray[totalGameLength];     	// array to determine
int simonMasterArray[totalGameLength];    	// array that input will be checked against
int inputCounter;                         	// checks to see if input matches master sequence and increments by one each time a level is passed    	
int levelLength;							// the current level
boolean playback_sequence;
boolean generate_random_sequence;

// tones
int buttonTone[numButtons] = {				// the 4 tones that Simon playback and button presses will invoke
NOTE_C3, NOTE_E3, NOTE_G3, NOTE_C4};
int melody[] = {							// game winning melody (currently)
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
int noteDurations[] = {						// note durations in correspondence with this melody
  4, 8, 8, 4,4,4,4,4 };

void setup() {
	// open Serial and LCD communication
	Serial.begin(9600);
  	mySerial.begin(57600);
  	// clear LCD screen (just in case)
 	lcdClear();
 	// set your pin states
  	for (int i = 0; i < numButtons; i++) {
	  	pinMode(buttonPin[i], INPUT_PULLUP);
	  	pinMode(ledPin[i], OUTPUT);
	  	digitalWrite(buttonPin[i], HIGH);	
  	}
  	// set boolean flags to move into the game setup and game intro (not gameplay itself)
  	game_start = false;
  	one_time_setup = true;
}

void loop() {
	// before the game actually begins...
	if (game_start == false) {
		if (one_time_setup) {
			// run this game setup once
			gameSetup();
			one_time_setup = false;
		}
		// run the game intro until the user has pressed start to move into gameplay
		gameIntro();
	}
	// now that the game has started...
	if (game_start) {
		// play back the Simon sequence;
		// no user input is being listened to during this time
		if (playback_sequence) {
			// the game will play back faster as the game continues
			playbackSimonSequence(gameSpeed);
			// exit this statement
			playback_sequence = false;
			// move to the user input
			gametime = true;
		}
		// where gameplay actually takes place
		if (gametime) {
			for (int i = 0; i < numButtons; i++) {
				// read button state
				readButton(i);
				// check if button was actually pressed and check input: actions are decided from there
				checkButtonAndDecide(i);
			}
		}
	}
}

// initialize game variables
void gameSetup() {
	// Notify user that the next game is being set up on the LCD screen
	lcdClear();
	lcdPrint("Setting up game");
	lcdSetCursor(1,0);
	lcdPrint("...");
	delay(2000);
	lcdClear();
	// generate the Simon sequence
	generate_random_sequence = true;
	for (int i = 0; i < totalGameLength; i++) {
		generateSimonSequence(i);
	}
	for (int i = 0; i < numButtons; i++) {
		buttonPrevious[i] = HIGH;
	    ledState[i] = LOW;
	    buttonDebounceTime[i] = 0;
		button_actually_pressed[i] = false;
		// do something
	}
	inputCounter = 0;
	levelLength = 3;
	maxButtonReadLength = 3000;
	write_intro_text = true;
	gameSpeed = 350;
}

void generateSimonSequence(int i) {
	// generate a random seed once
	while (generate_random_sequence) {
		randomSeed(analogRead(A0+A1+A2+A3+A4+A5));
		generate_random_sequence = false;
	}
	// any number 0-3 will be an index in the Simon sequence
	simonMasterArray[i] = random(numButtons);
	// print each one to the Serial monitor
	Serial.print("Simon\t");
	Serial.print(i);
	Serial.print("\t");
	Serial.println(simonMasterArray[i]);
}

void gameIntro() {
	// write text to LCD to instruct user how to begin the game
	if (write_intro_text) {
		// print "Press start to begin"
		lcdPrint("Press outside");
		lcdSetCursor(1,0);
		lcdPrint("buttons to begin");
		write_intro_text = false;
	}
	// digitalWrite(ledPin[0], HIGH);
	// digitalWrite(ledPin[3], HIGH);
	for (int i = 0; i < numButtons; i++) {
		// wait for button press input
		readButton(i);
		// if the user pressed the outside buttons, the game will begin
		if (buttonState[i] == LOW && buttonState[3] == LOW) {
			// clear the LCD and move into gameplay
			lcdClear();
			// digitalWrite(ledPin[0], LOW);
			// digitalWrite(ledPin[3], LOW);
			playback_sequence = true;
			game_start = true;
		}
	}
}

void playbackSimonSequence(int lengthOfLED) {
	// if the game isn't over, write what level you are on
	if (!game_over) {
		lcdClear();
		lcdPrint("Level");
		lcdSetCursor(1,0);
		// the level is offset by two to make more sense (starts from level 1)
		lcdPrint(levelLength - 2);
	}
	// depending on what level you are on, play back the Simon sequence
	for (int i = 0; i < levelLength; i++) {
		// light LED that corresponds to each 
		digitalWrite(ledPin[simonMasterArray[i]], HIGH);
		// play tone that corresponds to each LED
		tone(speakerPin, buttonTone[simonMasterArray[i]]);
		// the delay between on and off will be shorter and the game progresses
		delay(lengthOfLED);
		digitalWrite(ledPin[simonMasterArray[i]], LOW);
		noTone(speakerPin);
		delay(lengthOfLED);
	}
}

int readButton(int i) {
	// read the state of a button and return its state as an integer (0 or 1)
	buttonState[i] = digitalRead(buttonPin[i]);
	return buttonState[i];
}

// some debouncing code ideas in this function were taken from http://arduino.cc/en/Tutorial/Debounce
void checkButtonAndDecide(int i) {
	// debounce the button; if the state has changed and enough time has gone by to confirm that there was no bouncing...
	if (buttonState[i] == LOW && buttonPrevious[i] == HIGH && millis() - buttonDebounceTime[i] > debounceTime) {
		// means button was pressed
		button_actually_pressed[i] = true;
		// check the input once and perform an action based on that input
		if (button_actually_pressed[i]) {
			checkInput(i);
			button_actually_pressed[i] = false;
		}
		// set time of button to current time of program for debounce checking later
		buttonDebounceTime[i] = millis(); 
	}
	// update this to check again
	buttonPrevious[i] = buttonState[i];
}

void checkInput(int i) {
	// add the current button press to the input array
	simonInputArray[inputCounter] = i;
	// check the input to see if it is a match or not
	// if it is a match...
	if (simonInputArray[inputCounter] == simonMasterArray[inputCounter]) {
		Serial.println("MATCH");
		Serial.print("Index ");
		Serial.print(inputCounter);
		Serial.print("\t");
		Serial.println(i);
		// light up the correct LED and play a tone
		lightsAndBeeps(i, gameSpeed);
		// increase the input counter and keep checking
		inputCounter += 1;
		// if the input index is the same as the level length
		if (inputCounter == (levelLength)) {
			// the level has been successfully completed
			winLevel();
		}
	}
	// if the input does not match...
	else {
		// the game has been lost, the user will see the correct sequence, and the game will be able to be played again from the beginning
		gameOver();
	}
}

// this function will light an LED and play a tone depending on which button was pressed
// used to visually verify that user input is correct in each level of Simon
void lightsAndBeeps(int i, int lengthOfLED) {
	digitalWrite(ledPin[i], HIGH);
	tone(speakerPin, buttonTone[i]);
	delay(lengthOfLED);
	digitalWrite(ledPin[i], LOW);
	noTone(speakerPin);
	delay(lengthOfLED / 2);
}

void winLevel() {
	// print something that lets the user know they were successful
	lcdClear();
	lcdPrint("Nice!");
	lcdSetCursor(1,0);
	lcdPrint("...");
	// a little audiovisual cue that you have completed the level
	for (int i = 0; i < numButtons; i++) {
		digitalWrite(ledPin[i], HIGH);
		tone(speakerPin, buttonTone[i]);
		delay(50);
		digitalWrite(ledPin[i], LOW);
		noTone(speakerPin);
		delay(50);
	}
	// decrease speed between LED flashes and button reading to make game more difficult
	gameSpeed -= 10;
	// go to the next level; add one more instance to the Simon sequence
	levelLength++;
	// set input counter back to 0 to start the next level from the beginning of the array
	inputCounter = 0;
	// but before moving on, check to see if you've made it to the end of the game
	if (levelLength == totalGameLength) {
		// if so, you win!
		youWinTheGame();
	}
	// wait a second or so before moving on to the next level; makes the transition seem less abrupt
	delay(1000);
	gametime = false;
	playback_sequence = true;
}

void gameOver() {
	lcdClear();
	// this flag prevents the LCD from writing the next level on the screen
	game_over = true;
	game_over_text = true;
	if (game_over_text) {
		// print "You Lose!" or something along those lines once
		lcdPrint("Game over");
		lcdSetCursor(1,0);
		lcdPrint("The answer:");
		game_over_text = false;
	}
	// play a low buzzer tone and light all lights; an audiovisual cue that is easily distinguishable from winning a level
	for (int i = 0; i < numButtons; i++) {
		digitalWrite(ledPin[i], HIGH);
		tone(speakerPin, NOTE_FS1);
		delay(70);
		digitalWrite(ledPin[i], LOW);
		noTone(speakerPin);
		delay(70);
	}
	playback_sequence = true;
	// play game sequence back so the user can see where their mistake was
	if (playback_sequence) {
		playbackSimonSequence(gameSpeed);
		playback_sequence = false;
	}
	// reset some boolean flags so the game transitions back to the intro, where the user can begin the game again
	game_over = false;
	gametime = false;
	game_start = false;
	one_time_setup = true;
}

void youWinTheGame() {
	// print "You win the game" on the LCD once
	boolean game_won_text = true;
	if (game_won_text) {
		lcdClear();
		lcdPrint("YOU WIN");
		lcdSetCursor(1,0);
		lcdPrint("THE GAME");
		game_won_text = false;
	}
	// play a little tune!
	// currently the tune is taken from http://www.arduino.cc/en/Tutorial/Tone
	for (int thisNote = 0; thisNote < 8; thisNote++) {
	    int noteDuration = 1000/noteDurations[thisNote];
	    tone(speakerPin, melody[thisNote],noteDuration);
	    int pauseBetweenNotes = noteDuration * 1.30;
	    delay(pauseBetweenNotes);
	    noTone(speakerPin);
	}
	// move back to gameSetup() and continue on with starting the game over again
	game_start = false;
	one_time_setup = true;
}