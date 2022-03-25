#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHMotor.h>
#include <FEHSD.h>
#include <FEHRPS.h>

AnalogInputPin CdS(FEHIO::P3_0);
FEHMotor leftWheel(FEHMotor::Motor0, 9.0);
FEHMotor rightWheel(FEHMotor::Motor1, 9.0); 
AnalogInputPin rightOpt(FEHIO::P1_5);
AnalogInputPin middleOpt(FEHIO::P1_6);
AnalogInputPin leftOpt(FEHIO::P1_7);
DigitalEncoder rightEncoder(FEHIO::P0_0);
DigitalEncoder leftEncoder(FEHIO::P3_7);
FEHServo servoForkLift(FEHServo::Servo7);
FEHServo servoRack(FEHServo::Servo0);
#define SERVO_MIN_FORKLIFT 754
#define SERVO_MAX_FORKLIFT 2269
#define SERVO_MIN_RACK 736
#define SERVO_MAX_RACK 2444
#define MOTOR_PERCENT 50
#define COUNTS_INCHES 40.5
#define COUNTS_DEGREE 2.48

// Defines for pulsing the robot
#define PULSE_TIME .1
#define PULSE_POWER 25

// Orientation of QR Code
#define PLUS 0 
#define MINUS 1 

// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35

enum LineStates {
    MIDDLE,
    RIGHT,
    LEFT
};

/*
 * Pulse counterclockwise a short distance using time
 */
