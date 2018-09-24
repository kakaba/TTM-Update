
bool setupModeActive = 0;

int pwmSetup = 255;

unsigned long lowModeSetup = 50;



#include <EEPROM.h>
#include <Arduino.h>



int dinPin = 9;
int ledPin = 12;
int relayPin = 2;
int buzzerPin = 10;
int pwmPin = 6;



int pwmConfig;
int pwmReduce;
long configDelay;
long configDelay1;



int ledState = LOW;

int pwmPower = 255;

bool switchMode = 0;

bool eepromCheck = 0;
int configPwm = 0;
unsigned long configDelayLow = 0;
unsigned long configDelayHigh = 0;

bool signalPath[9] = {0, 0, 0, 0, 0, 0, 0, 0};
bool overSpeedMode = 0;
bool slowDownMode = 0;
bool relayState = 0;

unsigned long signalTimeSet = 0;
unsigned long signalDelay = 10;
unsigned long offTime = 400;
unsigned long blinkTimeSet = 0;
unsigned long blinkInterval;
unsigned long blinkTimeHigh = 200;
unsigned long blinkTimeLow = 1500;
unsigned long overSpeedTimeSet = 0;
unsigned long slowDownTime = 100;
unsigned long overSpeedTime = 0;
unsigned long pwmTimeSet = 0;
unsigned long pwmInterval = 20;
unsigned long pwmDelay = 40;
unsigned long pwmDelay1 = 15;
unsigned long pwmDelaySet = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(10);
  pinMode(dinPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  setPwmFrequency(3, 128);


}

void loop() {
  // put your main code here, to run repeatedly:

  //bootUp();

  setupMode(); // კონფიგურაციის რეჟიმი

  //setupModeTest();

  //readSignal(); // ციფრულ შესასვლელზე dinPin ზე შემოსული სიგნალის წაკითხვა - შეერთებულია ტელტონიკაზე

  //printArray(); // სიგნალების ერრის დაბეჭდვა ბოლო რვა მონაცემის

  //overSpeed(); // გადამოწმება თუ ტელტონიკა მუშაობს სწორედ და გადაჭარბების სიგნალი მოდის

  //blinkLed(); // თუ სიჩქარის გადაჭარბება დაფიქსირდა ჩართოს ბიპერი და ლედ ნათურა

  //slowDown(); // სიჩქარის შენელება 15 წამის შემდგომ

}


void bootUp() {

  Serial.println(F("Booting Up"));
  EEPROMCheck();
  if ( eepromCheck ) {

    configPwm = EEPROMReadint(0);
    configDelayLow = EEPROMReadlong(2);
    configDelayHigh = EEPROMReadlong(6);
    Serial.println(configPwm);
    Serial.println(configDelayLow);
    Serial.println(configDelayHigh);
    Serial.println(F("Bootup Procces Succsesful!"));

  }

  else {

    Serial.println(F("Bootup Procces Failed!"));
    Serial.println(F("Configuring Default Mode!"));
    defaultMode();

  }

}

void defaultMode() {

  configPwm = 120;
  configDelayLow = 50;
  configDelayHigh = 50;
  Serial.println(F("Default Mode Configured!"));

}

void reboot() {
  Serial.println(F("restart Device"));
  bootUp();
}

//  Configuration Mode


void setupMode() {

  if (Serial && setupModeActive != 1) {
    Serial.println(F("........................................"));
    Serial.println(F("..You entered the setup mode!         .."));
    Serial.println(F("..to Enter Easy Setup Mode Press ENTER.."));
    Serial.println(F("..To enter advanced setup mode press A.."));
    Serial.println(F("..To exit setup mode press N          .."));
    Serial.println(F("........................................"));


    setupModeActive = 1;

  }



  if (Serial.available()) {
    int j = Serial.read();

    switch ( j ) {
      case 110:
        Serial.println(F("Exit the function"));
        break;

      case 78:
        Serial.println(F("Exit the function"));
        break;

      case 13:
        Serial.println(F("You Entered Easy Setup Mode"));
        easySetup();
        break;

      case 65:
        Serial.println(F("You Entered Advanced Setup Mode"));
        advancedSetup();
        break;

      case 97:
        Serial.println(F("You Entered Advanced Setup Mode"));
        advancedSetup();
        break;

      default:
        Serial.println(F("You Entered Incorrect Charecter"));
        break;

    }
  }
}


