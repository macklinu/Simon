Things that work:
	Reading button input
	Corresponding LEDs light up when button pressed
	Tones are played for each button
	The Simon sequence is generated in setup(), but it is a char array instead of an int array

Things that need some work:
	Checking the input against the Simon sequence, which is determined in Setup()
		Generating a new game sequence at the very beginning of loop() so you theoretically never have to restart the game
		Initializing other variables in this step would be a good idea as well
	Change arrays from char to int
	Overall structure of functions
	LCD display has not been incorporated yet
	Increasing game difficulty as the sequence progresses
