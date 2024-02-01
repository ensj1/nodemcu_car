#include <PS2X_lib.h>  //for v1.6


#define PS2_DAT        12  //MISO    
#define PS2_CMD        13  //MOSI
#define PS2_CS         15  //SS 
#define PS2_CLK        14  //CLK 

#define MOTOR_2_LF        9
#define MOTOR_2_RG        10
#define MOTOR_1_LF        4
#define MOTOR_1_RG        5

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
#define pressures   false
#define rumble      false

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning
//you must always either restart your Arduino after you connect the controller,
//or call config_gamepad(pins) again after connecting the controller.

int error = -1;
byte type = 0;
byte vibrate = 0;
int tryNum = 1;

void setup(){

  Serial.begin(115200);
  pinMode(MOTOR_1_LF, OUTPUT);   //left motors forward
  pinMode(MOTOR_2_RG, OUTPUT);   //left motors reverse
  pinMode(MOTOR_1_LF, OUTPUT);   //right motors forward
  pinMode(MOTOR_1_RG, OUTPUT);   //right motors reverse


  while (error != 0) {
    delay(1000);
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_CS, PS2_DAT, pressures, rumble);
    Serial.print("#try config ");
    Serial.println(tryNum);
    tryNum++;
  }

  Serial.println(ps2x.Analog(1), HEX);

  type = ps2x.readType();
  switch(type) {
    case 0:
      Serial.println(" Unknown Controller type found ");
      break;
    case 1:
      Serial.println(" DualShock Controller found ");
      break;
    case 2:
      Serial.println(" GuitarHero Controller found ");
      break;
	  case 3:
      Serial.println(" Wireless Sony DualShock Controller found ");
      break;
   }
}

void loop() {

  if(type == 1){ //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

    //will be TRUE as long as button is pressed
    if(ps2x.Button(PSB_START))
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");

    //will be TRUE as long as button is pressed
    if(ps2x.Button(PSB_PAD_UP)) {
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
      moveForward(255);
    }
    else if (ps2x.Button(PSB_PAD_RIGHT)){
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
      turnRight(255);
    }
    else if (ps2x.Button(PSB_PAD_LEFT)){
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
      turnLeft(255);
    }
    else if (ps2x.Button(PSB_PAD_DOWN)){
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
      moveBack(255);
    } else {
      stop();
    }

    vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if(ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if(ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if(ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
      if(ps2x.Button(PSB_TRIANGLE))
        Serial.println("△ pressed");
    }
    //△□○×
    if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
      Serial.println("○ just pressed");
    if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
      Serial.println("× just changed");
    if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
      Serial.println("□ just released");

    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
      Serial.print("Stick Values:");
      Serial.print(ps2x.Analog(PSS_LY)); //Left stick, Y axis. Other options: LX, RY, RX
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_LX), DEC);
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_RY), DEC);
      Serial.print(",");
      Serial.println(ps2x.Analog(PSS_RX), DEC);
    }

  }
  delay(50);
}

void moveForward(int speed) {
  analogWrite(MOTOR_1_LF, speed);
  analogWrite(MOTOR_1_RG, 0);
  analogWrite(MOTOR_2_LF, speed);
  analogWrite(MOTOR_2_RG, 0);
}

void moveBack(int speed) {
  analogWrite(MOTOR_1_LF, 0);
  analogWrite(MOTOR_1_RG, speed);
  analogWrite(MOTOR_2_LF, 0);
  analogWrite(MOTOR_2_RG, speed);
}

void turnLeft(int speed) {
  analogWrite(MOTOR_1_LF, 0);
  analogWrite(MOTOR_1_RG, speed);
  analogWrite(MOTOR_2_LF, speed);
  analogWrite(MOTOR_2_RG, 0);
}

void turnRight(int speed) {
  analogWrite(MOTOR_1_LF, speed);
  analogWrite(MOTOR_1_RG, 0);
  analogWrite(MOTOR_2_LF, 0);
  analogWrite(MOTOR_2_RG, speed);
}

void stop() {
  digitalWrite(MOTOR_1_LF, LOW);
  digitalWrite(MOTOR_1_RG, LOW);
  digitalWrite(MOTOR_2_LF, LOW);
  digitalWrite(MOTOR_2_RG, LOW);
}