from torch import torch
import itertools
import math

# Anchor_strides size must equal the number of feature map.
anchor_strides = [2, 4]
scales = torch.tensor([0.5, 1, 2])
# strides = torch.tensor(anchor_strides)
ratios = torch.tensor([0.5, 1.0, 2.0])

# scales = (strides.view(-1, 1) * scales).unsqueeze(-1)
# # print(scales)
# sqrt_ratios = torch.sqrt(ratios)
# w = scales * sqrt_ratios
# h = scales / sqrt_ratios
# # base_anchors = [stride, scales, ratio, 4]
# base_anchors = torch.stack([-w/2, -h/2, w/2, h/2], -1)
# print(base_anchors)

# IMPORTANT: At this point, all base anchors have center at (0, 0)


# generate anchors from base_anchors
grid_sizes = [[7, 7], [15, 15]]

# for idx in range(len(grid_sizes)):
#     grid_size = grid_sizes[idx]
#     base_anchor = base_anchors[idx]
#     gridx = grid_size[0]
#     gridy = grid_size[1]
    
#     x_offs = torch.arange(gridx, device=base_anchor.device) * anchor_strides[idx]
#     y_offs = torch.arange(gridy, device=base_anchor.device) * anchor_strides[idx]
#     print(x_offs)
#     print(y_offs)
#     anchor = base_anchor.repeat(gridx, gridy, 1, 1, 1)
#     print(anchor)

def get_base_anchor(stride):
    scales_i = (stride * scales).unsqueeze(-1)
    sqrt_ratios = torch.sqrt(ratios)
    w = scales_i * sqrt_ratios
    h = scales_i / sqrt_ratios
    # base_anchors = [stride, scales, ratio, 4]
    base_anchors = torch.stack([-w/2, -h/2, w/2, h/2], -1)
    return base_anchors

def generate_anchors():
    anchor_levels = []
    idx = 0
    for grid in grid_sizes:
        for x in range(grid[0]):
            for y in range(grid[1]):
                cx = (x + 0.5) * anchor_strides[idx]
                cy = (y + 0.5) * anchor_strides[idx]
                anchors = get_base_anchor(anchor_strides[idx])
                anchors = anchors + torch.tensor([cx, cy, cx, cy])
                print(anchors)
                valid_indices = ((anchors[:, 0] >= 0) &
                             (anchors[:, 1] >= 0) &
                             (anchors[:, 0] < grid[0] * anchor_strides[idx]) &
                             (anchors[:, 1] < grid[1] * anchor_strides[idx]))

                # valid_anchors = anchors[valid_indices]
                # anchor_levels.append(valid_anchors)
                # print(anchors)
        idx = idx + 1

generate_anchors()