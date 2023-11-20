to convert binary midi file to human readable hex:

hexdump -v -e '1/1 "%02X "' BINARY_FILE_PATH > HUMAN_READABLE_FILE_PATH
