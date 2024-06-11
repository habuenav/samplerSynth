#ifndef SAMPLERSYNTH_H
#define SAMPLERSYNTH_H

#include "soc/rtc_cntl_reg.h"
#include <driver/i2s.h>
#include <LittleFS.h>

#define MESSAGES             // manages most of the print and 
#if defined MESSAGES
   #define printLine(x)       Serial.println(x)
#else
   #define printLine(x)
#endif

#define SAMPLE_RATE 32000
#define BUFFER 512

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex(); // Create a mutex object     
TaskHandle_t synthTaskHandle;       

String instrumento[13] = {"/acordeon.wav","/bajo.wav","/banjo.wav","/cello.wav","/clarinete.wav","/flauta.wav","/guitarra.wav","/marimba.wav","/piano.wav","/saxofon.wav","/trompeta.wav","/ukulele.wav","/violin.wav"};

i2s_config_t i2sConfig;
i2s_pin_config_t pinConfig;

byte MAX_NOTES=8;
float frecBase=523.25f;
byte idxNota=0;
byte notasActivas=0;
float synthVolumen = 0.25f;
uint16_t tamSample = 0;
uint16_t DURATION=700;
byte *dataSample;
float fade=0.00009f;
uint32_t loopData[]={0,0};

struct Notas {
  uint32_t t_Ini=0;
  uint32_t t_Dur=0;
  bool iniFade=false;
  byte  notaMidi=0;
  float frec=0.0;
  float volNota=0.0f;
  float pos=0.0f;
  bool  activa=false;
  };
Notas *notas;

