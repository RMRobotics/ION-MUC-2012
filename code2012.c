#pragma config(Sensor, S1,     colorLeft,      sensorCOLORFULL)
#pragma config(Sensor, S2,     colorRight,     sensorCOLORFULL)
#pragma config(Motor,  motorB,          ,              tmotorNormal, PIDControl, reversed, encoder)
#pragma config(Motor,  motorC,          ,              tmotorNormal, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// ==================================================
// ==================================================

////////////////////////////////////////////////////////////
//                                                        //
//            RICHARD MONTGOMERY ROBOTICS TEAM            //
//                                                        //
//       CODE FOR THE 2011 ION MINI URBAN CHALLENGE       //
// PRIMARY VERSION FOR WASHINGTON DC REGIONAL COMPETITION //
//                                                        //
////////////////////////////////////////////////////////////

// ==================================================
// ==================================================

// CONSTANTS

#define HIGH 40 //60 // Speed to use at white-bordered zones.
#define LOW 20 //40 // Speed to use in yellow-bordered zones, parking lots, and intersections
#define SENSITIVITY 80 // Strength of steering reactions while driving
#define DECAY 1.5 // Strength of steering's tendancy towards steering straight
#define LEFTTURN 150 // Length of a 90-degree counterclockwise turn
#define RIGHTTURN 140 // Length of a 90-degree clockwise turn

// ==================================================
// ==================================================

// FUNCTION DECLARATIONS

void drive(int side, int destination);
void turn(int direction);
void park(int side, int space);
int color(int side);
void move(int distance);
void pivot(int direction);
void align();

// ==================================================
// ==================================================

// MAIN PROGRAM

task main()
{
  // Turn on the color sensors and give them time to callibrate
  wait1Msec(1000);
  color(0);
  wait1Msec(1000);
  color(1);
  wait1Msec(1000);
/*
  while(true)
  {
    color(0);
    color(1);
  }
*/
  drive(0, 0);
  turn(0);
  drive(0,1);
  park(0,0);

  wait1Msec(5000); //finish
}

// ==================================================
// ==================================================

// MAJOR SUBROUTINES

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// DRIVE
// Drives along a road, stopping at a specified trigger
// Arguments:
//   side: which side of the road it will hug (0: left, 1: right)
//   destination: when to stop (0: to intersection treating blue the same as white, 1: to parking lot, stops after encountering blue, 2: to stop box, special case)

