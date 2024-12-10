import RPi.GPIO as GPIO
import time

SERVO_PIN = 12
BUTTON_PIN = 16

def init_servo():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(SERVO_PIN, GPIO.OUT)
    
    pwm = GPIO.PWM(SERVO_PIN, 50)
    pwm.start(0)
    return pwm
    
def control_servo(pwm, angle):
    if angle == 0:
        duty_cycle = 2.5
    elif angle == 180:
        duty_cycle = 12.5
    else:
        print('Invalid Angle')
        return
    pwm.ChangeDutyCycle(duty_cycle)
    time.sleep(1)
    pwm.ChangeDutyCycle(0)
    
def shutdown_servo(pwm):
    pwm.stop()
    GPIO.cleanup()
    
def init_button():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    
def wait_for_button():
    try:
        while True:
            if GPIO.input(BUTTON_PIN) == GPIO.LOW:
                print("Button Pressed")
                time.sleep(0.2)
    except KeyboardInterrupt:
        print("Exiting program")
    finally:
        GPIO.cleanup()
    
def main():
    # pwm = init_servo()
    # control_servo(pwm, 0)
    # control_servo(pwm, 180)
    init_button()
    wait_for_button()
    # shutdown_servo()
    

    
if __name__ == "__main__":
    main()
