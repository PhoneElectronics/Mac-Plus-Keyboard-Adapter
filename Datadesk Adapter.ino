#include <Keyboard.h>

const int clockPin = 3; // Clock pin
const int dataPin = 2; //Data Pin
volatile byte clockCycleCounter = 0; // Counter for clock cycles detected on the clock pin
volatile byte receivedByte = 0;
volatile byte sendByte = 0x36;
bool keyboardConnected = false;  //Variable to track whether the arduino is communicating with the keyboard, this variable will be set to false before initialization or when the keybaord times out.
volatile long interruptTime = 0;
volatile long timeSinceLastInterrupt = 0;
volatile bool newByteAvailable = false;
volatile bool extendedKeyCode = false;

const int debounceDelay = 50;  // Debounce delay in milliseconds
const int keypadForwardSlashPin = 18;
const int escPin = 16;
const int keypadPlusPin = 14;
const int keypadAsteriskPin = 15;
bool keypadForwardSlashState = HIGH;
bool keypadForwardSlashLastState = HIGH;
unsigned long keypadForwardSlashLastDebounceTime = 0;
bool escState = HIGH;
bool escLastState = HIGH;
unsigned long escLastDebounceTime = 0;
bool keypadAsteriskState = HIGH;
bool keypadAsteriskLastState = HIGH;
unsigned long keypadAsteriskLastDebounceTime = 0;
bool keypadPlusState = HIGH;
bool keypadPlusLastState = HIGH;
unsigned long keypadPlusLastDebounceTime = 0;



void checkKeypadForwardSlashState() {
  int reading = digitalRead(keypadForwardSlashPin); // Read the button state

  // Check if the button state has changed
  if (reading != keypadForwardSlashLastState) {
    keypadForwardSlashLastDebounceTime = millis(); // Update the debounce time
  }

  // Check if the button state has been stable for the debounce delay
  if ((millis() - keypadForwardSlashLastDebounceTime) > debounceDelay) {
    // Check if the button state is different from the stored state
    if (reading != keypadForwardSlashState) {
      keypadForwardSlashState = reading; // Update the button state

      // Check if the button is pressed or released
      if (keypadForwardSlashState == LOW) {
        Keyboard.press(KEY_KP_SLASH);
      } else {
        Keyboard.release(KEY_KP_SLASH);
      }
    }
  }

  keypadForwardSlashLastState = reading; // Store the current button state for comparison in the next iteration
}

void checkEscState() {
  int reading = digitalRead(escPin); // Read the button state

  // Check if the button state has changed
  if (reading != escLastState) {
    escLastDebounceTime = millis(); // Update the debounce time
  }

  // Check if the button state has been stable for the debounce delay
  if ((millis() - escLastDebounceTime) > debounceDelay) {
    // Check if the button state is different from the stored state
    if (reading != escState) {
      escState = reading; // Update the button state

      // Check if the button is pressed or released
      if (escState == LOW) {
        Keyboard.press(KEY_ESC);
      } else {
        Keyboard.release(KEY_ESC);
      }
    }
  }

  escLastState = reading; // Store the current button state for comparison in the next iteration
}

void checkKeypadPlusState() {
  int reading = digitalRead(keypadPlusPin); // Read the button state

  // Check if the button state has changed
  if (reading != keypadPlusLastState) {
    keypadPlusLastDebounceTime = millis(); // Update the debounce time
  }

  // Check if the button state has been stable for the debounce delay
  if ((millis() - keypadPlusLastDebounceTime) > debounceDelay) {
    // Check if the button state is different from the stored state
    if (reading != keypadPlusState) {
      keypadPlusState = reading; // Update the button state

      // Check if the button is pressed or released
      if (keypadPlusState == LOW) {
        Keyboard.press(KEY_KP_PLUS);
      } else {
        Keyboard.release(KEY_KP_PLUS);
      }
    }
  }

  keypadPlusLastState = reading; // Store the current button state for comparison in the next iteration
}

void checkKeypadAsteriskState() {
  int reading = digitalRead(keypadAsteriskPin); // Read the button state

  // Check if the button state has changed
  if (reading != keypadAsteriskLastState) {
    keypadAsteriskLastDebounceTime = millis(); // Update the debounce time
  }

  // Check if the button state has been stable for the debounce delay
  if ((millis() - keypadAsteriskLastDebounceTime) > debounceDelay) {
    // Check if the button state is different from the stored state
    if (reading != keypadAsteriskState) {
      keypadAsteriskState = reading; // Update the button state

      // Check if the button is pressed or released
      if (keypadAsteriskState == LOW) {
        Keyboard.press(KEY_KP_ASTERISK);
      } else {
        Keyboard.release(KEY_KP_ASTERISK);
      }
    }
  }

  keypadAsteriskLastState = reading; // Store the current button state for comparison in the next iteration
}

