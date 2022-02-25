#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHMotor.h>

AnalogInputPin CdS(FEHIO::P3_0);
FEHMotor leftWheel(FEHMotor::Motor0, 9.0);
FEHMotor rightWheel(FEHMotor::Motor1, 9.0); 
AnalogInputPin right_opt(FEHIO::P2_0);
AnalogInputPin middle_opt(FEHIO::P2_1);
AnalogInputPin left_opt(FEHIO::P2_2);
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P3_7);

enum LineStates {
    MIDDLE,
    RIGHT,
    LEFT
};

void move_forward(int percent, int counts) //using encoders
{
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

void turn_right(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    //hint: set right motor backwards, left motor forwards

    rightWheel.SetPercent(-25);
    leftWheel.SetPercent(25);

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
    rightWheel.SetPercent(25);
    leftWheel.SetPercent(-25);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors

    rightWheel.Stop();

    leftWheel.Stop();
}

int main(void)
{
    // Psuedocode
    int motor_percent = 25; //Input power level here
    int expected_counts = 43; //Input theoretical counts here (200)
    double jukebox;
    bool begin = true;
    // When light turns on, move forward 7 5/8 in
    while (begin) {
        if(CdS.Value() < 1.0 && CdS.Value() > 0) {
            move_forward(motor_percent, 5 * expected_counts);
            begin = false;
        }
    }
    
    // Rotate left 45 degrees
    turn_left(motor_percent, 70);
    // Move forward 12 in
    rightWheel.SetPercent(25);
    leftWheel.SetPercent(25);
    bool jukeboxValue = true;
    while (jukeboxValue) {
        if (CdS.Value() <= .8) {
            Sleep(.1);
            rightWheel.Stop();
            leftWheel.Stop();
            jukebox = CdS.Value();
            jukeboxValue = false;
        }
        
    }
    LCD.Clear();
    LCD.WriteAt(jukebox, 100, 100);
    if (jukebox < .35) {
        turn_left(motor_percent, 50);
        move_forward(motor_percent, 2 * expected_counts);
    }
    else {
        turn_left(motor_percent, 72);
        move_forward(motor_percent, expected_counts);
    }
    
    leftWheel.SetPercent(-25);
    rightWheel.SetPercent(-25);
    Sleep(2.0);
    leftWheel.Stop();
    rightWheel.Stop();
    // Based on the color of the light, turn left or right by 15 degrees
    // When black line is found, follow the line and press the button
    // Move back and turn 90 degrees left
    // Move forward until behind left and turn left 90 degrees
    // Move up until on the top
    // Rotate 180 degrees and move down the ramp
}
