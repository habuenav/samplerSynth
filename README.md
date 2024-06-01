#### **[[ENGLISH]](#spanish)**

# ESP32 samplerSynth
Librería para crear proyectos de audio con la placa ESP32. Utiliza muestras de audio para generar sonidos de instrumentos musicales y ofrece funciones para controlar la reproducción, duración de las notas y ajuste de volumen, entre otras características. Su principal objetivo es permitir a la comunidad maker interesada en la creación de novedosos instrumentos musicales enfocarse en la forma y funcionalidad de sus creaciones, logrando reproducir sonidos sin necesidad de hardware dedicado, como los módulos VS1053b, SAM2695, Wav Trigger o SmartWav 2, que pueden ser costosos y ocupar mucho espacio. Además, se enfoca en la facilidad de uso, siendo una opción accesible para programadores principiantes, a diferencia de otros proyectos avanzados como los de Marcel Licence o Phil Schatzmann. Esta librería facilita el desarrollo rápido y sencillo con una calidad de sonido aceptable para la mayoría de proyectos DIY.
## Características
* <b>Reproducción de Samples:</b> Soporte para la carga y reproducción de archivos WAV.
* <b>Polifonia:</b> Reproduce múltiples notas simultáneamente.
* <b>Control de Volumen:</b> Volumen general y Ajuste dinámico del volumen de las notas. (Velocity)
* <b>Salida I2S:</b> Utiliza el periférico I2S del ESP32 y un DAC externo (MAX98357). (Podria modificarse para no usar DAC externo pero baja mucho la calidad de sonido)
* <b>Fácil de Usar:</b> Codigo simple basado en funciones, programacion no orientada a objetos. 
* <b>Pequeño:</b> Toda la libreria es tan solo un archivo de menos de 15kb y ocupa aproximadamente 15% de espacio de almacenamiento de programa.
 los samples incluidos tambien ocupan poco espacio aproximadamente 700Kb de los casi 4Mb de memoria flash.
