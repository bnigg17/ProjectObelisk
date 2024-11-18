import serial
from PIL import Image
import numpy as np
import time

def display_rgb565_image(rgb888_data, width, height):
    # Convert the list of RGB888 tuples to a NumPy array and reshape to (height, width, 3)
    pixels = np.array(rgb888_data, dtype=np.uint8).reshape((height, width, 3))
    # Create an image from the RGB888 data
    img = Image.fromarray(pixels, 'RGB')
    img.show()
    img.save('output.png')

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
ser = serial.Serial('/dev/ttyUSB0', 19200, timeout=36)

height = 32
width = 320
num_bytes_to_read = width * height * 3

with open('raw_rgb565.txt', 'w') as f:
    # Allow x time for snapshot to occur. Primarily for user to have enough time
    time.sleep(5)
    while True:
        byte = ser.read(1).decode('utf-8', errors='ignore')
        if byte == 'S':
            break  # Start character detected, exit loop

    # Now read the specified number of bytes
    data = ser.read(num_bytes_to_read)
    
    #print(data)
    print(len(data))
    if(len(data) != width * height * 3):
        print('UART ERROR')
    else:
        image = clean(data, num_bytes_to_read // 3)
        print(len(image))
    
        # Write data to file 
        # Write image data to file with 320 pixels per line
        for row in range(height):
            row_string = ''
            for col in range(width):  # Process exactly 320 pixels per row
                pixel = image[row * width + col]  # Get the pixel
                r, g, b = pixel  # Unpack the RGB values
                row_string += f"({r},{g},{b}) "  # Add the formatted pixel to the row string
            
            f.write(row_string.strip() + '\n')  # Write the row to file, stripping any trailing space

        # Example: Replace with actual RGB565 data and dimensions
        display_rgb565_image(image, width, height)
    
