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
        if (CdS.Value() <= 1.0) {
            Sleep(.1);
            rightWheel.Stop();
            leftWheel.Stop();
            jukebox = CdS.Value();
            jukeboxValue = false;
        }
        
    }
    LCD.Clear();
    
    if (jukebox < .35) {
        LCD.WriteAt("Red", 100, 100);
        turn_left(motor_percent, 50);
        move_forward(motor_percent, (2 * expected_counts + 10));
        move_forward((-1 * motor_percent), 5 * expected_counts);
        turn_right(motor_percent, 175);
        move_forward(motor_percent, 9 * expected_counts);
        move_forward((-1 * motor_percent), 9 * expected_counts);
    }
    else {
        LCD.WriteAt("Blue", 100, 100);
        move_forward((-1 * motor_percent), 10);
        turn_left(motor_percent, 70);
        move_forward(motor_percent, expected_counts + 8);
        turn_right(motor_percent, 10);
        move_forward((-1 * motor_percent), 5 * expected_counts);
        turn_right(motor_percent, 175);
        move_forward(motor_percent, 9 * expected_counts);
        move_forward((-1 * motor_percent), 9 * expected_counts);
    }
}
