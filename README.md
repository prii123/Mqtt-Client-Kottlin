1. agregar la libreria al gradle
    implementation 'org.eclipse.paho:org.eclipse.paho.client.mqttv3:1.2.5'

2. crear una clase llamada MQTTClient de la siguiente forma

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

3. utilizar el subscriptor para obtener los datos que nececites de la siguiente forma

val mqttClient = MQTTClient()
        mqttClient.connect()
        mqttClient.subscribeToTopic("test", 0, object : IMqttMessageListener {
            override fun messageArrived(topic: String?, message: MqttMessage?) {
                // Aquí puedes manejar el mensaje recibido en este caso lo estoy imprimiendo por consola
                Log.d("TAG", " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!  "+message.toString())
            }
        })
