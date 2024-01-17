import os
import torch
import cv2
import csv
import copy
import time
from utils import load_data
from facenet_pytorch import InceptionResnetV1

import argparse

# Refer here https://github.com/lucagessi/facerecognition/blob/master/face_reco_1_class.py

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

def train_model(model, criterion, optimizer, scheduler, num_epochs, data_info, device):

    train_metrics = []
    since = time.time()
    # The best accuracy
    best_model_wts = copy.deepcopy(model.state_dict())
    best_acc = 0.0
    for epoch in range(num_epochs):
        print('Epoch {}'.format(epoch))

        for phase in ['train', 'val']:
            if phase == 'train':
                # set model to training model
                model.train()
            else:
                model.eval()
            
            running_loss = 0.0
            running_corrects = 0
            for inputs, labels in data_info['dataloaders'][phase]:
                inputs = inputs.to(device)
                labels = labels.to(device)

                optimizer.zero_grad()

                with torch.set_grad_enabled(phase == 'train'):
                    outputs = model(inputs)
                    _, preds = torch.max(outputs, 1)
                    loss = criterion(outputs, labels)

                    if phase == 'train':
                        loss.backward()
                        optimizer.step()

                running_loss += loss.item() * inputs.size(0)
                running_corrects += torch.sum(preds==labels.data)
            
            if phase == 'train':
                scheduler.step()

            epoch_loss = running_loss / data_info['dataset_sizes'][phase]
            epoch_acc = running_corrects.double() / data_info['dataset_sizes'][phase]
            
            if phase == 'val':
                train_metrics.append(
                    {
                        'Phase': phase,
                        'Loss': epoch_loss,
                        'Accuracy': epoch_acc.item()
                    }
                )

                # We keep the best model so far
                if epoch_acc > best_acc:
                    print('Updating the best model with accuracy {}'.format(epoch_acc))
                    best_acc = epoch_acc
                    best_model_wts = copy.deepcopy(model.state_dict())
        
    time_elapsed = time.time() - since
    print('Training complete in {:.0f}m {:.0f}s'.format(time_elapsed // 60, time_elapsed % 60))
    print('Best val Accuracy: {:4f}'.format(best_acc))

    # load best model weights
    model.load_state_dict(best_model_wts)
    return train_metrics, model

def main(epochs):
    data_dir = './dataset/'
    data_info = load_data(data_dir=data_dir, batch_size=8)
    model_path = "../models/resnet_vggface.pt"
    print("Going to train with {}".format(device))
    
    num_classes = len(data_info['class_names'])
    model = InceptionResnetV1(pretrained='vggface2', device=device, classify=True, num_classes=num_classes)
    model.to(device=device)

    if os.path.exists(model_path):
        model.load_state_dict(torch.load(model_path))

    criterion = torch.nn.CrossEntropyLoss()

    # define a optimizer using SGD
    optimizer = torch.optim.SGD(model.parameters(), lr=0.001, momentum=0.9)
    # optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

    # Decay Learning Rate by a factor of 0.1 every 7 epochs
    exp_lr_scheduler = torch.optim.lr_scheduler.StepLR(optimizer=optimizer, step_size=8, gamma=0.1)

    metrics, trained_model = train_model(model=model, 
                                criterion=criterion, 
                                optimizer=optimizer,
                                scheduler=exp_lr_scheduler, 
                                num_epochs=epochs, 
                                data_info=data_info, 
                                device=device)
    
    torch.save(trained_model.state_dict(), model_path)

    metric_file = open('./metrics.csv', 'w', newline='')
    with metric_file:
        header = ['Phase', 'Loss', 'Accuracy']
        writer = csv.DictWriter(metric_file, fieldnames=header)
        writer.writeheader()
        for row in metrics:
            writer.writerow(row)



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--epochs",
        type=int,
        default=16,
        help="Number of epochs to train"
    )
    
    args = parser.parse_args()
    main(args.epochs)
    
    # data_dir = './dataset'
    # image_path_labels = []

    # data_info = load_data(data_dir=data_dir)
    # embedding_folder = os.path.join(data_dir, 'for_embeddings')

    # for i, class_name in enumerate(data_info['class_names']):
    #     class_path = os.path.join(embedding_folder, class_name)
    #     images = [os.path.join(class_path, img) for img in os.listdir(class_path)]
    #     image_path_labels.append({
    #         'image': images[0],
    #         'class_index': [i] * len(images),
    #         'class_name': class_name
    #     })

    # model_path = "../models/resnet_vggface.pt"
    # device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    # print('Device: {}'.format(device))

    # mtcnn = MTCNN(device=device, image_size=224, keep_all=True, thresholds=[0.4, 0.5, 0.5], min_face_size=60)
    # mtcnn.detect_box = MethodType(detect_box, mtcnn)

    # model = InceptionResnetV1(pretrained='vggface2', device=device, classify=True, num_classes=NUM_CLASSES)
    # model.load_state_dict(torch.load(model_path))
    # model.eval()

    # for itm in image_path_labels:
    #     img = cv2.imread(itm['image'])
    #     boxes, cropped_images = mtcnn.detect_box(img)
    #     if cropped_images is not None:
    #         for box, cropped in zip(boxes, cropped_images):
    #             x, y, x2, y2 = [int(x) for x in box]
    #             img_embedding = model(torch.Tensor(cropped.unsqueeze(0)).to(device))
    #             print('Class {} embedding {}'.format(itm['class_name'], img_embedding))
