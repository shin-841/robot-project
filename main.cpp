// Main code for robot

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
AnalogInputPin right_opt(FEHIO::P2_0);
AnalogInputPin middle_opt(FEHIO::P2_1);
AnalogInputPin left_opt(FEHIO::P2_2);
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P3_7);
FEHServo servoForkLift(FEHServo::Servo7);
FEHServo servoRack(FEHServo::Servo0);
#define SERVO_MIN_FORKLIFT 754
#define SERVO_MAX_FORKLIFT 2269
#define SERVO_MIN_RACK 736
#define SERVO_MAX_RACK 2444
#define MOTOR_PERCENT 40
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

// use 0 for seconds when using encoder
void move_forward(int percent, int counts, int seconds) //using encoders
{
    if (seconds == 0) {
        //Reset encoder counts
        right_encoder.ResetCounts();
        left_encoder.ResetCounts();

        //Set both motors to desired percent
        rightWheel.SetPercent(percent);
        leftWheel.SetPercent(percent);

        //While the average of the left and right encoder is less than counts,
        //keep running motors
        while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

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
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    //hint: set right motor backwards, left motor forwards

    rightWheel.SetPercent(-1 * percent);
    leftWheel.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors

    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    rightWheel.Stop();
    leftWheel.Stop();
}

void turn_left(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    
    //Set both motors to desired percent
    rightWheel.SetPercent(percent);
    leftWheel.SetPercent(-1 * percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    rightWheel.Stop();
    leftWheel.Stop();
}

void move_while_turning(int rightPercent, int leftPercent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    
    //Set both motors to desired percent
    rightWheel.SetPercent(rightPercent);
    leftWheel.SetPercent(leftPercent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

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

    /*
        SUGGESTED ALGORITHM:
        1. Check the current orientation of the QR code and the desired orientation of the QR code
        2. Check if the robot is within the desired threshold for the heading based on the orientation
        3. Pulse in the correct direction based on the orientation
    */
    while(RPS.Heading() >= 0 && (heading - RPS.Heading() > 4 || heading - RPS.Heading() < -4)) {
        int i = heading - RPS.Heading();
        if(i < 0) 
        {
            i = heading - i;
        }
        if(i >= 180)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(-PULSE_POWER, PULSE_TIME);
        }
        else if(i < 180)
        {
            // Pulse the motors for a short duration in the correct direction
           pulse_counterclockwise(PULSE_POWER, PULSE_TIME);
        }
        
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/*
For reference
void check_heading(float heading)
{
    //You will need to fill out this one yourself and take into account
    //checking for proper RPS data and the edge conditions
    //(when you want the robot to go to 0 degrees or close to 0 degrees)

    while(RPS.Heading() >= 0 && (RPS.Heading() < heading - 4 || RPS.Heading() > heading + 4))
    {
        if((RPS.Heading() > heading + 4))
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(-PULSE_POWER, PULSE_TIME);
        }
        else if(RPS.Heading() < heading - 4)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(PULSE_POWER, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}
*/


int main(void)
{
    //servoRack.TouchCalibrate(); //   calibrate servo 
    /*
    servoRack.SetMin(SERVO_MIN_RACK);
    servoRack.SetMax(SERVO_MAX_RACK);
    //Set arm servo to 0 degrees
    while(true) {
        LCD.Clear();
        Sleep(2.0);
        for (int degree = 180; degree >= 0; degree -= 10) {
            servoRack.SetDegree(degree);
            LCD.Write(degree);
            Sleep(.5);
        }
        for (int degree = 0; degree <= 180; degree += 10) {
            servoRack.SetDegree(degree);
            LCD.Write(degree);
            Sleep(.5);
        }
    }
    */
    // RPS.InitializeTouchMenu();

    LCD.Clear();
    
    // Face
    LCD.SetFontColor(LIGHTGOLDENRODYELLOW);
    LCD.DrawLine(63, 79, 127, 79);
    LCD.DrawLine(191, 79, 255, 79);
    LCD.DrawLine(95, 159, 223, 159);
    
    // Calibrate Servo
    servoForkLift.SetMin(SERVO_MIN_FORKLIFT);
    servoForkLift.SetMax(SERVO_MAX_FORKLIFT);
    servoForkLift.SetDegree(100);
    
    // Start robot
    bool begin = true;
    while (begin) {
        if(CdS.Value() < 1.0 && CdS.Value() > 0) {
            move_forward(MOTOR_PERCENT, 7 * COUNTS_INCHES + 10, 0);
            begin = false;
        }
    }

}
