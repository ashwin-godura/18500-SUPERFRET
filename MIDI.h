
#include <cassert>

uint8_t MIDI[488] = {
  0x4D, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x01, 0x00, 0x03, 0x01, 0x80, 0x4D, 0x54, 0x72, 0x6B, 0x00, 0x00, 0x01, 0xD2, 0x00, 0xFF, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08,
  0x00, 0xFF, 0x51, 0x03, 0x08, 0x52, 0xAE, 0x00, 0xFF, 0x03, 0x15, 0x45, 0x6C, 0x65, 0x63, 0x2E, 0x20, 0x50, 0x69, 0x61, 0x6E, 0x6F, 0x20, 0x28, 0x43, 0x6C, 0x61, 0x73, 0x73, 0x69,
  0x63, 0x29, 0x00, 0xC0, 0x00, 0x00, 0x90, 0x3C, 0x32, 0x81, 0x40, 0x80, 0x3C, 0x00, 0x81, 0x40, 0x90, 0x3C, 0x32, 0x81, 0x40, 0x80, 0x3C, 0x00, 0x81, 0x40, 0x90, 0x43, 0x3, 0x81,
  0x40, 0x80, 0x43, 0x00, 0x81, 0x40, 0x90, 0x43, 0x32, 0x81, 0x40, 0x80, 0x43, 0x00, 0x81, 0x40, 0x90, 0x45, 0x32, 0x81, 0x40, 0x80, 0x45, 0x00, 0x81, 0x40, 0x90, 0x45, 0x32, 0x81,
  0x40, 0x80, 0x45, 0x00, 0x81, 0x40, 0x90, 0x43, 0x32, 0x83, 0x00, 0x80, 0x43, 0x00, 0x83, 0x00, 0x90, 0x41, 0x32, 0x81, 0x40, 0x80, 0x41, 0x00, 0x81, 0x40, 0x90, 0x41, 0x32, 0x81,
  0x40, 0x80, 0x41, 0x00, 0x81, 0x40, 0x90, 0x40, 0x32, 0x81, 0x40, 0x80, 0x40, 0x00, 0x81, 0x40, 0x90, 0x40, 0x32, 0x81, 0x40, 0x80, 0x40, 0x00, 0x81, 0x40, 0x90, 0x3E, 0x32, 0x81,
  0x40, 0x80, 0x3E, 0x00, 0x81, 0x40, 0x90, 0x3E, 0x32, 0x81, 0x40, 0x80, 0x3E, 0x00, 0x81, 0x40, 0x90, 0x3C, 0x32, 0x83, 0x00, 0x80, 0x3C, 0x00, 0x83, 0x00, 0x90, 0x43, 0x32, 0x81,
  0x40, 0x80, 0x43, 0x00, 0x81, 0x40, 0x90, 0x43, 0x32, 0x81, 0x40, 0x80, 0x43, 0x00, 0x81, 0x40, 0x90, 0x41, 0x32, 0x81, 0x40, 0x80, 0x41, 0x00, 0x81, 0x40, 0x90, 0x41, 0x32, 0x81,
  0x40, 0x80, 0x41, 0x00, 0x81, 0x40, 0x90, 0x40, 0x32, 0x81, 0x40, 0x80, 0x40, 0x00, 0x81, 0x40, 0x90, 0x40, 0x32, 0x81, 0x40, 0x80, 0x40, 0x00, 0x81, 0x40, 0x90, 0x3E, 0x32, 0x83,
  0x00, 0x80, 0x3E, 0x00, 0x83, 0x00, 0x90, 0x43, 0x32, 0x81, 0x40, 0x80, 0x43, 0x00, 0x81, 0x40, 0x90, 0x43, 0x32, 0x81, 0x40, 0x80, 0x43, 0x00, 0x81, 0x40, 0x90, 0x41, 0x32, 0x81,
  0x40, 0x80, 0x41, 0x00, 0x81, 0x40, 0x90, 0x41, 0x32, 0x81, 0x40, 0x80, 0x41, 0x00, 0x81, 0x40, 0x90, 0x40, 0x32, 0x81, 0x40, 0x80, 0x40, 0x00, 0x81, 0x40, 0x90, 0x40, 0x32, 0x81,
  0x40, 0x80, 0x40, 0x00, 0x81, 0x40, 0x90, 0x3E, 0x32, 0x83, 0x00, 0x80, 0x3E, 0x00, 0x83, 0x00, 0x90, 0x3C, 0x32, 0x81, 0x40, 0x80, 0x3C, 0x00, 0x81, 0x40, 0x90, 0x3C, 0x32, 0x81,
  0x40, 0x80, 0x3C, 0x00, 0x81, 0x40, 0x90, 0x43, 0x32, 0x81, 0x40, 0x80, 0x43, 0x00, 0x81, 0x40, 0x90, 0x43, 0x32, 0x81, 0x40, 0x80, 0x43, 0x00, 0x81, 0x40, 0x90, 0x45, 0x32, 0x81,
  0x40, 0x80, 0x45, 0x00, 0x81, 0x40, 0x90, 0x45, 0x32, 0x81, 0x40, 0x80, 0x45, 0x00, 0x81, 0x40, 0x90, 0x43, 0x32, 0x83, 0x00, 0x80, 0x43, 0x00, 0x83, 0x00, 0x90, 0x41, 0x32, 0x81,
  0x40, 0x80, 0x41, 0x00, 0x81, 0x40, 0x90, 0x41, 0x32, 0x81, 0x40, 0x80, 0x41, 0x00, 0x81, 0x40, 0x90, 0x40, 0x32, 0x81, 0x40, 0x80, 0x40, 0x00, 0x81, 0x40, 0x90, 0x40, 0x32, 0x81,
  0x40, 0x80, 0x40, 0x00, 0x81, 0x40, 0x90, 0x3E, 0x32, 0x81, 0x40, 0x80, 0x3E, 0x00, 0x81, 0x40, 0x90, 0x3E, 0x32, 0x81, 0x40, 0x80, 0x3E, 0x00, 0x81, 0x40, 0x90, 0x3C, 0x32, 0x83,
  0x00, 0x80, 0x3C, 0x00, 0x00, 0xFF, 0x2F, 0x00
};

