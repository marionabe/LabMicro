import serial
import csv

puerto_serial = serial.Serial('COM3', 9600, timeout=1)

while True:
    """ if puerto_serial.in_waiting > 0:"""
    voltajes = puerto_serial.readline().decode('utf-8').rstrip()  
    print(voltajes + "\n")
    voltajes = voltajes.split()
    
    with open('reg_datos.csv', mode='a', newline='') as reg_datos:
        
        writer = csv.writer(reg_datos)
        writer.writerow(voltajes)