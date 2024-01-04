import torch
import torch.nn as nn
import torchvision.models as models

class AnchorGenerator(nn.Module):
    def __init__(self, base_size=16, scales=[0.5, 1, 2], aspect_ratios=[0.5, 1, 2]):
        super(AnchorGenerator, self).__init__()
        self.base_size = base_size
        self.scales = scales
        self.aspect_ratios = aspect_ratios

    def forward(self, feature_map):
        # Get feature map size
        _, _, H, W = feature_map.shape

        # Generate anchors based on feature map size, scales, and aspect ratios
        anchors = []
        for y in range(0, H):
            for x in range(0, W):
                for scale in self.scales:
                    for aspect_ratio in self.aspect_ratios:
                        # Calculate anchor width and height
                        anchor_width = self.base_size * scale * (aspect_ratio ** 0.5)
                        anchor_height = self.base_size * scale / (aspect_ratio ** 0.5)

                        # Calculate anchor coordinates
                        x_center = x * self.base_size + self.base_size / 2
                        y_center = y * self.base_size + self.base_size / 2

                        # Create anchor box: [x1, y1, x2, y2]
                        anchor = [
                            max(x_center - anchor_width / 2, 0),
                            max(y_center - anchor_height / 2, 0),
                            min(x_center + anchor_width / 2, W * self.base_size),
                            min(y_center + anchor_height / 2, H * self.base_size)
                        ]
                        anchors.append(anchor)

        return torch.tensor(anchors, dtype=torch.float32)

class RPN(nn.Module):
    def __init__(self, in_channels, num_anchors):
        super(RPN, self).__init__()
        self.conv = nn.Conv2d(in_channels, num_anchors * 2, kernel_size=3, padding=1)
        self.sigmoid = nn.Sigmoid()

    def forward(self, x):
        x = self.conv(x)
        x = self.sigmoid(x)
        return x

# Usage example combining AnchorGenerator with RPN
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# Initialize FPN and RPN
fpn = FPN().to(device)
rpn = RPN(in_channels=256, num_anchors=9).to(device)

# Example input
input_image = torch.randn(1, 3, 224, 224).to(device)

# Get feature maps from FPN
p1, p2, p3, p4, p5 = fpn(input_image)

# Use AnchorGenerator on a specific feature map (e.g., p3)
anchor_generator = AnchorGenerator()
anchors = anchor_generator(p3)

# Use RPN on a specific feature map (e.g., p3)
rpn_output = rpn(p3)

# Process RPN outputs with anchors as needed for object detection pipeline

# Assuming you have RPN outputs 'rpn_scores' and 'rpn_deltas' and anchors

# Example values (rpn_scores, rpn_deltas)
rpn_scores = torch.randn(1, 18, H, W)  # Assuming 2 scores for each anchor
rpn_deltas = torch.randn(1, 36, H, W)  # Assuming 4 deltas for each anchor

# Reshape RPN outputs for compatibility with anchors
rpn_scores = rpn_scores.permute(0, 2, 3, 1).reshape(1, -1, 2)  # Reshape scores to (batch_size, -1, 2)
rpn_deltas = rpn_deltas.permute(0, 2, 3, 1).reshape(1, -1, 4)  # Reshape deltas to (batch_size, -1, 4)

# Apply sigmoid to scores to get probabilities
rpn_scores = torch.sigmoid(rpn_scores[..., 1])  # Assuming 2 scores with the second one as the objectness score

# Generate proposals by applying deltas to anchors
# For simplicity, assuming a dummy delta application here
proposals = anchors.unsqueeze(0)  # Make anchors a batch

# Example dummy delta application
proposals = proposals + rpn_deltas

# Filter proposals based on scores (e.g., apply a threshold)
proposal_threshold = 0.7
mask = rpn_scores > proposal_threshold
filtered_proposals = proposals[mask]

# Apply non-maximum suppression (NMS) to get final proposals
# This step is important to remove redundant/duplicate proposals
# You might need to adjust the threshold and other NMS parameters
from torchvision.ops import nms
nms_threshold = 0.7
selected_indices = nms(filtered_proposals[:, :4], filtered_proposals[:, 4], nms_threshold)

final_proposals = filtered_proposals[selected_indices]

# The 'final_proposals' are the selected region proposals for object detection
# These can be passed to subsequent stages for classification and bounding box refinement
