#include <SPI.h>
#include <MFRC522.h>

/*Using Hardware SPI of Arduino */
/*MOSI (11), MISO (12) and SCK (13) are fixed */
/*You can configure SS and RST Pins*/
#define SS_PIN 10  /* Slave Select Pin */
#define RST_PIN 7  /* Reset Pin */

MFRC522 mfrc522(SS_PIN, RST_PIN); /* Create an instance of MFRC522 */
MFRC522::MIFARE_Key key; /* Create an instance of MIFARE_Key */          

/* Set the block to which we want to write data */
/* Be aware of Sector Trailer Blocks */
int blockNum_ID = 4; // 4th block is used to store the Student ID
int blockNum_Name = 5; // 5th block is used to store the Name of Student
int blockNum_Phone = 6; //6th block is used to store the phone number of the student
int blockNum_Room = 8; // 8th block is used to store the romm number of the student

// byte blockData_ID [16] = {"202151069"}; //Creating an 16 bytes of array to store Student ID 
// byte blockData_Name [16] = {"Atharva Kale"}; //Creating an 16 bytes of array to store Name of the student
// byte blockData_Phone [16] = {"9867915375"}; //Creating an 16 bytes of array to store Phone number of student
// byte blockData_Room [16] = {"E-704"}; //Creating an 16 bytes of array to store Room number of student
/* This is the actual data which is going to be written into the card */

//Inorder to reset the data stored inside the RFID Card, Comment the above four Lines and Uncomment the below code
byte blockData_ID [16] = {0};
byte blockData_Name [16] = {0};
byte blockData_Phone [16] = {0};
byte blockData_Room [16] = {0};


/* Create another arrays to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData_ID[18];
byte readBlockData_Name[18];
byte readBlockData_Phone[18];
byte readBlockData_Room[18];

MFRC522::StatusCode status;

void setup() 
{
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  /* Initialize SPI bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  Serial.println("Scan a MIFARE 1K Tag to write data...");
}

void loop()
{

  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.print("\n");
  Serial.println("**Card Detected**");
  /* Print UID of the Card */
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("\n");
  /* Print type of card (for example, MIFARE 1K) */
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
         
   /* Call 'WriteDataToBlock' function, which will write data to the block */
   Serial.print("\n");
   Serial.println("Writing to Data Block...");
   WriteDataToBlock(blockNum_ID, blockData_ID);
   WriteDataToBlock(blockNum_Name, blockData_Name);
   WriteDataToBlock(blockNum_Phone, blockData_Phone);
   WriteDataToBlock(blockNum_Room, blockData_Room);
   
   /* Read data from the same block */
   Serial.print("\n");
   Serial.println("Reading from Data Block...");
   ReadDataFromBlock(blockNum_ID, readBlockData_ID);
   ReadDataFromBlock(blockNum_Name, readBlockData_Name);
   ReadDataFromBlock(blockNum_Phone, readBlockData_Phone);
   ReadDataFromBlock(blockNum_Room, readBlockData_Room);
   /* If you want to print the full memory dump, uncomment the next line */
   //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
   
   /* Print the data read from block */
   Serial.print("\n");
   Serial.print("Data in Block:");
   Serial.print(blockNum_ID);
   Serial.print(" --> ");
   for (int j=0 ; j<16 ; j++)
   {
     Serial.write(readBlockData_ID[j]);
   }
   Serial.print("\n");

   Serial.print("\n");
   Serial.print("Data in Block:");
   Serial.print(blockNum_Name);
   Serial.print(" --> ");
   for (int j=0 ; j<16 ; j++)
   {
     Serial.write(readBlockData_Name[j]);
   }
   Serial.print("\n");

   Serial.print("\n");
   Serial.print("Data in Block:");
   Serial.print(blockNum_Phone);
   Serial.print(" --> ");
   for (int j=0 ; j<16 ; j++)
   {
     Serial.write(readBlockData_Phone[j]);
   }
   Serial.print("\n");

   Serial.print("\n");
   Serial.print("Data in Block:");
   Serial.print(blockNum_Room);
   Serial.print(" --> ");
   for (int j=0 ; j<16 ; j++)
   {
     Serial.write(readBlockData_Room[j]);
   }
   Serial.print("\n");
}



void WriteDataToBlock(int blockNum, byte blockData[]) 
{
  /* Authenticating the desired data block for write access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  
  /* Write data to the block */
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }
  
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{
  /* Authenticating the desired data block for Read access using Key A */
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK)
  {
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");  
  }
  
}