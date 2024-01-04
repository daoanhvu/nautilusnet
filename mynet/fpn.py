import torch
import torch.nn as nn
import torchvision.models as models

class FPN(nn.Module):
    def __init__(self):
        super(FPN, self).__init__()
        # Load pre-trained VGG16
        self.vgg = models.vgg16(pretrained=True).features
        
        # Intermediate layers for feature extraction
        self.conv1 = nn.Conv2d(512, 256, kernel_size=1)
        self.conv2 = nn.Conv2d(512, 256, kernel_size=1)
        self.conv3 = nn.Conv2d(1024, 256, kernel_size=1)
        self.conv4 = nn.Conv2d(512, 256, kernel_size=1)
        
        # Smooth layers for upsampled features
        self.smooth1 = nn.Conv2d(256, 256, kernel_size=3, padding=1)
        self.smooth2 = nn.Conv2d(256, 256, kernel_size=3, padding=1)
        self.smooth3 = nn.Conv2d(256, 256, kernel_size=3, padding=1)
        self.smooth4 = nn.Conv2d(256, 256, kernel_size=3, padding=1)
        
        # Lateral connections for merging features
        self.lateral_conv1 = nn.Conv2d(256, 256, kernel_size=1)
        self.lateral_conv2 = nn.Conv2d(256, 256, kernel_size=1)
        self.lateral_conv3 = nn.Conv2d(256, 256, kernel_size=1)
        
    def forward(self, x):
        # Backbone network (VGG16)
        c1 = self.vgg[:5](x)
        c2 = self.vgg[5:10](c1)
        c3 = self.vgg[10:17](c2)
        c4 = self.vgg[17:24](c3)
        c5 = self.vgg[24:](c4)
        
        # Top-down pathway
        p5 = self.conv1(c5)
        p4 = self.lateral_conv1(c4) + nn.functional.interpolate(p5, scale_factor=2, mode='nearest')
        p4 = self.smooth1(p4)
        
        p3 = self.lateral_conv2(c3) + nn.functional.interpolate(p4, scale_factor=2, mode='nearest')
        p3 = self.smooth2(p3)
        
        p2 = self.lateral_conv3(c2) + nn.functional.interpolate(p3, scale_factor=2, mode='nearest')
        p2 = self.smooth3(p2)
        
        p1 = nn.functional.interpolate(p2, scale_factor=2, mode='nearest')
        p1 = self.smooth4(p1)
        
        return p1, p2, p3, p4, p5

class RPN(nn.Module):
    def __init__(self, in_channels, num_anchors):
        super(RPN, self).__init__()
        self.conv = nn.Conv2d(in_channels, num_anchors * 2, kernel_size=3, padding=1)
        self.sigmoid = nn.Sigmoid()

    def forward(self, x):
        x = self.conv(x)
        x = self.sigmoid(x)
        return x

class FasterRCNN(nn.Module):
    def __init__(self):
        super(FasterRCNN, self).__init__()
        self.fpn = FPN()
        self.rpn = RPN(in_channels=256, num_anchors=9)  # Example values, adjust as needed

    def forward(self, x):
        # Generate feature pyramid
        p1, p2, p3, p4, p5 = self.fpn(x)

        # Use FPN features as input to RPN
        rpn_output_p1 = self.rpn(p1)
        rpn_output_p2 = self.rpn(p2)
        rpn_output_p3 = self.rpn(p3)
        rpn_output_p4 = self.rpn(p4)
        rpn_output_p5 = self.rpn(p5)

        # You can perform further operations with the RPN outputs or
        # combine them for proposals, etc., depending on your needs

        return rpn_output_p1, rpn_output_p2, rpn_output_p3, rpn_output_p4, rpn_output_p5

# Example usage of FasterRCNN
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
faster_rcnn = FasterRCNN().to(device)

# Example input
input_image = torch.randn(1, 3, 224, 224).to(device)
rpn_outputs = faster_rcnn(input_image)
for output in rpn_outputs:
    print(output.shape)

# Example usage FPN
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model = FPN().to(device)

# Example input
input_image = torch.randn(1, 3, 224, 224).to(device)
outputs = model(input_image)
for output in outputs:
    print(output.shape)
