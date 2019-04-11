import os, sys, glob, shutil
sys.dont_write_bytecode = True
import numpy as np
testfile_dir=os.path.dirname(__file__)

itg="-h"
if len(sys.argv) >1:
    if sys.argv[1]=="-b":
        itg="-b"
py3=0
#print (sys.hexversion)
if sys.hexversion  > 50000000: 
    py3=1
python_="python"
if py3==1:
    python_="python3"
if "win" in sys.platform.lower():
    home=os.getenv("HOMEPATH")
    oddir=os.path.join(home, "OneDrive", "Desktop","obdyn")
    wb=os.path.join(oddir, "wb")
    pydir=os.path.join(oddir, "obdyn", "src", "python")
    libdir=os.path.join(oddir, "od_solver", "odsystem", "x64", "Debug")
    shutil.copy(os.path.join(libdir, "_odsystem.pyd"), os.path.join(wb, "_odsystem.pyd"))
    shutil.copy(os.path.join(pydir, "odsystem.py"), os.path.join(wb, "odsystem.py"))
    python_="c:\python27\python"
    if py3==1:
        python_="c:\python35\python"
else:
    home=os.getenv("HOME")
    oddir=os.path.join(home, "obdyn")
    libdir=os.path.join(oddir, "build")
    wb=os.path.join(oddir, "wb")
    shutil.copy(os.path.join(libdir, "_odsystem.so"), os.path.join(wb, "_odsystem.so"))
    shutil.copy(os.path.join(libdir, "odsystem.py"), os.path.join(wb, "odsystem.py"))
os.chdir(wb)
pyfiles=glob.glob(os.path.join(oddir, "obdyn", "test",  "*.py"))
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
        bnpy=os.path.join(wb, "..", "baseline", basename.replace("py", "npy"))
        nnpy=os.path.join(wb,  basename.replace("py", "npy"))
        if os.path.exists(bnpy)==False: continue
        if os.path.exists(nnpy)==False: continue
        ba=np.load(bnpy)
        na=np.load(nnpy)
        if np.size(na) == np.size(ba):
            dif=na-ba
        fn[basename]=np.amax(dif)
for i in fn.keys(): 
    print "%20s    %5.3f"%(i, fn[i])
