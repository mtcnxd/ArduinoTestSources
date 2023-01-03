#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>	// Library for Mifare RC522 Devices
#include <SD.h>         // We are going to read and write PICC's UIDs from/to SD

#define COMMON_ANODE

#ifdef COMMON_ANODE
#define LED_ON LOW
#define LED_OFF HIGH
#else
#define LED_ON HIGH
#define LED_OFF LOW
#endif

#define redLed 2		// Set Led Pins
#define greenLed 6
#define blueLed 5

#define relay 3			// Set Relay Pin

#define sdPin 4			// Set SD Pin

boolean match = false;          // initialize card match to false
boolean programMode = false;	// initialize programming mode to false

int successRead;		// Variable integer to keep if we have Successful Read from Reader

byte readCard[4];		// Stores scanned ID read from RFID Module
byte masterCard[4];		// Stores master card's ID

char filename[] = "XXXXXXXXXXXXXXX.DAT";  // Stores variable filename
char extension[] = "DAT";          // sometimes the extension gets modified
char dir[] = "/PICCS/";


/*
	We need to define MFRC522's pins and create instance
	Pin layout should be as follows (on Arduino Uno):
	MOSI: Pin 11 / ICSP-4
	MISO: Pin 12 / ICSP-1
	SCK : Pin 13 / ICSP-3
	SDA : (Configurable)
	RST : Not Needed

 */

#define SS_PIN 9
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

void setup() {
  //Arduino Pin Configuration
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(relay, OUTPUT);        // Be careful how relay circuit behave on while resetting or power-cycling your Arduino
  digitalWrite(relay, HIGH);		// Make sure door is locked
  digitalWrite(redLed, LED_OFF);	// Make sure led is off
  digitalWrite(greenLed, LED_OFF);	// Make sure led is off
  digitalWrite(blueLed, LED_OFF);	// Make sure led is off

  //Initialize
  Serial.begin(9600);	 // Initialize serial communications with PC
  Serial.println(F("Access Control v4.3"));   // For debugging purposes
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  if (!SD.begin(sdPin)) {	                     // Initialize SD Hardware
    Serial.println(F("SD initialization failed!"));  // Could not initialize
    redSolid();
    while (true);                                    // Do not go further
  }
  Serial.println(F("SD initialization done."));      // Yay all SPI slaves work
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  ShowReaderDetails();	 // Show details of PCD - MFRC522 Card Reader details

  checkMaster();      // Check if masterCard defined
  Serial.println(F("Everything Ready"));
  Serial.println(F("Waiting PICCs to be scanned"));
  cycleLeds();        // Lets give user some feedback that hardware initialized by cycling leds
}

void loop () {
  do {
    successRead = getID(); 	// sets successRead to 1 when we get read from reader otherwise 0
    if (programMode) {
      cycleLeds();              // Program Mode cycles through RGB waiting to read a new card
    }
    else {
      blueSolid(); 		// Normal mode, blue Power LED is on, all others are off
    }
  }
  while (!successRead); 	// the program will not go further while you not get a successful read
  if (programMode) {
    if (isMaster(readCard)) {   // If master card scanned again exit program mode
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      return;
    }
    else {
      if ( findID() ) { // If scanned card is known delete it
        Serial.println(F("I know this PICC, removing..."));
        removeID();
        Serial.println(F("-----------------------------"));
      }
      else {            // If scanned card is not known add it
        Serial.println(F("I do not know this PICC, adding..."));
        writeID();
        Serial.println(F("-----------------------------"));
      }
    }
  }
  else {
    if (isMaster(readCard)) {  	// If scanned card's ID matches Master Card's ID enter program mode
      programMode = true;
      Serial.println(F("Hello Master - Entered Program Mode"));
      Serial.println(F("Scan a PICC to ADD or REMOVE"));
      Serial.println(F("-----------------------------"));
    }
    else {
      if (findID()) {	// If not, check if we can find it
        Serial.println(F("Welcome, You shall pass"));
        granted(300);        	// Open the door lock for 300 ms
      }
      else {			// If not, show that the ID was not valid
        Serial.println(F("You shall not pass"));
        denied();
      }
    }
  }
}

