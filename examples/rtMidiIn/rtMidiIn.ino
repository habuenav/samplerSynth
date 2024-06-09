#include <samplerSynth.h>
#include <midiRead.h>

bool velocidad=true;
void onMidi(MidiMessageTypes type, int channel, int data1, int data2)
{   byte instr,speed;
  switch(type){
    case NoteOn:  speed=velocidad?data2:100; notaOn(data1,speed);  break;
    case NoteOff: notaOff(data1);     break;
    case ProgramChange: instr=(data1>12)?12:data1; setInstrumento(instr); break;
    case ControlChange: alterPitchNota(data2);   break;
    case PitchBend:  alterVolNota(data2);   break;
  }
}
MidiRead midi(Serial1,31250);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  initSynth(16,25,22);
  delay(100);
  setVolumen(30);
  setInstrumento(1);
  midi.handleMidi(onMidi);
}

// the loop function runs over and over again forever
void loop() {
       midi.read();            // wait for a second
}
