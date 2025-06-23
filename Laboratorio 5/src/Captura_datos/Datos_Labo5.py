import paho.mqtt.client as mqtt
import serial
import time
import json
import csv

# Configuración del puerto serie
try:
    ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=1)
    print("Conexión con STM32 establecida.")
except serial.SerialException as e:
    print("Error al conectar con el puerto serie.")
    exit()

try:
    archivo = open('../Datos_obtenidos/Mov_Extension_Brazo.csv', 'w')  #Renombrar (Datos_Labo5.csv) para diferenciar toma de datos
    escritura_csv = csv.writer(archivo)
    datos_columnas = []
    encabezados = ['Eje X', 'Eje Y', 'Eje Z']
    print(encabezados)
    escritura_csv.writerow(encabezados)
except IOError as e:
    print("No se pudo crear el archivo CSV.")
    ser.close()
    exit()

# Bucle principal para la lectura de datos
try:
    while(1):
        data = ser.readline().decode('utf-8').replace('\r', "").replace('\n', "")
        data = data.split('\t')
        if len(data) >= 3:
            escritura_csv.writerow(data)
            print(data) 
except KeyboardInterrupt:
    archivo.close()
    ser.close()
    print("Archivo CSV cerrado correctamente.")

