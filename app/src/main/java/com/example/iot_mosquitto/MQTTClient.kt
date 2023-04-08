package com.example.iot_mosquitto

import org.eclipse.paho.client.mqttv3.*
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence

class MQTTClient {
    private val brokerUrl = "tcp://192.168.20.55:1883"
    private val clientId = "android-client"
    private val persistence = MemoryPersistence()
    private val mqttClient = MqttClient(brokerUrl, clientId, persistence)

    fun connect() {
        val options = MqttConnectOptions()
        options.isCleanSession = true
        mqttClient.connect(options)
    }

    fun disconnect() {
        mqttClient.disconnect()
    }

    fun subscribeToTopic(topic: String, qos: Int, listener: IMqttMessageListener) {
        mqttClient.subscribe(topic, qos, listener)
    }

    fun publishMessage(topic: String, message: String, qos: Int) {
        val mqttMessage = MqttMessage(message.toByteArray())
        mqttMessage.qos = qos
        mqttClient.publish(topic, mqttMessage)
    }
}
