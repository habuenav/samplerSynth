# ESP32 samplerSynth
Librería para crear proyectos de audio utilizando la placa ESP32. Utiliza muestras de audio para generar sonidos de instrumentos musicales y proporciona funciones para controlar la reproducción y duracion de las notas asi como para ajustar el volumen entre otras caracteristicas.
Fue desarrollada con la idea de crear instrumentos musicales que puedan reproducir su propio sonido sin necesidad de hardware dedicado a dicha funcion, como los modulos basados en el VS1053b, el SAM2695 o Wav Trigger o SmartWav 2 los cuales pueden llegar a ser costosos
y segun el caso ocupar mucho espacio. la calidad del sonido que ofrece esta libreria puede ser suficiente para la mayoria de proyectos DIY.

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
Los samples tiene una duracion de entre 0.4 y 1.1 Segundos y su tamañao es de 30 a 65 Kb, tiene un sample rate de 32Khz y 16bits, gracias a su pequeño tamaño
y los aproximadamente 4Mb de memoria flash del Esp32 se pueden agregar muchos mas samples (Instrumentos). Lista de instrumentos con numero y nombre:
0 Acordeon, 1 Bajo, 2 Banjo, 3 Cello, 4 Clarinete, 5 Flauta, 6 Guitarra, 7 Marimba, 8 Piano, 9 Saxofon, 10 Trompeta, 11 Ukulele, 12 Violin

## Conexion



## Uso Basico
Primero se debe cargar el sketch firstRun que esta en la carpeta examples y luego ir a herramientas y ejecutar la opcion Esp32 Sketch Data Upload
Esto cargara los samples en la memoria flash del Esp32, tambien es recomendable seleccionar la opcion NO OTA large APP en Partition Scheme.
Inicializar el sintetizador con initSynth(bck, ws, data), donde bck, ws y data son los pines de la placa ESP32 utilizados para la comunicación I2S.
Cargar un sample de instrumento utilizando setInstrumento(num), donde num es el índice del instrumento deseado.
Activar notas musicales utilizando notaOn(nota, velocity, duracion). el parametro velocity y duracion son opcionales, si duracion es 0, la nota es tiene un sostenido indefinido.
Controlar la reproducción de las notas utilizando las funciones de manejo de notas.
Aplicar efectos y filtros  de sonido utilizando las funciones correspondientes.

# Agradecimientos
Quiero expresar mi sincero agradecimiento a Turman por su contribución inicial en la función de reproducción de sonido que sirvió como punto de partida para el desarrollo de esta librería. Su código proporcionó una base sobre la cual construir y explorar nuevas funcionalidades.
Este es su Github https://github.com/turmandreams/Piano-ESP32 ahi encontraran tambien enlaces a su canal de youtube.

## Donacion 
Si te gusto este proyecto o simplemente te sientes generoso, considera invitarme una cerveza. ¡Salud! :beers:<br/>
<a href="https://www.paypal.com/donate/?business=T8UBSMVJ2QT9Y&no_recurring=0&item_name=%C2%A1Gracias+por+tu+apoyo%21%0ATu+donaci%C3%B3n+es+de+gran+ayuda+y+es+un+incentivo+para+seguir+mejorando.&currency_code=USD"><img src="https://www.paypalobjects.com/digitalassets/c/website/marketing/latam/mx/accept-payments-online/icons/img_btn-donate2x.png" height="80"></a>
