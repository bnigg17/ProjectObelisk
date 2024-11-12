import serial
from PIL import Image
import numpy as np
import time

def rgb565_to_rgb888(rgb565):
    # Assume rgb565 is a list with [red, green, blue]
    red, green, blue = rgb565

    # Convert to 8-bit RGB888 (expand each component to 8 bits)
    r = (red << 3) | (red >> 2)    # Red: Expand from 5 bits to 8 bits
    g = (green << 2) | (green >> 4)  # Green: Expand from 5 bits to 8 bits
    b = (blue << 3) | (blue >> 2)   # Blue: Expand from 5 bits to 8 bits
    return (r, g, b)

def display_rgb565_image(rgb565_data, width, height):
    # Convert all RGB565 values to RGB888
    rgb888_data = [rgb565_to_rgb888(pixel) for pixel in rgb565_data]
    #print(rgb888_data)
    # Create an image from the RGB888 data
    # Convert the list of RGB888 tuples to a NumPy array and reshape to (height, width, 3)
    pixels = np.array(rgb888_data, dtype=np.uint8).reshape((height, width, 3))
    # Create an image from the RGB888 data
    img = Image.fromarray(pixels, 'RGB')
    img.show()
    img.save('output.png')

def clean(data, num_pixels):
    rgb_565 = []
    for i in range(0, num_pixels):
        red = int(data[3*i])
        green = int(data[3*i+1])
        blue = int(data[3*i+2])
        rgb = [red, green, blue]
        rgb_565.append(rgb)
    return rgb_565



# Replace 'COMx' with your port and set your baud rate
ser = serial.Serial('/dev/ttyUSB0', 19200, timeout=1)

height = 6
width = 320
num_bytes_to_read = width * height * 3

with open('raw_rgb565.txt', 'w') as f:
    # Allow x time for snapshot to occur. Primarily for user to have enough time
    time.sleep(5)
    
    # Wait until the start character 'S' is detected
    while True:
        byte = ser.read(1).decode('utf-8', errors='ignore')
        if byte == 'S':
            break  # Start character detected, exit loop

    # Now read the specified number of bytes
    data = ser.read(num_bytes_to_read)
    
    # print(data)
    print(len(data))
    image = clean(data, num_bytes_to_read // 3)
    #print(image)
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
    
