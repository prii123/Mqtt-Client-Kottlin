package com.example.iot_mosquitto

import android.annotation.SuppressLint
import android.graphics.Color
import android.os.Bundle
import android.view.View
import android.view.View.OnClickListener
import android.widget.Button
import android.widget.ImageView
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.ekn.gruzer.gaugelibrary.HalfGauge
import com.ekn.gruzer.gaugelibrary.Range
import org.eclipse.paho.client.mqttv3.IMqttMessageListener
import org.eclipse.paho.client.mqttv3.MqttMessage

class MainActivity : AppCompatActivity() {


    lateinit var halfGauge: HalfGauge
    lateinit var rango1: Range
    lateinit var rango2: Range
    lateinit var rango3: Range

    lateinit var mensajeError: TextView
    lateinit var porcentaje: TextView
    lateinit var numeroEstado: TextView

    lateinit var led_1: ImageView

    lateinit var btn_activa_led_prueba: Button
    lateinit var servo_motor_principal: Button


    @SuppressLint("MissingInflatedId")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        /*
        *  inicializacion de variables
        *
        */
        halfGauge = findViewById(R.id.halfGauge)
        mensajeError = findViewById(R.id.mensajeError)
        porcentaje = findViewById(R.id.porcentaje)
        led_1 = findViewById(R.id.led_1)
        btn_activa_led_prueba = findViewById(R.id.btn_activa_led_prueba)
        numeroEstado = findViewById(R.id.numeroEstado)
        servo_motor_principal = findViewById(R.id.btn_activar_servo_principal)

        /*
        *  configuracion de la grafica principal - estado del total de la comida
        *
        */

        rango1 = Range()
        rango1.from = 0.0
        rango1.to = 10.0
        rango1.color = Color.RED

        rango2 = Range()
        rango2.from = 11.0
        rango2.to = 20.0
        rango2.color = Color.YELLOW

        rango3 = Range()
        rango3.from = 21.0
        rango3.to = 30.0
        rango3.color = Color.GREEN

        //add color ranges to gauge
        halfGauge.addRange(rango1)
        halfGauge.addRange(rango2)
        halfGauge.addRange(rango3)

        //set min max and current value
        halfGauge.minValue = 0.0
        halfGauge.maxValue = 30.0
        halfGauge.value = 00.0

        /*
        *  inicializacion de las subscripciones al broker mqtt
        *
        */

        // suscriptor MQTT
        val mqttClient = MQTTClient()
        mqttClient.connect()
        /*
        * TOPIC      esp32/distancia_centimetros
        * DESCRIPCION
        *  este topic se encarga de recibir la informacion en centimetros del estado
        * del recibiente contenedor total de la comida de la mascota
        *
        */
        mqttClient.subscribeToTopic(
            "esp32/distancia_centimetros",
            0,
            object : IMqttMessageListener {
                override fun messageArrived(topic: String?, message: MqttMessage?) {
                    // mensaje recibido
                    val messageContent = message?.toString()
                    // Convertir la cadena en un número Double
                    val doubleNumber = messageContent?.toDoubleOrNull()
                    // Verificar si la conversión fue exitosa
                    if (doubleNumber != null) {
                        val valor = 30 - doubleNumber
                        halfGauge.value = valor
                        //mensaje debajo de la grafica
                        if (valor < 0.0) {
                            mensajeError.text = "Posiblemente la tapa del dispensador esta abierta."
                        } else {
                            mensajeError.text = "-"
                        }
                    }
                    //Log.d("TAG", " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!  "+message.toString())
                }
            })
        /*
              * TOPIC      esp32/distancia_porcentual
              * DESCRIPCION
              *  este topic se encarga de recibir la informacion en porcentaje del estado
              * del recibiente contenedor total de la comida de la mascota
              *
              */
        mqttClient.subscribeToTopic("esp32/distancia_porcentual", 0, object : IMqttMessageListener {
            override fun messageArrived(topic: String?, message: MqttMessage?) {
                // mensaje recibido
                val messageContent = message?.toString() + "%"
                porcentaje.text = messageContent
                //Log.d("TAG", " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!  "+message.toString())
            }
        })

        /*
            * TOPIC      esp32/estadoLedPrueba
            * DESCRIPCION
            *  este topic se encarga de recibir la informacion  del estado del led
            *
            */
        mqttClient.subscribeToTopic("esp32/estadoLedPrueba", 0, object : IMqttMessageListener {
            override fun messageArrived(topic: String?, message: MqttMessage?) {
                //Log.d("TAG", " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!  "+message.toString())
                // mensaje recibido
                val messageContent = message?.toString()


                if (messageContent == "1.00") {
                    led_1.setColorFilter(Color.YELLOW)
                    numeroEstado.text = messageContent
                } else {
                    led_1.setColorFilter(Color.GRAY)
                    numeroEstado.text = messageContent
                }


            }
        })


/*
        *  configuracion del evento Click a cada boton
        *
        */
        btn_activa_led_prueba.setOnClickListener(object : OnClickListener {
            override fun onClick(p0: View?) {
                encenderLed()
            }
        })


    }


    fun encenderLed() {
        val mqttClient = MQTTClient()
        mqttClient.connect()


        val topic = "esp32/ledprueba"
        lateinit var message: String
        val qos = 0

        if (numeroEstado.text.toString() == "1.00") {
            message = "false"
            mqttClient.publishMessage(topic, message, qos)
            mqttClient.disconnect()
        } else {
            message = "true"
            mqttClient.publishMessage(topic, message, qos)
            mqttClient.disconnect()
        }


    }


}