import pika
import json


class MessageSender(object):
    def __init__(self, host: str, exchange: str, exchange_type: str, queue_name: str, topic: str):
        self._topic = topic
        self._exchange = exchange
        self._queue_name = queue_name
        credentials = pika.PlainCredentials('user', 'password')
        parameters = pika.ConnectionParameters(host=host,
                                       port=5672, virtual_host='/', credentials=credentials)
        self._connection = pika.BlockingConnection(parameters)
        self._channel = self._connection.channel()
        # self._channel.exchange_declare(exchange=exchange, exchange_type=exchange_type)
        # self._channel.queue_declare(queue=queue_name)
        headers = {
            
        }
        self._properties = pika.BasicProperties(app_id='nface-publisher',
                                          content_type='text/plain',
                                          content_encoding='UTF-8',
                                          headers=headers)


    def publish_message(self, message: dict):
        str_message = json.dumps(message)
        self._channel.basic_publish(self._exchange, self._queue_name,
                                    str_message, self._properties)

