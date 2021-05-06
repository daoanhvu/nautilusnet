import tensorflow as tf
from tensorflow.keras.layers import ( 
    Add, 
    Input, 
    Lambda,
    Conv2D, 
    LeakyReLU, 
    ZeroPadding2D, 
    UpSampling2D,
    MaxPool2D )
from mynet.yltf.batch_normal import BatchNormalization

# https://www.kaggle.com/soumikrakshit/yolo-v3-using-tensorflow-2-0

class TFDarkNet:
    pass

    def add_conv_layer(self, x, filters, ksize, strides=1, downsample = False, batch_normal=True):
        strides = 1
        padding = 'SAME'
        if downsample:
            # Top - left half padding
            x = ZeroPadding2D(((1, 0), (1, 0)))(x)
            padding = 'VALID'
        
        conv = Conv2D(filters=filters, kernel_size=ksize, strides=strides, padding=padding)(x)
        if batch_normal:
            conv = BatchNormalization()(conv)
            conv = LeakyReLU(alpha=0.1)(conv)
        return conv

    def residual_layer(self, x, filters):
        prev = x
        x = self.add_conv_layer(x, filters // 2, 1)
        x = self.add_conv_layer(x, filters, 3)
        x = Add()([prev, x])
        return x

    def block_layer(self, x, filters, blocks):
        x = self.add_conv_layer(x, filters, 3, strides=2)
        for _ in range(blocks):
            x = self.residual_layer(x, filters)
        return x

    def build_darknet(self, name=None):
        x = inputs = Input([None, None, 3])
        x = self.add_conv_layer(x, 32, 3)
        x = self.block_layer(x, 64, 1)
        x = self.block_layer(x, 128, 2) # Skip connections
        x = x_36 = self.block_layer(x, 256, 8)
        x = x_61 = self.block_layer(x, 512, 8)
        x = self.block_layer(x, 1024, 4)
        return tf.keras.Model(inputs, (x_36, x_61, x), name=name)

    def build_darknet_tiny(self, name=None):
        x = inputs = Input([None, None, 3])
        x = self.add_conv_layer(x, 16, 3)
        x = MaxPool2D(2, 2, 'same')(x)
        x = self.add_conv_layer(x, 32, 3)
        x = MaxPool2D(2, 2, 'same')(x)
        x = self.add_conv_layer(x, 64, 3)
        x = MaxPool2D(2, 2, 'same')(x)
        x = self.add_conv_layer(x, 128, 3)
        x = MaxPool2D(2, 2, 'same')(x)
        x = x_8 = self.add_conv_layer(x, 256, 3) # Skip connections
        x = MaxPool2D(2, 2, 'same')(x)
        x = self.add_conv_layer(x, 512, 3)
        x = MaxPool2D(2, 1, 'same')(x)
        x = self.add_conv_layer(x, 1024, 3)
        return tf.keras.Model(inputs, (x_8, x), name=name)

    def yolo_conv(self, filters, name=None):
        def yl_conv(x_in):
            if isinstance(x_in, tuple):
                inputs = Input(x_in[0].shape[1:]), Input(x_in[1].shape[1:])
                x, x_skip = inputs
                # concat with skip connections
                x = self.add_conv_layer(x, filters, 1)
            else:
                x = inputs = Input(x_in.shape[1:])
            return tf.keras.Model(inputs, x, name=name)(x_in)
        return yl_conv

    def train(self):
        pass

    def loss(self):
        pass