typedef struct {
  uint32_t duration;  // [ticks / quarter-note]
  uint8_t note;
  bool ON;
} NOTE_t;

#define MAX_MIDI_FILE_SIZE 10'000  // [bytes]
#define MAX_NUM_NOTES 1000         // [notes] // TODO hold more notes?
NOTE_t notes[MAX_NUM_NOTES];

uint32_t NUM_NOTES_FOUND = 0;
uint32_t MICROSECONDS_PER_BEAT = 500'000;  // default
uint32_t TICKS_PER_QUARTER_NOTE = 48;      // default

bool found_MThd_Chunck(uint8_t* buff) {
  return (buff[0] == 0x4D) and (buff[1] == 0x54) and (buff[2] == 0x68) and (buff[3] == 0x64);
}

bool found_MTrk_Chunck(uint8_t* buff) {
  return (buff[0] == 0x4D) and (buff[1] == 0x54) and (buff[2] == 0x72) and (buff[3] == 0x6B);
}

bool found_Time_Signature(uint8_t* buff) {
  return (buff[0] == 0xFF) and (buff[1] == 0x58) and (buff[2] == 0x04);
}

bool found_Set_Tempo_Event(uint8_t* buff) {
  return (buff[0] == 0xFF) and (buff[1] == 0x51) and (buff[2] == 0x03);
}

bool found_Track_Name(uint8_t* buff) {
  return (buff[0] == 0xFF) and (buff[1] == 0x03);
}

bool found_Program_Change(uint8_t* buff) {
  return (buff[0] & 0xF0) == 0xC0;
}
bool found_Note_ON(uint8_t* buff) {
  return (buff[0] & 0xF0) == 0x90;
}

bool found_Note_OFF(uint8_t* buff) {
  return (buff[0] & 0xF0) == 0x80;
}

bool found_End_Of_Track(uint8_t* buff) {
  return (buff[0] == 0xFF) and (buff[1] == 0x2F) and (buff[2] == 0x00);
}

uint64_t decodeVariableLengthEncoding(uint8_t* buff, int& bytes_read) {
  uint64_t result = 0;

  int i = 0;
  while (i < 8) {  // don't look at more than 8 bytes
    uint64_t currByte = buff[i];
    i++;
    result = (result & (~((uint64_t)0x7F))) | (currByte & 0x7F);
    if (not(currByte & 0x80)) {  // MSB is 0
      break;
    }
    result = result << 7;  // accomodate next 7 bits
  }
  bytes_read = i;
  return result;
}


