import serial
from PIL import Image
import numpy as np
import time
import os

def display_rgb565_image(rgb888_data, width, height, filename):
    """Convert the list of RGB888 tuples to an image and save it."""
    pixels = np.array(rgb888_data, dtype=np.uint8).reshape((height, width, 3))
    img = Image.fromarray(pixels, 'RGB')
    img.show()
    img.save(filename)

def clean(data, num_pixels):
    rgb_888 = []
    for i in range(0, num_pixels):
        red = int(data[3*i])
        green = int(data[3*i+1])
        blue = int(data[3*i+2])
        rgb = [red, green, blue]
        rgb_888.append(rgb)
    test_rgb888(rgb_888)
    return rgb_888

def test_rgb888(image):
    for pixel in image:
        if pixel[0] > 255 or pixel[0] < 0:
            print("ERROR RED")
        if pixel[1] > 255 or pixel[1] < 0:
            print("ERROR GREEN")
        if pixel[2] > 255 or pixel[2] < 0:
            print("ERROR BLUE")

# Replace 'COMx' with your port and set your baud rate
ser = serial.Serial('/dev/ttyUSB0', 19200, timeout=40)
# Ensure the directory for results exists
output_directory = "test_results"
os.makedirs(output_directory, exist_ok=True)

height = 30
width = 320
num_bytes_to_read = width * height * 3


height = 30
width = 320
num_bytes_to_read = width * height * 3

while True:
    # Prompt user for test input
    test_number = input("Enter the test number to run (or 'exit' to quit): ")
    if test_number.lower() == 'exit':
        print("Exiting program.")
        break

    if not test_number.isdigit():
        print("Please enter a valid number.")
        continue

    # Generate filenames for the image and text output in the test_results directory
    image_filename = os.path.join(output_directory, f"image_test_{test_number}.png")
    text_filename = os.path.join(output_directory, f"raw_rgb565_test_{test_number}.txt")

    # Send the test number to the MCU
    ser.write(test_number.encode() + b'\n')  # Send test number followed by newline

    # Allow time for snapshot to occur
    time.sleep(1)

    print("Waiting for start character ('S') from MCU...")
    while True:
        byte = ser.read(1).decode('utf-8', errors='ignore')
        if byte == 'S':
            break  # Start character detected, exit loop

    # Now read the specified number of bytes
    data = ser.read(num_bytes_to_read)

    print(f"Received {len(data)} bytes, test number: {test_number}.")
    if len(data) != width * height * 3:
        print('UART ERROR')
    else:
        # Process the received data
        image = clean(data, num_bytes_to_read // 3)
        print(f"Cleaned image size: {len(image)}, test number: {test_number}")

        # Write image data to text file
        with open(text_filename, 'w') as f:
            for row in range(height):
                row_string = ''
                for col in range(width):  # Process exactly 320 pixels per row
                    pixel = image[row * width + col]  # Get the pixel
                    r, g, b = pixel  # Unpack the RGB values
                    row_string += f"({r},{g},{b}) "  # Add the formatted pixel to the row string
                f.write(row_string.strip() + '\n')  # Write the row to file, stripping any trailing space

        print(f"Saved RGB data to {text_filename}.")

        # Display and save the image
        display_rgb565_image(image, width, height, image_filename)
        print(f"Saved image as {image_filename}.")
    
