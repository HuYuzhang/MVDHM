"""
pwcnet_predict_from_img_pairs.py

Run inference on a list of images pairs.

Written by Phil Ferriere

Licensed under the MIT License (see LICENSE for details)
"""

from __future__ import absolute_import, division, print_function
from copy import deepcopy
from skimage.io import imread
from model_pwcnet import ModelPWCNet, _DEFAULT_PWCNET_TEST_OPTIONS
from visualize import display_img_pairs_w_flows
import os
import sys

# TODO: Set device to use for inference
# Here, we're using a GPU (use '/device:CPU:0' to run inference on the CPU)
# gpu_devices = ['/device:GPU:0']  
# controller = '/device:GPU:0'
gpu_devices = ['/device:CPU:0']  
controller = '/device:CPU:0'

# TODO: Set the path to the trained model (make sure you've downloaded it first from http://bit.ly/tfoptflow)
# ckpt_path = '.\\models\\pwcnet-lg-6-2-multisteps-chairsthingsmix\\pwcnet.ckpt-595000'
ckpt_path = sys.argv[3]

# Build a list of image pairs to process
img_name1 = sys.argv[1]
img_name2 = sys.argv[2]
image1, image2 = imread(img_name1), imread(img_name2)
img_pairs = []
img_pairs.append((image1, image2))
# for pair in range(1, 4):
#     # image_path1 = f'./samples/mpisintel_test_clean_ambush_1_frame_00{pair:02d}.png'
#     # image_path2 = f'./samples/mpisintel_test_clean_ambush_1_frame_00{pair+1:02d}.png'
#     image_path1 = f'./samples/1.jpg'
#     image_path2 = f'./samples/2.jpg'
#     image1, image2 = imread(image_path1), imread(image_path2)
#     img_pairs.append((image1, image2))

# Configure the model for inference, starting with the default options
nn_opts = deepcopy(_DEFAULT_PWCNET_TEST_OPTIONS)
nn_opts['verbose'] = True
nn_opts['ckpt_path'] = ckpt_path
nn_opts['batch_size'] = 1
nn_opts['gpu_devices'] = gpu_devices
nn_opts['controller'] = controller

# We're running the PWC-Net-large model in quarter-resolution mode
# That is, with a 6 level pyramid, and upsampling of level 2 by 4 in each dimension as the final flow prediction
nn_opts['use_dense_cx'] = True
nn_opts['use_res_cx'] = True
nn_opts['pyr_lvls'] = 6
nn_opts['flow_pred_lvl'] = 2

# The size of the images in this dataset are not multiples of 64, while the model generates flows padded to multiples
# of 64. Hence, we need to crop the predicted flows to their original size
# nn_opts['adapt_info'] = (1, 872, 2048, 2)
nn_opts['adapt_info'] = (1, 436, 1024, 2)


# Instantiate the model in inference mode and display the model configuration
nn = ModelPWCNet(mode='test', options=nn_opts)
nn.print_config()

# Generate the predictions and display them
pred_labels = nn.predict_from_img_pairs(img_pairs, batch_size=1, verbose=False)
# print(pred_labels[0].shape)
import numpy as np
# print(np.mean(pred_labels[0][:,:,0]), np.mean(pred_labels[0][:,:,1]))
with open("opt.log", 'w') as f:
    f.write(str(np.mean(pred_labels[0][:,:,0])))
    f.write(' ')
    f.write(str(np.mean(pred_labels[0][:,:,1])))
    f.close()

x_flow = pred_labels[0][:,:,0]
y_flow = pred_labels[0][:,:,1]
# import IPython
# IPython.embed()
# below we will write the optical flow data to the .flow file
with open('x.flow', 'w') as f:
    for i in range(x_flow.shape[0]):
        for j in range(x_flow.shape[1]):
            f.write(str(x_flow[i,j]) + ' ')
        f.write('\n')

with open('y.flow', 'w') as f:
    for i in range(y_flow.shape[0]):
        for j in range(y_flow.shape[1]):
            f.write(str(y_flow[i,j]) + ' ')
        f.write('\n')