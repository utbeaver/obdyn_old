import os, sys, glob
import numpy as np
cwd=os.getcwd()
testfile_dir=os.path.dirname(__file__)
#print testfile_dir
pyfiles=glob.glob(os.path.join(testfile_dir, "*.py"))
#print pyfiles
fn={}
for pyf in pyfiles:
    pf=open(pyf, "r")
    lines=pf.readlines()
    pf.close()
    if lines[0][:5]==5*"#":
        os.system("python %s -v"%pyf) 
        #print pyf
	basename=os.path.basename(pyf)
	fn[basename]=None
	bnpy=os.path.join(cwd, "baseline", basename.replace("py", "npy"))
	nnpy=os.path.join(cwd,  basename.replace("py", "npy"))
	if os.path.exists(bnpy)==False: continue
	if os.path.exists(nnpy)==False: continue
	ba=np.load(bnpy)
	na=np.load(nnpy)
        dif=na-ba
	fn[basename]=np.amax(dif)
for i in fn.keys(): 
    print i, fn[i]