//LOOKUP TABLE FOR KEY CODES CONVERTED BY parseKeyboardByte()
struct LookupEntry {
  byte value;
  char character;
};
const LookupEntry keycodeTable[] = {
                {0xA3, KEY_F1}, {0xB5, KEY_F2}, {0xBB, KEY_F3}, {0xBD, KEY_F4},      {0xBF, KEY_F5}, {0xC1, KEY_F6}, {0xC7, KEY_F7}, {0xC9, KEY_F8},       {0xCB, KEY_F9},  {0xD3, KEY_F10}, {0xD5, KEY_F11}, {0xD7, KEY_F12},       {0xDF, KEY_LEFT_GUI}, {0xE1, KEY_F14}, {0xE3, KEY_F15},
  
  {0x65, '`'},      {0x25, '1'}, {0x27, '2'},{0x29, '3'}, {0x2B, '4'}, {0x2F, '5'},   {0x2D, '6'}, {0x35, '7'}, {0x39, '8'}, {0x33, '9'}, {0x3B, '0'}, {0x37, '-'}, {0x31, '='}, {0x67, KEY_BACKSPACE}, {0x8B, KEY_INSERT}, {0x95, KEY_HOME}, {0x9F, KEY_PAGE_UP}, {0x8F, KEY_NUM_LOCK}, {0x9D, KEY_KP_MINUS}, 

  {0x61, KEY_TAB},    {0x19, 'q'}, {0x1B, 'w'}, {0x1D, 'e'}, {0x1F, 'r'}, {0x23, 't'}, {0x21, 'y'}, {0x41, 'u'}, {0x45, 'i'}, {0x3F, 'o'}, {0x47, 'p'}, {0x43, '['}, {0x3D, ']'}, {0x55, '\\'}, {0x93, KEY_DELETE}, {0x97, KEY_END}, {0xA1, KEY_PAGE_DOWN},          {0xB3, KEY_KP_7}, {0xB7, KEY_KP_8}, {0xB9, KEY_KP_9}, 

  {0x73, KEY_CAPS_LOCK}, {0x01, 'a'}, {0x03, 's'}, {0x05, 'd'}, {0x07, 'f'}, {0x0B, 'g'}, {0x09, 'h'}, {0x4D, 'j'}, {0x51, 'k'}, {0x4B, 'l'}, {0x53, ';'}, {0x4F, '\''}, {0x49, KEY_RETURN},                                                                         {0xAD, KEY_KP_4}, {0xAF, KEY_KP_5},  {0xB1, KEY_KP_6}, 

  {0x71, KEY_LEFT_SHIFT}, {0x0D, 'z'}, {0x0F, 'x'}, {0x11, 'c'}, {0x13, 'v'}, {0x17, 'b'}, {0x5B, 'n'}, {0x5D, 'm'}, {0x57, ','}, {0x5F, '.'}, {0x59, '/'},                                  {0x9B, KEY_UP_ARROW},                                                  {0xA7, KEY_KP_1}, {0xA9, KEY_KP_2}, {0xAB, KEY_KP_3}, {0x99, KEY_KP_ENTER},

  {0x75, KEY_LEFT_CTRL}, {0x6F, KEY_LEFT_ALT},                                        {0x63, ' '},                                                                  {0x8D, KEY_LEFT_ARROW}, {0x91, KEY_DOWN_ARROW}, {0x85, KEY_RIGHT_ARROW},                        {0xA5, KEY_KP_0},                   {0x83, KEY_KP_DOT},
};

char findKey(byte value) {
  int numEntries = sizeof(keycodeTable) / sizeof(LookupEntry);
  for (int i = 0; i < numEntries; i++) {
    if (keycodeTable[i].value == value) {
      return keycodeTable[i].character;
    }
  }
}


