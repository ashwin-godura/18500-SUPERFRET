# import RPi.GPIO as GPIO
# import serial
# import time

RUNNING_ON_PI = False

class Uart:
    def __init__(self, uart_port='/dev/ttyS0', start_pin=17, stop_pin=18, pause_pin=27, restart_pin=22):
        # # # Initialize UART
        # self.uart = serial.Serial(uart_port, 115200, timeout=1)

        # # Initialize GPIO
        # self.start_pin = start_pin
        # self.stop_pin = stop_pin
        # self.pause_pin = pause_pin
        # self.restart_pin = restart_pin

        # GPIO.setmode(GPIO.BCM)
        # GPIO.setup(self.start_pin, GPIO.OUT)
        # GPIO.setup(self.stop_pin, GPIO.OUT)
        # GPIO.setup(self.pause_pin, GPIO.OUT)
        # GPIO.setup(self.restart_pin, GPIO.OUT)
        # self.set_gpio_low(self.start_pin)
        # self.set_gpio_low(self.stop_pin)
        # self.set_gpio_low(self.pause_pin)
        return

    # def set_gpio_high(self, pin):
    #     GPIO.output(pin, GPIO.HIGH)

    # def set_gpio_low(self, pin):
    #     GPIO.output(pin, GPIO.LOW)

    def start_song(self, file):
        # try:
        #     # Trigger dedicated GPIO interrupt for start_song
        #     self.set_gpio_high(self.start_pin)
        #     time.sleep(0.1)

        #     # print("len=" + str(len(file)))
        #     # hex_string = ' '.join([hex(byte) for byte in file])
        #     # print(hex_string)

        #     # file = Uart.remove_bytes_between_markers(file)
        #     # Perform UART communication (send file, for example)

        #     self.uart.write(file)

        #     time.sleep(0.5)  # Briefly set GPIO pin high
        #     self.set_gpio_low(self.start_pin)

        # except Exception as e:
        #     self.set_gpio_low(self.start_pin)
        #     print(f"Error in start_song: {e}")

        # print("file sent")
        return
    
    # def remove_bytes_between_markers(byte_array):
    #     # # Define the marker sequences
    #     start_marker = bytes([0x4d, 0x54, 0x72, 0x6b])
    #     end_marker = bytes([0xff, 0x2f, 0x00])

    #     # Find the indices of the start and end markers
    #     start_index = byte_array.find(start_marker)
    #     end_index = byte_array.find(end_marker)

    #     # If both markers are found, remove the bytes between them
    #     if start_index != -1 and end_index != -1:
    #         byte_array = byte_array[:start_index] + byte_array[end_index + len(end_marker):]
    #         print("Bytes between markers removed successfully.")
    #     else:
    #         print("Markers not found in the byte array.")

    #     return byte_array
    
    def read_feedback(self):
        # try:
        #     # Wait until data is available on UART
        #     if not self.uart.in_waiting >= 4:
        #         return None  # Block until data is available
            
        #     feedbacks = []

        #     # Read the data byte by byte
        #     # feedback = int.from_bytes(self.uart.read(4), byteorder='big')
        #     while self.uart.in_waiting >= 4:
        #         sumx = 0
        #         tot = 0

        #         data = self.uart.read(4).hex()
        #         x = int(data, 16)# Read data from the serial port
        #         if x > 0x7FFFFFFF:
        #             x -= 0x100000000
        #         sumx += x%2
        #         tot += 1
        #         feedbacks.append({'correct': x%2, 'diff': x})
        #         print(f"Received: {(x%2)} {x} {sumx / tot}")
 	
        #     return feedbacks

        # except Exception as e:
        #     print(f"Error in read_packet: {e}")
        #     return None
        return

    # def pause_song(self):
    #     try:
    #         # Trigger dedicated GPIO interrupt for pause_song
    #         self.set_gpio_high(self.pause_pin)

    #     except Exception as e:
    #         print(f"Error in pause_song: {e}")
    #     return

    # def resume_song(self):
    #     try:
    #         # Trigger dedicated GPIO interrupt for pause_song
    #         self.set_gpio_low(self.pause_pin)

    #     except Exception as e:
    #         print(f"Error in pause_song: {e}")
    #     return

    def restart_song(self):
        # try:
        #     # Trigger dedicated GPIO interrupt for pause_song
        #     self.set_gpio_high(self.restart_pin)
        #     self.set_gpio_low(self.pause_pin)
        #     time.sleep(0.1)  # Briefly set GPIO pin high
        #     self.set_gpio_low(self.restart_pin)

        # except Exception as e:
        #     print(f"Error in pause_song: {e}")
        return

    def cleanup(self):
        # Clean up resources
        GPIO.cleanup()
        self.uart.close()
        return

# if __name__ == "__main__":
#     try:
#         music_controller = Uart()

#         # Example: Start Song
#         music_controller.start_song()

#         # Example: Stop Song
#         music_controller.stop_song()

#         # Example: Pause Song
#         music_controller.pause_song()

#     finally:
#         # Cleanup GPIO and UART on exit
#         music_controller.cleanup()
