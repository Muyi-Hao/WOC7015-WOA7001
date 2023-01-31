#include <Servo.h>

// inverse kinematics library https://github.com/cgxeiji/CGx-InverseK
#include <InverseK.h>


// define servos, with number same as pin assignment to ease undertsanding

// initiate Servos object
Servo servo3;
Servo servo5;
Servo servo6;
Servo servo9;
Servo servo10;
Servo servo11;

// initiate i
int i = 0;

// initiate inverse kinematic coordinates
float a3, a5, a6, a9;


// Quick conversion from the Braccio angle system to radians
float b2a(float b){
  return b / 180.0 * PI - HALF_PI;
}

// Quick conversion from radians to the Braccio angle system
float a2b(float a) {
  return (a + HALF_PI) * 180 / PI;
}

void setup() {

  Serial.begin(115200);
  
 // assign the relevant UNO pin to the servos
  servo3.attach(3);
  servo5.attach(5);
  servo6.attach(6);
  servo9.attach(9);
  servo10.attach(10);
  servo11.attach(11);


  // Setup the lengths and rotation limits for each link
  Link base, upperarm, forearm, hand;

  base.init(0, b2a(0.0), b2a(180.0));
  upperarm.init(105, b2a(15.0), b2a(165.0));
  forearm.init(146, b2a(0.0), b2a(180.0));
  hand.init(95, b2a(0.0), b2a(180.0));

  // Attach the links to the inverse kinematic model
  InverseK.attach(base, upperarm, forearm, hand);
  // InverseK.solve() return true if it could find a solution and false if not.

  // Calculates the angles without considering a specific approach angle
  // InverseK.solve(x, y, z, a0, a1, a2, a3)
  if (InverseK.solve(100, 0, 300, a3, a5, a6, a9)) {
    Serial.print(a2b(a3));
    Serial.print(',');
    Serial.print(a2b(a5));
    Serial.print(',');
    Serial.print(a2b(a6));
    Serial.print(',');
    Serial.println(a2b(a9));

    servo9.write(int(a9));



  } else {
    Serial.println("No solution found!");
  }




}



void loop() {

//   for (i = 0; i < 180; i++) {

//     servo3.write(i);
//     servo5.write(i);
//     servo6.write(i);
//     servo9.write(i);
//     servo10.write(i);
//     servo11.write(i);


//     delay(10);
//   }

//   for (i = 180; i > 0; i--) {

//     servo3.write(i);
//     servo5.write(i);
//     servo6.write(i);
//     servo9.write(i);
//     servo10.write(i);
//     servo11.write(i);
//     delay(10);
//   }
}
