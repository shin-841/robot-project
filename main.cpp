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

// Servo, motor, and counts variables
#define SERVO_MIN_FORKLIFT 754
#define SERVO_MAX_FORKLIFT 2269
#define SERVO_MIN_RACK 736
#define SERVO_MAX_RACK 2444
#define MOTOR_PERCENT 50
#define COUNTS_INCHES 40.5
#define COUNTS_DEGREE 2.48

// Defines for pulsing the robot
#define PULSE_TIME .08
#define PULSE_POWER 25

// Orientation of QR Code
#define PLUS 0 
#define MINUS 1 

// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35

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
void move_forward(int percent, int counts, float seconds) //using encoders
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
        Sleep(seconds);
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

    rightWheel.SetPercent(-percent);
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
    leftWheel.SetPercent(-percent);

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
            move_forward(-power, 0, PULSE_TIME);
        }
        else if (RPS.X() < x_coordinate - 1)
        {
            // Pulse the motors for a short duration in the correct direction
            move_forward(power, 0, PULSE_TIME);
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
            move_forward(-power, 0, PULSE_TIME);
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

    while(RPS.Heading() >= 0 && (RPS.Heading() - heading > 2 || RPS.Heading() - heading < -2)) {
        int i = RPS.Heading() - heading;
        if(i < 0) 
        {
            i = 360 + i;
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

void neutralFace(void) {
    // Neutral Face
    LCD.Clear();
    LCD.SetFontColor(LIGHTGOLDENRODYELLOW);
    LCD.DrawLine(63, 79, 127, 79);
    LCD.DrawLine(191, 79, 255, 79);
    LCD.DrawLine(95, 159, 223, 159);
}

void owo(void) {
    // owo Face
    LCD.Clear();
    LCD.SetFontColor(PINK);
    LCD.WriteAt("owo", 63, 150);
}

void calibrateRPS(void) {
    //Declare variables
    float touch_x, touch_y;
    int n;
    char points[4] = {'A','B','C','D'};

    //Open SD file for writing
    FEHFile *fptr = SD.FOpen("RPS_Test.txt","w");

    //Wait for touchscreen to be pressed and released
    LCD.WriteLine("Press Screen to Start");
    while(!LCD.Touch(&touch_x, &touch_y));
    while(LCD.Touch(&touch_x, &touch_y));
    Sleep(1.0);

    //Clear screen
    LCD.Clear();

    //Write initial screen info
    LCD.WriteRC("X Position:", 2, 0);
    LCD.WriteRC("Y Position:", 3, 0);
    LCD.WriteRC("   Heading:", 4, 0);

    //Step through each path point to record position and heading
    for (n=0; n<=3; n++)
    {
        //Write point letter
        LCD.WriteRC("Touch to set point ", 0, 0);
        LCD.WriteRC(points[n], 0, 20);

        //Wait for touchscreen to be pressed and display RPS data
        while(!LCD.Touch(&touch_x, &touch_y))
        {
            LCD.WriteRC(RPS.X(), 2, 12); //update the x coordinate
            LCD.WriteRC(RPS.Y(), 3, 12); //update the y coordinate
            LCD.WriteRC(RPS.Heading(), 4, 12); //update the heading

            Sleep(1.0); //wait for a 10ms to avoid updating the screen too quickly
        }
        while(LCD.Touch(&touch_x, &touch_y));

        //Print RPS data for this path point to file
        SD.FPrintf(fptr, "%f %f %f\n", RPS.X(), RPS.Y(), RPS.Heading());
    }
    LCD.Clear();
    //Close SD file
    SD.FClose(fptr);
    return;
}

int main(void)
{
    // Initialize variables for important spots
    float X[4], Y[4], Heading[4];
    int degree, i;

    // Set servos
    servoForkLift.SetMin(SERVO_MIN_FORKLIFT);
    servoForkLift.SetMax(SERVO_MAX_FORKLIFT);
    servoRack.SetMin(SERVO_MIN_RACK);
    servoRack.SetMax(SERVO_MAX_RACK);
    servoForkLift.SetDegree(100);
    servoRack.SetDegree(180);

    RPS.InitializeTouchMenu();

    calibrateRPS();
    
    // Place values in variables
    FEHFile *ifptr = SD.FOpen("RPS_Test.txt","r");
    for (i = 0; i < 4; i++) {
        SD.FScanf(ifptr, "%f%f%f", &X[i], &Y[i], &Heading[i]);
    }
    SD.FClose(ifptr);

    // Save starting values
    double startingX = RPS.X();
    double startingY = RPS.Y();

    // Account for placement back to the center
    Sleep(5.0);

    // Psuedocode
    neutralFace();
    
    bool begin = true;
    // When light turns on, move forward 7 5/8 in
    while (begin) {
        if(CdS.Value() < 1.0 && CdS.Value() > 0) {
            move_forward(MOTOR_PERCENT, 6.5 * COUNTS_INCHES, 0);
            begin = false;
        }
    }

    // Turn right in order to get on to the ramp
    turn_right(MOTOR_PERCENT, 15 * COUNTS_DEGREE);
    check_heading(90);
    move_forward(MOTOR_PERCENT, 0, 2.5);

    // Move right to account for the movement left
    turn_right(MOTOR_PERCENT, 15 * COUNTS_DEGREE);
    move_forward(MOTOR_PERCENT, COUNTS_INCHES, 0);
    turn_left(MOTOR_PERCENT, 25 * COUNTS_DEGREE);  // 20
    check_heading(90);

    // Move towards the ticket
    check_y(Y[0], PLUS); // 44.3
    turn_left(MOTOR_PERCENT, 40 * COUNTS_DEGREE);
    check_heading(Heading[0]);  // 184
    move_forward(-MOTOR_PERCENT, 5.5 * COUNTS_INCHES, 0);
    check_x(X[0], MINUS);  // 32.8
    
    // Rack and Pinion system
    servoRack.SetDegree(0);
    Sleep(.5);
    move_forward(MOTOR_PERCENT, 3 * COUNTS_INCHES, 0);
    Sleep(.1);
    for (degree = 0; degree < 180; degree += 10) {
        servoRack.SetDegree(degree);
        Sleep(.1);
    }
    check_heading(180);
    
    // Move towards the sink
    move_forward(MOTOR_PERCENT, 3 * COUNTS_INCHES, 0);
    turn_left(MOTOR_PERCENT, 20 * COUNTS_DEGREE);
    move_forward(MOTOR_PERCENT, 0, .75);
    
    // Move servo down
    for (degree = 100; degree <= 160; degree += 10) {
            servoForkLift.SetDegree(degree);
            Sleep(.05);
    }
    Sleep(.2);
    move_forward(-MOTOR_PERCENT, .5 * COUNTS_INCHES, 0);
    Sleep(.1);
    move_forward(MOTOR_PERCENT,  0, .5);
    Sleep(.2);

    // Moves towards ice cream lever
    move_forward(-MOTOR_PERCENT, COUNTS_INCHES, 0);
    check_x(X[1], MINUS); // 18.7
    turn_right(MOTOR_PERCENT, 40 * COUNTS_DEGREE); // changed from 30
    check_heading(Heading[1]);
    move_forward(MOTOR_PERCENT, 5 * COUNTS_INCHES, 0);
    check_y(Y[1], PLUS); // 52.699
    servoForkLift.SetDegree(70);

    // Check which ice cream lever to flip
    int iceCream = RPS.GetIceCream();
    if(iceCream == 0)
    {
        // Vanilla
        turn_left(MOTOR_PERCENT, 35 * COUNTS_DEGREE);
        check_heading(155);
        move_forward(MOTOR_PERCENT, 7.5 * COUNTS_INCHES, 0);
        servoForkLift.SetDegree(120);
        Sleep(.2);
        servoForkLift.SetDegree(70);
        Sleep(.2);
        move_forward(-MOTOR_PERCENT, 6 * COUNTS_INCHES, 0);
        turn_right(MOTOR_PERCENT, 80 * COUNTS_DEGREE);
    } 
    else if(iceCream == 1)
    {
        // Twist
        turn_left(MOTOR_PERCENT, 20 * COUNTS_DEGREE);
        check_heading(135);
        move_forward(MOTOR_PERCENT, 7.5 * COUNTS_INCHES, 0);
        servoForkLift.SetDegree(120);
        Sleep(.2);
        servoForkLift.SetDegree(70);
        Sleep(.2);
        move_forward(-MOTOR_PERCENT, 6 * COUNTS_INCHES, 0);
        turn_right(MOTOR_PERCENT, 80 * COUNTS_DEGREE);
    }
    else if(iceCream == 2)
    {
        // Chocolate
        turn_left(MOTOR_PERCENT, 10 * COUNTS_DEGREE);
        check_heading(123);
        move_forward(MOTOR_PERCENT, 7.5 * COUNTS_INCHES, 0);
        servoForkLift.SetDegree(120);
        Sleep(.2);
        servoForkLift.SetDegree(70);
        Sleep(.2);
        move_forward(-MOTOR_PERCENT, 6 * COUNTS_INCHES, 0);
        turn_right(MOTOR_PERCENT, 60 * COUNTS_DEGREE);
    }

    // Go towards hot plate
    check_heading(0);
    servoForkLift.SetDegree(150);
    move_forward(MOTOR_PERCENT, 2 * COUNTS_INCHES, 0);
    LCD.Clear();
    check_x(X[2], PLUS); // 24.2
    turn_left(MOTOR_PERCENT, 40 * COUNTS_DEGREE);
    check_heading(90);

    // Move up to hot plate and flip it
    move_forward(MOTOR_PERCENT, 0, 1);
    // servoForkLift.SetDegree(70);
    Sleep(.2);
    turn_right(MOTOR_PERCENT, 25 * COUNTS_DEGREE);
    Sleep(.2);
    turn_left(MOTOR_PERCENT, 20 * COUNTS_DEGREE);
    // servoForkLift.SetDegree(120);
    Sleep(.2);

    // Move back towards ice cream
    move_forward(-MOTOR_PERCENT, 4 * COUNTS_INCHES, 0);
    check_y(Y[1], PLUS); // 52.699
    turn_left(MOTOR_PERCENT, 40 * COUNTS_DEGREE);
    check_x(X[1], MINUS); // 18.7
    if(iceCream == 0)
    {
        // Vanilla
        turn_right(MOTOR_PERCENT, 10 * COUNTS_DEGREE);
        check_heading(150);
    } 
    else if(iceCream == 1)
    {
        // Twist
        turn_right(MOTOR_PERCENT, 20 * COUNTS_DEGREE);
        check_heading(135);
    }
    else if(iceCream == 2)
    {
        // Chocolate
        turn_right(MOTOR_PERCENT, 10 * COUNTS_DEGREE);
        check_heading(120);
    }
    move_forward(MOTOR_PERCENT, 5.5 * COUNTS_INCHES, 0);
    servoForkLift.SetDegree(70);
    Sleep(.2);
    servoForkLift.SetDegree(120);
    Sleep(.2);

    // Go down the ramp
    move_forward(-MOTOR_PERCENT, 4 * COUNTS_INCHES, 0);
    servoForkLift.SetDegree(40);
    turn_left(MOTOR_PERCENT, 55 * COUNTS_DEGREE);
    check_heading(270);
    move_forward(MOTOR_PERCENT, 19 * COUNTS_INCHES, 0);
    check_y(Y[3], MINUS); // 12.8

    // Turn towards the CdS light
    turn_right(MOTOR_PERCENT, 40 * COUNTS_DEGREE);
    check_heading(Heading[3]); // 184
    double jukebox;
    rightWheel.SetPercent(MOTOR_PERCENT);
    leftWheel.SetPercent(MOTOR_PERCENT);
    bool jukeboxValue = true;
    while (jukeboxValue) {
        if (CdS.Value() <= 1.0) {
            Sleep(.1);
            rightWheel.Stop();
            leftWheel.Stop();
            jukebox = CdS.Value();
            jukeboxValue = false;
        } else if (RPS.X() < 10.8) {
            check_x(X[3], MINUS); // 10.8
            jukeboxValue = false;
        }
        
    }

    LCD.Clear();
    
    if (jukebox < .35) {
        LCD.WriteAt("Red", 100, 100);
    }
    else {
        LCD.WriteAt("Blue", 100, 100);
    }

    // Hit jukebox button
    turn_left(MOTOR_PERCENT, 40 * COUNTS_DEGREE);
    check_heading(270);
    move_forward(30, 0, 3);

    // Go to final button
    move_forward(-MOTOR_PERCENT, 5 * COUNTS_INCHES, 0);
    turn_left(MOTOR_PERCENT, 40 * COUNTS_DEGREE);
    move_forward(MOTOR_PERCENT, 6 * COUNTS_INCHES, 0);
    turn_right(MOTOR_PERCENT, 20 * COUNTS_DEGREE);
    move_forward(MOTOR_PERCENT, 0, 3);
}
