from PIL import Image
import scipy.signal as sg
import sys
import numpy as np
import serial
import cameraAI_shippable
# import uart_shippable


def main():
    while True:
        button_key = wait_for_button()

        if button_key == "0": # Run AI on a capture
            capture = request_capture()
            if(decide(capture)):
                # Then the capture was accepted by the AI model...
                # TURN ON GREEN LIGHT AND SPIN SERVO TO 'UNLOCK'
            else:
                # MAKE SURE SERVO IS 'LOCKED' AND LIGHT IS RED
        if button_key == "1": # Retrain training image 1
            capture = request_capture()
            capture.save("training_1.png")
        if button_key == "2": # Retrain training image 2
            capture = request_capture()
            capture.save("training_2.png")





if __name__ == "__main__":
    main()