void drive(int side, int destination)
{
  tMotor master;
  float steering = 0;
  int adjustment = SENSITIVITY * (2 * side -1);
  int lastColor = color(side);
  int otherColor = color((side + 1) % 2);
  int speed = LOW;
  int stopping = 0;

  while((destination != 0 || lastColor != 2 || otherColor != 2) && (destination != 1 || lastColor != 3) && (destination != 2 || stopping != 1)) // If destination == 0, this loops until red is seen, if destination == 1, blue.
  {
    steering = steering/DECAY;

    lastColor = color(side);
    otherColor = color((side + 1) % 2);
    if (lastColor == 2)
    {
      stopping++;
      nxtDisplayCenteredBigTextLine(4, "%2i", stopping);
    }
    if (lastColor == 0 || (destination == 2 && lastColor == 2))
    {
      steering = steering + adjustment/speed; //steer outwards
    }
    else
    {
      stopping--;
      steering = steering - adjustment/speed; //steering inwards
      if (lastColor == 2 || otherColor == 2)
      {
        speed = LOW/4; //go extra slow if any red has been seen
      }
      else if (lastColor == 4)
      {
        speed = LOW; //go slow if you see yellow
      }
      else if (lastColor == 1)
      {
        speed = HIGH; //go fast if you see white
      }
    }
    //nxtDisplayCenteredBigTextLine(4, "%3.0f", steering);

    if (steering > 0)
    {
      master = motorC;
      nSyncedMotors = synchCB; //motor C is the master, motor B is the slave
    }
    else
    {
      master = motorB;
      nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
    }

    nSyncedTurnRatio = 100 - abs(steering); //set the relative motor speeds

    motor[master] = speed; //move forwards

    wait1Msec(5);
  }
  motor[master] = 0; //stop

  nSyncedMotors = synchNone; //unsync motors

  wait1Msec(1000);

  if (destination == 0) // if stopped at a stop sign
  {
    nxtDisplayCenteredBigTextLine(4, "STOPPED");
    wait1Msec(2000); //stay at a complete stop for two seconds
  }
  else //if stopped at a parking lot, continue moving forward a certain distance.
  {
    nxtDisplayCenteredBigTextLine(5, "TO PARKING");
    for(int counter = 0; counter < 230; counter++) //For a certain distance
    {
      steering = steering/DECAY;

      lastColor = color(side);
      if (lastColor == 0)
      {
        steering = steering + adjustment/LOW;
      }
      else
      {
        steering = steering - adjustment/LOW;
      }
      nxtDisplayCenteredBigTextLine(4, "%3.0f", steering);

      if (steering > 0)
      {
        master = motorC;
        nSyncedMotors = synchCB; //motor C is the master, motor B is the slave
      }
      else
      {
        master = motorB;
        nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
      }

      nSyncedTurnRatio = 100 - abs(steering); //set the relative motor speeds

      motor[master] = LOW;

      wait1Msec(5);
    }
    motor[master] = 0; // stop moving

    nSyncedMotors = synchNone; // clear syncronization

    nxtDisplayCenteredBigTextLine(5, ""); //Clear line
  }
  nxtDisplayCenteredBigTextLine(4, ""); //Clear line
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// TURN
// Moves into an intersection, pivots as specified, and moves out of the intersection
// Arguments:
//   direction: which way to go (0: left, 1: straight, 2: right)

void turn(int direction)
{
  move(350); //Enter the intersection

  //Turn
  if (direction == 0)
  {
    pivot(0);
  }
  else if (direction == 2)
  {
    pivot(1);
  }

  //move(100); //Leave the intersection
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// PARK
// Turns and enters a parking lot, parks, and then returns to the road
// Arguments:
//   side: which side the lot is on (0: left, 1: right)
//   space: which space in the lot to park in (0: first space, 1: second space, 2: third space, etc.)

void park(int side, int space)
{
  //Park
  nxtDisplayCenteredBigTextLine(4, "Entering Lot");
  pivot(side); //turn to face the lot
  //align(); //align against the outer edge of the lot
  move(500); //enter the lot
//  align(); //align against the inner edge of the lot

  // TODO: Move to a specific space within the lot. For now, it just parks in the first space of the lot.

  nxtDisplayCenteredBigTextLine(4, "PARKED");
  wait1Msec(5000); //remain parked for five seconds

  nxtDisplayCenteredBigTextLine(4, "Leaving Lot");
  //Unpark
  pivot(side); //turn to face backwards
//  align(); //align against the edge of the space or lot
  pivot(side); //turn to face the road
  //align(); //align with outer edge of the lot
  move(550); //exit the lot
  pivot(side); //turn to face down the road

  nxtDisplayCenteredBigTextLine(4, ""); //Clear

//  drive(side, 0); // Continue driving to next intersection
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// ==================================================
// ==================================================

// MINOR FUNCTIONS

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// COLOR
// Returns the color under the sensor indicated (0: black or error, 1: white, 2: red, 3: blue, 4: yellow, 5: green)
// Arguments:
//   side which side�s color sensor to query (0: left, 1: right)

int color(int side)
{
  tSensors colorSensor;
  if (side == 0)
  {
    colorSensor = colorLeft;
  }
  else
  {
    colorSensor = colorRight;
  }

  if (!validColorSensor(colorSensor))
  {
    nxtDisplayCenteredBigTextLine(side*2, "Sensor Error");
    return 0;
  }

  int lastColor = SensorValue[colorSensor];
  wait1Msec(1);
  int currentColor = SensorValue[colorSensor];
  while (lastColor != currentColor) // For reliability, color will not move past this loop unless there are two consecutive consistent measurements at least a milisecond apart.
  {
    wait1Msec(1);
    lastColor = currentColor;
    currentColor = SensorValue[colorSensor];
  }
  switch (currentColor)
  {
    case BLACKCOLOR: nxtDisplayCenteredBigTextLine(side*2, "Black"); return 0;
    case REDCOLOR: nxtDisplayCenteredBigTextLine(side*2, "Red"); return 2;
    case GREENCOLOR: nxtDisplayCenteredBigTextLine(side*2, "Green"); return 5;
    case WHITECOLOR: nxtDisplayCenteredBigTextLine(side*2, "White"); return 1;
    case YELLOWCOLOR: nxtDisplayCenteredBigTextLine(side*2, "Yellow"); return 4;
    case BLUECOLOR: nxtDisplayCenteredBigTextLine(side*2, "Blue"); return 3;
    default: nxtDisplayCenteredBigTextLine(side*2, "Bad Type"); return 0;
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// MOVE
// Drives straight forward a specified distance at LOW speed
// Arguments:
//   distance: number of degrees each wheel will spin

void move(int distance)
{


  nMotorEncoder[motorC] = 0;  //clear the motor encoders
  nMotorEncoder[motorB] = 0;

  nMotorEncoderTarget[motorC] = distance; //set the target stopping position
  nMotorEncoderTarget[motorB] = distance;

  motor[motorC] = LOW; //turn both motors on
  motor[motorB] = LOW;

  while (nMotorRunState[motorC] != runStateIdle && nMotorRunState[motorB] != runStateIdle) //while the encoder wheels are turning
  {
    // This loop waits for motors B + C to come to an idle position.
  }

  motor[motorC] = 0; //turn both motors off
  motor[motorB] = 0;

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// PIVOT
// Spins 90 degrees about a vertical axis centered between the wheels
// Arguments:
//  direction: which direction to spin (0: left, 1: right

void pivot(int direction)
{
  tMotor master;
  tSensors side;
  bool Black;
  if (direction == 0)
  {
    master = motorB;
    nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
    nMotorEncoder[motorB] = 0;  //clear the motor encoders
    //nMotorEncoderTarget[motorB] = LEFTTURN; //set the target stopping position
    side = colorLeft;
  }
  else
  {
    master = motorC;
    nSyncedMotors = synchCB; //motor C is the master, motor B is the slave
    nMotorEncoder[motorC] = 0;  //clear the motor encoders
    //nMotorEncoderTarget[motorC] = RIGHTTURN; //set the target stopping position
    side = colorRight;
  }
  nSyncedTurnRatio = -100; //motors move in opposite directions of each other

  motor[master] = LOW; //turn both motors on

  while (color(direction) == 0)
  {
    // Loop until color sensor doesn't see black
  }

  /*
  do {
    switch(SensorValue[side])
    {
      case BLACKCOLOR: Black = true; break;
      default: Black = false;
    }
  } while(Black == true);
  */

  /*while (nMotorRunState[master] != runStateIdle) //while the encoder wheels are turning
  {
    // This loop waits for motors B + C to come to an idle position.
  }*/

  motor[master] = 0; //turn both motors off

  nSyncedMotors = synchNone; //unsync the motors so they are free to move independantly

  wait1Msec(1); //I don't know why this is necessary, but puting this here has caused some inexplicable errors to magically dissapear!
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// ALIGN
// Drives to and aligns with a line in front of it
// Arguments:
//   (None)

void align()
{
  // The following section moves back into the black, trying to better align with the edge in the process.
  if (color(0) != 0 && color(1) != 0) //if both sensors are off black
  {
    nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
    motor[motorB] = -LOW/2; //move forward
    while (color(0) != 0 && color(1) != 0) //until one sensor is on black
    {
      // wait
    }
    motor[motorB] = 0; //stop moving
    nSyncedMotors = synchNone; //unsync the motors so they are free to move independantly
  }
  if (color(0) != 0) //if left is off black
  {
    motor[motorC] = -LOW/4; //begin moving the left wheel backward

    while (color(0) != 0) //while still over black
    {
      // This loop waits for the sensor to find its way to a nonblack area
    }

    motor[motorC] = 0; //stop moving
  }
  if (color(1) != 0) //if right is off black
  {
    motor[motorB] = -LOW/4; //begin moving the right wheel backward

    while (color(1) != 0) //while still over black
    {
      // This loop waits for the sensor to find its way to a nonblack area
    }

    motor[motorB] = 0; //stop moving
  }


  // The following section moves forwards from black to the first nonblack edge in front of it, trying to align in the process.
  if (color(0) == 0 && color(1) == 0) //if both sensors are on black
  {
    nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
    motor[motorB] = LOW/8; //move forward
    while (color(0) == 0 && color(1) == 0) //until one sensor is off black
    {
      // wait
    }
    motor[motorB] = 0; //stop moving
    nSyncedMotors = synchNone; //unsync the motors so they are free to move independantly
  }
  if (color(0) == 0) //if left is in black
  {
    motor[motorC] = LOW/10; //begin moving the left wheel forward

    while (color(0) == 0) //while still over black
    {
      // This loop waits for the sensor to find its way to a nonblack area
    }

    motor[motorC] = 0; //stop moving
  }
  if (color(1) == 0) //if right is in black
  {
    motor[motorB] = LOW/10; //begin moving the right wheel forward

    while (color(1) == 0) //while still over black
    {
      // This loop waits for the sensor to find its way to a nonblack area
    }

    motor[motorB] = 0; //stop moving
  }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++

// ==================================================
