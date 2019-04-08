import os, sys, glob, shutil
import numpy as np
cwd=os.getcwd()
testfile_dir=os.path.dirname(__file__)
itg="-h"
if len(sys.argv) >1:
    if sys.argv[1]=="-b":
        itg="-b"
py3=0
print (sys.hexversion)
if sys.hexversion  > 50000000: 
    py3=1
#print testfile_dir
pyfiles=glob.glob(os.path.join(testfile_dir, "*.py"))
#print pyfiles
python_="python"
if py3==1:
    python_="python3"
if "win" in sys.platform.lower():
    shutil.copy(os.path.join(testfile_dir.replace("test", "src"), "python", "odsystem.py"), os.path.join(cwd, "odsystem.py"))
    shutil.copy(os.path.join(cwd, "odsystem", "x64", "Debug", "_odsystem.pyd"), os.path.join(cwd, "_odsystem.pyd"))
    python_="c:\python27\python"
    if py3==1:
        python_="c:\python35\python"
fn={}
for pyf in pyfiles:
    pf=open(pyf, "r")
    lines=pf.readlines()
    pf.close()
    if lines[0][:5]==5*"#":
        basename=os.path.basename(pyf)
        #if basename[:4]!="t2r1": continue
        os.system("%s %s %s"%(python_, pyf, itg)) 
        #print pyf
        fn[basename]=-1
        bnpy=os.path.join(cwd, "..", "baseline", basename.replace("py", "npy"))
        nnpy=os.path.join(cwd,  basename.replace("py", "npy"))
        if os.path.exists(bnpy)==False: continue
        if os.path.exists(nnpy)==False: continue
        ba=np.load(bnpy)
        na=np.load(nnpy)
        dif=na-ba
        fn[basename]=np.amax(dif)
for i in fn.keys(): 
    print "%20s    %5.3f"%(i, fn[i])
