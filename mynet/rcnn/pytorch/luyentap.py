import torch
import cv2

x = torch.zeros([2, 4, 6])
print(x.shape)
print(x)

a = x[:, :, None, :]
print(a.shape)
print(a)

# a = torch.stack((x, torch.tensor([4])), 0)
# print(a)

# b = torch.cat((x, x), 0)
# print(b)