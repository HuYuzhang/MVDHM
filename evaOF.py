import cv2
import sys
import os
import numpy as np
path_name = "ydiffop2"

ofs = os.scandir(path_name)
ofs = [tmp for tmp in ofs if not tmp.name.startswith('.') and not tmp.is_dir()]
# print(ofs)


height = 240
width = 416
raw_data = np.zeros((height, width, 2))

for img in ofs:
    cur_r = 0
    cur_c = 0
    with open(img.path, 'r') as f:
        while True:
            line = f.readline()
            if line == "":
                break
            row = line.split(' ')[:-1]
            # print(len(row), row[-1])
            raw_data[cur_r, :, 0] = row
            cur_r = cur_r + 1
        # for i in range(height):
        #     print(raw_data[i,:,0])
    with open(img.path[:-4] + '.avg', 'w') as f:
        for y in range(0,height,64):
            for x in range(0,width,64):
                yy = y + 64
                xx = x + 64
                if yy > height:
                    yy = height
                if xx > width:
                    xx = width
                avg = np.mean(raw_data[y:yy,x:xx,0])
                f.write(str(x) + ' ' + str(y) + ' ' + str(avg) + '\n')
            