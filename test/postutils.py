
import matplotlib.pyplot as plt
import numpy as np

def display(datas, types, dt, hht, name_):
    t=datas[:,0]
    dimx, dimy=datas.shape
    for i in range(1, dimy):
        plt.subplot(dimy-1, 1, i)
        plt.xlabel("%s %s %5.2f hht %d"%(name_, types[i], dt, hht))
        plt.plot(t, datas[:, i])
        plt.grid()
    plt.show()   

def ppt(datas, types, dt, hht, name_, argv):
    if len(argv)>1:
        if argv[1]=="-B" or argv[1]=="-H" :
            display(datas, types, dt, hht, name_)
        else:    
            np.save(name_, datas)
    else:
        display(datas, types, dt, hht, name_)