void advancedSetup() {
  Serial.println(F(".............................................................."));
  Serial.println(F("..You Entered Advanced Setup Mode                           .."));
  Serial.println(F("..For Lowering Engine Power press L for higening press H    .."));
  Serial.println(F("..For Lowering Engine Power press L for higening press H    .."));
  Serial.println(F("..Press Enter To Save The Setup Or Press N To Exit The Setup.."));
  Serial.println(F(".............................................................."));

  int power = 100;

  int pwmValue = 255;

  bool active = 0;

  while ( active == 0 ) {


    engineTest( pwmValue , (100 - power) , power );

    if ( Serial.available() ) {

      int i = Serial.read();

      switch ( i ) {
        case 100:
          power -= 1;
          break;

        case 68:
          power -= 1;
          break;

        case 97:
          power += 1;
          break;

        case 65:
          power += 1;
          break;

        case 83:
          pwmValue -= 1;
          break;

        case 115:
          pwmValue -= 1;
          break;

        case 87:
          pwmValue += 1;
          break;

        case 119:
          pwmValue += 1;
          break;
      }

      if ( power < 1 ) {
        power = 1;
      }

      if ( power > 100 ) {
        power = 100;
      }

      if ( pwmValue < 1 ) {
        pwmValue = 1;
      }

      if ( pwmValue > 255 ) {
        pwmValue = 255;
      }

      Serial.print(F(" Power: "));
      Serial.print(pwmValue);
      Serial.print(F(" High Time: "));
      Serial.print(power);
      Serial.print(F(" Low Time: "));
      Serial.println((100 - power));


      switch ( i ) {
        case 110:
          Serial.println(F("Exit the function"));
          active = 1;
          break;

        case 78:
          Serial.println(F("Exit the function"));
          active = 1;
          break;

        case 13:
          Serial.println(F("PWM power saved, entering next stage"));
          active = 1;
          saveConfiguration( pwmValue , 100 - power , power );
          break;
      }
    }
  }
}


void easySetup() {
  Serial.println(F(".............................................................."));
  Serial.println(F("..You Entered Easy Setup Mode                               .."));
  Serial.println(F("..For Lowering Engine Power press L for higening press H    .."));
  Serial.println(F("..Press Enter To Save The Setup Or Press N To Exit The Setup.."));
  Serial.println(F(".............................................................."));

  int power = 100;

  bool active = 0;

  while ( active == 0 ) {


    engineTest( 120 , (100 - power) , power );

    if ( Serial.available() ) {

      int i = Serial.read();

      switch ( i ) {
        case 83:
          power -= 1;
          break;

        case 115:
          power -= 1;
          break;

        case 87:
          power += 1;
          break;

        case 119:
          power += 1;
          break;
      }

      if ( power <= 1 ) {
        power = 1;
      }

      if ( power > 100 ) {
        power = 100;
      }

      Serial.print( power );
      Serial.println(F( " % " ));

      switch ( i ) {
        case 110:
          Serial.println(F("Exit the function"));
          active = 1;
          break;

        case 78:
          Serial.println(F("Exit the function"));
          active = 1;
          break;

        case 13:
          Serial.println(F("PWM power saved, entering next stage"));
          active = 1;
          saveConfiguration( 120 , 100 - power , power );
          break;
      }
    }
  }
}


void engineTest( int power, long delayLow, long delayHigh ) {

  if ( millis() > ( pwmDelaySet + pwmDelay )) {
    pwmDelaySet = millis();

    if ( pwmPower > 0 ) {
      pwmPower = 0;
      pwmDelay = delayLow;
    }

    else {
      pwmPower = power;
      pwmDelay = delayHigh;
    }

    analogWrite(pwmPin, pwmPower);

  }
}

