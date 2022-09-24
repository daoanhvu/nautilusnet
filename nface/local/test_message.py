import json
import os
from datetime import datetime
from nqueue.message_sender import MessageSender


if __name__=='__main__':
    message_sender = MessageSender(host='localhost', exchange='NFACE', exchange_type='topic',
        queue_name='nface_messages', topic='nface_detection')
    message = {
        'actionCommand': 'PUNCH_INOUT_CMD',
            'orgId': 1,
            'classIndex': 0,
            'className': 'AnhVu',
            'device': 1,
            'datetime': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    }
    message_sender.publish_message(message=message)
