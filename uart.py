import serial
from PIL import Image
import numpy as np
import time
import os

# Converts a squashed array of bytes of image data into an array, and then a pillow image
# NOTE: bytes will be a flat array of size 153600<->FRAME_BUFFER_SIZE
# TODO: PILLOW DOESN"T ALLOW FOR 16 BIT RGB SO WE NEED TO MAKE A WORKAROUND
def create_image_from_bytes(bytes):
    rgb = np.zeros((240,320,3), dtype=np.uint8)
    for i in range(320*240):
        byte_one = bytes[i*2][0]
        byte_two = bytes[i*2+1][0]

        r = (byte_one & 0xF8) >> 3 # top 5 bits
        g = ((byte_one & 0x07) << 3) | ((byte_two & 0xE0) >> 5) # middle 6 bits
        b = byte_two & 0x1F # last 5 bits

        r = (r<<3)|(r>>2)
        g = (g<<2)|(g>>4)
        b = (b<<3)|(b>>2)

        rgb[int(i/320)][i%320] = (r, g, b)

        # flat_pixel_array[i] = (bytes[i*2][0]*256 + bytes[i*2+1][0]) #shift left bits by hex vals(*256)
    # pixel_array = np.reshape(flat_pixel_array, (240, -1))
    # print(pixel_array)
    img = Image.fromarray(rgb, 'RGB')
    return img

def main():
    # Replace 'COMx' with your port and set your baud rate
    # '/dev/tyyXXXX' for linux
    ser = serial.Serial('COM3', 500_000, timeout=1)

    FRAME_BUFFER_SIZE = 320*240*2
    VSYNC_SIZE = 7
    IN_BETWEEN_SIZE = 18
    SYNC_VAL = 9
    SYNCHRONIZER = IN_BETWEEN_SIZE + SYNC_VAL
    NUM_FRAMES_READ = 8 # change its size to see different results

    print("UART ready for transmition")

    byte_counter = 0
    command_count = 0
    current_frame = 0
    cur_frame_index = 0
    within_frame_flag = False
    buffer = []
    frames = {}
    for i in range(NUM_FRAMES_READ):
        frames[i+1] = []

    while True:
        try:
            # ser.reset_input_buffer()   # Clears the input buffer
            # ser.reset_output_buffer()  # Clears the output buffer
            byte = ser.read(1)
            
            # Check if any data is received
            if byte:
                # Print the received byte in hexadecimal format
                # print(f"Received: {byte.hex()}")
                byte_counter+=1
                buffer.append(byte)
                #SEND BYTE NUMBER {byte_counter} to PILLOW OBJECT

                # Ask if we should begin compiling a frame
                if(byte_counter == VSYNC_SIZE+1 or byte_counter == FRAME_BUFFER_SIZE+2*VSYNC_SIZE or (current_frame >= 2 and byte_counter == current_frame*(FRAME_BUFFER_SIZE + SYNCHRONIZER)-(2*VSYNC_SIZE-1))):
                    within_frame_flag = True
                    current_frame += 1
                    # print(byte_counter)

                # Try to add to current frame buffer
                if(within_frame_flag):
                    frames[current_frame].append(byte)
                    cur_frame_index += 1
                    if(cur_frame_index >= 153600):
                        cur_frame_index = 0
                        within_frame_flag = False
                
                if(byte_counter == (FRAME_BUFFER_SIZE + IN_BETWEEN_SIZE) * NUM_FRAMES_READ):
                    break
        
        except serial.SerialException as e:
            print(f"Serial error: {e}")
            break
        except KeyboardInterrupt:
            print("User interrupted.")
            break
        
    #Display pillow image
    # for byte in buffer:
    #     print(f'Byte: {byte.hex()}')

    # Close the serial connection
    ser.close()

    # THIS SECTION INCLUDES A MORE SPECIFIC BREAKDOWN OF BUFFERSYNC VALUES
    # jump_val = 0
    # for i in range(NUM_FRAMES_READ):
    #     if(i < 2):
    #         for i in range(i*FRAME_BUFFER_SIZE+jump_val,i*FRAME_BUFFER_SIZE+jump_val+VSYNC_SIZE):
    #             print("Byte ", i, ": ", buffer[i].hex())
    #         jump_val += VSYNC_SIZE
    #     else:
    #          jump_val += SYNC_VAL
    #          for i in range(i*FRAME_BUFFER_SIZE+jump_val,i*FRAME_BUFFER_SIZE+jump_val+IN_BETWEEN_SIZE):
    #             print("Byte ", i, ": ", buffer[i].hex())
    #          jump_val += IN_BETWEEN_SIZE

    # THIS SECTION JUST JUMPS BETWEEN VSYNC BUFFER SECTIONS
    # jump_val = 0
    # for i in range(NUM_FRAMES_READ):
    #     if(i < 1):
    #         for i in range(i*FRAME_BUFFER_SIZE+jump_val,i*FRAME_BUFFER_SIZE+jump_val+VSYNC_SIZE):
    #             print("Byte ", i, ": ", buffer[i].hex())
    #         jump_val += VSYNC_SIZE
    #     else:
    #         for i in range(i*FRAME_BUFFER_SIZE+jump_val,i*FRAME_BUFFER_SIZE+jump_val+VSYNC_SIZE):
    #             print("Byte ", i, ": ", buffer[i].hex())
    #         jump_val += SYNC_VAL + IN_BETWEEN_SIZE

    # hunt down the location of the 'in-between-bytes' or 'vsync bytes'
    for i in range(FRAME_BUFFER_SIZE * 8):
        if(buffer[i] == b'\x00' 
        and buffer[i+1] == b'\x04'
        and buffer[i+2] == b'\x11'
        and buffer[i+3] == b'\x40'
        and buffer[i+4] == b'\xf0'
        and buffer[i+5] == b'\x11'
        and buffer[i+6] == b'\xb0'):
            print("vsync found at index ", i)

    new_img = create_image_from_bytes(frames[1]) #CAPTURE 1
    new_img.save("img1.png")
    new_img = create_image_from_bytes(frames[2]) #CAPTURE 2
    new_img.save("img2.png")
    new_img = create_image_from_bytes(frames[3]) #CAPTURE 3
    new_img.save("img3.png")
    new_img = create_image_from_bytes(frames[4]) #CAPTURE 4
    new_img.save("img4.png")
    new_img = create_image_from_bytes(frames[5]) #CAPTURE 5
    new_img.save("img5.png")
    new_img = create_image_from_bytes(frames[6]) #CAPTURE 6
    new_img.save("img6.png")
    new_img = create_image_from_bytes(frames[7]) #CAPTURE 7
    new_img.save("img7.png")
    
        
if __name__ == "__main__":
    main()