//F210502

//LIBRARIES
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

//COLOURS
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

//MAIN
#define WAITING_FOR_INPUT 0
#define HOLD_SELECT 1
#define SCROLL_UP 2
#define SCROLL_DOWN 3
//HOLDSELECT
#define SELECT_HELD 4
#define DISPLAYING_ID 5

enum DeviceTypes {
  S,
  O,
  L,
  T,
  C
};

//CUSTOM CHARACTERS
byte downArrow[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100
};
byte upArrow[8] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};
byte degrees[8] = {
  B11100,
  B10100,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
int currentState = WAITING_FOR_INPUT;  //default state after start up
DeviceTypes type = S;                  //default type
long previousMillis = 0;               //for printQ
unsigned long startTime = 0;           //for holdSelect
String inputString;                    //user's input
bool stopClear = true;                 //flag which prevents flickering when a device is shown
int deviceIndex = 0;                   //index for which device is shown
bool topArrow = false;                 //flag to show up arrow
bool bottomArrow = false;              //flag to show down arrow
const int MAX_DEVICES = 10;

class Device {
private:
  String deviceID;
  char type;
  String location;
  bool state;
  String power;

public:
  Device() {  //default/null constructor
    deviceID = "";
    type = ' ';
    location = "";
    state = false;
    power = "";
  }

  Device(String id, char t, String loc) {
    deviceID = id;
    type = t;
    location = loc;
    state = false;  // Default state is OFF
    power = "";     //default state is no power
  }

  String getDeviceID() {
    return deviceID;
  }

  char getType() {
    return type;
  }

  String getLocation() {
    return location;
  }

  void setLocation(String l) {
    location = l;
  }

  bool getState() {
    return state;
  }

  void setState(bool s) {
    state = s;
  }

  void setPower(String p) {
    power = p;
  }

  void setType(char newtype){
    type = newtype;
  }

  void printDeviceLCD() { //displays the device information on screen
    if (stopClear) {
      lcd.clear();
      stopClear = false;
    }
    if (topArrow) { //bool to show Up arrow
      lcd.write(byte(2));
    } else {
      lcd.print(" ");
    }

    lcd.print(deviceID);
    lcd.print(" ");
    lcd.print(location);

    lcd.setCursor(0, 1);
    if (bottomArrow) { //bool to show Down Arrow
      lcd.write(byte(1));
    } else {
      lcd.print(" ");
    }
    lcd.print(type);
    lcd.print(" ");
    if (state) { //state is true or false (on/off)
      lcd.print(" ON");
      lcd.setBacklight(GREEN);
    } else {
      lcd.print("OFF");
      lcd.setBacklight(YELLOW);
    }
    lcd.print(" ");
    if (power) {
      lcd.print(power);
    }
    lcd.print("          ");
  }
};

class DeviceManager {
private:
  Device* devices; //pointer to array of devices
  int numDevices;

public:
  DeviceManager() {
    devices = new Device[MAX_DEVICES](); //makes size "dynamic" as we don't know size on run time
    numDevices = 0;
  }

  ~DeviceManager() {
    delete[] devices;
  }

  bool isFull() {
    return numDevices == MAX_DEVICES;
  }

  void addDevice(String id, char t, String loc) {
    //ADD DEVICE TO THE ARRAY
    if (!isFull()) {
      devices[numDevices] = Device(id, t, loc);
      numDevices++;
    } else {
      Serial.println(F("Device Limit Reached."));
    }
  }

  int getNumDevices() {
    return numDevices;
  }

  Device* getDeviceList() {
    return devices;
  }

  Device* findDevice(String id, DeviceManager& manager) {
    //FIND DEVICE GIVEN ID
    for (int i = 0; i < manager.getNumDevices(); i++) {
      if (manager.devices[i].getDeviceID() == id) {
        return &manager.devices[i];
      }
    }
    return NULL;  // If device ID not found
  }

  void removeDevice(String id) {
    //ITERATE THROUGH MY DEVICES. IF ID IS FOUND THEN REMOVE
    for (int i = 0; i < numDevices; i++) {
      if (devices[i].getDeviceID() == id) {
        if (i == deviceIndex) {
          if (deviceIndex > 0) {
            deviceIndex--;
          } else if (deviceIndex < numDevices - 1) {
            deviceIndex++;
          }
        }
        for (int j = i; j < numDevices - 1; j++) {
          devices[j] = devices[j + 1];
        }
        numDevices--;
        if (numDevices == 0) {
          lcd.clear();
        }
        return;
      }
    }
  }

  void sortDevices() { //selection sort
    //ITERATE THROUGH MY DEVICE LIST TO SORT DEVICES BY ALPHABETICAL ORDER#
    int n = getNumDevices();

    for (int i = 0; i < n - 1; i++) {
      int minIndex = i;
      for (int j = i + 1; j < n; j++) {
        if (devices[j].getDeviceID() < devices[minIndex].getDeviceID()) {
          minIndex = j;
        }
      }
      if (minIndex != i) { //get minimum item to bottom
        Device temp = devices[i];
        devices[i] = devices[minIndex];
        devices[minIndex] = temp;
      }
    }
  }
};

DeviceManager deviceManager;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setBacklight(VIOLET);

  lcd.createChar(1, downArrow);
  lcd.createChar(2, upArrow);
  lcd.createChar(3, degrees);
  lcd.setCursor(0, 0);

  char firstInput = (char)Serial.read();
  char x = 'X';
  while (firstInput != x) {
    firstInput = (char)Serial.read();
    printQ();
  }
  Serial.println(F("UDCHARS,FREERAM"));
  lcd.setBacklight(WHITE);
}

