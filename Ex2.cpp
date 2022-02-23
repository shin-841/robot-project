/*
// Part II
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor0,9.0);
FEHMotor left_motor(FEHMotor::Motor1,9.0);

void move_forward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn_right(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    //hint: set right motor backwards, left motor forwards

    right_motor.SetPercent(-25);
    left_motor.SetPercent(25);

    //While the average of the left and right encoder is less than counts,
    //keep running motors

    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn_left(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    
    //Set both motors to desired percent
    right_motor.SetPercent(25);
    left_motor.SetPercent(-25);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors

    right_motor.Stop();

    left_motor.Stop();
}

int main(void)
{
    int motor_percent = 25; //Input power level here
    int expected_counts = 43; //Input theoretical counts here (200)

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Shaft Encoder Exploration Test");
    LCD.WriteLine("Touch the screen");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed

    //Move forward 14 in
    move_forward(motor_percent, 14 * expected_counts);
    Sleep(2.0);
    turn_left(motor_percent, 200);
    Sleep(2.0);
    move_forward(motor_percent, 10 * expected_counts);
    Sleep(2.0);
    turn_right(motor_percent, 200);
    Sleep(2.0);
    move_forward(motor_percent, 4 * expected_counts);

    Sleep(2.0); //Wait for counts to stabilize
    //Print out data
    LCD.Write("Theoretical Counts: ");
    LCD.WriteLine(expected_counts);
    LCD.Write("Motor Percent: ");
    LCD.WriteLine(motor_percent);
    LCD.Write("Actual LE Counts: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("Actual RE Counts: ");
    LCD.WriteLine(right_encoder.Counts());

    return 0;
}
*/

/*
// Part I
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHMotor.h>

FEHMotor leftWheel(FEHMotor::Motor0, 9.0);
FEHMotor rightWheel(FEHMotor::Motor1, 9.0);
AnalogInputPin right_opt(FEHIO::P2_0);
AnalogInputPin middle_opt(FEHIO::P2_1);
AnalogInputPin left_opt(FEHIO::P2_2);

enum LineStates {
    MIDDLE,
    RIGHT,
    LEFT
};

int main() {
    // Black line around 2 V, not black 0.64 V
    int state = MIDDLE; // Set the initial state
    while (true) { // I will follow this line forever!
        switch(state) {
            // If I am in the middle of the line...
            case MIDDLE:
                // Set motor powers for driving straight
                rightWheel.SetPercent(15);
                leftWheel.SetPercent(15);
                if ( right_opt.Value() > 1.5 ) {
                    state = RIGHT; // update a new state
                } 
                else if ( left_opt.Value() > 1.5 ) {
                    state = LEFT;
                }
                break;
                // If the right sensor is on the line...
            case RIGHT:
                // Set motor powers for right turn
                rightWheel.Stop();
                leftWheel.Stop();
                Sleep(.25);
                leftWheel.SetPercent(15);
                if( right_opt.Value() < 1 && left_opt.Value() < 1 && middle_opt.Value() > 1.5) {
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
                if( right_opt.Value() < 1 && left_opt.Value() < 1 && middle_opt.Value() > 1.5) {
                    state = MIDDLE;
                }
                break;
            default: // Error. Something is very wrong.
                break;
        }
        Sleep(.2);
    }
}
*/