void parseMIDI() {
  int NUM_BYTES = sizeof(MIDI);

  bool parsed_MThd = false;
  uint32_t bytes_in_MThd_header = 0;
  bool parsed_bytes_in_MThd_header = false;
  bool parsed_bytes_in_MTrk_header = false;
  bool parsed_ticks_per_quarter_note = false;
  bool parsed_MTrk = false;
  uint32_t bytes_in_MTrk_header = 0;

  NUM_NOTES_FOUND = 0;
  MICROSECONDS_PER_BEAT = 500'000;  // default
  TICKS_PER_QUARTER_NOTE = 48;      // default

  int i = 0;
  while (i < NUM_BYTES) {
    if (not parsed_MThd) {  // look at next 4 bytes for "4D 54 68 64" MThd sequence
      if (i + 4 <= NUM_BYTES and found_MThd_Chunck(&MIDI[i])) {
        parsed_MThd = true;
        i += 4;
        continue;
      }
    } else if (not parsed_bytes_in_MThd_header) {  // look at next 4 bytes for size of MThd header
      if (i + 4 <= NUM_BYTES) {
        bytes_in_MThd_header = (((uint32_t)(MIDI[i])) << 24) | (((uint32_t)(MIDI[i + 1])) << 16) | (((uint32_t)(MIDI[i + 2])) << 8) | ((uint32_t)(MIDI[i + 3]));
        assert(bytes_in_MThd_header == 6);  //assuming 6 bytes of header after MThd
        parsed_bytes_in_MThd_header = true;
        i += 4;
        continue;
      }
    } else if (not parsed_ticks_per_quarter_note) {
      i += 4;  //skip 2 byte-format and 2-byte track count

      // should now be at the ticks quarter note field
      TICKS_PER_QUARTER_NOTE = (((uint32_t)(MIDI[i])) << 8) | ((uint32_t)(MIDI[i + 1]));
      parsed_ticks_per_quarter_note = true;

      i += 2;
      continue;
    } else if (not parsed_MTrk) {  // look at next 4 bytes for "4D 54 72 6B" MThd sequence
      if (i + 4 <= NUM_BYTES and found_MTrk_Chunck(&MIDI[i])) {
        parsed_MTrk = true;
        i += 4;
        continue;
      }
    } else if (not parsed_bytes_in_MTrk_header) {  // look at next 4 bytes for size of MThd header
      if (i + 4 <= NUM_BYTES) {
        bytes_in_MTrk_header = (((uint32_t)(MIDI[i])) << 24) | (((uint32_t)(MIDI[i + 1])) << 16) | (((uint32_t)(MIDI[i + 2])) << 8) | ((uint32_t)(MIDI[i + 3]));
        parsed_bytes_in_MTrk_header = true;
        i += 4;
        continue;
      }
    } else {  //parse the MTrk chunck:
      int bytes_read;
      uint64_t dt_ticks = decodeVariableLengthEncoding(&MIDI[i], bytes_read);
      i += bytes_read;




      if (found_Time_Signature(&MIDI[i])) {
        i += 3;
        //not at the actual time signature info
        i += 4;
        // TODO parse time signature
        continue;
      } else if (found_Set_Tempo_Event(&MIDI[i])) {
        i += 3;
        MICROSECONDS_PER_BEAT = (((uint32_t)(MIDI[i])) << 16) | (((uint32_t)(MIDI[i + 1])) << 8) | ((uint32_t)(MIDI[i + 2]));
        i += 3;
        continue;
      } else if (found_Track_Name(&MIDI[i])) {
        i += 2;
        // at the byte which specifies length of track name
        uint8_t track_name_size = MIDI[i];

        // at the first byte of track name
        i += 1;

        // ignore track name

        i += track_name_size;
        continue;
      } else if (found_Program_Change(&MIDI[i])) {  // ignore this
        i += 2;
        continue;
      } else if (found_Note_ON(&MIDI[i])) {
        //ignoring velocity
        notes[NUM_NOTES_FOUND] = NOTE_t{ .duration = dt_ticks, .note = MIDI[i + 1], .ON = true };
        i += 3;
        NUM_NOTES_FOUND++;
        assert(NUM_NOTES_FOUND <= MAX_NUM_NOTES);
        continue;
      } else if (found_Note_OFF(&MIDI[i])) {
        //ignoring velocity
        notes[NUM_NOTES_FOUND] = NOTE_t{ .duration = dt_ticks, .note = MIDI[i + 1], .ON = false };
        i += 3;
        NUM_NOTES_FOUND++;
        assert(NUM_NOTES_FOUND <= MAX_NUM_NOTES);
        continue;
      } else if (found_End_Of_Track(&MIDI[i])) {
        return;
      } else {
        //should not reach here
        assert(false);
      }
    }
    //should not reach here
    assert(false);
  }
}



void printMIDI() {
  Serial.print("ticks/beat: ");
  Serial.println(TICKS_PER_QUARTER_NOTE);

  Serial.print("uS/beat: ");
  Serial.println(MICROSECONDS_PER_BEAT);

  Serial.print("NUM_NOTES_FOUND: ");
  Serial.println(NUM_NOTES_FOUND);

  for (int i = 0; i < NUM_NOTES_FOUND; i++) {
    Serial.print("Note");
    Serial.print(i);
    Serial.print(": [");

    Serial.print("Duration: ");
    Serial.print(notes[i].duration);

    if (notes[i].ON) {
      Serial.print("\t ON ");
    } else {
      Serial.print("\t OFF ");
    }

    Serial.print(" \t Note: ");
    Serial.print(notes[i].note);

    Serial.println("]");
  }
}