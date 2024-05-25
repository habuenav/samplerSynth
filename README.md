# ESP32 samplerSynth
Librería para crear proyectos de audio con la placa ESP32. Utiliza muestras de audio para generar sonidos de instrumentos musicales y ofrece funciones para controlar la reproducción, duración de las notas y ajuste de volumen, entre otras características. Su principal objetivo es permitir a la comunidad maker interesada en la creación de novedosos instrumentos musicales enfocarse en la forma y funcionalidad de sus creaciones, logrando reproducir sonidos sin necesidad de hardware dedicado, como los módulos VS1053b, SAM2695, Wav Trigger o SmartWav 2, que pueden ser costosos y ocupar mucho espacio. Además, se enfoca en la facilidad de uso, siendo una opción accesible para programadores principiantes, a diferencia de otros proyectos avanzados como los de Marcel Licence o Phil Schatzmann. Esta librería facilita el desarrollo rápido y sencillo con una calidad de sonido aceptable para la mayoría de proyectos DIY.

## Características
* <b>Reproducción de Samples:</b> Soporte para la carga y reproducción de archivos WAV.
* <b>Polifonia:</b> Reproduce múltiples notas simultáneamente.
* <b>Efectos:</b> Reverb, tremolo y delay. (cuando se usa noteOff y se estable un tiempo de delay significativo) (En desarrollo)
* <b>Filtros:</b> Paso bajo y FIR para intentar mejorar un poco la calidad de sonido. (En desarrollo)
* <b>Control de Volumen:</b> Volumen general y Ajuste dinámico del volumen de las notas. (Velocity)
* <b>Salida I2S:</b> Utiliza el periférico I2S del ESP32 y un DAC externo (MAX98357). (Podria modificarse para no usar DAC externo pero baja mucho la calidad de sonido)
* <b>Fácil de Usar:</b> Codigo simple basado en funciones, programacion no orientada a objetos. 
* <b>Pequeño:</b> Toda la libreria es tan solo un archivo de menos de 15kb y ocupa aproximadamente 15% de espacio de almacenamiento de programa.
 los samples incluidos tambien ocupan poco espacio aproximadamente 700Kb de los casi 4Mb de memoria flash
* <b>Dependencias:</b> libreria LittleFS. 
  

## Funciones
* <b>initSynth(bck, ws, data):</b> Inicializa el sintetizador y configura los pines para la comunicación I2S.
* <b>setMaxNotas(maxNotas):</b> Establece el número máximo de notas simultáneas que pueden reproducirse. 8 por defecto maximo recomendado 16
* <b>setInstrumento(num):</b> Carga un sample de instrumento específico para ser reproducido.
* <b>setVolumen(vol):</b> Ajusta el volumen global del sintetizador.
* <b>setSustainNota(DuracionNota):</b> configura el tiempo que dura una nota por defecto al llamar la funcion noteOn
* <b>setDelayNota(dFade):</b> Configura el tiempo de fade usado en notaOff para silenciar la nota activa.
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



## Uso Basico
Primero, carga el sketch "firstRun" que está en la carpeta "examples" y luego ve a "Herramientas" y selecciona "ESP32 Sketch Data Upload". Esto cargará los samples en la memoria flash del ESP32. También es recomendable seleccionar la opción "NO OTA (Large APP)" en "Partition Scheme".<br/>
Inicializa el sintetizador con initSynth(bck, ws, data), donde bck, ws y data son los pines de la placa ESP32 utilizados para la comunicación I2S.<br/>
Carga un sample de instrumento utilizando setInstrumento(num), donde num es el índice del instrumento deseado.<br/>
Activa notas musicales utilizando notaOn(nota, velocity, duracion). Los parámetros velocity y duracion son opcionales; si duracion es 0, la nota tiene un sostenido indefinido.<br/>
Controla la reproducción de las notas utilizando las funciones de manejo de notas. Aplica efectos y filtros de sonido utilizando las funciones correspondientes.
#### Codigo de ejemplo
```
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
}
```

# Agradecimientos
Quiero expresar mi sincero agradecimiento a Turman por su contribución inicial en la función de reproducción de sonido que sirvió como punto de partida para el desarrollo de esta librería. Su código proporcionó una base sobre la cual construir y explorar nuevas funcionalidades.
Este es su Github https://github.com/turmandreams/Piano-ESP32 ahi encontraran tambien enlaces a su canal de youtube.

## Donaciones
Si te gusto este proyecto o simplemente te sientes generoso, considera invitarme una cerveza. ¡Salud! :beers:<br/>
<a href="https://www.paypal.com/donate/?business=T8UBSMVJ2QT9Y&no_recurring=0&item_name=%C2%A1Gracias+por+tu+apoyo%21%0ATu+donaci%C3%B3n+es+de+gran+ayuda+y+es+un+incentivo+para+seguir+mejorando.&currency_code=USD"><img src="https://www.paypalobjects.com/digitalassets/c/website/marketing/latam/mx/accept-payments-online/icons/img_btn-donate2x.png" height="80"></a><br/>
Si has realizado una donación, por favor escribeme un correo y te enviare una version ligeramente optimizada y totalmente comentada, ademas de recibir rápidamente cualquier actualización o mejora adicional que haga a la librería. ¡Tu apoyo es invaluable!
