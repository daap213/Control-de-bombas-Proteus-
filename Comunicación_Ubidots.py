from turtle import position
from ubidots import ApiClient
import serial
import time

if __name__ == '__main__':
    dato = 0

    try:
        print("CONECTADO...")
        Atmega = serial.Serial(
            'COM10', 9600,
            timeout=None)  #Se conecta Python al puerto 10 y al Atmega328p
        time.sleep(1)

    except:
        print("FALLO LA CONEXION")

    try:
        print("CONECTADO API...")
        api = ApiClient(token='BBFF-qAyWPS8azj2HpzvFdEqHFD7dAVz85W'
                        )  #Se conecta dispositivo de Ubidots
        #Se conecta con las variables de Ubidots que recibiran datos
        presion = api.get_variable('61bed20d5b02682aee6f9502')
        auto = api.get_variable('61e1d9b8536fcf0c4575a692')
        manual = api.get_variable('61e1d9c1ac400707c1ea2808')
        B1H = api.get_variable('61bed1ef39b89b0df3b7a0d7')
        B2H = api.get_variable('61bed1fb5932052c0369a454')
        B3H = api.get_variable('61bed2055b02682aed6baef5')
        leds = [[auto, manual], B1H, B2H, B3H]
        Paro = api.get_variable('61bf8909321f013aefba5d88')
        valorPre = ""
        print(presion)
        print('probando si conecta')

    except:
        print("FALLO LA CONEXION API")

    while True:

        valorAct = int(Paro.get_values(1)[0]['value'])
        if valorAct != valorPre:
            if valorAct == 1:
                Atmega.write(b'P\r\n')
                print('Paro de bombas')
            else:
                Atmega.write(b'F\r\n')
                print('Función normal')
        valorPre = valorAct

        pres = Atmega.readline()  #Se lee cada dato recibido del Atmega328p
        dato = pres.decode("utf-8").strip()  #Se decodifica el dato
        print(dato)

        if dato.isdigit():
            presion.save_value({
                'value': dato
            })  #Si el dato es un número se envia al medidor de presión

        elif dato.isalnum():  #Si es un dato alfanúmerico se verifica a que variable corresponde
            ledN = dato[0]

            if ledN == "0":
                habilita = lambda x: True if x == "A" else False

                if habilita(dato[1]):
                    leds[int(ledN)][0].save_value({'value': 1})
                    leds[int(ledN)][1].save_value({'value': 0})
                else:
                    leds[int(ledN)][0].save_value({'value': 0})
                    leds[int(ledN)][1].save_value({'value': 1})

            elif ledN.isdigit():
                habilita = lambda x: 1 if x == "T" else 0
                leds[int(ledN)].save_value({'value': int(habilita(dato[1]))})
