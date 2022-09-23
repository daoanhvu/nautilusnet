import json
from keras_vggface.vggface import VGGFace
from tensorflow.keras.layers import Dense, GlobalAveragePooling2D
from tensorflow.keras.preprocessing import image
from tensorflow.keras.applications.mobilenet import preprocess_input
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.models import Model
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.models import load_model
import argparse
from constants import INPUT_SHAPE, INPUT_SHAPE_CHANNEL


def train_existing_model(batch_size: int, epochs: int):
    project_dir = 'D:\\projects\\nface'
    train_datagen = ImageDataGenerator(preprocessing_function=preprocess_input)
    train_generator = train_datagen.flow_from_directory('{}\\data\\train'.format(project_dir), 
        target_size=INPUT_SHAPE, color_mode='rgb', batch_size=batch_size, 
        class_mode='categorical', shuffle=True)
    train_generator.class_indices.values()
    # dict_values([0, 1, 2])
    NO_CLASSES = len(train_generator.class_indices.values())
    print("Number of classes: {}".format(NO_CLASSES))
    class_dictionary = train_generator.class_indices
    class_dictionary = {
        value:key for key, value in class_dictionary.items()
    }
    with open("{}\\data\\classes.json".format(project_dir), "w") as classes_file:
        json.dump(class_dictionary, classes_file)
    vgg_model = load_model('{}/models/transfer_learning_trained_face_cnn_model.h5'.format(project_dir))
    vgg_model.fit(train_generator, batch_size = batch_size, verbose = 1, epochs = epochs)
    vgg_model.save('{}\\models\\transfer_learning_trained_face_cnn_model_001.h5'.format(project_dir))


def main(batch_size: int, epochs: int):
    project_dir = 'D:\\projects\\nface'
    train_datagen = ImageDataGenerator(preprocessing_function=preprocess_input)
    train_generator = train_datagen.flow_from_directory('{}\\data\\train'.format(project_dir), 
        target_size=INPUT_SHAPE, color_mode='rgb', batch_size=batch_size, 
        class_mode='categorical', shuffle=True)
    train_generator.class_indices.values()
    # dict_values([0, 1, 2])
    NO_CLASSES = len(train_generator.class_indices.values())
    print("Number of classes: {}".format(NO_CLASSES))
    class_dictionary = train_generator.class_indices
    class_dictionary = {
        value:key for key, value in class_dictionary.items()
    }
    with open("{}\\data\\classes.json".format(project_dir), "w") as classes_file:
        json.dump(class_dictionary, classes_file)

    vgg_model = VGGFace(include_top=False, model='vgg16', input_shape=INPUT_SHAPE_CHANNEL)

    x_output = vgg_model.output
    x_output = GlobalAveragePooling2D()(x_output)
    x_output = Dense(1024, activation='relu')(x_output)
    x_output = Dense(1024, activation='relu')(x_output)
    x_output = Dense(512, activation='relu')(x_output)
    preds = Dense(NO_CLASSES, activation='softmax')(x_output)

    vgg_model = Model(inputs = vgg_model.input, outputs = preds)

    for layer in vgg_model.layers[:19]:
        layer.trainable = False

    for layer in vgg_model.layers[19:]:
        layer.trainable = True

    vgg_model.summary()
    print("Number of layers: {}".format(len(vgg_model.layers)))

    vgg_model.compile(optimizer='Adam', loss='categorical_crossentropy', metrics=['accuracy'])
    vgg_model.fit(train_generator, batch_size = batch_size, verbose = 1, epochs = epochs)
    vgg_model.save('{}\\models\\transfer_learning_trained_face_cnn_model.h5'.format(project_dir))

if __name__=='__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--c",
        type=bool,
        default=False,
        help="Train existing model"
    )
    parser.add_argument(
        "--epochs",
        type=int,
        default=10,
        help="Number of epochs to train"
    )
    parser.add_argument(
        "--batch",
        type=int,
        default=4,
        help="Batch size"
    )
    args = parser.parse_args()
    if args.c == False:
        main(args.batch, args.epochs)
    else:
        train_existing_model(args.batch, args.epochs)