static inline float midiToFreq(float midiNote){  return 440.0f * powf(2.0f, (midiNote-69.0f)/12.0f);}
static inline int freqToMidi(float freq){  return round( 12.0f * (logf(freq / 440.0f) / log(2)) + 69);}
static inline String midiToNota(byte midiNote,byte notacion=0){ const char* notes[2][12] = {{"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"},{"Do","Do#","Re","Re#","Mi","Fa","Fa#","Sol","Sol#","LA","LA#","Si"}}; return (String(notes[notacion][midiNote % 12])+String(midiNote/12));}
bool setMaxNotas(byte maxNotas=MAX_NOTES)
{ MAX_NOTES=maxNotas;
 bool result=false;
 notas = new Notas[MAX_NOTES];
 if(notas == nullptr) { printLine("No se pudo asignar memoria al array"); } 
 else{ result=true; printLine("Max Notas: "+String(MAX_NOTES)); }
 return result;
}
bool setInstrumento(byte num)
{ byte data_pos,smpl_pos,raw[115];
  byte result=true;
  String nombrearchivo = instrumento[num];
  File file = LittleFS.open(nombrearchivo, "r");
  if (!file)  { result=false; printLine("Error al leer el Sample"); }
  else{
  printLine(nombrearchivo);
  file.seek(0);
  file.read(raw, sizeof(raw)); // leo 60 bytes donde se supone que está la información del loop start y loop end
   for(byte k=0;k<110;k++)
     { if(raw[k]=='d' && raw[k+1]=='a' && raw[k+2]=='t' && raw[k+3]=='a'){ data_pos=k;  printLine("Data pos: " + String(data_pos) ); }    
       else if(raw[k]=='s' && raw[k+1]=='m' && raw[k+2]=='p' && raw[k+3]=='l'){ smpl_pos=k;  printLine("SMPL pos: " + String(smpl_pos) ); }
     }
  file.seek(114);
  uint32_t tsonido=*(uint32_t*)&raw[data_pos+4];
  delete[] dataSample;
  dataSample = new byte[tsonido];
  if(dataSample == nullptr) { printLine("No se pudo asignar memoria al array");    }  
  tamSample=file.read(dataSample, tsonido); 
     if (smpl_pos) 
     { loopData[0]=(*(uint16_t*)&raw[smpl_pos+52]) *2; //inicio loop
       loopData[1]=(*(uint16_t*)&raw[smpl_pos+56])  *2; //fin loop
       printLine("Loop Inicio: " + String(loopData[0]) + " Loop Fin: " + String(loopData[1]));   
     }
  // Cerrar el archivo después de cargar la información
  file.close();
 printLine("Memoria libre: " + String(ESP.getFreeHeap()));
 }
return result;
}
inline void notaOff(byte nota)
{ for(byte i = 0; i <MAX_NOTES; i++) 
    { if(notas[i].activa && notas[i].notaMidi==nota && !notas[i].iniFade) {notas[i].iniFade = true;  notasActivas--;}
    }
}
inline void notaOn(byte nota,byte velocity=127,int duracion=DURATION)
{
  if(velocity==0){notaOff(nota);}
  else
  {idxNota=0;
  for(byte i = 0; i <MAX_NOTES; i++) { if(!notas[i].activa){idxNota=i;  notasActivas++; break;}  }
  notas[idxNota].t_Ini=millis();
  notas[idxNota].t_Dur=duracion;
  notas[idxNota].notaMidi=nota;
  notas[idxNota].frec=midiToFreq(nota);
  notas[idxNota].volNota=map(velocity,0,127,0,100)*0.01f;
  notas[idxNota].pos=0;
  notas[idxNota].activa=true;
  notas[idxNota].iniFade=false;
  idxNota++;
  printLine("Nota Actual: " + String(midiToNota(nota)) + " Notas Activas: " + String(notasActivas));  
  }  
}
inline void alterVolNota(byte velocity)
{ idxNota=0;
  for(byte i = 0; i <MAX_NOTES; i++) { if(notas[i].activa){idxNota=i; break;}  }
  notas[idxNota].volNota=map(velocity,0,127,0,100)/100.0f;
}
inline void alterPitchNota(byte cant)
{  idxNota=0;
  for(byte i = 0; i <MAX_NOTES; i++) { if(notas[i].activa){idxNota=i; break;}  }
  notas[idxNota].frec=midiToFreq(notas[idxNota].notaMidi)+map(cant,0,127,-16,17);
}
void setDelayNota(byte dFade) 
{dFade = constrain(dFade, 0, 127);
fade=(dFade<21)? 0.00009f + (dFade / 127.0f) * (0.00003f - 0.00009f): 0.00003f + (dFade / 127.0f) * (0.000003f - 0.00003f);
}
void setSustainNota(uint16_t sustainNota=DURATION) { DURATION = constrain(sustainNota, 0, 65535); }
void allNotaOff()
{ for(byte i = 0; i <MAX_NOTES; i++)
{ if(notas[i].activa)
  { notas[i].activa=false;
    notas[i].t_Ini=notas[i].t_Dur=notas[i].notaMidi=notas[i].frec=notas[i].pos=0;
  }
}
  notasActivas=0;
}
void setVolumen(byte vol)
{vol = constrain(vol, 0, 100);
synthVolumen = vol/100.0f;
}
static inline void IRAM_ATTR synth_process()
{ float vel; 
 uint32_t t_act,ii;//tiempo actual, indice de posicion de la muestra
 int16_t audioBuffer[BUFFER],muestra,dato,j=0; //bufer de audio,datos del sample, dato de la muestra sin procesar
 size_t bytesWritten;
 for(;;)
 {  muestra = 0;
    for(byte i = 0; i <MAX_NOTES; i++)
    {   vel=notas[i].frec/frecBase;
      if(notas[i].pos < tamSample && notas[i].activa)
          {  if ((ii=notas[i].pos)%2) {ii++;}
             dato=dataSample[ii] | dataSample[ii+1]<<8;
             muestra += dato*(synthVolumen*notas[i].volNota);
             notas[i].pos+=vel;
          }
         if(notas[i].pos > loopData[1]  && notas[i].activa) { notas[i].pos = loopData[0]; }           
         if (notas[i].iniFade){notas[i].volNota-=fade; if(notas[i].volNota<0.0f) { notas[i].activa=notas[i].iniFade=false; } }         
         if((notas[i].pos>tamSample || notas[i].t_Ini+notas[i].t_Dur < t_act ) && (notas[i].activa && notas[i].t_Dur>0)){notas[i].activa=notas[i].iniFade=false; notasActivas--; }
    }
   audioBuffer[j]=audioBuffer[j+1]=(muestra > 32766) ? 32767 : ((muestra < -32767) ? -32768 : muestra);
   if(j++ == BUFFER)
    {i2s_write(I2S_NUM_0, audioBuffer,sizeof(audioBuffer), &bytesWritten, portMAX_DELAY);
    t_act=millis(); 
    yield();
     j = 0;
    }
 }
}
void thread_synth(void * pvParameters)
{  for(;;)  
   {if(xSemaphoreTake(xMutex, portMAX_DELAY))
     { synth_process();
      xSemaphoreGive(xMutex); // release the mutex
      vTaskDelay(1);
     }
  }
}
void initSynth(byte bck=16, byte ws=25, byte data=22)
{   WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
   delay(100);
  // Configuración del bus I2S 
  i2sConfig.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_CHANNEL_MONO);
  i2sConfig.sample_rate = SAMPLE_RATE;
  i2sConfig.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  i2sConfig.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  i2sConfig.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S | I2S_COMM_FORMAT_I2S_MSB);
  i2sConfig.intr_alloc_flags = 0;
  i2sConfig.dma_buf_count = 8;
  i2sConfig.dma_buf_len = 64;
  i2sConfig.use_apll = false;
  i2sConfig.tx_desc_auto_clear = true;
  i2sConfig.fixed_mclk = 0;
   // Configurar el pinout del bus I2S
  pinConfig.bck_io_num = bck; // Pin de reloj de bit
  pinConfig.ws_io_num = ws; // Pin de selección de palabra
  pinConfig.data_out_num = data; // Pin de datos de salida
  pinConfig.data_in_num = -1; // No se utiliza la entrada de datos
  // Inicializar el bus I2S con la configuración
  i2s_driver_install(I2S_NUM_0, & i2sConfig, 0, NULL);
  i2s_set_pin(I2S_NUM_0, & pinConfig);
  i2s_set_sample_rates(I2S_NUM_0, SAMPLE_RATE);
  i2s_zero_dma_buffer(I2S_NUM_0); 
  i2s_start(I2S_NUM_0);
 if(LittleFS.begin() && setMaxNotas() &&  setInstrumento(0))  
  {delay(100);
  xTaskCreatePinnedToCore(thread_synth, "TaskSynth",4096, NULL, tskIDLE_PRIORITY+1, &synthTaskHandle,(CONFIG_ARDUINO_RUNNING_CORE) ? 0 : 1);
  printLine("Inicio Correcto");   
 }
  else { printLine("Error al Iniciar");  } 
}
void pauseSynth(){ vTaskSuspend(synthTaskHandle);  delete[] dataSample; dataSample = new byte[0];}
void resumeSynth(){ setInstrumento(0);  vTaskResume(synthTaskHandle); }
#endif // SAMPLERSYNTH_H