//  Configuration Mode End



//  EEPROM CODE

void saveConfiguration( int value1 , long value2 , long value3 ) {
  EEPROMWriteint( 0 , value1);
  EEPROMWritelong( 2 , value2);
  EEPROMWritelong( 6 , value3);

  EEPROMCheck();

  int checkOne = EEPROMReadint( 0 );
  long checkTwo = EEPROMReadlong( 2 );
  long checkThree = EEPROMReadlong( 6 );

  if ( checkOne == value1 && checkTwo == value2 && checkThree && eepromCheck ) {

    Serial.println("Write Succsesful");
    reboot();
  }

  else {
    Serial.println("ERROR");
    Serial.println("Rebooting Device");

    Serial.println("No Configuration Saved");
    Serial.println("Please Try Again Later");

    reboot();

  }
}


void EEPROMCheck() {
  //Print length of data to run CRC on.
  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());

  //Print the result of calling eeprom_crc()
  Serial.print("CRC32 of EEPROM data: 0x");
  Serial.println(eeprom_crc(), HEX);
  Serial.print("CRC32 of EEPROM data in Decimal: ");
  Serial.println(eeprom_crc());

  int address = EEPROM.length() - 4;
  unsigned long calc = eeprom_crc();
  Serial.println(calc);
  EEPROMWritelong( address , eeprom_crc() );

  unsigned long check = EEPROMReadlong( address );
  Serial.println(check);

  if ( check == calc ) {
    Serial.println("EEPROM CHECK");
    eepromCheck = 1;
  }
  else {
    Serial.println("EEPROM DATA CORRUPTED");
    eepromCheck = 0;
    Serial.println("No Configuration Saved");
    Serial.println("Please Try Again Later");
    Serial.println("Setup Default Mode");
  }
}

//This function will write a 2 byte (16bit) long to the eeprom at
//the specified address to address + 2.
void EEPROMWriteint(int address, int value)
{
  //Decomposition from a long to 2 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  //      byte two = ((value >> 16) & 0xFF);
  //      byte one = ((value >> 24) & 0xFF);

  //Write the 2 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  //      EEPROM.write(address + 2, two);
  //      EEPROM.write(address + 3, one);
}

//This function will return a 4 byte (32bit) long from the eeprom
//at the specified address to address + 3.
int EEPROMReadint(int address)
{
  //Read the 4 bytes from the eeprom memory.
  //      int four = EEPROM.read(address);
  //      int three = EEPROM.read(address + 1);
  int two = EEPROM.read(address);
  int one = EEPROM.read(address + 1);

  //Return the recomposed long by using bitshift.
  return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}

