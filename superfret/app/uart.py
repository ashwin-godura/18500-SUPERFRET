import RPi.GPIO as GPIO
import serial
import time

RUNNING_ON_PI = False

class Uart:
    def __init__(self, uart_port='/dev/ttyS0', start_pin=17, stop_pin=18, pause_pin=27, restart_pin=22):
        # Initialize UART
        self.uart = serial.Serial(uart_port, 9600, timeout=1)

        # Initialize GPIO
        self.start_pin = start_pin
        self.stop_pin = stop_pin
        self.pause_pin = pause_pin
        self.restart_pin = restart_pin

        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.start_pin, GPIO.OUT)
        GPIO.setup(self.stop_pin, GPIO.OUT)
        GPIO.setup(self.pause_pin, GPIO.OUT)
        GPIO.setup(self.restart_pin, GPIO.OUT)
        self.set_gpio_low(self.start_pin)
        self.set_gpio_low(self.stop_pin)
        self.set_gpio_low(self.pause_pin)
        return

    def set_gpio_high(self, pin):
        GPIO.output(pin, GPIO.HIGH)

    def set_gpio_low(self, pin):
        GPIO.output(pin, GPIO.LOW)

    def start_song(self, file):
        print("sending file")
        try:
            # Trigger dedicated GPIO interrupt for start_song
            self.set_gpio_high(self.start_pin)

            # Perform UART communication (send file, for example)
            self.uart.write(bytes(file))

            time.sleep(0.5)  # Briefly set GPIO pin high
            self.set_gpio_low(self.start_pin)

        except Exception as e:
            self.set_gpio_low(self.start_pin)
            print(f"Error in start_song: {e}")

        print("file sent")
        return

    def pause_song(self):
        try:
            # Trigger dedicated GPIO interrupt for pause_song
            self.set_gpio_high(self.pause_pin)

        except Exception as e:
            print(f"Error in pause_song: {e}")
        return

    def resume_song(self):
        try:
            # Trigger dedicated GPIO interrupt for pause_song
            self.set_gpio_low(self.pause_pin)

        except Exception as e:
            print(f"Error in pause_song: {e}")
        return

    def restart_song(self):
        try:
            # Trigger dedicated GPIO interrupt for pause_song
            self.set_gpio_high(self.restart_pin)
            self.set_gpio_low(self.pause_pin)
            time.sleep(0.1)  # Briefly set GPIO pin high
            self.set_gpio_low(self.restart_pin)

        except Exception as e:
            print(f"Error in pause_song: {e}")
        return

    def cleanup(self):
        # Clean up resources
        GPIO.cleanup()
        self.uart.close()
        return

if __name__ == "__main__":
    try:
        music_controller = Uart()

        # Example: Start Song
        music_controller.start_song()

        # Example: Stop Song
        music_controller.stop_song()

        # Example: Pause Song
        music_controller.pause_song()

    finally:
        # Cleanup GPIO and UART on exit
        music_controller.cleanup()