void pulse_counterclockwise(int percent, float seconds) 
{
    // Set both motors to desired percent
    rightWheel.SetPercent(percent);
    leftWheel.SetPercent(-percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    rightWheel.Stop();
    leftWheel.Stop();
}

void line_following(int RPSValue) {
    // Black line around 2 V, not black 0.64 V
    int state = MIDDLE; // Set the initial state
    while (true) { // I will follow this line forever!
        switch(state) {
            // If I am in the middle of the line...
            case MIDDLE:
                // Set motor powers for driving straight
                rightWheel.SetPercent(30);
                leftWheel.SetPercent(30);
                if ( rightOpt.Value() > 1.5 ) {
                    state = RIGHT; // update a new state
                } 
                else if ( leftOpt.Value() > 1.5 ) {
                    state = LEFT;
                }
                break;
                // If the right sensor is on the line...
            case RIGHT:
                // Set motor powers for right turn
                rightWheel.Stop();
                leftWheel.Stop();
                Sleep(.25);
                leftWheel.SetPercent(30);
                if( rightOpt.Value() < 1 && leftOpt.Value() < 1 && middleOpt.Value() > 1.5) {
                    state = MIDDLE;
                }
                break;
            // If the left sensor is on the line...
            case LEFT:
                // Set motor powers for right turn
                rightWheel.Stop();
                leftWheel.Stop();
                Sleep(.25);
                rightWheel.SetPercent(15);
                if( rightOpt.Value() < 1 && leftOpt.Value() < 1 && middleOpt.Value() > 1.5) {
                    state = MIDDLE;
                }
                break;
            default: // Error. Something is very wrong.
                break;
        }
        Sleep(.2);
    }
}

// use 0 for seconds when using encoder
void move_forward(int percent, int counts, int seconds) //using encoders
{
    if (seconds == 0) {
        //Reset encoder counts
        rightEncoder.ResetCounts();
        leftEncoder.ResetCounts();

        //Set both motors to desired percent
        rightWheel.SetPercent(percent);
        leftWheel.SetPercent(percent);

        //While the average of the left and right encoder is less than counts,
        //keep running motors
        while((leftEncoder.Counts() + rightEncoder.Counts()) / 2. < counts);

        //Turn off motors
        rightWheel.Stop();
        leftWheel.Stop();
    }
    else {
        rightWheel.SetPercent(percent);
        leftWheel.SetPercent(percent);
        Sleep(seconds * 1000);
        rightWheel.Stop();
        leftWheel.Stop();
    }
    
}

void turn_right(int percent, int counts) //using encoders
{
    //Reset encoder counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();
    //Set both motors to desired percent
    //hint: set right motor backwards, left motor forwards

    rightWheel.SetPercent(-1 * percent);
    leftWheel.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors

    while((leftEncoder.Counts() + rightEncoder.Counts()) / 2. < counts);

    //Turn off motors
    rightWheel.Stop();
    leftWheel.Stop();
}

void turn_left(int percent, int counts) //using encoders
{
    //Reset encoder counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();
    
    //Set both motors to desired percent
    rightWheel.SetPercent(percent);
    leftWheel.SetPercent(-1 * percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((leftEncoder.Counts() + rightEncoder.Counts()) / 2. < counts);

    //Turn off motors
    rightWheel.Stop();
    leftWheel.Stop();
}

void move_while_turning(int rightPercent, int leftPercent, int counts) //using encoders
{
    //Reset encoder counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();
    
    //Set both motors to desired percent
    rightWheel.SetPercent(rightPercent);
    leftWheel.SetPercent(leftPercent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((leftEncoder.Counts() + rightEncoder.Counts()) / 2. < counts);

    //Turn off motors
    rightWheel.Stop();
    leftWheel.Stop();
}

/* 
 * Use RPS to move to the desired x_coordinate based on the orientation of the QR code
 */
void check_x(float x_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code 
    int power = PULSE_POWER;
    if(orientation == MINUS){
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while(RPS.X() >= 0 && (RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1))
    {
        if (RPS.X() > x_coordinate + 1)
        {
            // Pulse the motors for a short duration in the correct direction
            move_forward(-power, 0,PULSE_TIME);
        }
        else if (RPS.X() < x_coordinate - 1)
        {
            // Pulse the motors for a short duration in the correct direction
            move_forward(power, 0,PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/* 
 * Use RPS to move to the desired y_coordinate based on the orientation of the QR code
 */
void check_y(float y_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    if(orientation == MINUS){
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while(RPS.Y() >= 0 && (RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1))
    {
        if(RPS.Y() > y_coordinate + 1)
        {
            // Pulse the motors for a short duration in the correct direction
            move_forward(-power, 0,PULSE_TIME);
        }
        else if(RPS.Y() < y_coordinate - 1)
        {
            // Pulse the motors for a short duration in the correct direction
           move_forward(power, 0, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/* 
 * Use RPS to move to the desired heading
 */
void check_heading(float heading)
{
    //You will need to fill out this one yourself and take into account
    //checking for proper RPS data and the edge conditions
    //(when you want the robot to go to 0 degrees or close to 0 degrees)

    while(RPS.Heading() >= 0 && (heading - RPS.Heading() > 10 || heading - RPS.Heading() < -10)) {
        int i = heading - RPS.Heading();
        if(i < 0) 
        {
            i = heading - i;
        }
        if(i < 180)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(-PULSE_POWER, PULSE_TIME);
        }
        else if(i >= 180)
        {
            // Pulse the motors for a short duration in the correct direction
           pulse_counterclockwise(PULSE_POWER, PULSE_TIME);
        }
        
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

int main(void)
{
    RPS.InitializeTouchMenu();

    // Psuedocode
    LCD.Clear();
    LCD.SetFontColor(LIGHTGOLDENRODYELLOW);
    
    // Face
    LCD.DrawLine(63, 79, 127, 79);
    LCD.DrawLine(191, 79, 255, 79);
    LCD.DrawLine(95, 159, 223, 159);
    
    // Set servos
    servoForkLift.SetMin(SERVO_MIN_FORKLIFT);
    servoForkLift.SetMax(SERVO_MAX_FORKLIFT);
    servoRack.SetMin(SERVO_MIN_RACK);
    servoRack.SetMax(SERVO_MAX_RACK);
    servoForkLift.SetDegree(100);
    servoRack.SetDegree(180);

    bool begin = true;
    // When light turns on, move forward 7 5/8 in
    while (begin) {
        if(CdS.Value() < 1.0 && CdS.Value() > 0) {
            move_forward(MOTOR_PERCENT, 6.5 * COUNTS_INCHES, 0);
            begin = false;
        }
    }

    // Turn right in order to get on to the ramp
    turn_right(MOTOR_PERCENT, 13 * COUNTS_DEGREE);
    move_forward(MOTOR_PERCENT, 12 * COUNTS_INCHES, 0);

    // Move towards the ticket
    check_y(42.8, PLUS);
    turn_left(MOTOR_PERCENT, 90 * COUNTS_DEGREE);
    move_forward(-1 * MOTOR_PERCENT, 5 * COUNTS_INCHES, 0);
    check_heading(180);
    check_x(32.599, PLUS);

    // Rack and Pinion system
    servoRack.SetDegree(0);
    Sleep(.5);
    move_forward(MOTOR_PERCENT, 4 * COUNTS_INCHES, 0);
    Sleep(.1);
    servoRack.SetDegree(180);
    /*
    // Move towards the sink
    turn_left(MOTOR_PERCENT, 50 * COUNTS_DEGREE);
    move_forward(MOTOR_PERCENT, 4 * COUNTS_INCHES, 0);
    turn_left(MOTOR_PERCENT, 60 * COUNTS_DEGREE);
    move_forward(-1 * MOTOR_PERCENT, 2 * COUNTS_INCHES, 0);
    turn_right(MOTOR_PERCENT, 14 * COUNTS_DEGREE);
    move_forward(MOTOR_PERCENT, 0, 2);
    
    // Move servo down
    for (int degree = 100; degree <= 160; degree += 10) {
            servoForkLift.SetDegree(degree);
            Sleep(.05);
    }
    for (int i = 0; i < 5; i++) {
            servoForkLift.SetDegree(155);
            servoForkLift.SetDegree(160);
    }
    
    // turn towards the ice cream machine
    turn_left(MOTOR_PERCENT, 27 * COUNTS_DEGREE);

    /*
    // Check which ice cream lever to flip
    if(RPS.GetIceCream() == 0)
    {
        // Flip vanilla lever
    } 
    else if(RPS.GetIceCream() == 1)
    {
        // Flip twist lever
    }
    else if(RPS.GetIceCream() == 2)
    {
       // Flip chocolate lever
    }
    */
    /*
    servoForkLift.SetDegree(70);
    move_forward(25, 0, 3);
    // Move servo to flip lever
    move_forward(-25, 4 * COUNTS_INCHES, 0);
    Sleep(.5);
    servoForkLift.SetDegree(120);
    Sleep(7.0);
    move_forward(25, 2 * COUNTS_INCHES, 0);
    servoForkLift.SetDegree(70);
    Sleep(.5);
    servoForkLift.SetDegree(120);

    // Move backwards and towards the button
    move_forward(-1 * MOTOR_PERCENT, 5 * COUNTS_INCHES, 0);
    turn_right(MOTOR_PERCENT, 30 * COUNTS_DEGREE);
    move_forward(-1 * MOTOR_PERCENT + 15, 18 * COUNTS_INCHES, 0);
    turn_left(MOTOR_PERCENT, 13 * COUNTS_DEGREE);
    move_forward(-1 * MOTOR_PERCENT, 0, 3);
    */
}
