import caffe as cf
import numpy as np
from PIL import Image

cf.set_mode_cpu()

# net = cf.Net('test.prototxt', cf.TEST)
net = cf.Classifier('deploy_nautilusnet.prototxt', cf.TEST)

# im = np.array(Image.open('cat_gray.jpg'))
# im_input = im[np.newaxis, np.newaxis, :, :]

# net.blobs['data'].reshape(*im_input.shape)
# net.blobs['data'].data[...] = im_input

# net.forward()

# net.save('mymodel.caffemodel')


print("OK!!!!")
