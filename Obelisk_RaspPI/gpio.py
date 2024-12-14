import RPi.GPIO as GPIO
import time

SERVO_PIN = 12
BUTTON_PIN_1 = 16
BUTTON_PIN_2 = 23
BUTTON_PIN_3 = 24
GREEN_LED_PIN = 17
RED_LED_PIN = 27
BLUE_LED_PIN = 22


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
    GPIO.setup(BUTTON_PIN_1, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(BUTTON_PIN_2, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(BUTTON_PIN_3, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    
def wait_for_button():
    try:
        while True:
            if GPIO.input(BUTTON_PIN_1) == GPIO.LOW:
                time.sleep(0.2)
                return 0
            if GPIO.input(BUTTON_PIN_2) == GPIO.LOW:
                time.sleep(0.2)
                return 1
            if GPIO.input(BUTTON_PIN_3) == GPIO.LOW:
                time.sleep(0.2)
                return 2
    except KeyboardInterrupt:
        print("Exiting program")

def init_LED():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(GREEN_LED_PIN, GPIO.OUT)
    GPIO.setup(RED_LED_PIN, GPIO.OUT)
    GPIO.setup(BLUE_LED_PIN, GPIO.OUT)
    
def set_LED(led_pin, en):
    if en:
        GPIO.output(led_pin, GPIO.LOW)
    else:
        GPIO.output(led_pin, GPIO.HIGH)
def shutdown():
    GPIO.cleanup()
    
# def main():
    # # pwm = init_servo()
    # # control_servo(pwm, 0)
    # # control_servo(pwm, 180)
    # init_button()
    # init_LED()
    # set_LED(GREEN_LED_PIN, 1)
    # time.sleep(1)
    # set_LED(GREEN_LED_PIN, 0)
    # set_LED(RED_LED_PIN, 1)
    # time.sleep(1)
    # set_LED(RED_LED_PIN, 0)
    # for i in range(3):
         # ret = wait_for_button()
         # print(f'Button {ret} pressed')
    # # shutdown_servo()
    # shutdown()
    

    
# if __name__ == "__main__":
    # main()