void loop() {
  uint8_t buttons = lcd.readButtons();
  switch (currentState) {
    case WAITING_FOR_INPUT:
      //FIRST I DISPLAY ANY DEVICES IF THERE ARE ANY
      topArrow = false;                                                          //no uparrow displayed
      bottomArrow = false;                                                       //no downarrow displayed
      if (deviceIndex < deviceManager.getNumDevices() - 1 && deviceIndex > 0) {  //if the device shown is in the middle of array (not top or bottom)
        topArrow = true;
        bottomArrow = true;
      } else if (deviceIndex > 0) {
        topArrow = true;
      } else if (deviceIndex < deviceManager.getNumDevices() - 1) {
        bottomArrow = true;
      }
      if (deviceManager.getNumDevices() == 0) {  //NO DEVICES
        lcd.setBacklight(WHITE);
      } else {
        deviceManager.sortDevices();
        ((deviceManager.getDeviceList())[deviceIndex]).printDeviceLCD();  //FIRST DEVICE
      }
      if (Serial.available()) {  //WAIITNG FOR USER INPUT
        inputString = Serial.readString();
        inputString.trim();
        String command = inputString.substring(0, 1);  // Get the first characters of the inputString
        command.toUpperCase();
        String deviceId = inputString.substring(2, 5);  // Get the device ID from the inputString
        deviceId.toUpperCase();
        int deviceType = getDeviceType(inputString);  //Use getDeviceType to check the user has entered an acceptable type
        int length = inputString.length();
        inputString[length] = '\0';
        switch (command.charAt(0)) {
          case 'A':
            {
              if (deviceType == -1) {  //invalid device type
                printError(inputString);
                currentState = WAITING_FOR_INPUT;
                break;
              }
              Device* myDevice = deviceManager.findDevice(deviceId, deviceManager);  //calls findDevice function
              String location = inputString.substring(8, (length));
              inputString.toUpperCase();
              char deviceTypeChar = inputString.charAt(6);
              if (location == "" || location.indexOf(' ') >= 0 || location.indexOf('-') >= 0) {  //if the location is empty or has spaces it is invalid
                printError(inputString);
                break;
              } else if (location.length() > 11) {
                location = location.substring(0, 11);  //trims location if too long so it can fit on the screen
              }
              if (myDevice == NULL) {  //if the deviceID doesnt exist yet
                Device newDevice(deviceId, deviceTypeChar, location);
                newDevice.printDeviceLCD();
                deviceManager.addDevice(newDevice.getDeviceID(), newDevice.getType(), newDevice.getLocation());
              } else {
                myDevice->setLocation(location);  //if the deviceID does exist, just update the location
                if (myDevice->getType() != deviceTypeChar){
                  myDevice->setType(deviceTypeChar); //and the state
                  myDevice->setPower("");  
                }
                    
              }
              currentState = DISPLAYING_ID;  //this is done to essentially refresh my screen incase a new device should be alphabetically first
              break;
            }
          case 'S':
            {
              inputString.toUpperCase();
              String state = inputString.substring(inputString.lastIndexOf("-") + 1);  //gets information after last dash (on or off)
              Device* myDevice = deviceManager.findDevice(deviceId, deviceManager);    //uses findDevice method to get device by that ID
              state.trim();
              if (myDevice != NULL) {  //if the device exists
                if (state == "ON") {
                  myDevice->setState(true);
                } else if (state.equals("OFF")) {
                  myDevice->setState(false);
                } else {
                  printError(inputString);  //the state was not on or off
                }
              } else {
                printError("Device not found.");  //cannot set state for an unknown device
              }
              break;
            }
          case 'P':
            {
              inputString.toUpperCase();
              int power = inputString.substring(inputString.lastIndexOf("-") + 1).toInt();
              Device* myDevice = deviceManager.findDevice(deviceId, deviceManager);
              char type = (myDevice->getType());
              String result = String(power);
              if (myDevice != NULL) {
                switch (type) {
                  case 'S':
                    {
                      if (power < 101 && power > -1) {
                        if (power < 100) {
                          result = " " + result;
                        }
                        result += "%";
                        myDevice->setPower(result);
                      } else {
                        printError(inputString);
                      }
                    }
                    break;
                  case 'O':
                    {
                      printError(inputString);
                      break;
                    }
                  case 'L':
                    {
                      if (power < 101 && power > -1) {
                        if (power < 100) {
                          result = " " + result;
                        }
                        result += "%";
                        myDevice->setPower(result);
                      } else {
                        printError(inputString);
                      }
                    }
                    break;
                  case 'T':
                    {
                      if (power < 33 && power > 8) {
                        char degrees = byte(3);
                        result = result + degrees + "C";
                        myDevice->setPower(result);
                      } else {
                        printError(inputString);
                      }
                    }
                    break;
                  case 'C':
                    {
                      printError(inputString);
                      break;
                    }
                }
                break;
              }
            }
          case 'R':
            {
              deviceManager.removeDevice(deviceId);
              currentState = DISPLAYING_ID;  //this is done to essentially refresh my screen incase a removed device was the one shown
              break;
            }
          default:

            break;
        }
      } else if (buttons & BUTTON_SELECT) {
        currentState = HOLD_SELECT;
      } else if (buttons & BUTTON_UP) {
        currentState = SCROLL_UP;
      } else if (buttons & BUTTON_DOWN) {
        currentState = SCROLL_DOWN;
      }
      break;

    case HOLD_SELECT:
      if (buttons & BUTTON_SELECT) {
        startTime = millis();
        currentState = SELECT_HELD;
      }
    case SELECT_HELD:
      if (!(buttons & BUTTON_SELECT)) {
        startTime = 0;
        currentState = WAITING_FOR_INPUT;
      } else if (millis() - startTime > 1000) {
        lcd.clear();
        lcd.setBacklight(VIOLET);
        lcd.setCursor(0, 0);
        lcd.print("ID: F210502");
        lcd.setCursor(0, 1);
        lcd.print("SRAM: ");
        lcd.print(freeMemory());
        // Serial.print(freeRam());
        currentState = DISPLAYING_ID;
      }
      break;
    case DISPLAYING_ID:
      if (!(buttons & BUTTON_SELECT)) {
        lcd.clear();
        currentState = WAITING_FOR_INPUT;
      }
      break;
    case SCROLL_UP:  //UP TO 0 WHICH IS FIRST DEVICE
      {
        if (deviceIndex < 1) {  //do nothing if top of list
        } else {                //else scroll up
          stopClear = true;
          deviceIndex--;
        }
        currentState = WAITING_FOR_INPUT;
        break;
      }
    case SCROLL_DOWN:  //DOWN TOWARDS NUM_DEVICES WHICH IS LAST DEVICE
      {
        if (deviceIndex >= deviceManager.getNumDevices() - 1) {  //do nothing if bottom of list
        } else {                                                 //else scroll down
          stopClear = true;
          deviceIndex++;
        }
        currentState = WAITING_FOR_INPUT;
        break;
      }
  }
}


