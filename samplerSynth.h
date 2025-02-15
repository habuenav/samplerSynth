#ifndef SAMPLERSYNTH_H
#define SAMPLERSYNTH_H
#include <driver/i2s_std.h>
#include <LittleFS.h>
#include <FastMapLite.h>
#include "progmem/instrSamples.h"

#if defined(CONFIG_SPIRAM_BOOT_INIT)
#define _SLOTS 15  // Número de sonidos en PSRAM (0-15)
#else
#define _SLOTS 1   // Número de sonidos en RAM (0-1)
#endif 

// Definiciones
#define _SAMPLE_RATE 44100 
#define _MAX_CHANNELS 16
#define getElement(x)  pgm_read_byte(&x)
//#define MESSAGES             // manages most of the print and 
#if defined MESSAGES
   #define printLine(x)       Serial.println(x)
#else
   #define printLine(x)
#endif
#define Millis() esp_timer_get_time() * 0.001f // Reemplazo de millis() para mayor eficiencia
#define enRango(v, rI, rS) ((v > rS) ? rS : ((v < rI) ? rI : v))
#define _BUFFER 256  // Buffer de audio
#define _FREQBASE 0.0038222562f 
#define WAVETABLELENGTH 336

uint8_t sinInt[WAVETABLELENGTH];
uint8_t triInt[WAVETABLELENGTH];
uint8_t sawInt[WAVETABLELENGTH];

void InitWaveTables() 
{   for (int i = 0; i < WAVETABLELENGTH; i++) 
      { sinInt[i] = sin(i * (2*M_PI)/(WAVETABLELENGTH)) *256;
       // Triangular
        if (i < WAVETABLELENGTH / 4) { triInt[i] = (i * 255) / (WAVETABLELENGTH / 4); }
        else if (i < 3 * WAVETABLELENGTH / 4) { triInt[i] = 256 - ((i - WAVETABLELENGTH / 4) * 512) / (WAVETABLELENGTH / 2); }
        else { triInt[i] = -256 + ((i - 3 * WAVETABLELENGTH / 4) * 256) / (WAVETABLELENGTH / 4); }
        // Diente de sierra
        sawInt[i] = -256 + ((int32_t)i * 512) / WAVETABLELENGTH;
    }
}

TaskHandle_t _synthTaskHandle;       
i2s_chan_handle_t _i2sConfigHand;

FastMapLite mapVolume;
char** _wavInstr;

struct Instrumento 
{   char* nombre;
    uint32_t tamSample = 0, loopIni = 0, loopFin = 0;
    uint8_t* data = nullptr;
};

Instrumento _instrumento[] = 
{   {.nombre = "SinWave", .tamSample = WAVETABLELENGTH, .loopIni = 0, .loopFin = WAVETABLELENGTH, .data = sinInt},
    {.nombre = "TriWave", .tamSample = WAVETABLELENGTH, .loopIni = 0, .loopFin = WAVETABLELENGTH, .data = triInt},
    {.nombre = "SawWave", .tamSample = WAVETABLELENGTH, .loopIni = 0, .loopFin = WAVETABLELENGTH, .data = sawInt},
    {.nombre = "Acordeon", .tamSample = 50263, .loopIni = 25056, .loopFin = 50240, .data =(uint8_t*) acordeon},
    {.nombre = "Celesta", .tamSample = 39155, .loopIni = 38448, .loopFin = 39120, .data =(uint8_t*) celesta},
    {.nombre = "Guitarra", .tamSample = 44297, .loopIni = 26730, .loopFin = 44284, .data =(uint8_t*) guitarra},
    {.nombre = "Piano", .tamSample = 85279, .loopIni = 42446, .loopFin = 85236, .data =(uint8_t*) piano},
    {.nombre = "Saxofon", .tamSample = 43861, .loopIni = 21382, .loopFin = 43838, .data =(uint8_t*) saxofon},
    {.nombre = "Trompeta", .tamSample = 19925, .loopIni = 19574, .loopFin = 19910, .data =(uint8_t*) trompeta}
};

Instrumento _instrumentoWav[_SLOTS+1]; // Array para almacenar sonidos en la RAM
Instrumento* _Canal[_MAX_CHANNELS];

struct Global 
{   float synthVolumen = 0.15f, fadeOut = 1.00f,fadeIn= 0.001f;
    uint16_t sostenido = 600, fadeInSamples = 1000;
    uint8_t idxNota = 0 , notasActivas = 0, MAX_NOTAS = 8, sigSlot = 0;
    bool interpolation = false, effectReverb = false,effectChorus = false,effectTremolo = false, firFilter = false;
};
Global _Set;

