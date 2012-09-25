// currently implemented functions:

////////////////
// void lcdSetCursor(int row, int col) 
//     move cursor to position specified by row/col
//     row from 0-1, col from 0-15
////////////////

////////////////
// void lcdClear() 
//     clear display, return cursor to 0,0
////////////////

////////////////
// void lcdPrint(arg) 
//     arg may be int, float, char or String
//     display text at current cursor position
////////////////

#define MAXLINES 1
#define MAXCOLUMNS 15
#define LCDDELAY_LONG 2000
#define LCDDELAY_SHORT 200

void lcdSetBaudRate(int baud) {
  byte arg;      
  switch (baud) {
  case 300 : 
    arg = 1;
    break;
  case 1200 : 
    arg = 2;
    break;
  case 2400 : 
    arg = 3;
    break;
  case 9600 : 
    arg = 4;
    break;
  case 14400 : 
    arg = 5;
    break;
  case 19200 : 
    arg = 6;
    break;
  case 57600 : 
    arg = 7;
    break;
  case 115200 : 
    arg = 8;
    break;
  }
  mySerial.write(0xFE);
  mySerial.write(0x61);
  mySerial.write(arg);
  delay(15);
}

void lcdSetCursor(int row, int col) {
  int arg;
  if (((row >= 0) && (row <= MAXLINES)) && ((col >= 0) && (col <= MAXCOLUMNS))) {
    if (row == 0) 
      arg = 0x00 + col;
    else 
      arg = 0x40 + col;
    mySerial.write(0xFE);
    mySerial.write(0x45);
    mySerial.write(arg);
    delayMicroseconds(LCDDELAY_LONG);
  } 
}


void lcdClear() {
  mySerial.write(0xFE);
  mySerial.write(0x51);
  delayMicroseconds(LCDDELAY_LONG);
}  

void lcdPrint(int val) {
  mySerial.print(val);
  delayMicroseconds(LCDDELAY_SHORT);
}

void lcdPrint(String val) {
  mySerial.print(val);
  delayMicroseconds(LCDDELAY_SHORT);
}

void lcdPrint(float val) {
  mySerial.print(val);
  delayMicroseconds(LCDDELAY_SHORT);
}    

void lcdPrint(char val) {
  mySerial.print(val);
  delayMicroseconds(LCDDELAY_SHORT);
}