* <b>Dependencias:</b> libreria LittleFS, libreria MIDI opcional [https://github.com/FortySevenEffects/arduino_midi_library] (usada en el ejemplo rtMidiIn)
### En Desarrollo
* <b>Efectos:</b> Reverb, tremolo y delay. (cuando se usa noteOff y se estable un tiempo de delay significativo)
* <b>Filtros:</b> Paso bajo y FIR para intentar mejorar un poco la calidad de sonido.
## Funciones
* <b>initSynth(bck, ws, data):</b> Inicializa el sintetizador y configura los pines para la comunicación I2S.
* <b>setMaxNotas(maxNotas):</b> Establece el número máximo de notas simultáneas que pueden reproducirse. 8 por defecto maximo recomendado 16
* <b>setInstrumento(num):</b> Carga un sample de instrumento específico para ser reproducido.
* <b>setVolumen(vol):</b> Ajusta el volumen global del sintetizador de (0-100)
* <b>setSustainNota(DuracionNota):</b> configura el tiempo en milisegundos por defecto que dura una nota al llamar la funcion noteOn (0-65535) si es 0 suena indefinidamente.
* <b>setDelayNota(dFade):</b> Configura el tiempo de desvanecido usado en notaOff para silenciar la nota activa (0-127) 0 inmediato 127 duración maxima.
* <b>notaOn(nota, velocity, duracion):</b> Activa una nota musical con la frecuencia correspondiente, volumen y duración especificados.
* <b>notaOff(nota):</b> Desactiva una nota musical gradualmente, disminuyendo su volumen durante un tiempo determinado antes de silenciarla por completo.
* <b>allnotaOff():</b> Desactiva todas las notas musicales actualmente activas.
* <b>alterVolNota(velocity):</b> Ajusta el volumen de una nota inclusive si esta activa.
* <b>alterPitchNota(cant):</b> Ajusta el tono de una nota inclusive si esta activa.
* <b>pauseSynth():</b> Pausa la tarea del sintetizador y libera recursos.
* <b>resumeSynth():</b> Reanuda la tarea del sintetizador.
## Samples de Instrumentos Incluidos
Los samples tienen una duración de entre 0.4 y 1.1 segundos y un tamaño de 30 a 65 KB, con una tasa de muestreo de 32 kHz y 16 bits. Gracias a su pequeño tamaño y a los aproximadamente 4 MB de memoria flash del ESP32, se pueden agregar muchos más samples (instrumentos). Lista de instrumentos con número y nombre: 0 Acordeón, 1 Bajo, 2 Banjo, 3 Cello, 4 Clarinete, 5 Flauta, 6 Guitarra, 7 Marimba, 8 Piano, 9 Saxofón, 10 Trompeta, 11 Ukulele, 12 Violín.
## Conexion
<img src='https://svgshare.com/i/16Rz.svg' title='' />
## Uso Basico
Primero, carga el sketch "firstRun" que está en la carpeta "examples" y luego ve a "Herramientas" y selecciona "ESP32 Sketch Data Upload". Esto cargará los samples en la memoria flash del ESP32. También es recomendable seleccionar la opción "NO OTA (Large APP)" en "Partition Scheme".<br>
Inicializa el sintetizador con initSynth(bck, ws, data), donde bck=16, ws=25 y data=22 son los pines por defecto usados en la placa ESP32 utilizados para la comunicación I2S.<br>
Carga un sample de instrumento utilizando setInstrumento(num), donde num es el índice del instrumento deseado.<br/>
Activa notas musicales utilizando notaOn(nota, velocity, duracion). Los parámetros velocity y duracion son opcionales; si duracion es 0, la nota tiene un sostenido indefinido.<br>
Controla la reproducción de las notas utilizando las funciones de manejo de notas. Aplica efectos y filtros de sonido utilizando las funciones correspondientes.

#### Codigo de ejemplo
```
#include <samplerSynth.h>

void setup() {
  Serial.begin(115200);
  initSynth();
  setVolumen(3);
  setInstrumento(6);
}
byte nota=60;
void loop() {
notaOn(nota);
delay(500);
nota++;
if(nota==72){nota=60; delay(5000);}   
}
```

# Agradecimientos
Quiero expresar mi sincero agradecimiento a Turman por su contribución inicial en la función de reproducción de sonido que sirvió como punto de partida para el desarrollo de esta librería. Su código proporcionó una base sobre la cual construir y explorar nuevas funcionalidades.
Este es su Github https://github.com/turmandreams/Piano-ESP32 ahi encontraran tambien enlaces a su canal de youtube.
## Comunidad
¡Las contribuciones son bienvenidas! Si tienes sugerencias, encuentra un error o deseas agregar una nueva característica, por favor abre un issue o envía un pull request.
## Donaciones
Si te gusto este proyecto o simplemente te sientes generoso, considera invitarme una cerveza. ¡Salud! :beers:<br/>
<a href="https://www.paypal.com/donate/?business=T8UBSMVJ2QT9Y&no_recurring=0&item_name=%C2%A1Gracias+por+tu+apoyo%21%0ATu+donaci%C3%B3n+es+de+gran+ayuda+y+es+un+incentivo+para+seguir+mejorando.&currency_code=USD"><img src="https://www.paypalobjects.com/digitalassets/c/website/marketing/latam/mx/accept-payments-online/icons/img_btn-donate2x.png" height="80"></a><br/>
Si has realizado una donación, por favor escribeme un correo y te enviare una version ligeramente optimizada y totalmente comentada, ademas de recibir rápidamente cualquier actualización o mejora adicional que haga a la librería. ¡Tu apoyo es invaluable!

#### **[[SPANISH]](#english)** 

# ESP32 samplerSynth
Library to create audio projects with the ESP32 board. It uses audio samples to generate musical instrument sounds and offers functions to control playback, note duration, and volume adjustment, among other features. Its main objective is to allow the maker community interested in the creation of new musical instruments to focus on the form and functionality of their creations, being able to reproduce sounds without the need for dedicated hardware, such as the VS1053b, SAM2695, Wav Trigger or SmartWav 2 modules, which They can be expensive and take up a lot of space. In addition, it focuses on ease of use, being an accessible option for beginner programmers, unlike other advanced projects such as those by Marcel Licence or Phil Schatzmann. This library facilitates quick and easy development with acceptable sound quality for most DIY projects.
## Characteristics
* <b>Sample Playback:</b> Support for loading and playing WAV files.
* <b>Polyphony:</b> Play multiple notes simultaneously.
* <b>Volume Control:</b> General volume and Dynamic adjustment of note volume. (Velocity)
* <b>I2S output:</b> Uses the I2S peripheral of the ESP32 and an external DAC (MAX98357). (It could be modified to not use external DAC but it lowers the sound quality a lot)
* <b>Easy to Use:</b> Simple function-based code, non-object-oriented programming.
* <b>Small:</b> The entire library is just one file of less than 15kb and takes up approximately 15% of program storage space.
 The included samples also take up little space, approximately 700Kb of the almost 4Mb of flash memory.
* <b>Dependencies:</b> LittleFS library, optional MIDI library [https://github.com/FortySevenEffects/arduino_midi_library] (used in the rtMidiIn example)
### Developing
* <b>Effects:</b> Reverb, tremolo and delay. (when noteOff is used and a significant delay time is set)
* <b>Filters:</b> Low pass and FIR to try to improve the sound quality a little.
## Features
* <b>initSynth(bck, ws, data):</b> Initializes the synthesizer and configures the pins for I2S communication.
* <b>setMaxNotes(maxNotes):</b> Sets the maximum number of simultaneous notes that can be played. 8 default maximum recommended 16
* <b>setInstrument(num):</b> Loads a specific instrument sample to be played.
* <b>setVolume(vol):</b> Sets the overall synth volume from (0-100)
* <b>setSustainNota(NoteDuration):</b> configures the default time in milliseconds that a note lasts when calling the noteOn function (0-65535) if it is 0 it sounds indefinitely.
* <b>setDelayNota(dFade):</b> Sets the fade time used in noteOff to mute the active note (0-127) 0 immediate 127 maximum duration.
* <b>noteOn(note, velocity, duration):</b> Activates a musical note with the corresponding frequency, volume and duration specified.
* <b>noteOff(note):</b> Turns off a musical note gradually, decreasing its volume for a set amount of time before muting it completely.
* <b>allnotaOff():</b> Disables all currently active musical notes.
* <b>alterNoteVol(velocity):</b> Adjusts the volume of a note even if it is active.
* <b>alterPitchNote(qty):</b> Adjusts the pitch of a note even if it is active.
* <b>pauseSynth():</b> Pauses the synthesizer task and frees resources.
* <b>resumeSynth():</b> Resume the synthesizer task.
## Instrument Samples Included
Samples have a duration between 0.4 and 1.1 seconds and a size of 30 to 65 KB, with a sampling rate of 32 kHz and 16 bits. Thanks to its small size and the ESP32's approximately 4 MB of flash memory, many more samples (instruments) can be added. List of instruments with number and name: 0 Accordion, 1 Bass, 2 Banjo, 3 Cello, 4 Clarinet, 5 Flute, 6 Guitar, 7 Marimba, 8 Piano, 9 Saxophone, 10 Trumpet, 11 Ukulele, 12 Violin.
## Connection
<img src='https://svgshare.com/i/16Rz.svg' title='' />

## Basic Use
First, upload the "firstRun" sketch that is in the "examples" folder and then go to "Tools" and select "ESP32 Sketch Data Upload". This will load the samples into the ESP32's flash memory. It is also advisable to select the "NO OTA (Large APP)" option in "Partition Scheme".<br/>
Initialize the synthesizer with initSynth(bck, ws, data), where bck=16, ws=25 and data=22 are the default pins used on the ESP32 board used for I2S communication.<br/>
Load an instrument sample using setInstrument(num), where num is the index of the desired instrument.<br/>
Activate musical notes using noteOn(note, velocity, duration). The velocity and duration parameters are optional; If duration is 0, the note has an indefinite sharp.<br/>
Control note playback using note management functions. Apply sound effects and filters using the corresponding functions.

#### Example code
```
#include <samplerSynth.h>

void setup() {
  Serial.begin(115200);
  initSynth();
  setVolumen(3);
  setInstrumento(6);
}
byte nota=60;
void loop() {
notaOn(nota);
delay(500);
nota++;
if(nota==72){nota=60; delay(5000);}   
}
```
# Thanks
I would like to express my sincere gratitude to Turman for his initial contribution to the sound playback function that served as the starting point for the development of this library. Its code provided a foundation on which to build and explore new functionality.
This is his Github https://github.com/turmandreams/Piano-ESP32 there you will also find links to his YouTube channel.
## Community
Contributions are welcome! If you have suggestions, find a bug, or want to add a new feature, please open an issue or submit a pull request.
## Donations
If you liked this project or are just feeling generous, consider buying me a beer. Health! :beers:<br/>
<a href="https://www.paypal.com/donate/?business=T8UBSMVJ2QT9Y&no_recurring=0&item_name=%C2%A1Gracias+por+tu+apoyo%21%0ATu+donaci%C3%B3n+es+de+gran+ayuda+y+es+un+incentivo+para+seguir+mejorando.&currency_code=USD"><img src="https://www.paypalobjects.com/digitalassets/c/website/marketing/latam/mx/accept-payments-online/icons/img_btn-donate2x.png" height="80"></a><br/>
If you have made a donation, please write me an email and I will send you a slightly optimized and fully commented version, in addition to quickly receiving any additional updates or improvements I make to the library. Your support is invaluable!