struct Notas 
{   float frec = 0.0f, volNota = 0.0f, pos = 0.0f, velNota = 0.0f;
    uint32_t t_Ini = 0, t_Sus = 0;
    uint8_t notaMidi = 0, nCanal = 0;
    bool outFade = false, activa = false;  
};
Notas* _notas;

// Funciones inline
static inline float IRAM_ATTR midi2Freq(uint8_t midiNote) { return 8.17579891f * expf(0.05776226f * midiNote);  }
static inline int freq2Midi(float freq) { return 17.31234050f * (logf(0.12231220f) + logf(freq)); }

static inline String midi2Nota(uint8_t midiNote, uint8_t notacion = 0) 
{  const char* notes[2][12] = { {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, {"Do", "Do#", "Re", "Re#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"} };
    return String(notes[notacion][midiNote % 12]) + String((midiNote / 12) - 1);
}

uint8_t countFiles() 
{  uint8_t counter = 0;
   File root = LittleFS.open("/");
   while (root.openNextFile()) {counter++;}
   return counter;
}

void asignSampleArray(bool imprimir = true) {
    uint8_t counter = countFiles();
    File file, root = LittleFS.open("/");
    _wavInstr = new char*[counter]; // Asignar memoria para el array de punteros
    for (int i = 0; i < counter; i++) 
      { file = root.openNextFile();
        _wavInstr[i] = new char[strlen(file.name()) + 1]; // +1 para el terminador nulo
        strcpy(_wavInstr[i], strtok((char*)file.name(), "."));
        printLine(String(i) + " " + _wavInstr[i]); 
    }
}

bool setMaxNotas(uint8_t maxNotas = _Set.MAX_NOTAS) 
{  _Set.MAX_NOTAS = maxNotas;
   _notas = new Notas[_Set.MAX_NOTAS];
    if (_notas == nullptr) { printLine("No se pudo asignar memoria al array"); return false; }
    else { printLine("Max Notas: " + String(_Set.MAX_NOTAS));  return true; }
}

bool cargarWav(uint8_t nroCanal, uint8_t num, uint8_t slot) 
{   uint32_t tamSample;  
    uint8_t data_pos = 106, smpl_pos = 38, raw[115];
    bool result = true;  
    File file = LittleFS.open("/" + String(_wavInstr[num]) + ".wav", "r");
    if (!file) {result = false;  printLine("Error al leer el Sample");  }
    else 
      { printLine("Canal " + String(nroCanal) + " " + String(_wavInstr[num]));   
        file.read(raw, sizeof(raw)); // Leer el encabezado del archivo
        file.seek(114);
        tamSample = *(uint32_t*)&raw[data_pos + 4];
        free(_instrumentoWav[slot].data);
        #if defined(CONFIG_SPIRAM_BOOT_INIT)
        if (psramInit()) { _Set.dataTemp[slot] = (uint8_t*)heap_caps_malloc(tamSample, MALLOC_CAP_SPIRAM);  }
        #else
        _instrumentoWav[slot].data = new uint8_t[tamSample];  
        #endif
        if (_instrumentoWav[slot].data == nullptr) { printLine("No se pudo asignar memoria al array");}
        else 
           { _instrumentoWav[slot].tamSample = file.read(_instrumentoWav[slot].data, tamSample)-1; 
            printLine("Tam Sample Wav " + String(_instrumentoWav[slot].tamSample));   
            if (smpl_pos) 
              { _instrumentoWav[slot].loopIni = (*(uint32_t*)&raw[smpl_pos + 52]) * 2; // Inicio loop
                _instrumentoWav[slot].loopFin = (*(uint32_t*)&raw[smpl_pos + 56]) * 2; // Fin loop
                printLine("Loop Inicio: " + String(_instrumentoWav[slot].loopIni) + " Loop Fin: " + String(_instrumentoWav[slot].loopFin));   
              }
            file.close();
            #if defined(CONFIG_SPIRAM_BOOT_INIT) 
            printLine("Slot PSRAM usado: " + String(slot) + " PSRAM libre: " + String(ESP.getFreePsram()));
            #else
            printLine("Slot RAM usado: " + String(slot) + " RAM libre: " + String(ESP.getFreeHeap()));
            #endif
        }
    }
    _Canal[nroCanal] = &_instrumentoWav[slot];
    return result;
}

void setInstrumento(uint8_t nroCanal, uint8_t nroInstr = 0, uint8_t slot = 255) 
{   nroCanal =  nroCanal % _MAX_CHANNELS ;
    uint8_t nroProgInstr = sizeof(_instrumento) / sizeof(_instrumento[0]);
    if (nroInstr < nroProgInstr) 
      { _Canal[nroCanal] = &_instrumento[nroInstr];
        printLine("Canal " + String(nroCanal) + " " + String(_Canal[nroCanal]->nombre));   
        printLine("Tam Sample array " + String(_Canal[nroCanal]->tamSample));   
        printLine("Loop Inicio: " + String(_Canal[nroCanal]->loopIni) + " Loop Fin: " + String(_Canal[nroCanal]->loopFin));   
    } 
    else 
     {  slot = (slot == 255) ? _Set.sigSlot : slot;
         _Set.sigSlot = (_Set.sigSlot >= _SLOTS) ? 0 : ++_Set.sigSlot;
         cargarWav(nroCanal, nroInstr - nroProgInstr,slot);
    }
}

inline void notaOff(uint8_t nCanal, uint8_t nota) 
{ for (uint8_t i = 0; i < _Set.MAX_NOTAS; i++) 
      { if (_notas[i].activa && _notas[i].notaMidi == nota && !_notas[i].outFade && _notas[i].nCanal == nCanal) 
          { _notas[i].outFade = true;
            printLine("Nota Off: " + String(midi2Nota(nota, 1)) + " Notas Activas: " + String(_Set.notasActivas));   
            break;
         }
    }
}

inline void notaOn(uint8_t nCanal, uint8_t nota, uint8_t velocity = 127, int sostenido = _Set.sostenido) 
{  if (velocity == 0) {  notaOff(nCanal, nota); return;  } 
   _Set.idxNota = 0;
    for (uint8_t i = 0; i < _Set.MAX_NOTAS; i++){ if (!_notas[i].activa) { _Set.idxNota = i; _Set.notasActivas++; break; } }
    _notas[_Set.idxNota].t_Ini = Millis();
     _notas[_Set.idxNota].t_Sus = sostenido;
    _notas[_Set.idxNota].notaMidi = nota;
    _notas[_Set.idxNota].frec = midi2Freq(nota);
    _notas[_Set.idxNota].volNota = mapVolume.map(velocity);
    _notas[_Set.idxNota].velNota = _notas[_Set.idxNota].frec * _FREQBASE;
    _notas[_Set.idxNota].pos = 0;
    _notas[_Set.idxNota].activa = true;
    _notas[_Set.idxNota].outFade = false;
    _notas[_Set.idxNota].nCanal = nCanal; 
    _Set.idxNota++;
    printLine("Canal: " + String(nCanal) + " Nota On: " + String(midi2Nota(nota, 1)) + " Notas Activas: " + String(_Set.notasActivas));   
}

void allNotaOff() 
{ for (uint8_t i = 0; i < _Set.MAX_NOTAS; i++) 
     { if (_notas[i].activa) 
        {  _notas[i].activa = false;
           _notas[i].t_Ini = _notas[i].t_Sus = _notas[i].notaMidi = _notas[i].frec = _notas[i].pos = 0;
        }
    }
    _Set.notasActivas = 0;
}

inline void alterVolNota(uint8_t nCanal, uint8_t velocity) 
{ for (uint8_t i = 0; i < _Set.MAX_NOTAS; i++) 
      { if (_notas[i].activa && _notas[i].nCanal == nCanal) 
         {  _notas[i].volNota = mapVolume.map(velocity);
           break;             
        }
    }
}

inline void alterPitchNota(uint8_t nCanal, uint8_t cant) 
{  for (uint8_t i = 0; i < _Set.MAX_NOTAS; i++) 
      { if (_notas[i].activa && _notas[i].nCanal == nCanal) 
         { _notas[i].frec = midi2Freq(_notas[i].notaMidi) + map(cant, 0, 127, -16, 17);
            _notas[i].velNota = _notas[i].frec * _FREQBASE;
            break;
        }
    }
}

void setDelayNota(uint8_t dFade) 
{  dFade = constrain(dFade, 0, 127);
   _Set.fadeOut = (dFade < 21) ? 0.00009f - dFade * 0.000003f : 0.00003f - (dFade - 21) * 0.00000021f;
}

void setFadeIn(uint16_t frames) {     _Set.fadeInSamples = constrain(frames, 0, 65535); _Set.fadeIn=1/_Set.fadeInSamples; }
void setSustainNota(uint16_t sustainNota) {     _Set.sostenido = constrain(sustainNota, 0, 65535); }
void setVolumen(uint8_t vol) { _Set.synthVolumen = constrain(vol, 0, 100) / 100.0f; }

static inline void IRAM_ATTR synthProcess(void* pvParameters) 
{   uint32_t t_act, ii; // Tiempo actual
    int32_t muestra; // Datos del sample
    int16_t audioBuffer[_BUFFER], dato1, j = 0; // Buffer de audio
    float fadeFactor;
    size_t bytesWritten;
    for (;;) 
       { muestra=0; 
         for (uint8_t i = 0; i < _Set.MAX_NOTAS; i++) 
             { if (_notas[i].activa) 
               { if (_notas[i].outFade) 
                   { if (_notas[i].volNota -= _Set.fadeOut < 0.0f) 
                      { _notas[i].activa = _notas[i].outFade = false;
                        --_Set.notasActivas;
                           continue;
                      }
                   }
                if (_notas[i].pos < _Canal[_notas[i].nCanal]->tamSample) 
                   {
                   ii= ((uint32_t)_notas[i].pos & 1)?_notas[i].pos+1:_notas[i].pos;
                   dato1 = getElement(_Canal[_notas[i].nCanal]->data[ii]) | (getElement(_Canal[_notas[i].nCanal]->data[ii + 1]) << 8);
                    // Aplicar efecto de "fade in" al inicio de cada nota
                    fadeFactor = (_notas[i].pos < _Set.fadeInSamples ) ? _notas[i].pos * _Set.fadeIn : 1.0f;
                    muestra += dato1 * (_Set.synthVolumen * _notas[i].volNota * fadeFactor);
                    _notas[i].pos += _notas[i].velNota;
                   }
              _notas[i].pos=(_notas[i].pos > _Canal[_notas[i].nCanal]->loopFin)?_Canal[_notas[i].nCanal]->loopIni:_notas[i].pos;
              if ((_notas[i].pos > _Canal[_notas[i].nCanal]->tamSample || _notas[i].t_Ini + _notas[i].t_Sus < t_act) &&  _notas[i].t_Sus > 0) {  _notas[i].outFade = true; }
            }
         }
     audioBuffer[j] = enRango(muestra, -32767, 32768);
     if (++j == _BUFFER) 
        { i2s_channel_write(_i2sConfigHand, audioBuffer, sizeof(audioBuffer), &bytesWritten, portMAX_DELAY);
          t_act = Millis();
          j=0;
          vTaskDelay(0); 
        }
    }
}

void initSynth(uint8_t bck = 26, uint8_t ws = 25, uint8_t data = 22) 
{ 
       i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
       i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(_SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {.bclk =(gpio_num_t)bck,.ws=(gpio_num_t)ws,.dout=(gpio_num_t)data}
        };
 	i2s_new_channel(&chan_cfg,&_i2sConfigHand, NULL);      
   i2s_channel_init_std_mode(_i2sConfigHand, &std_cfg);
	i2s_channel_enable(_i2sConfigHand);

    // Inicialización del sistema de archivos
    if (LittleFS.begin()) {
        _wavInstr = new char*[countFiles()];
        asignSampleArray(false);
        InitWaveTables();
        printLine("Almacenamiento inicio Correctamente"); 
    }
    // Configuración de las notas
    if (setMaxNotas()) 
       { mapVolume.init(0, 127, 0.0, 1.0);
        printLine("RAM total: " + String(ESP.getHeapSize()));
        printLine("RAM Libre: " + String(ESP.getFreeHeap()));
        #if defined(CONFIG_SPIRAM_BOOT_INIT)
        printLine("PSRAM total: " + String(ESP.getPsramSize()));
        printLine("PSRAM Libre: " + String(ESP.getFreePsram()));  
        #endif
        setInstrumento(0, 0); // Selecciona el instrumento inicial
        xTaskCreatePinnedToCore(synthProcess, "TaskSynth", 4096, NULL, tskIDLE_PRIORITY+1, &_synthTaskHandle, 0); 
    } 
    else { printLine("Error al Iniciar"); }
}
void pauseSynth() { vTaskSuspend(_synthTaskHandle); }
void resumeSynth() { vTaskResume(_synthTaskHandle); }
#endif // SAMPLERSYNTH_H
