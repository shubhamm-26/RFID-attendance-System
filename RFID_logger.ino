#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;
String StudentID;
String Name;
String Phone;
String Room;

// Buzzer and LED setup
int buzzerPin = 7;  // Connect the buzzer to pin 7
int ledPin = 6;    // Connect the LED to pin 13

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  delay(1000);
}

void loop() {
  while (true) {
    
    mfrc522.PCD_Init(); // Re-initialize the RFID reader
    
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // Function to activate Buzzer and LED
      activateBuzzerAndLED();

      for (int i = 0; i < 4; i++) {
        readcard[i] = mfrc522.uid.uidByte[i]; // Storing the UID of the tag in readcard
        array_to_string(readcard, 4, str);
        StrUID = str;
      }

      // Read data from specific blocks (4, 5, 6, 8)
      StudentID = readBlock(4);
      Name = readBlock(5);
      Phone = readBlock(6);
      Room = readBlock(8);

      // Print RFID data to serial monitor
      Serial.println(StrUID + "," + StudentID + "," + Name + "," + Phone + "," + Room);

      // Halt the card and add a delay after successfully reading a card
      mfrc522.PICC_HaltA();
      delay(3000);
    }
  }
}

// Function to activate Buzzer and LED
void activateBuzzerAndLED() {
  tone(buzzerPin, 3200);  // Turn on the buzzer
  digitalWrite(ledPin, HIGH);     // Turn on the LED
  delay(400);  // Keep them on for 1 second
  noTone(buzzerPin);   // Turn off the buzzer
  digitalWrite(ledPin, LOW);      // Turn off the LED
}

// Function to get RFID card ID and read associated data
int getid() {  
  if(!mfrc522.PICC_IsNewCardPresent()){
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()){
    return 0;
  }

  for(int i=0; i<4; i++){
    readcard[i] = mfrc522.uid.uidByte[i]; // Storing the UID of the tag in readcard
    array_to_string(readcard, 4, str);
    StrUID = str;
  }

  // Read data from specific blocks (4, 5, 6, 8)
  StudentID = readBlock(4);
  Name = readBlock(5);
  Phone = readBlock(6);
  Room = readBlock(8);

  mfrc522.PICC_HaltA();
  return 1;  
}

// Function to read data from a specific block
String readBlock(int blockNumber) {
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNumber, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return "";
  }

  status = mfrc522.MIFARE_Read(blockNumber, buffer, &size);
  if (status == MFRC522::STATUS_OK) {
    String data = "";
    for (byte i = 0; i < 16; i++) {
      char c = (char)buffer[i];
      if (c != '\0') {
        data += c;
      }
    }
    return data;
  } else {
    return "";
  }
}

// Function to convert byte array to string
void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len*2] = '\0';
}
