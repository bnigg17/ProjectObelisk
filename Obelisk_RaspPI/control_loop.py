from PIL import Image
import scipy.signal as sg
import sys
import numpy as np
import serial
from cameraAI import *
from gpio import *
from uart import *

SERVO_PIN = 12
BUTTON_PIN_1 = 16
BUTTON_PIN_2 = 23
BUTTON_PIN_3 = 24
GREEN_LED_PIN = 17
RED_LED_PIN = 27

def main():
    shutdown()
    # Init Devices
    pwm = init_servo()
    control_servo(pwm, 0)
    init_button()
    init_LED()
    set_LED(GREEN_LED_PIN, 1)
    time.sleep(1)
    set_LED(GREEN_LED_PIN, 0)
    set_LED(RED_LED_PIN, 1)
    time.sleep(1)
    set_LED(RED_LED_PIN, 0)
    
    # Loop
    while True:
        try:
            button_key = wait_for_button()
            print("button pressed: ", button_key)
            if button_key == 0: # Run AI on a capture
                set_LED(GREEN_LED_PIN, 1)
                set_LED(RED_LED_PIN, 1)
                capture = request_capture()
                if(decide(capture)):
                    # Then the capture was accepted by the AI model...
                    # TURN ON GREEN LIGHT AND SPIN SERVO TO 'UNLOCK'
                    set_LED(GREEN_LED_PIN, 1)
                    set_LED(RED_LED_PIN, 0)
                    control_servo(pwm, 180)
                    time.sleep(5)
                    control_servo(pwm, 0)
                    set_LED(GREEN_LED_PIN, 0)
                    set_LED(RED_LED_PIN, 1)
                else:
                    # MAKE SURE SERVO IS 'LOCKED' AND LIGHT IS RED
                    set_LED(RED_LED_PIN, 1)
                    set_LED(GREEN_LED_PIN, 0)
                    time.sleep(.5)
                    set_LED(RED_LED_PIN, 0)
                    time.sleep(.5)
                    set_LED(RED_LED_PIN, 1)
                    time.sleep(.5)
                    set_LED(RED_LED_PIN, 0)
                    time.sleep(.5)
                    set_LED(RED_LED_PIN, 1)
                    control_servo(pwm, 0)
            if button_key == 1: # Retrain training image 1
                set_LED(GREEN_LED_PIN, 1)
                set_LED(RED_LED_PIN, 1)
                capture = request_capture()
                capture.save("training_1.png")
                set_LED(GREEN_LED_PIN, 0)
                set_LED(RED_LED_PIN, 1)
            if button_key == 2: # Retrain training image 2
                set_LED(GREEN_LED_PIN, 1)
                set_LED(RED_LED_PIN, 1)
                capture = request_capture()
                capture.save("training_2.png")
                set_LED(GREEN_LED_PIN, 0)
                set_LED(RED_LED_PIN, 1)
        except KeyboardInterrupt:
            print('Keybboard Int')
            break
    shutdown_servo()
    shutdown()

if __name__ == "__main__":
    main()