void printQ() {
  // If a second has passed since last display, display again
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 1000) {  //if time - 0 > 1000 go // if newTime - oldTime > 1000 go // etc
    Serial.print(F("Q"));
    previousMillis = currentMillis;
  }
}

int getDeviceType(String inputString) {
  char deviceTypeChar = inputString.charAt(6);
  switch (deviceTypeChar) {
    case 'S':
    case 's':
      {
        return S;
      }
    case 'O':
    case 'o':
      {
        return O;
      }
    case 'L':
    case 'l':
      {
        return L;
      }
    case 'T':
    case 't':
      {
        return T;
      }
    case 'C':
    case 'c':
      {
        return C;
      }
    default:
      {
        return -1;
      }  // Unknown device type
  }
}

void printError(String error) {
  Serial.print(F("ERROR:"));
  Serial.println(error);
}


//BELOW IS MY SRAM CODE. THIS IS FROM THE Arduino MemoryFree library. Link - https://playground.arduino.cc/Code/AvailableMemory/
extern unsigned int __heap_start;
extern void* __brkval;
struct __freelist {
  size_t sz;
  struct __freelist* nx;
};
extern struct __freelist* __flp;

int freeListSize() {
  struct __freelist* current;
  int total = 0;
  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int)current->sz;
  }
  return total;
}

int freeMemory() {
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += freeListSize();
  }
  return free_memory;
}
//OTHER WAY TO CALCULATE FREERAM, POTENTIALLY FASTER BUT LESS ACCURATE
// int freeRam () {
//   extern int __heap_start, *__brkval;
//   int v;
//   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
// }