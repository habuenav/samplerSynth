#include <samplerSynth.h>
#include <MIDI.h>

struct CustomBaudRate : public MIDI_NAMESPACE::DefaultSettings {
  static const long BaudRate = 31250;
};

//check Rx pin for Serial1 on your board, Wemos lolin32 lite is pin 26
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI,CustomBaudRate); 

void handleNoteOn(byte channel, byte pitch, byte velocity)
{    notaOn(pitch,velocity,0); 
}
void handleNoteOff(byte channel, byte pitch, byte velocity)
{    notaOff(pitch);
}

void setup()
{initSynth(16,25,22); //bck,ws,data 
delay(1000);
setVolumen(2);
setInstrumento(0);
MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
MIDI.setHandleNoteOff(handleNoteOff);
MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{   MIDI.read();
}
