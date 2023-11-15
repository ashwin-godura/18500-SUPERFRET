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
    midi_data = pretty_midi.PrettyMIDI(midi_file_path)

    target_track = None
    for idx, instrument in enumerate(midi_data.instruments):
        print("requested track: " +track+ " actual track: " + instrument.name)
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

def get_midi_tracks(filepath):
    try:

        # Use pretty_midi to parse the MIDI file
        pretty_midi_obj = pretty_midi.PrettyMIDI(filepath)

        # Extract information about tracks
        tracks_info = []
        print("starting loop")
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
    
def process_midi_for_teensy(midi_file_path, speed_factor, selected_track):
    try:
        print("0\n" + str(midi_file_path) + "\n\n\n\n")
        # Load the MIDI file using pretty_midi
        midi_data = pretty_midi.PrettyMIDI(midi_file_path)
        print("1\n")
        
         # Adjust the tempo by the speed factor
        # midi_data.adjust_tempo(speed_factor)
        print("3\n")

        # Select the specified track
        selected_instrument = get_track_by_name(midi_data, selected_track)
        if selected_instrument is None:
            selected_instrument = midi_data.instruments[-1]

        print("2\n")

        # Create a new PrettyMIDI object with only the selected track
        new_midi_data = pretty_midi.PrettyMIDI()
        new_midi_data.instruments.append(selected_instrument)
        print("4\n")

        # Convert the PrettyMIDI object to bytes
        midi_bytes_buffer = io.BytesIO()
        new_midi_data.write(midi_bytes_buffer)
        midi_bytes = midi_bytes_buffer.getvalue()
        
        print("5\n")

        return midi_bytes

    except Exception as e:
        print(f"Error: {e}")
        return None

def main():
    midi_file_path = "static/Twinkle Twinkle Little Star.mid"  # Replace with the path to your MIDI file
    notes = extract_notes_from_midi(midi_file_path)
    
    print("Notes extracted:")
    for note in notes:
        print(f"Note: {note['note_value']}, Start Time: {note['start_time']} seconds")

if __name__ == "__main__":
    main()
