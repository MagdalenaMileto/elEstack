## tp-2016-1c-El-algoritmo-del-ritmo


#Protocolos  



##Consola - Nucleo  

**Handshake**  

Nucleo -> 101 -> Consola  
Consola -> 102 -> Nucleo  
Handshake exitoso  

CONSOLA -> 103 -> Nucleo  
mando en data el codigo  
  
NUCLEO -> 108 -> Consola
fin de proceso por desconexion abrupta con cpu


##UMC - Nucleo  

**Handshake**  

UMC -> 201 -> Nucleo  
Nucleo -> 202 -> UMC  
Handshake exitoso  

Nucleo -> 204 -> UMC    
Nuevo proceso. Pregunto si tiene espacio para almacenar X cantidad de paginas  
UMC -> 205 -> Nucleo  
Le digo que si o no tengo espacio  
Nucleo ->206 -> Umc  
escribo paginas  


  
  

##CPU - Nucleo  

**Handshake**  

Nucleo -> 301 -> CPU  
CPU -> 302 -> Nucleo  
Handshake exitoso  
  

Nucleo -> 303 -> CPU  
envio PCB  
CPU -> 304 -> NUCLEO  
fin de quantum




