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
#define MOTOR_PERCENT 40
#define COUNTS_INCHES 40.5
#define COUNTS_DEGREE 2.48

enum LineStates {
    MIDDLE,
    RIGHT,
    LEFT
};

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
    servoForkLift.SetMin(SERVO_MIN_FORKLIFT);
    servoForkLift.SetMax(SERVO_MAX_FORKLIFT);
    servoForkLift.SetDegree(100);
    bool begin = true;
    // When light turns on, move forward 7 5/8 in
    while (begin) {
        if(CdS.Value() < 1.0 && CdS.Value() > 0) {
            move_forward(MOTOR_PERCENT, 7 * COUNTS_INCHES + 10, 0);
            begin = false;
        }
    }

    // turn right in order to get on to the ramp
    turn_right(MOTOR_PERCENT, 15 * COUNTS_DEGREE);
    move_forward(MOTOR_PERCENT, 16 * COUNTS_INCHES, 0);
    turn_right(MOTOR_PERCENT, 15 * COUNTS_DEGREE);
    // Move forward towards the hot plate
    move_forward(MOTOR_PERCENT, 2 * COUNTS_INCHES, 0);
    turn_left(MOTOR_PERCENT, 30 * COUNTS_DEGREE);
    // Move forward 2 inches to flip hot plate
    servoForkLift.SetDegree(150);
    move_forward(25, 4 * COUNTS_INCHES, 2);
    Sleep(1.0);
    servoForkLift.SetDegree(80);
    Sleep(.5);
    move_forward(MOTOR_PERCENT, 20, 0);
    Sleep(.5);
    turn_left(MOTOR_PERCENT, 25 * COUNTS_DEGREE);
    servoForkLift.SetDegree(150);
    move_forward(-1 * MOTOR_PERCENT, 2 * COUNTS_INCHES, 0);
    turn_left(MOTOR_PERCENT, 25 * COUNTS_DEGREE);
    servoForkLift.SetDegree(70);
    move_forward(MOTOR_PERCENT, 5 * COUNTS_INCHES, 0);
    turn_right(MOTOR_PERCENT, 5 * COUNTS_DEGREE);
    move_forward(25, 2 * COUNTS_INCHES, 0);
    servoForkLift.SetDegree(100);

    // turn left and move towards a ice cream lever
    // flip ice cream lever
    
}
