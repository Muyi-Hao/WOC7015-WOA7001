// inverse kinematics library https://github.com/cgxeiji/CGx-InverseK
#include <InverseK.h>

// Json library https://arduinojson.org/
#include <ArduinoJson.h>

// Arduino servo library https://www.arduino.cc/reference/en/libraries/servo/
#include <Servo.h>

bool debug = true;

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
float pos1; //= b2a(90);  // 0 - 180
float pos2; //= b2a(90);  // 90 default, 9 - 180
float pos3; //= b2a(90);  // 90 default, 0 - 180
float pos4; //= b2a(90);  // 90 default, 0 - 180
float pos5; //= b2a(0);   // 0 - 180
float pos6; //= b2a(30);  // 90 - closed, 30 - opened.



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

  // // Setup the lengths and rotation limits for each link
  // Link base, upperarm, forearm, hand;

  // base.init(0, b2a(0.0), b2a(180.0));
  // upperarm.init(105, b2a(15.0), b2a(165.0));
  // forearm.init(150, b2a(0.0), b2a(180.0));
  // hand.init(270, b2a(0.0), b2a(180.0));  // need update
  // // Power of 5 V and 3 A (1 A)
  // // Attach the links to the inverse kinematic model
  // InverseK.attach(base, upperarm, forearm, hand);
}

void loop() {
  // put your main code here, to run repeatedly:
  // 90.00, 40.76, 111.37, 5.87
  // 90.00, 51.73, 155.28, 62.99
  servo1.write(90);
  
  servo2.write(90);
  servo3.write(90);
  servo4.write(180);

  servo5.write(30);
  servo6.write(60);
}
