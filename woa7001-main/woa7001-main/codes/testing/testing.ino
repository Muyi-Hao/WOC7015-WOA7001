// inverse kinematics library https://github.com/cgxeiji/CGx-InverseK
#include <InverseK.h>

// Json library https://arduinojson.org/
#include <ArduinoJson.h>

// Arduino servo library https://www.arduino.cc/reference/en/libraries/servo/
#include <Servo.h>

bool debug = false;

// Initializing Servos and starting positions
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;

// Quick conversion from the Braccio angle system to radians
float b2a(float b) {
  return b / 180.0 * PI - HALF_PI;
}

// Quick conversion from radians to the Braccio angle system
float a2b(float a) {
  return (a + HALF_PI) * 180 / PI;
}

// the angles for servo
float pos1 = b2a(90);  // 0 - 180
float pos2 = b2a(90);  // 90 default, 9 - 180
float pos3 = b2a(90);  // 90 default, 0 - 180
float pos4 = b2a(90);  // 90 default, 0 - 180
float pos5 = b2a(0);   // 0 - 180
float pos6 = b2a(30);  // 90 - closed, 30 - opened.

// data format <x,y,z> , i.e. < as start marker, > as end marker and , as delimiter
// the x, y and z coordinate
float x = 50;
float y = 0;
float z = 50;

// define global variable for Serial communication
const byte numChars = 128;
char receivedChars[numChars];  // to receive Serial.read() data
char tempChars[numChars];      // temporary array for use when parsing
int code = 0;
char msg[32];

// variables to hold the parsed data is x, y and z, defined at the top

boolean newData = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo1.attach(3);
  servo2.attach(5);
  servo3.attach(6);
  servo4.attach(9);
  servo5.attach(10);
  servo6.attach(11);

  // Setup the lengths and rotation limits for each link
  Link base, upperarm, forearm, hand;

  base.init(0, b2a(0.0), b2a(180.0));
  upperarm.init(110, b2a(15.0), b2a(165.0));
  forearm.init(150, b2a(0.0), b2a(180.0));
  hand.init(180, b2a(0.0), b2a(180.0));  // need update

  // Power of 5 V and 3 A (1 A)
  // Attach the links to the inverse kinematic model
  InverseK.attach(base, upperarm, forearm, hand);
}

void loop() {
  // when there is Serial.available() > 0, i.e. there is signal then read the x, y and z coordinate
  // and update pos angle

  recvWithStartEndMarkers();
  if (newData == true) {
    // translate data from serial com to JSON and then to assigned variables, such as x, y and z
    parseJSON();

    //only show during debugging
    if(debug){
      showParsedData(); 
    }

    // update position variables
    updatePos();
    if(debug){
      Serial.println(code);
    }

    if(code==2){
      // catch
      pos6=b2a(90);
    } else if(code==3){
      // release
      pos6=b2a(30);
    }
    newData = false;
  }

  // Serial.println(pos1 + ',' + pos2 + ',' + pos3 + ',' + pos4 + ',' + pos5 + ',' + pos6);
  // put your main code here, to run repeatedly:
  servo1.write(a2b(pos1));
  servo2.write(a2b(pos2));
  servo3.write(a2b(pos3));
  servo4.write(a2b(pos4));
  servo5.write(a2b(pos5));
  servo6.write(a2b(pos6));

  // delay(5);
}

// TODO, change to JSON format in serial to send to GUI
void updatePos() {
  if (InverseK.solve(x, y, z, pos1, pos2, pos3, pos4)) {
    if (debug) {
      successMsg();
    }
    successSerial();
  } else {
    if (debug) {
      failMsg();
    }
    failSerial();
  }
}

void successMsg() {
  Serial.println((String) "Moving to coordinate x, y and z: " + x + ", " + y + ", " + z);
  Serial.println((String) "Moving angle for pos1, pos2, pos3, pos4: " + a2b(pos1) + ", " + a2b(pos2) + ", " + a2b(pos3) + ", " + a2b(pos4));
}

void failMsg() {
  Serial.println("Coordinate could not be reached");
  Serial.println((String) "Failed to reach coordinate x, y and z: " + x + ", " + y + ", " + z);
  Serial.println((String) "Angle calculated for pos1, pos2, pos3, pos4: " + a2b(pos1) + ", " + a2b(pos2) + ", " + a2b(pos3) + ", " + a2b(pos4));
}

void successSerial() {
  Serial.println((String)"{\"code\":"+code+",\"xyz\":" + x + "," + y + "," + z + "\",\"msg\":\"success\"}");
}

void failSerial() {
  Serial.println((String)"{\"code\":"+code+",\"xyz\":" + x + "," + y + "," + z + "\",\"msg\":\"fail\"}");
}

//============
// https://forum.arduino.cc/t/serial-input-basics-updated/382007/3

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';  // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

//============

void parseData() {  // split the data into its parts

  char *strtokIndx;  // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");  // get the first part - the string
  x = atof(strtokIndx);                 // assign first part to x as float

  strtokIndx = strtok(NULL, ",");  // this continues where the previous call left off
  y = atof(strtokIndx);            // assign second data to y as float

  strtokIndx = strtok(NULL, ",");
  z = atof(strtokIndx);  // assign last data to z as float
}

// if using JSON format with <>, such as <{"code":1, "xyz":"400,0,100", "msg":"bla bla bla", "pos5": "0", "pos6": "0"}>
void parseJSON() {
  // declare doc to hold serialized json data
  StaticJsonDocument<200> doc;

  // declare temporary char to hold raw string/char data
  char json[numChars];
  strcpy(json, receivedChars);

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    Serial.println(receivedChars);
    return;
  } else {
    // i.e. if no error
    code = doc["code"];
    strcpy(msg, doc["msg"]);

    // get the code, msg, pos and xyz
    pos5 = doc["pos5"];
    pos6 = doc["pos6"];

    // get x, y and z to each array
    // const char *xyz = doc["xyz"];
    // strcpy(tempChars, xyz);

    strcpy(tempChars, doc["xyz"]);
    parseData();
  }
}

//============

void showParsedData() {
  Serial.print("x: ");
  Serial.println(x);
  Serial.print("y: ");
  Serial.println(y);
  Serial.print("z: ");
  Serial.println(z);
}