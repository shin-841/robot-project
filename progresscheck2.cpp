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
FEHServo servoForkLift(FEHServo::Servo7);
FEHServo servoRack(FEHServo::Servo0);
#define SERVO_MIN_FORKLIFT 754
#define SERVO_MAX_FORKLIFT 2269
#define SERVO_MIN_RACK 736
#define SERVO_MAX_RACK 2444

enum LineStates {
    MIDDLE,
    RIGHT,
    LEFT
};

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

    // Psuedocode
    LCD.Clear();
    LCD.SetFontColor(LIGHTGOLDENRODYELLOW);
    LCD.DrawLine(63, 79, 127, 79);
    LCD.DrawLine(191, 79, 255, 79);
    LCD.DrawLine(95, 159, 223, 159);
    int motor_percent = 25; //Input power level here
    int expected_counts = 43; //Input theoretical counts here (200)
    servoForkLift.SetMin(SERVO_MIN_FORKLIFT);
    servoForkLift.SetMax(SERVO_MAX_FORKLIFT);
    servoForkLift.SetDegree(100);
    bool begin = true;
    // When light turns on, move forward 7 5/8 in
    while (begin) {
        if(CdS.Value() < 1.0 && CdS.Value() > 0) {
            move_forward(motor_percent, 7 * expected_counts + 10, 0);
            begin = false;
        }
    }
    
    // Rotate right 45 degrees
    turn_right(motor_percent, 45);
    move_forward(motor_percent, 15 * expected_counts, 0);
    turn_left(motor_percent, 125);
    move_forward(motor_percent, 4 * expected_counts + 5, 0);
    turn_left(motor_percent, 150);
    move_forward(-1 * motor_percent, 2 * expected_counts + 5, 0);
    turn_right(motor_percent, 35);
    move_forward(motor_percent, 3 * expected_counts + 25, 0);
    for (int degree = 100; degree <= 160; degree += 10) {
            servoForkLift.SetDegree(degree);
            Sleep(.05);
    }
    for (int i = 0; i < 5; i++) {
            servoForkLift.SetDegree(155);
            servoForkLift.SetDegree(160);
    }
    Sleep(1.0);
    move_forward(-1 * motor_percent, 4 * expected_counts, 0);
    turn_right(motor_percent, 55);
    move_forward(-1 * motor_percent, 3 * expected_counts, 0);
    turn_right(motor_percent, 90);
    move_forward(-1 * motor_percent, 9 * expected_counts, 0);
    turn_left(motor_percent, 155);
    move_forward(motor_percent, 10 * expected_counts, 4);
    move_forward(-1 * motor_percent, 15, 0);
    turn_right(motor_percent, 80);
    turn_left(motor_percent, 80);
    move_forward(-1 * motor_percent, 11 * expected_counts, 0);
}
