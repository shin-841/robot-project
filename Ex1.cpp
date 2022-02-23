#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHMotor.h>

AnalogInputPin CdS(FEHIO::P3_0);
FEHServo servo(FEHServo::Servo7);
DigitalInputPin rightBack(FEHIO::P0_1);
DigitalInputPin rightFront(FEHIO::P0_0);
DigitalInputPin leftBack(FEHIO::P2_7);
DigitalInputPin leftFront(FEHIO::P2_6);
FEHMotor leftWheel(FEHMotor::Motor0, 9.0);
FEHMotor rightWheel(FEHMotor::Motor1, 9.0);

int main(void)
{
    // Input is touching then releasing the screen
    float x, y;
    while(!LCD.Touch(&x,&y)) {};
    while(LCD.Touch(&x,&y)) {};
    
    while(rightFront.Value() || leftFront.Value()) {
        rightWheel.SetPercent(25);
        leftWheel.SetPercent(25);
    }

    rightWheel.Stop();
    leftWheel.Stop();
    
    Sleep(1.0);
    
    while(rightBack.Value() || leftBack.Value()) {
        rightWheel.SetPercent(-25);
        leftWheel.SetPercent(-10);
    }
    
    rightWheel.Stop();
    leftWheel.Stop();

    Sleep(1.0);

    while(rightFront.Value() || leftFront.Value()) {
        rightWheel.SetPercent(25);
        leftWheel.SetPercent(25);
    }

    rightWheel.Stop();
    leftWheel.Stop();

    Sleep(1.0);

    while(rightBack.Value() || leftBack.Value()) {
        leftWheel.SetPercent(-25);
        if (!leftBack.Value()) {
            rightWheel.SetPercent(-10);
        }
    }

    rightWheel.Stop();
    leftWheel.Stop();

    Sleep(1.0);

    while(rightFront.Value() || leftFront.Value()) {
        rightWheel.SetPercent(25);
        leftWheel.SetPercent(25);
    }

    rightWheel.Stop();
    leftWheel.Stop();

    Sleep(1.0);

    /*
    Part 2
    servo.SetMin(840);
    servo.SetMax(2454);

    while (CdS.Value()) {
        if(CdS.Value() > 0 && CdS.Value() < .6) {
            servo.SetDegree(0);
        }
        else {
            servo.SetDegree(180);
        }
        
    }
    */
	return 0;
}