unsigned long EEPROMReadlong(int address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EEPROMWritelong(int address, unsigned long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

unsigned long eeprom_crc(void) {

  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  for (int index = 0 ; index < (EEPROM.length() - 4)  ; ++index) {
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}


// EEPROM CODE END


void readSignal() {

  bool reading = !digitalRead(dinPin);

  if (( millis() - signalTimeSet) > signalDelay ) {

    signalTimeSet = millis();

    if ( reading == HIGH ) {
      addArray(1);
    }
    if ( reading == LOW ) {
      addArray(0);
    }
  }
}

void printArray() {

  for ( int i = 0 ; i < 8 ; i++ ) {
    if (i == 0) {
      Serial.println(signalPath[i]);
    }
    if (i != 0 ) {
      Serial.print(signalPath[i]);
    }
  }
}

void addArray(bool x) {

  for ( int i = 0; i < 8 ; i++ ) {
    if ( i != 7 ) {
      signalPath[i] = signalPath[i + 1];
    }
    if ( i == 7 ) {
      signalPath[i] = x;
    }
  }
}

void overSpeed() {

  int chek = 0;
  for ( int i = 0; i < 8 ; i++ ) {
    chek += (int)signalPath[i];
  }

  if ( chek > 5 ) {
    overSpeedMode = 1;
    overSpeedTimeSet = millis();
  }
  if ( chek <= 5 && (millis() - overSpeedTimeSet) > offTime ) {
    overSpeedMode = 0;
  }

  if ( overSpeedMode == 1 && overSpeedTime == 0 ) {
    overSpeedTime = millis();
  }
  if ( overSpeedMode == 0 ) {
    overSpeedTime = 0;
  }

}

void blinkLed() {

  if ( overSpeedMode == 1 ) {


    if (millis() > blinkTimeSet + blinkInterval ) {
      // save the last time you blinked the LED
      blinkTimeSet = millis();

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW ) {
        blinkInterval = blinkTimeHigh;
        ledState = HIGH;
        tone(buzzerPin, 2000);
      } else {
        blinkInterval = blinkTimeLow;
        ledState = LOW;
        noTone(buzzerPin);
      }

      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
    }
  }

  else {
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }

}

void slowDown() {

  if ( overSpeedMode == 1 && millis() > slowDownTime + overSpeedTime ) {


    if (millis() > pwmInterval + pwmTimeSet && pwmPower > pwmConfig) {

      pwmTimeSet = millis();

      relayState = HIGH;
      digitalWrite(relayPin, relayState);

      if (pwmPower > pwmConfig) {
        pwmPower -= pwmReduce;
      }
      if (pwmPower <= pwmConfig) {
        pwmPower = pwmConfig;
      }

      analogWrite(pwmPin, pwmPower);
      Serial.println(pwmPower);
    }


    if (millis() > (pwmDelay + pwmDelaySet) && pwmPower <= pwmConfig) {

      pwmDelaySet = millis();

      if ( pwmPower == pwmConfig ) {
        pwmPower = 0;
        pwmDelay = pwmDelay + pwmDelay1;
        Serial.println(" pwm 0 ");
      }
      else {
        pwmPower = pwmConfig;
        pwmDelay = pwmDelay - pwmDelay1;
        Serial.println(" pwm : ");
        Serial.print(pwmPower);
      }


      analogWrite(pwmPin, pwmPower);
    }
  }

  if ( overSpeedMode == 0 ) {
    pwmDelay = 40;
    pwmDelay1 = 5;
    digitalWrite(relayPin, LOW);
    analogWrite(pwmPin, 0);
    pwmPower = 255;
  }
}


void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}





/* dump
  void runMode() {

  Serial.println(F("Running engine on given pwm"));
  Serial.println(F("For Lowering PWM press L for higening press H (unit is 5)"));
  Serial.println(F("Press enter to save the setup or press n to exit the setup"));



  int active = 0;

  while ( active == 0 ) {



    if ( Serial.available() ) {

      int i = Serial.read();

      switch ( i ) {
        case 108:
          pwmSetup -= 5;
          break;

        case 76:
          pwmSetup -= 5;
          break;

        case 104:
          pwmSetup += 5;
          break;

        case 72:
          pwmSetup += 5;
          break;
      }

      if ( pwmSetup <= 5 ) {
        pwmSetup = 5;
      }

      if ( pwmSetup > 255 ) {
        pwmSetup = 255;
      }

      Serial.println(pwmSetup);

      switch ( i ) {
        case 110:
          Serial.println(F("Exit the function"));
          active = 1;
          break;

        case 78:
          Serial.println(F("Exit the function"));
          active = 1;
          break;

        case 13:
          Serial.println(F("PWM power saved, entering next stage"));
          lowRunMode();
          active = 2;
          break;
      }
    }
  }
  }

  void lowRunMode() {
  Serial.println(F("Running engine on Pwm Low Mode"));
  Serial.println(F("For Lowering engine Low Mode press L for higening press H (unit is 5)"));
  Serial.println(F("Press enter to save the setup or press n to exit the setup"));

  int active = 0;

  while ( active == 0 ) {

    if ( Serial.available() ) {
      int i = Serial.read();

      switch ( i ) {
        case 108:
          lowModeSetup -= 1;
          break;

        case 76:
          lowModeSetup -= 1;
          break;

        case 104:
          lowModeSetup += 1;
          break;

        case 72:
          lowModeSetup += 1;
          break;
      }

      if ( lowModeSetup <= 5 ) {
        lowModeSetup = 5;
      }

      if ( lowModeSetup > 150 ) {
        lowModeSetup = 150;
      }

      Serial.println(lowModeSetup);

      if ( i == 110 ) {
        Serial.println(F("Exit the function"));
        active = 1;
      }

      if ( i == 13 ) {
        Serial.println(F("Low Mode Setup saved"));

        Serial.println(F("To finish setup please press enter to exit from setup mode press n"));
        while ( !Serial.available() ) {
          ;
        }

        int j = Serial.read();

        if ( j == 110 ) {
          Serial.println(F("exitign Setup Mode"));
          // gamosvla kodidan
        }

        if ( j == 13 ) {
          Serial.println(F("Setup finished sucsessfuly"));
          Serial.println(F("wait for saving data and rebooting"));
          // gagrdzeleba kodissssssssssssssssssssssssssssssssssssss
        }

        active = 2;
      }

    }

    // dzravis martvis kodi

  }
  }


  boolean setupModeTest() {

  lable1:


  if (Serial && setupModeActive != 1) {
    Serial.println(F("You entered the setup mode!"));
    Serial.println(F("to Enter Easy Setup Mode Press ENTER"));
    Serial.println(F("To enter advanced setup mode press A"));
    Serial.println(F("To exit setup mode press N"));

    setupModeActive = 1;
  }



  if (Serial.available()) {
    int j = Serial.read();

    switch ( j ) {
      case 110:
        Serial.println(F("Exit the function"));
        return 0;
        break;
      case 78:
        Serial.println(F("Exit the function"));
        return 0;
        break;
      case 13:
        Serial.println(F("You Entered Simple Setup Mode"));
        easySetup();
        break;
      case 65:
        Serial.println(F("You Entered Advanced Setup Mode"));
        break;
      case 97:
        Serial.println(F("You Entered Advanced Setup Mode"));
        break;
      default:
        Serial.println(F("You Entered Incorrect Charecter"));
        goto lable1;
        break;
    }

  lable2:

    Serial.println(F("Please enter pwm power value and press enter"));
    Serial.println(F("Value range is between 5 and 255"));

    int c = 0;

    String val = "";

  lable:

    while (!Serial.available()) {
      ;
    }

    int i = Serial.read();

    if ( i < 48 || i > 57 ) {
      Serial.println(F("You Entered Incorrect Value Please Try Again"));
      goto lable2;
    }

    char k = (char)i;

    Serial.print(k);

    if ( c < 3 ) {
      val += k;
      c++;
      if ( c < 3 ) {
        goto lable;
      }
    }

    int a = val.toInt();

    if ( !(a >= 5) || !(a <= 255) ) {
      Serial.println(F("you entered incorrect value"));
      goto lable2;
    }

    Serial.println("");
    Serial.print(F("You entered : "));
    Serial.println(val);

  lable3:

    Serial.println(F("If you wish to continue press Enter"));
    Serial.println(F("To Exit Setup Function Press N"));



    while (!Serial.available()) {
      ;
    }

    i = Serial.read();


    switch (i) {
      case 13:
        Serial.println(F("Saving given value"));
        pwmSetup = a;
        runMode();
        break;

      case 78:
        Serial.println(F("Full exit from Setup Mode"));
        break;

      case 110:
        Serial.println(F("Full exit from Setup Mode"));
        break;

      default: {
          Serial.println(F("You Entered Incorrect Value"));
          setupModeActive = 0;
          goto lable3;
          break;
        }
    }
  }
  }

*/
