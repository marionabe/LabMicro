import paho.mqtt.client as mqtt
import serial
import time
import json

# === Configuración Serial ===
try:
    ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=1)
    print("Conectado al MCU")
except serial.SerialException as e:
    print("Error al conectar con el puerto serial:", e)
    exit(1)

# === Configuración MQTT para ThingsBoard ===
MQTT_BROKER = "iot.eie.ucr.ac.cr"
MQTT_PORT = 1883
MQTT_TOPIC = "v1/devices/me/telemetry"
MQTT_DEVICE_TOKEN = "OJ59vj06MFnpq9Un8oAh"
CLIENT_ID = "Lab4_B71137_B75398"

# === Callbacks para el cliente MQTT ===
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        client.connected_flag = True
        print("Conectado al broker MQTT")
    else:
        print(f"Fallo en la conexión MQTT. Código: {rc}")
        client.loop_stop()

def on_disconnect(client, userdata, rc):
    if rc == 0:
        print("Desconexión MQTT exitosa")
    else:
        print(f"Desconexión inesperada. Código: {rc}")

# Inicialización del cliente MQTT
mqtt.Client.connected_flag = False
client = mqtt.Client(client_id=CLIENT_ID, protocol=mqtt.MQTTv311)
client.username_pw_set(MQTT_DEVICE_TOKEN)
client.on_connect = on_connect
client.on_disconnect = on_disconnect

# Conexión al broker
try:
    client.connect(MQTT_BROKER, MQTT_PORT)
    client.loop_start()
except Exception as e:
    print("No se pudo conectar al broker MQTT:", e)
    exit(1)

# Encabezados de datos esperados desde el MCU
ENCABEZADOS = ['Eje X', 'Eje Y', 'Eje Z', 'Bateria Baja', 'Nivel de Bateria']
print("Encabezados de datos:", ENCABEZADOS)

# === Bucle principal de lectura y envío de datos ===
while True:
    try:
        # Lectura desde el puerto serial
        linea = ser.readline().decode('utf-8').strip()
        datos = linea.split('\t')

        # Verifica que se hayan recibido al menos 5 campos
        if len(datos) >= 5:
            try:
                # Cálculo de magnitud
                magnitud = (int(datos[0])**2 + int(datos[1])**2 + int(datos[2])**2)**0.5

                # Construcción del mensaje de telemetría
                telemetria = {
                    "Eje X": datos[0],
                    "Eje Y": datos[1],
                    "Eje Z": datos[2],
                    "Bateria Baja": "SI" if datos[3] == "1" else "NO",
                    "Nivel de Bateria": datos[4],
                    "Sismo Detectado": "SI" if magnitud > 2 else "NO"
                }

                # Envío del mensaje como JSON
                mensaje_json = json.dumps(telemetria)
                print(mensaje_json)

                if client.connected_flag:
                    client.publish(MQTT_TOPIC, mensaje_json)
                else:
                    print("MQTT no conectado, datos no enviados")
            except ValueError:
                print("Error al convertir los datos numéricos")

        time.sleep(0.1)  
        
    except Exception as e:
        print("Error al procesar los datos:", e)
