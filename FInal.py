import paho.mqtt.client as paho
import ssl
from tkinter import Tk, Canvas, PhotoImage
from pathlib import Path

# Variables globales para almacenar los datos de los sensores
sensor_data = {
    "LM35": "N/A",
    "LM75": "N/A",
    "Ds1820": "N/A"
}

# Callbacks MQTT
def on_connect(client, userdata, flags, rc, properties=None):
    print("Conectado con código %s." % rc)

def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Suscrito: " + str(mid) + " " + str(granted_qos))

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    sensor_name = msg.topic.split('/')[0]

    sensor_data[sensor_name] = msg.payload.decode('utf-8')
    update_dashboard()

# Configuración del cliente MQTT
client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect
client.on_publish = on_publish
client.on_subscribe = on_subscribe
client.on_message = on_message

client.tls_set(tls_version=ssl.PROTOCOL_TLS)
client.username_pw_set("Temperatura", "Micro2024")
client.connect("e3d2e6ca00a64fe9ae0003b6f55f25eb.s1.eu.hivemq.cloud", 8883)

#Suscripcion a los topicos

client.subscribe("LM35/#", qos=0)
client.subscribe("LM75/#", qos=0)
client.subscribe("Ds1820/#", qos=0)

client.loop_start()

# Diseño del dashboard en Tkinter
OUTPUT_PATH = Path(__file__).parent
ASSETS_PATH = OUTPUT_PATH / Path(r".\assets\frame0")

def relative_to_assets(path: str) -> Path:
    return ASSETS_PATH / Path(path)

#Defino las propiedadades de la ventana
window = Tk()
window.geometry("1000x550")
window.configure(bg="#2A2F4F")

#Fondo del Panel

canvas = Canvas(
    window,
    bg="#f2f0f3",
    height=600,
    width=1000,
    bd=0,
    highlightthickness=0,
    relief="ridge"
)
#rectangulo de encabezado

canvas.place(x=0, y=0)
canvas.create_rectangle(0.0, 0.0, 1000.0, 72.0, fill="#173645", outline="")
canvas.create_text(83.0, 16.0, anchor="nw", text="Temperaturas", fill="#FFFFFF", font=("Roboto", 30 * -1))

image_image_1 = PhotoImage(file=relative_to_assets("image_1.png"))
image_1 = canvas.create_image(170.0, 143.0, image=image_image_1)

image_image_2 = PhotoImage(file=relative_to_assets("image_4.png"))
image_2 = canvas.create_image(170.0, 364.0, image=image_image_2)

image_image_3 = PhotoImage(file=relative_to_assets("image_4.png"))
image_3 = canvas.create_image(830.0, 364.0, image=image_image_3)

image_image_4 = PhotoImage(file=relative_to_assets("image_4.png"))
image_4 = canvas.create_image(498.0, 364.0, image=image_image_4)

image_image_5 = PhotoImage(file=relative_to_assets("image_5.png"))
image_5 = canvas.create_image(498.0, 143.0, image=image_image_5)

image_image_6 = PhotoImage(file=relative_to_assets("image_6.png"))
image_6 = canvas.create_image(830.0, 142.0, image=image_image_6)

image_image_12 = PhotoImage(file=relative_to_assets("image_12.png"))
image_12 = canvas.create_image(56.0, 31.0, image=image_image_12)


canvas.create_text(135, 125.0, anchor="nw", text="LM35", fill="#FFFFFF", font=("Roboto", 25 * -1))
canvas.create_text(465.0, 125.0, anchor="nw", text="LM75", fill="#FFFFFF", font=("Roboto", 25 * -1))
canvas.create_text(795.0, 125.0, anchor="nw", text="DS1820", fill="#FFFFFF", font=("Roboto", 25 * -1))



# Simbolo de grados centigrados
canvas.create_text(145, 280.0, anchor="nw", text="°C", fill="#FFFFFF", font=("Roboto", 35 * -1))
canvas.create_text(475.0, 280.0, anchor="nw", text="°C", fill="#FFFFFF", font=("Roboto", 35 * -1))
canvas.create_text(815.0, 280.0, anchor="nw", text="°C", fill="#FFFFFF", font=("Roboto", 35 * -1))


# Texto de las temperaturas mostrarlas en el Dashaboard
lm35_text = canvas.create_text(125.0, 325.0, anchor="nw", text="LM35: N/A", fill="#FFFFFF", font=("Roboto", 40 * -1))
lm75_text = canvas.create_text(455.0, 325.0, anchor="nw", text="LM75: N/A", fill="#FFFFFF", font=("Roboto", 40 * -1))
ds1820_text = canvas.create_text(780.0, 325.0, anchor="nw", text="DS1820: N/A", fill="#FFFFFF", font=("Roboto", 40 * -1))

def update_dashboard():
    canvas.itemconfig(lm35_text, text=sensor_data['LM35'])
    canvas.itemconfig(lm75_text, text=sensor_data['LM75'])
    canvas.itemconfig(ds1820_text, text=sensor_data['Ds1820'])

window.resizable(False, False)
window.mainloop()

client.loop_stop()
client.disconnect()
