from tqdm import tqdm
import numpy as np
import pandas as pd
import os


def read_Files(DIR ='data', count_frames=3):   
    CLASS = {'Cars':0, 'Drones': 1, 'People':2}
    list_of_file = []   # filename/details
    list_of_img = []    # x
    list_of_classi = [] # y
    arr = []
    print(os.walk(DIR))
    for root, dirs, files in tqdm(list(os.walk(DIR))):
        for file in files:
            if file.endswith('.csv'):
                tmp = root.strip().split('\\')
                if tmp[-2] in CLASS:
                    list_of_file.append(os.path.join('/'.join(tmp[-3:]),file))
                    df = pd.read_csv(os.path.join(root,file), sep=',',header=None)

                    if len(arr) == 0:
                        a = df.to_numpy()
                        a = np.expand_dims(a, axis=0) 
                        arr.append(a.copy())
                        continue

                    if len(arr) < count_frames and list_of_file[-1].split('\\')[0] == list_of_file[-2].split('\\')[0]:
                        a = df.to_numpy()
                        a = np.expand_dims(a, axis=0)
                        arr.append(a.copy())

                        if len(arr) == count_frames:
                            tmp_arr = arr[0]
                            for elem in range(1, len(arr)):
                                tmp_arr = np.concatenate((tmp_arr, arr[elem]), 0)
                            list_of_img.append(tmp_arr.copy())
                            list_of_classi.append(CLASS[tmp[-2]])
                            arr.clear()

                    if list_of_file[-1].split('\\')[0] != list_of_file[-2].split('\\')[0]:
                        arr.clear()

    x = np.array(list_of_img)
    y = np.array(list_of_classi)
    d = np.array(list_of_file)
    print(len(list_of_file), 'files loaded.')
    return x, y, d

# %matplotlib qt
# fig = plt.figure()

# for i in range(20):
#     ax = plt.axes()
#     img = ax.imshow(list_of_img[i], vmin=-140, vmax=-70, cmap='jet')
#     plt.title('Range-Doppler for {}'.format(list_of_file[i]))
#     plt.axis('tight')
#     plt.ylabel('Range Bin')
#     plt.xlabel('Doppler Bin')
#     fig.colorbar(img)
#     plt.pause(0.05)
#     plt.clf()
    