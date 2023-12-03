import io
import math
import sys
import mido
import pretty_midi

MAX_FRET_VALUE = 14
REPLACE_FRET_VALUE = 1

def extract_notes_from_track(track, speed, transpose):
    notes = []
    prevfret = 0
    prevstring = 1
    prevstart = float(-1)

    if len(track.notes) > 0:
        initial_start_time = (track.notes[0].start * 5 / speed)

    for note in track.notes:
        note.pitch = note.pitch + transpose
        fret, string = midi_to_bass_fret_string(note, prevfret, prevstring, prevstart)
        prevfret = fret
        prevstring = string
        prevstart = note.start

        note_dict = {
            "note_value": note.pitch,
            "start_time": (note.start * 5) / speed - initial_start_time,
            "end_time": (note.end * 5) / speed - initial_start_time,
            "fret": fret,
            "string": string
        }
        notes.append(note_dict)


    # send a dummy note for ending the file
    prev_end_time = notes[-1]["end_time"]
    notes.append({
            "note_value": note.pitch,
            "start_time": prev_end_time,
            "end_time": prev_end_time+1,
            "fret": 15,
            "string": 15
        })

    return notes

def extract_tempo(notes):
    time_dict = {}
    for i in range(len(notes) - 1):
        time = notes[i+1]["start_time"] - notes[i]["start_time"]
        rounded = round(5*time)
        if rounded in time_dict:
            time_dict[rounded].append(time)
        else:
            time_dict[rounded]=[time]
    best = None
    bestCount = 0
    for key in time_dict:
        if len(time_dict[key]) > bestCount:
            bestCount = len(time_dict[key])
            best = key
    tempo = 60/(sum(time_dict[best]) / len(time_dict[best]))
    while tempo > 200:
        tempo = tempo/2
    print(f"TEMPO {tempo}")
        
        
        
    
    
    """
    (options, probs) = midi_data.estimate_tempi()
    print(options)
    for tempo in options:
        if tempo % 1 < .001 and tempo < 201:
            return tempo
    return options[0]"""
    

def extract_notes_from_midi(midi_file_path, speed, transpose, track):
    with open(str(midi_file_path), 'rb') as file:
            midi_data = pretty_midi.PrettyMIDI(file)
            tempo_changes = midi_data.get_tempo_changes()

            # Print the tempo changes
            print("Tempo changes:")
            for tempo in tempo_changes:
                print(f"Tempo: {tempo}")

    target_track = None
    for idx, instrument in enumerate(midi_data.instruments):
        # print("requested track: " +track+ " actual track: " + instrument.name)
        if track in instrument.name:
            target_track = midi_data.instruments[idx]
            break

    if not target_track:
        # print("No track named 'bass' found. Selecting the last track.")
        target_track = midi_data.instruments[-1]

    notes = extract_notes_from_track(target_track, speed, transpose)
    t = extract_tempo(notes)
    print(f"TEMPO: {t}")
    return notes

def midi_to_bass_fret_string(midi_note, prev_fret, prev_string, prev_start):
    # Define the standard tuning for a 4-string bass guitar (E1, A1, D2, G2)
    string_notes = [40, 45, 50, 55]  # MIDI notes for E1, A1, D2, G2

    # Check if the MIDI note is within the range of the bass guitar
    # if not then pick the same note but in the range of a bass guitar
    if not (40 <= int(midi_note.pitch) <= 55 + MAX_FRET_VALUE): 
        midi_note.pitch = (midi_note.pitch % 12) + 48

    if midi_note.start - prev_start > REPLACE_FRET_VALUE:
        print("Gap detected:")
        print(f"Current Note Start: {midi_note.start}")
        print(f"Previous Note Start: {prev_start}")
        print(f"Time Gap: {midi_note.start - prev_start} seconds")
        print("Resetting previous string to 1 and previous fret to 0")
        prev_string = 1
        prev_fret = 0

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
