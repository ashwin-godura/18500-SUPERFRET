import io
import math
import sys
import mido
import pretty_midi

MAX_FRET_VALUE = 12

def extract_notes_from_track(track, speed):
    notes = []
    prevfret = 0
    prevstring = 1
    for note in track.notes:
        fret, string = midi_to_bass_fret_string(note, prevfret, prevstring)
        prevfret = fret
        prevstring = string
        note_dict = {
            "note_value": note.pitch,
            "start_time": (note.start * 5) / speed,
            "fret": fret,
            "string": string
        }
        notes.append(note_dict)

    return notes

def extract_notes_from_midi(midi_file_path, speed, track):
    with open(str(midi_file_path), 'rb') as file:
            midi_data = pretty_midi.PrettyMIDI(file)

    target_track = None
    for idx, instrument in enumerate(midi_data.instruments):
        # print("requested track: " +track+ " actual track: " + instrument.name)
        if track in instrument.name:
            target_track = midi_data.instruments[idx]
            break

    if not target_track:
        # print("No track named 'bass' found. Selecting the last track.")
        target_track = midi_data.instruments[-1]

    return extract_notes_from_track(target_track, speed)

def midi_to_bass_fret_string(midi_note, prev_fret, prev_string):
    # Define the standard tuning for a 4-string bass guitar (E1, A1, D2, G2)
    string_notes = [40, 45, 50, 55]  # MIDI notes for E1, A1, D2, G2

    # Check if the MIDI note is within the range of the bass guitar
    # if not then pick the same note but in the range of a bass guitar
    if not (40 <= int(midi_note.pitch) <= 55 + MAX_FRET_VALUE): 
        midi_note.pitch = (midi_note.pitch % 12) + 48

    min_distance = 1000000000
    fret_answer = -1
    string_answer = -1

    # Iterate through each string
    for string in range(4):
        # get fret number associated with string
        fret_num = midi_note.pitch - string_notes[string]
        # check if fret num is possible
        if fret_num < 0 or fret_num > MAX_FRET_VALUE:
            continue
        # calculate distance
        distance = math.sqrt((prev_string - (string+1))**2 + (prev_fret - fret_num)**2)
        # update answer
        if distance < min_distance:
            min_distance = distance
            fret_answer = fret_num 
            string_answer = string + 1

    return fret_answer, string_answer

def convert_notes_to_bytes(notes):
    byte_array = bytearray()

    for note in notes:
        # Convert start time to an unsigned 32-bit integer in milliseconds
        start_time_ms = int(note["start_time"] * 1000)
        start_time_bytes = start_time_ms.to_bytes(4, byteorder='big', signed=False)

        # Combine fret and string values into one byte
        fret_and_string = ((note["string"] << 4)) | (note["fret"])
        fret_and_string_byte = bytes([fret_and_string])

        # Append the bytes to the result
        byte_array.extend(start_time_bytes + fret_and_string_byte)

    total_length = len(byte_array) + 4
    total_length_bytes = total_length.to_bytes(4, byteorder='big', signed=False)

    # Prepend the length bytes to the byte array
    byte_array = total_length_bytes + byte_array

    return byte_array

def get_midi_tracks(filepath):
    try:

        # Use pretty_midi to parse the MIDI file
        with open(str(filepath), 'rb') as file:
            pretty_midi_obj = pretty_midi.PrettyMIDI(file)

        # Extract information about tracks
        tracks_info = []
        for i, instrument in enumerate(pretty_midi_obj.instruments):
            print(instrument.name)
            tracks_info.append(instrument.name)

        return tracks_info

    except Exception as e:
        # Handle exceptions, e.g., file not found, invalid MIDI file, etc.
        print(f"Error: {e}")
        return None
    
def get_track_by_name(midi_data, track_name):
    for instrument in midi_data.instruments:
        if instrument.name == track_name:
            return instrument
    return None
    
# def process_midi_for_teensy(midi_file_path, speed_factor, selected_track):

#     # instead send byte array first four bytes is start time in millis, next byte is packed fret and string
#     try:
#         # Load the MIDI file using pretty_midi
#         with open(str(midi_file_path), 'rb') as file:
#             midi_data = pretty_midi.PrettyMIDI(file)

#         print("file=" + str(midi_file_path))
#         midi_bytes_buffer = io.BytesIO()
#         midi_data.write(midi_bytes_buffer)
#         midi_bytes = midi_bytes_buffer.getvalue()
#         midi_bytes_buffer.close()

#         print("len=" + str(len(midi_bytes)))
#         hex_string = ' '.join([hex(byte) for byte in midi_bytes])
#         print(hex_string)
        
#         # Adjust the tempo by the speed factor
#         #  midi_data.adjust_times(speed_factor)

#         # Select the specified track
#         selected_instrument = get_track_by_name(midi_data, selected_track)
#         if selected_instrument is None:
#             selected_instrument = midi_data.instruments[-1]

#         # Create a new PrettyMIDI object with only the selected track
#         new_midi_data = pretty_midi.PrettyMIDI()
#         new_midi_data.instruments.append(selected_instrument)
        
#         # prevfret = 0
#         # prevstring = 1
#         # for note in selected_instrument.notes:
#         #     # Modify the pitch attribute (replace 60 with the desired new pitch value)
#         #     prevfret, prevstring = midi_to_bass_fret_string(note, prevfret, prevstring)
#         #     # print("prevfret {} prevstring {} makes {}".format(prevfret, prevstring, ((prevstring << 4) + prevfret)))
#         #     note.pitch = (prevstring << 4) + prevfret

#         # Convert the PrettyMIDI object to bytes
#         midi_bytes_buffer = io.BytesIO()
#         new_midi_data.write(midi_bytes_buffer)
#         midi_bytes = midi_bytes_buffer.getvalue()
#         midi_bytes_buffer.close()
        
#         return midi_bytes

#     except Exception as e:
#         print(f"Error: {e}")
#         return None
