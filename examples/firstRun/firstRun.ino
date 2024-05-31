#include <samplerSynth.h>
int miVolumen=2;
void pulsacion(){
  if(Serial.available()){         
        char c = Serial.read();                
        switch (c){          
            case 'q':  notaOn(60);  break;
            case 'w':  notaOn(62); break;
            case 'e':  notaOn(64); break;
            case 'r':  notaOn(65);  break;
            case 't':  notaOn(67);  break;
            case 'y':  notaOn(69);  break;
            case 'u':  notaOn(71);  break;
            case 'i':  notaOn(72);  break;
            
            case '0':  allNotaOff();  break;   
            case '1':  miVolumen=(miVolumen>0)?miVolumen-1:0;  setVolumen(miVolumen); Serial.println(synthVolumen); break;   
            case '2':  miVolumen=(miVolumen<20)?miVolumen+1:20; setVolumen(miVolumen);Serial.println(synthVolumen);  break;   
            case '3':  setDelayNota(20);  break;   
            case '4':  setDelayNota(120);  break;   
            //sostenido indefinido por lo tanto la nota suena hasta que se llame la funcion notaOff(nota) o allNotaOff()
            case '5':  setSustainNota(0);  break;   //sustained indefinitely therefore the note sounds until the noteOff(note) or allNotaOff() function is called
            //sostiene el sonido de la nota X cantidad de tiempo en milisegundos
            case '6':  setSustainNota(700); break;  //holds the sound of the note for X amount of time in milliseconds
            case '5':  pauseSynth();  break;   
            case '6':  resumeSynth(); break; 
            
            case 'a':  setInstrumento(0);    break;   
            case 's':  setInstrumento(1);   break;   
            case 'd':  setInstrumento(2);   break;   
            case 'f':  setInstrumento(3);   break;   
            case 'g':  setInstrumento(4);   break;   
            case 'h':  setInstrumento(5);   break; 
            case 'j':  setInstrumento(6);   break; 
            case 'k':  setInstrumento(7);   break; 
            case 'l':  setInstrumento(8);   break; 
            case 'z':  setInstrumento(9);   break; 
            case 'x':  setInstrumento(10);   break; 
            case 'c':  setInstrumento(11);   break; 
            case 'v':  setInstrumento(12);   break; 
        }   
     }
 }

void setup()
{
 Serial.begin(115200);
 setVolumen(miVolumen);
 initSynth(); 
 delay(1000);
}

void loop()
{
  pulsacion();
}
