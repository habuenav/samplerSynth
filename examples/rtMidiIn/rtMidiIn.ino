#include <samplerSynth.h>
#include <MIDI.h>

struct CustomBaudRate : public MIDI_NAMESPACE::DefaultSettings {  static const long BaudRate = 31250;};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI,CustomBaudRate);
USING_NAMESPACE_MIDI;
typedef Message<MIDI_NAMESPACE::DefaultSettings::SysExMaxSize> MidiMessage;

bool velocidad=true;

void onMessage(const MidiMessage& message) {
  byte instr,speed;
  
  switch (message.type) 
   {
    case NoteOn:  speed=velocidad?message.data2:100; notaOn(message.data1,speed);  break;
    case NoteOff: notaOff(message.data1);     break;
    case ProgramChange: instr=(message.data1>12)?12:message.data1; setInstrumento(instr); break;
    case ControlChange: alterPitchNota(message.data2);   break;
    case PitchBend:  alterVolNota(message.data2);   break;
  }
}
void handleStart(){  setSustainNota(0); }
void handleContinue(){ velocidad=!velocidad; }
void handleStop(){  setSustainNota(700); }
// -----------------------------------------------------------------------------
void setup()
{
  initSynth(16,25,22);
  delay(100);
  setVolumen(40);
  setInstrumento(0);
  Serial.begin(115200);
   
   MIDI.setHandleMessage(onMessage);
   MIDI.setHandleStart(handleStart);
   MIDI.setHandleContinue(handleContinue);
   MIDI.setHandleStop(handleStop);
   MIDI.begin(MIDI_CHANNEL_OMNI);
}
void loop()
{
   MIDI.read();
}
