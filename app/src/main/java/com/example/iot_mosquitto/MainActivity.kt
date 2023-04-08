package com.example.iot_mosquitto

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import org.eclipse.paho.client.mqttv3.IMqttMessageListener
import org.eclipse.paho.client.mqttv3.MqttMessage

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)


        val mqttClient = MQTTClient()
        mqttClient.connect()
        mqttClient.subscribeToTopic("test", 0, object : IMqttMessageListener {
            override fun messageArrived(topic: String?, message: MqttMessage?) {
                // Aquí puedes manejar el mensaje recibido
                Log.d("TAG", " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!  "+message.toString())
            }
        })



    }
}