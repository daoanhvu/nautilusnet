import os
import torch
from torchvision import datasets, models, transforms
import cv2
from facenet_pytorch import InceptionResnetV1

# Refer here https://github.com/lucagessi/facerecognition/blob/master/face_reco_1_class.py

data_transforms = {
    'train': transforms.Compose([
        transforms.RandomResizedCrop(299),
        #transforms.RandomHorizontalFlip(),
        transforms.ToTensor(),
        transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
    ]),
    'val': transforms.Compose([
        transforms.Resize(299),
        transforms.CenterCrop(299),
        transforms.ToTensor(),
        transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
    ])
}

data_dir = './dataset/'
image_datasets = {x: datasets.ImageFolder(os.path.join(data_dir, x),
                                          data_transforms[x]) for x in ['train', 'val']}
dataloaders = {x: torch.utils.data.DataLoader(image_datasets[x], batch_size=4,
                                             shuffle=True, num_workers=4)
              for x in ['train', 'val']}
dataset_sizes = {x: len(image_datasets[x]) for x in ['train', 'val']}
print(dataset_sizes)
class_names = image_datasets['train'].classes
print(dataloaders['train'])

def train_model(model, criterion, optimizer, scheduler, num_epochs=24, dataloaders, device):

    for epoch in range(num_epochs):
        for phase in ['train', 'val']:
            if phase == 'train':
                # set model to training model
                model.train()
            else:
                model.val()
            
            running_loss = 0.0
            running_corrects = 0
            for inputs, labels in dataloaders[phase]:
                inputs = inputs.to(device)

    return model


if __name__ == '__main__':
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    
    sigmoid_fun = torch.nn.Sigmoid()

    model = InceptionResnetV1(pretrained='vggface2', 
        device=device, classify=True, num_classes=4)
    model.to(device=device)

    criterion = torch.nn.BCEWithLogitsLoss

    # define a optimizer using SGD
    optimizer = torch.optim.SGD(model.parameters(), lr=0.001, momentum=0.9)

    # Decay Learning Rate by a factor of 0.1 every 7 epochs
    exp_lr_scheduler = torch.optim.lr_scheduler.StepLR(optimizer=optimizer, step_size=7, gamma=0.1)

    trained_model = train_model(model=model, criterion=criterion, optimizer=optimizer,
                                scheduler=exp_lr_scheduler, num_epochs=1)
    
    model_path = "../models/resnet_vggface.pt"
    torch.save(trained_model.state_dict(), model_path)
