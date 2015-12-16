#Proyecto de Sistemas Operativos primer parcial
![screenshot from 2015-12-16
07-03-17](https://cloud.githubusercontent.com/assets/5729175/11840361/4968daa2-a3c3-11e5-912c-b3ffaa9aaa3d.png)

##Requisitos

Necesitas [ZeroMQ 4.1](https://github.com/zeromq/zeromq4-1) para compilar este proyecto. Basta con hacer clone del repositorio oficial y seguir las instrucciones.
También necesitas tener GTK 2.0 instalado para correr el UI del cliente. No se
puede usar el servidor IRC sin este cliente.
##Ejecutar

Para compilar y correr el servidor: 

```
make server
./server
```

Para compilar el cliente GUI (indispensable):
```
make client
./client
```