void getFilename() {  // We will store UIDs as files on SD card
  sprintf(filename, "%s%02x%02x%02x%02x.%s", dir, readCard[0], readCard[1], // Convert readCard data to char and append extension
          readCard[2], readCard[3], extension);
}

boolean findID () {  // Check If we can find UID's specific file
  File fileopen = SD.open(filename);
  if (fileopen) {
    fileopen.close();  // Found it close
    return true;
  }
  else {  	// If not, return false
  }
  return false;
}

void writeID () {
  File filewrite = SD.open(filename, FILE_WRITE);
  filewrite.close();
  if (SD.exists(filename)) {
    Serial.println(F("Succesfully added ID record"));
    greenBlink();
  }
  else {
    Serial.println(F("Failed to add record"));
    redBlink();
  }
}

void removeID () {
  SD.remove(filename);
  if (SD.exists(filename)) {
    Serial.println(F("Failed to remove. Record still exists"));
    redBlink();
  }
  else {
    Serial.println(F("Succesfully removed ID record"));
    blueBlink();
  }
}

/////////////////////////////////////////  Access Granted    ///////////////////////////////////
void granted (int setDelay) {
  digitalWrite(relay, LOW); 		// Unlock door!
  delay(setDelay); 			// Hold door lock open for given seconds
  digitalWrite(relay, HIGH); 		// Relock door
  greenSolid();
}

///////////////////////////////////////// Access Denied  ///////////////////////////////////
void denied() {
  redSolid();
}


///////////////////////////////////////// Get PICC's UID ///////////////////////////////////
int getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println(F("Scanned PICC's UID:"));
  for (int i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  getFilename();    // Get data ready
  return 1;
}

void checkMaster() {
  if (SD.exists("/SYS/master.dat")) {              // Check if we have master.dat on SD card
    Serial.print(F("Master Card's UID: "));      // Since we have it print to serial
    File masterfile = SD.open("/SYS/master.dat");  // Open file
    for (int i = 0; i < 4; i++) {             // Loop 4 times to get 4 bytes
      readCard[i] = masterfile.read();
      Serial.print(readCard[i], HEX);         // Actual serial printing of each byte
      masterCard[i] = readCard[i];            // Prepare bytes for future comparing
    }
    Serial.println("");
    masterfile.close();                       // Close file
  }
  else {
    Serial.println(F("No Master Card Defined"));
    Serial.println(F("Scan A PICC to Define as Master Card"));
    do {
      successRead = getID(); // sets successRead to 1 when we get read from reader otherwise 0
      blueBlink(); // Visualize Master Card need to be defined
    }
    while (!successRead); //the program will not go further while you not get a successful read
    File masterfile = SD.open("/SYS/master.dat", FILE_WRITE);
    if (masterfile) {
      Serial.println(F("Writing to master.dat..."));
      masterfile.write(readCard, 4);
      // close the file:
      masterfile.close();
      Serial.println(F("Master Card successfuly defined"));
    } else {
      // if the file didn't open, print an error:
      Serial.println(F("error creating master.dat"));
      redBlink();
    }
  }
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    redSolid();
    while (true); // do not go further
  }
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != NULL ) 			// Make sure there is something in the array first
    match = true; 			// Assume they match at first
  for ( int k = 0; k < 4; k++ ) { 	// Loop 4 times
    if ( a[k] != b[k] ) 		// IF a != b then set match = false, one fails, all fail
      match = false;
  }
  if ( match ) { 			// Check to see if if match is still true
    return true; 			// Return true
  }
  else  {
    return false; 			// Return false
  }
}

////////////////////// Check readCard IF is masterCard   ///////////////////////////////////
// Check to see if the ID passed is the master programing card
boolean isMaster( byte test[] ) {
  if ( checkTwo( test, masterCard ) )
    return true;
  else
    return false;
}

