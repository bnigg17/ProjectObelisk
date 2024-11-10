import serial
from PIL import Image
import time

def rgb565_to_rgb888(rgb565):
    # Convert to 8-bit RGB888
    r = (r << 3) | (r >> 2)
    g = (g << 2) | (g >> 4)
    b = (b << 3) | (b >> 2)
    return (r, g, b)

def display_rgb565_image(rgb565_data, width, height):
    # Convert all RGB565 values to RGB888
    rgb888_data = [rgb565_to_rgb888(pixel) for pixel in rgb565_data]
    # Create an image from the RGB888 data
    img = Image.new("RGB", (width, height))
    img.putdata(rgb888_data)
    img.show()
    
def clean(data):
    rgb_565 = []
    start_flag = 0
    pixel_flag = 0
    pixel_idx = 0
    pixel = [0, 0, 0]  # Use a list to store RGB components
    hex_char = ''  # To store the current hex character we're parsing

    for c in data:
        print(c)
        if start_flag == 0:
            start_flag = 1 if c == 'S' else 0  # Start processing after 'S'
        elif c == 'E':  # Stop processing when 'E' is found
            break
        elif c == '(':
            pixel_flag = 1  # Begin parsing a new pixel
            pixel = [0, 0, 0]  # Reset pixel values
        elif c == ')' or ',':
            continue  # Skip closing parentheses
        else:
            if pixel_flag == 1:
                hex_char += c  # Accumulate hex characters
                if len(hex_char) == 4:  # Once we have 4 characters, convert to integer
                    pixel[pixel_idx] = int(hex_char, 16)  # Convert the pair to an integer
                    pixel_idx += 1  # Move to the next color component
                    hex_char = ''  # Reset hex_char for the next pair
                    if pixel_idx == 2:  # Once we have all 3 components, append the pixel
                        rgb_565.append(tuple(pixel))  # Append the pixel as a tuple
                        pixel_idx = 0  # Reset the pixel index for the next pixel
    return rgb_565

# Example: Replace with actual RGB565 data and dimensions
# width, height = 100, 100
# rgb565_data = [0xF800] * (width * height)  # Just an example with red pixels
# display_rgb565_image(rgb565_data, width, height)

# Replace 'COMx' with your port and set your baud rate
ser = serial.Serial('/dev/ttyUSB0', 19200, timeout=1)

with open('raw_rgb565.txt', 'w') as f:
    # Allow x time for snapshot to occur. Primarily for user to have enough time
    time.sleep(5)
    data = str()
    # Data to come in 1 pulse as print_image() from MCU pushes data in one transmition
    for _ in range(1):
        # Wait until input is recieeved
        if ser.in_waiting:
            data = ser.readline().decode('utf-8', errors='ignore').strip()
            f.write(data)   # Write data to file 
    print(data)
    print(clean(data))
