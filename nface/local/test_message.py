import json
import os
from nqueue.message_sender import MessageSender


if __name__=='__main__':
    message_sender = MessageSender(host='localhost', exchange='amq.topic', exchange_type='topic',
        queue_name='nface_messages', topic='nface_detection')
    message = {
        'text': "This is a test message"
    }
    message_sender.publish_message(message=message)
