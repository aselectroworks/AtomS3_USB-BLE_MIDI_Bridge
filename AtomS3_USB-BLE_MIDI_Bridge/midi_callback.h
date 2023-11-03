/*
MIT License

Copyright (c) 2023 aselectroworks

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

BEGIN_MIDI_NAMESPACE

// USB MIDI Callback
// Handle Message
// N/A
// Handle Error
// N/A
#define USBMIDINOTEOFFCALLBACK(n)                                \
    [](byte channel, byte note, byte velocity) {                 \
        Serial2.printf("RX: USBMIDI_%s NoteOff\r\n", #n);        \
        BleMIDI_Client[n]->sendNoteOff(note, velocity, channel); \
    }
#define USBMIDINOTEONCALLBACK(n)                                \
    [](byte channel, byte note, byte velocity) {                \
        Serial2.printf("RX: USBMIDI_%s NoteOn\r\n", #n);        \
        BleMIDI_Client[n]->sendNoteOn(note, velocity, channel); \
    }
#define USBMIDIAFTERTOUCHPOLYCALLBACK(n)                            \
    [](byte channel, byte note, byte velocity) {                    \
        Serial2.printf("RX: USBMIDI_%s AfterTouchPoly\r\n", #n);    \
        BleMIDI_Client[n]->sendAfterTouch(note, velocity, channel); \
    }
#define USBMIDICONTROLCHANGECALLBACK(n)                               \
    [](byte channel, byte number, byte value) {                       \
        Serial2.printf("RX: USBMIDI_%s ControlChange\r\n", #n);       \
        BleMIDI_Client[n]->sendControlChange(number, value, channel); \
    }
#define USBMIDIPROGRAMCHANGECALLBACK(n)                         \
    [](byte channel, byte number) {                             \
        Serial2.printf("RX: USBMIDI_%s ProgramChange\r\n", #n); \
        BleMIDI_Client[n]->sendProgramChange(number, channel);  \
    }
#define USBMIDIAFTERTOUCHCHANNELCALLBACK(n)                      \
    [](byte channel, byte number) {                              \
        Serial2.printf("RX: USBMIDI_%s AfterTouchbyte\r\n", #n); \
        BleMIDI_Client[n]->sendAfterTouch(number, channel);      \
    }
#define USBMIDIPITCHBENDCALLBACK(n)                         \
    [](byte channel, int value) {                           \
        Serial2.printf("RX: USBMIDI_%s PitchBend\r\n", #n); \
        BleMIDI_Client[n]->sendPitchBend(value, channel);   \
    }
#define USBMIDISYSTEMEXCLUSIVECALLBACK(n)                         \
    [](byte* array, unsigned size) {                              \
        Serial2.printf("RX: USBMIDI_%s SystemExclusive\r\n", #n); \
        Serial2.printf("SYSEX: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ", *(array + 0), *(array + 1), *(array + 2), *(array + 3), *(array + 4) \
        , *(array + 5), *(array + 6), *(array + 7), *(array + 8), *(array + 9)); \
        BleMIDI_Client[n]->sendSysEx(size, array, false);          \
    }
#define USBMIDITIMECODEQUARTERFRAMECALLBACK(n)                                 \
    [](byte inData) {                                   \
        Serial2.printf("RX: USBMIDI_%s TimeCodeQuarterFrame\r\n", #n);         \
        BleMIDI_Client[n]->sendTimeCodeQuarterFrame(inData); \
    }
#define USBMIDISONGPOSITIONCALLBACK(n)                         \
    [](unsigned beats) {                                       \
        Serial2.printf("RX: USBMIDI_%s SongPosition\r\n", #n); \
        BleMIDI_Client[n]->sendSongPosition(beats);            \
    }
#define USBMIDISONGSELECTCALLBACK(n)                         \
    [](byte songNumber) {                                    \
        Serial2.printf("RX: USBMIDI_%s SongSelect\r\n", #n); \
        BleMIDI_Client[n]->sendSongSelect(songNumber);       \
    }
#define USBMIDITUNEREQUESTCALLBACK(n)                         \
    [](void) {                                                \
        Serial2.printf("RX: USBMIDI_%s TuneRequest\r\n", #n); \
        BleMIDI_Client[n]->sendTuneRequest();                 \
    }
#define USBMIDICLOCKCALLBACK(n)                         \
    [](void) {                                          \
        Serial2.printf("RX: USBMIDI_%s Clock\r\n", #n); \
        BleMIDI_Client[n]->sendClock();                 \
    }
#define USBMIDISTARTCALLBACK(n)                         \
    [](void) {                                          \
        Serial2.printf("RX: USBMIDI_%s Start\r\n", #n); \
        BleMIDI_Client[n]->sendStart();                 \
    }
#define USBMIDITICKCALLBACK(n)                         \
    [](void) {                                         \
        Serial2.printf("RX: USBMIDI_%s Tick\r\n", #n); \
        BleMIDI_Client[n]->sendTick();                 \
    }
#define USBMIDICONTINUECALLBACK(n)                         \
    [](void) {                                             \
        Serial2.printf("RX: USBMIDI_%s Continue\r\n", #n); \
        BleMIDI_Client[n]->sendContinue();                 \
    }
#define USBMIDISTOPCALLBACK(n)                         \
    [](void) {                                         \
        Serial2.printf("RX: USBMIDI_%s Stop\r\n", #n); \
        BleMIDI_Client[n]->sendStop();                 \
    }
#define USBMIDIACTIVESENSINGCALLBACK(n)                         \
    [](void) {                                                  \
        Serial2.printf("RX: USBMIDI_%s ActiveSensing\r\n", #n); \
        BleMIDI_Client[n]->sendActiveSensing();                 \
    }
#define USBMIDISYSTEMRESETCALLBACK(n)                         \
    [](void) {                                                \
        Serial2.printf("RX: USBMIDI_%s SystemReset\r\n", #n); \
        BleMIDI_Client[n]->sendSystemReset();                 \
    }

// BLE MIDI Callback
#define BLEMIDINOTEOFFCALLBACK(n)                         \
    [](byte channel, byte note, byte velocity) {          \
        Serial2.printf("RX: BLEMIDI_%s NoteOff\r\n", #n); \
        UsbMIDI[n]->sendNoteOff(note, velocity, channel); \
    }
#define BLEMIDINOTEONCALLBACK(n)                         \
    [](byte channel, byte note, byte velocity) {         \
        Serial2.printf("RX: BLEMIDI_%s NoteOn\r\n", #n); \
        UsbMIDI[n]->sendNoteOn(note, velocity, channel); \
    }
#define BLEMIDIAFTERTOUCHPOLYCALLBACK(n)                         \
    [](byte channel, byte note, byte velocity) {                 \
        Serial2.printf("RX: BLEMIDI_%s AfterTouchPoly\r\n", #n); \
        UsbMIDI[n]->sendAfterTouch(note, velocity, channel);     \
    }
#define BLEMIDICONTROLCHANGECALLBACK(n)                         \
    [](byte channel, byte number, byte value) {                 \
        Serial2.printf("RX: BLEMIDI_%s ControlChange\r\n", #n); \
        UsbMIDI[n]->sendControlChange(number, value, channel);  \
    }
#define BLEMIDIPROGRAMCHANGECALLBACK(n)                         \
    [](byte channel, byte number) {                             \
        Serial2.printf("RX: BLEMIDI_%s ProgramChange\r\n", #n); \
        UsbMIDI[n]->sendProgramChange(number, channel);         \
    }
#define BLEMIDIAFTERTOUCHCHANNELCALLBACK(n)                      \
    [](byte channel, byte number) {                              \
        Serial2.printf("RX: BLEMIDI_%s AfterTouchbyte\r\n", #n); \
        UsbMIDI[n]->sendAfterTouch(number, channel);             \
    }
#define BLEMIDIPITCHBENDCALLBACK(n)                         \
    [](byte channel, int value) {                           \
        Serial2.printf("RX: BLEMIDI_%s PitchBend\r\n", #n); \
        UsbMIDI[n]->sendPitchBend(value, channel);          \
    }
#define BLEMIDISYSTEMEXCLUSIVECALLBACK(n)                         \
    [](byte* array, unsigned size) {                              \
        Serial2.printf("RX: BLEMIDI_%s SystemExclusive\r\n", #n); \
        UsbMIDI[n]->sendSysEx(size, array, false);                 \
    }
#define BLEMIDITIMECODEQUARTERFRAMECALLBACK(n)                          \
    [](byte typeNibble, byte valuesNibble) {                            \
        Serial2.printf("RX: BLEMIDI_%s TimeCodeQuarterFrame\r\n", #n);  \
        UsbMIDI[n]->sendTimeCodeQuarterFrame(typeNibble, valuesNibble); \
    }
#define BLEMIDISONGPOSITIONCALLBACK(n)                         \
    [](unsigned beats) {                                       \
        Serial2.printf("RX: BLEMIDI_%s SongPosition\r\n", #n); \
        UsbMIDI[n]->sendSongPosition(beats);                   \
    }
#define BLEMIDISONGSELECTCALLBACK(n)                         \
    [](byte songNumber) {                                    \
        Serial2.printf("RX: BLEMIDI_%s SongSelect\r\n", #n); \
        UsbMIDI[n]->sendSongSelect(songNumber);              \
    }
#define BLEMIDITUNEREQUESTCALLBACK(n)                         \
    [](void) {                                                \
        Serial2.printf("RX: BLEMIDI_%s TuneRequest\r\n", #n); \
        UsbMIDI[n]->sendTuneRequest();                        \
    }
#define BLEMIDICLOCKCALLBACK(n)                         \
    [](void) {                                          \
        Serial2.printf("RX: BLEMIDI_%s Clock\r\n", #n); \
        UsbMIDI[n]->sendClock();                        \
    }
#define BLEMIDISTARTCALLBACK(n)                         \
    [](void) {                                          \
        Serial2.printf("RX: BLEMIDI_%s Start\r\n", #n); \
        UsbMIDI[n]->sendStart();                        \
    }
#define BLEMIDITICKCALLBACK(n)                         \
    [](void) {                                         \
        Serial2.printf("RX: BLEMIDI_%s Tick\r\n", #n); \
        UsbMIDI[n]->sendTick();                        \
    }
#define BLEMIDICONTINUECALLBACK(n)                         \
    [](void) {                                             \
        Serial2.printf("RX: BLEMIDI_%s Continue\r\n", #n); \
        UsbMIDI[n]->sendContinue();                        \
    }
#define BLEMIDISTOPCALLBACK(n)                         \
    [](void) {                                         \
        Serial2.printf("RX: BLEMIDI_%s Stop\r\n", #n); \
        UsbMIDI[n]->sendStop();                        \
    }
#define BLEMIDIACTIVESENSINGCALLBACK(n)                         \
    [](void) {                                                  \
        Serial2.printf("RX: BLEMIDI_%s ActiveSensing\r\n", #n); \
        UsbMIDI[n]->sendActiveSensing();                        \
    }
#define BLEMIDISYSTEMRESETCALLBACK(n)                         \
    [](void) {                                                \
        Serial2.printf("RX: BLEMIDI_%s SystemReset\r\n", #n); \
        UsbMIDI[n]->sendSystemReset();                        \
    }

END_MIDI_NAMESPACE

void setMidiCallbacks() {
    // USB MIDI Callbacks
    // Handle Note On
    UsbMIDI[0]->setHandleNoteOn(USBMIDINOTEONCALLBACK(0));
    UsbMIDI[1]->setHandleNoteOn(USBMIDINOTEONCALLBACK(1));
    UsbMIDI[2]->setHandleNoteOn(USBMIDINOTEONCALLBACK(2));
    // Handle Note Off
    UsbMIDI[0]->setHandleNoteOff(USBMIDINOTEOFFCALLBACK(0));
    UsbMIDI[1]->setHandleNoteOff(USBMIDINOTEOFFCALLBACK(1));
    UsbMIDI[2]->setHandleNoteOff(USBMIDINOTEOFFCALLBACK(2));
    // Handle After Touch Poly
    UsbMIDI[0]->setHandleAfterTouchPoly(USBMIDIAFTERTOUCHPOLYCALLBACK(0));
    UsbMIDI[1]->setHandleAfterTouchPoly(USBMIDIAFTERTOUCHPOLYCALLBACK(1));
    UsbMIDI[2]->setHandleAfterTouchPoly(USBMIDIAFTERTOUCHPOLYCALLBACK(2));
    // Handle Control Change
    UsbMIDI[0]->setHandleControlChange(USBMIDICONTROLCHANGECALLBACK(0));
    UsbMIDI[1]->setHandleControlChange(USBMIDICONTROLCHANGECALLBACK(1));
    UsbMIDI[2]->setHandleControlChange(USBMIDICONTROLCHANGECALLBACK(2));
    // Handle Program Change
    UsbMIDI[0]->setHandleProgramChange(USBMIDIPROGRAMCHANGECALLBACK(0));
    UsbMIDI[1]->setHandleProgramChange(USBMIDIPROGRAMCHANGECALLBACK(1));
    UsbMIDI[2]->setHandleProgramChange(USBMIDIPROGRAMCHANGECALLBACK(2));
    // Handle After Touch Channel
    UsbMIDI[0]->setHandleAfterTouchChannel(USBMIDIAFTERTOUCHCHANNELCALLBACK(0));
    UsbMIDI[1]->setHandleAfterTouchChannel(USBMIDIAFTERTOUCHCHANNELCALLBACK(1));
    UsbMIDI[2]->setHandleAfterTouchChannel(USBMIDIAFTERTOUCHCHANNELCALLBACK(2));
    // Handle Pitch Bend
    UsbMIDI[0]->setHandlePitchBend(USBMIDIPITCHBENDCALLBACK(0));
    UsbMIDI[1]->setHandlePitchBend(USBMIDIPITCHBENDCALLBACK(1));
    UsbMIDI[2]->setHandlePitchBend(USBMIDIPITCHBENDCALLBACK(2));
    // Handle System Exclusive
    UsbMIDI[0]->setHandleSystemExclusive(USBMIDISYSTEMEXCLUSIVECALLBACK(0));
    UsbMIDI[1]->setHandleSystemExclusive(USBMIDISYSTEMEXCLUSIVECALLBACK(1));
    UsbMIDI[2]->setHandleSystemExclusive(USBMIDISYSTEMEXCLUSIVECALLBACK(2));
    // Handle Time Code Quarter Frame
#if defined(MIDI_TIMECODE_QUARTER_FRAME_TRANSFER_SUPPORT)
    UsbMIDI[0]->setHandleTimeCodeQuarterFrame(
        USBMIDITIMECODEQUARTERFRAMECALLBACK(0));
    UsbMIDI[1]->setHandleTimeCodeQuarterFrame(
        USBMIDITIMECODEQUARTERFRAMECALLBACK(1));
    UsbMIDI[2]->setHandleTimeCodeQuarterFrame(
        USBMIDITIMECODEQUARTERFRAMECALLBACK(2));
#endif
    // Handle Song Position
    UsbMIDI[0]->setHandleSongPosition(USBMIDISONGPOSITIONCALLBACK(0));
    UsbMIDI[1]->setHandleSongPosition(USBMIDISONGPOSITIONCALLBACK(1));
    UsbMIDI[2]->setHandleSongPosition(USBMIDISONGPOSITIONCALLBACK(2));
    // Handle Song Select
    UsbMIDI[0]->setHandleSongSelect(USBMIDISONGSELECTCALLBACK(0));
    UsbMIDI[1]->setHandleSongSelect(USBMIDISONGSELECTCALLBACK(1));
    UsbMIDI[2]->setHandleSongSelect(USBMIDISONGSELECTCALLBACK(2));
    // Handle Tune Request
    UsbMIDI[0]->setHandleTuneRequest(USBMIDITUNEREQUESTCALLBACK(0));
    UsbMIDI[1]->setHandleTuneRequest(USBMIDITUNEREQUESTCALLBACK(1));
    UsbMIDI[2]->setHandleTuneRequest(USBMIDITUNEREQUESTCALLBACK(2));
    // Handle Clock
#if defined(MIDI_CLOCK_TRANSFER_SUPPORT)
    UsbMIDI[0]->setHandleClock(USBMIDICLOCKCALLBACK(0));
    UsbMIDI[1]->setHandleClock(USBMIDICLOCKCALLBACK(1));
    UsbMIDI[2]->setHandleClock(USBMIDICLOCKCALLBACK(2));
#endif
    // Handle Start
    UsbMIDI[0]->setHandleStart(USBMIDISTARTCALLBACK(0));
    UsbMIDI[1]->setHandleStart(USBMIDISTARTCALLBACK(1));
    UsbMIDI[2]->setHandleStart(USBMIDISTARTCALLBACK(2));
    // Handle Tick
#if 0
    UsbMIDI[0]->setHandleTick(USBMIDITICKCALLBACK(0));
    UsbMIDI[1]->setHandleTick(USBMIDITICKCALLBACK(1));
    UsbMIDI[2]->setHandleTick(USBMIDITICKCALLBACK(2));
#endif
    // Handle Continue
    UsbMIDI[0]->setHandleContinue(USBMIDICONTINUECALLBACK(0));
    UsbMIDI[1]->setHandleContinue(USBMIDICONTINUECALLBACK(1));
    UsbMIDI[2]->setHandleContinue(USBMIDICONTINUECALLBACK(2));
    // Handle Stop
    UsbMIDI[0]->setHandleStop(USBMIDISTOPCALLBACK(0));
    UsbMIDI[1]->setHandleStop(USBMIDISTOPCALLBACK(1));
    UsbMIDI[2]->setHandleStop(USBMIDISTOPCALLBACK(2));
    // Handle ActiveSEnsing
    UsbMIDI[0]->setHandleActiveSensing(USBMIDIACTIVESENSINGCALLBACK(0));
    UsbMIDI[1]->setHandleActiveSensing(USBMIDIACTIVESENSINGCALLBACK(1));
    UsbMIDI[2]->setHandleActiveSensing(USBMIDIACTIVESENSINGCALLBACK(2));
    // Handle SystemReset
    UsbMIDI[0]->setHandleSystemReset(USBMIDISYSTEMRESETCALLBACK(0));
    UsbMIDI[1]->setHandleSystemReset(USBMIDISYSTEMRESETCALLBACK(1));
    UsbMIDI[2]->setHandleSystemReset(USBMIDISYSTEMRESETCALLBACK(2));

    // BLE MIDI Client Callbacks
    // Handle Note On
    BleMIDI_Client[0]->setHandleNoteOn(BLEMIDINOTEONCALLBACK(0));
    BleMIDI_Client[1]->setHandleNoteOn(BLEMIDINOTEONCALLBACK(1));
    BleMIDI_Client[2]->setHandleNoteOn(BLEMIDINOTEONCALLBACK(2));
    // Handle Note Off
    BleMIDI_Client[0]->setHandleNoteOff(BLEMIDINOTEOFFCALLBACK(0));
    BleMIDI_Client[1]->setHandleNoteOff(BLEMIDINOTEOFFCALLBACK(1));
    BleMIDI_Client[2]->setHandleNoteOff(BLEMIDINOTEOFFCALLBACK(2));
    // Handle After Touch Poly
    BleMIDI_Client[0]->setHandleAfterTouchPoly(
        BLEMIDIAFTERTOUCHPOLYCALLBACK(0));
    BleMIDI_Client[1]->setHandleAfterTouchPoly(
        BLEMIDIAFTERTOUCHPOLYCALLBACK(1));
    BleMIDI_Client[2]->setHandleAfterTouchPoly(
        BLEMIDIAFTERTOUCHPOLYCALLBACK(2));
    // Handle Control Change
    BleMIDI_Client[0]->setHandleControlChange(BLEMIDICONTROLCHANGECALLBACK(0));
    BleMIDI_Client[1]->setHandleControlChange(BLEMIDICONTROLCHANGECALLBACK(1));
    BleMIDI_Client[2]->setHandleControlChange(BLEMIDICONTROLCHANGECALLBACK(2));
    // Handle Program Change
    BleMIDI_Client[0]->setHandleProgramChange(BLEMIDIPROGRAMCHANGECALLBACK(0));
    BleMIDI_Client[1]->setHandleProgramChange(BLEMIDIPROGRAMCHANGECALLBACK(1));
    BleMIDI_Client[2]->setHandleProgramChange(BLEMIDIPROGRAMCHANGECALLBACK(2));
    // Handle After Touch Channel
    BleMIDI_Client[0]->setHandleAfterTouchChannel(
        BLEMIDIAFTERTOUCHCHANNELCALLBACK(0));
    BleMIDI_Client[1]->setHandleAfterTouchChannel(
        BLEMIDIAFTERTOUCHCHANNELCALLBACK(1));
    BleMIDI_Client[2]->setHandleAfterTouchChannel(
        BLEMIDIAFTERTOUCHCHANNELCALLBACK(2));
    // Handle Pitch Bend
    BleMIDI_Client[0]->setHandlePitchBend(BLEMIDIPITCHBENDCALLBACK(0));
    BleMIDI_Client[1]->setHandlePitchBend(BLEMIDIPITCHBENDCALLBACK(1));
    BleMIDI_Client[2]->setHandlePitchBend(BLEMIDIPITCHBENDCALLBACK(2));
    // Handle System Exclusive
    BleMIDI_Client[0]->setHandleSystemExclusive(
        BLEMIDISYSTEMEXCLUSIVECALLBACK(0));
    BleMIDI_Client[1]->setHandleSystemExclusive(
        BLEMIDISYSTEMEXCLUSIVECALLBACK(1));
    BleMIDI_Client[2]->setHandleSystemExclusive(
        BLEMIDISYSTEMEXCLUSIVECALLBACK(2));
    // Handle Time Code Quarter Frame
#if defined(MIDI_TIMECODE_QUARTER_FRAME_TRANSFER_SUPPORT)
    BleMIDI_Client[0]->setHandleTimeCodeQuarterFrame(
        BLEMIDITIMECODEQUARTERFRAMECALLBACK(0));
    BleMIDI_Client[1]->setHandleTimeCodeQuarterFrame(
        BLEMIDITIMECODEQUARTERFRAMECALLBACK(1));
    BleMIDI_Client[2]->setHandleTimeCodeQuarterFrame(
        BLEMIDITIMECODEQUARTERFRAMECALLBACK(2));
#endif
    // Handle Song Position
    BleMIDI_Client[0]->setHandleSongPosition(BLEMIDISONGPOSITIONCALLBACK(0));
    BleMIDI_Client[1]->setHandleSongPosition(BLEMIDISONGPOSITIONCALLBACK(1));
    BleMIDI_Client[2]->setHandleSongPosition(BLEMIDISONGPOSITIONCALLBACK(2));
    // Handle Song Select
    BleMIDI_Client[0]->setHandleSongSelect(BLEMIDISONGSELECTCALLBACK(0));
    BleMIDI_Client[1]->setHandleSongSelect(BLEMIDISONGSELECTCALLBACK(1));
    BleMIDI_Client[2]->setHandleSongSelect(BLEMIDISONGSELECTCALLBACK(2));
    // Handle Tune Request
    BleMIDI_Client[0]->setHandleTuneRequest(BLEMIDITUNEREQUESTCALLBACK(0));
    BleMIDI_Client[1]->setHandleTuneRequest(BLEMIDITUNEREQUESTCALLBACK(1));
    BleMIDI_Client[2]->setHandleTuneRequest(BLEMIDITUNEREQUESTCALLBACK(2));
    // Handle Clock
#if defined(MIDI_CLOCK_TRANSFER_SUPPORT)
    BleMIDI_Client[0]->setHandleClock(BLEMIDICLOCKCALLBACK(0));
    BleMIDI_Client[1]->setHandleClock(BLEMIDICLOCKCALLBACK(1));
    BleMIDI_Client[2]->setHandleClock(BLEMIDICLOCKCALLBACK(2));
#endif
    // Handle Start
    BleMIDI_Client[0]->setHandleStart(BLEMIDISTARTCALLBACK(0));
    BleMIDI_Client[1]->setHandleStart(BLEMIDISTARTCALLBACK(1));
    BleMIDI_Client[2]->setHandleStart(BLEMIDISTARTCALLBACK(2));
    // Handle Tick
    BleMIDI_Client[0]->setHandleTick(BLEMIDITICKCALLBACK(0));
    BleMIDI_Client[1]->setHandleTick(BLEMIDITICKCALLBACK(1));
    BleMIDI_Client[2]->setHandleTick(BLEMIDITICKCALLBACK(2));
    // Handle Continue
    BleMIDI_Client[0]->setHandleContinue(BLEMIDICONTINUECALLBACK(0));
    BleMIDI_Client[1]->setHandleContinue(BLEMIDICONTINUECALLBACK(1));
    BleMIDI_Client[2]->setHandleContinue(BLEMIDICONTINUECALLBACK(2));
    // Handle Stop
    BleMIDI_Client[0]->setHandleStop(BLEMIDISTOPCALLBACK(0));
    BleMIDI_Client[1]->setHandleStop(BLEMIDISTOPCALLBACK(1));
    BleMIDI_Client[2]->setHandleStop(BLEMIDISTOPCALLBACK(2));
    // Handle ActiveSEnsing
    BleMIDI_Client[0]->setHandleActiveSensing(BLEMIDIACTIVESENSINGCALLBACK(0));
    BleMIDI_Client[1]->setHandleActiveSensing(BLEMIDIACTIVESENSINGCALLBACK(1));
    BleMIDI_Client[2]->setHandleActiveSensing(BLEMIDIACTIVESENSINGCALLBACK(2));
    // Handle SystemReset
    BleMIDI_Client[0]->setHandleSystemReset(BLEMIDISYSTEMRESETCALLBACK(0));
    BleMIDI_Client[1]->setHandleSystemReset(BLEMIDISYSTEMRESETCALLBACK(1));
    BleMIDI_Client[2]->setHandleSystemReset(BLEMIDISYSTEMRESETCALLBACK(2));
}