//THE INTERRUPT SERVICE ROUTINE FOR WHEN THE KEYBOARD CLOCK GOES LOW
void clockInterrupt() {
    interruptTime = micros(); //Capturue the time of this interrupt
    if(clockCycleCounter == 0 && !keyboardConnected){keyboardConnected = true;}
    // falling edges 0-7 are for sending data
    if (clockCycleCounter >= 0 && clockCycleCounter <= 7) {
      digitalWrite(dataPin, (sendByte >> 7 - clockCycleCounter) & 1);
    }

    //falling edges 8-15 are for receiving data
    if (clockCycleCounter >= 8 && clockCycleCounter <= 15) {
      receivedByte = receivedByte & ~(1 << 15 - clockCycleCounter);       //Clear the required bit from the receivedByte variable
      receivedByte |= (digitalRead(dataPin) << 15 - clockCycleCounter);  // Read the bit from the data pin
    }
    if(clockCycleCounter == 15) {
      newByteAvailable = true;
    }
    if(clockCycleCounter != 15){
      clockCycleCounter = (clockCycleCounter + 1);
    }  
}

void setup() {
  Keyboard.begin();
  //Serial.begin(9600);
  pinMode(clockPin, INPUT); // Set the clock pin as input
  pinMode(dataPin, OUTPUT);
  pinMode(keypadForwardSlashPin, INPUT_PULLUP);
  pinMode(keypadAsteriskPin, INPUT_PULLUP);
  pinMode(keypadPlusPin, INPUT_PULLUP);
  pinMode(escPin, INPUT_PULLUP);
  digitalWrite(dataPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(clockPin), clockInterrupt, FALLING); // Attach interrupt to the clock pin
 
}

void loop() {
  keyboardUpdate();
  checkKeypadForwardSlashState();
  checkKeypadAsteriskState();
  checkKeypadPlusState();
  checkEscState();
}

void parseKeyboardByte(byte data) { //Takes in a keycode byte, and presses or releases the key
  byte toLookupTable = data;
  toLookupTable = (toLookupTable & 0x7F) | (extendedKeyCode << 7); //Set the MSB of the transition code to extendedKeyCode
 // Serial.println(data, HEX);
 // Serial.println(toLookupTable, HEX);

  if (data == 0x79) {
    extendedKeyCode = true;
  } else if(data == 0x73 || data == 0xF3){
    Keyboard.press(KEY_CAPS_LOCK);
    Keyboard.release(KEY_CAPS_LOCK);

  } else if (data != 0x7D) {
    if ((data & B10000000) != 0) { //MSB being 1 means Key up
     // if(extendedKeyCode){Serial.print("Extended ");}
      //Serial.print(findKey(toLookupTable), HEX);
      //Serial.println(": Key Up");
      Keyboard.release(findKey(toLookupTable));
      extendedKeyCode = false;
    } else {
     // if(extendedKeyCode){Serial.print("Extended ");}
     // Serial.print(findKey(toLookupTable), HEX);
     // Serial.println(": Key Down");
      Keyboard.press(findKey(toLookupTable));
      extendedKeyCode = false;
    }
  }
}

void keyboardUpdate(){

  timeSinceLastInterrupt = micros() - interruptTime;  //Calculates the time since the last interrupt
  if(newByteAvailable){   //Check if a new byte is available (Set in clock interrupt)
    newByteAvailable = false;
    if(receivedByte != 0x7B){
      parseKeyboardByte(receivedByte);
    }
    receivedByte = 0x00;
  }




  if(timeSinceLastInterrupt >= 500000){  //Check if the keyboard clock has been inactive for a half second, and resets the board
    sendByte = 0x36;
    keyboardConnected = false;
    Keyboard.releaseAll();
    //Serial.println("Reconnecting");
    pinMode(dataPin, OUTPUT);
  }

  //Attempt to reconnect the keyboard if disconnected
  if(keyboardConnected == false){
    interruptTime = 0;
    timeSinceLastInterrupt = 0;
    clockCycleCounter = 0;
    noInterrupts();
    digitalWrite(dataPin, HIGH);
    delay(500);
    receivedByte = 0;
    interrupts();
    digitalWrite(dataPin, LOW);
  }


  if(clockCycleCounter == 8 && timeSinceLastInterrupt >= 1000){ //Switch to receiving a byte without using delay statements
    if(sendByte != 0x10){sendByte = 0x10;}  //Resets the byte to send if the keyboard has been reset or communication is interrupted
    digitalWrite(dataPin, HIGH);
    pinMode(dataPin, INPUT_PULLUP);
  }

  if(clockCycleCounter == 15 && timeSinceLastInterrupt >= 1000) { //this will run after a byte has been received
    clockCycleCounter = 0;
    pinMode(dataPin, OUTPUT);
    digitalWrite(dataPin, LOW); //pull the data pin low to indicate to the keyboard that we are ready to send
  }

}