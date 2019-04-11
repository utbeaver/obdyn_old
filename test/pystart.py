import os, sys
py3=0
if sys.hexversion == 50660592: 
    py3=1
    def execfile(f):
        exec(open(f).read())
home=os.getenv("HOMEPATH")
obdir=os.path.join(home, 'OneDrive', 'Desktop', 'obdyn')
if py3==1:
    odpy=os.path.join(obdir, 'obdyn', 'src', 'python')
else:    
    odpy=os.path.join(obdir, 'obdyn', 'src', 'python')
sys.path.append(odpy)
if py3==1:
    libdir=os.path.join(obdir, 'od_solver3', 'odsystem', 'x64', "debug" )
else:    
    libdir=os.path.join(obdir, 'od_solver', 'odsystem', 'x64', "debug" )
sys.path.append(libdir)
testdir=os.path.join(obdir, 'obdyn', 'test')
sys.path.append(testdir)
wb=os.path.join(obdir, 'wb')
os.chdir(wb)
cwd=os.getcwd()
#print "You are in %s"%cwd

#execfile(os.path.join(testdir, "two_link.py"))
#execfile(os.path.join(testdir, "parallel4barsphespdp.py"))
#execfile(os.path.join(testdir, "parallel4barspdp.py"))
#execfile(os.path.join(testdir, "parallel4bar.py"))
#execfile(os.path.join(testdir, "sphonly.py"))
execfile(os.path.join(testdir, "spht2r1.py"))
#execfile(os.path.join(testdir, "sph_rot.py"))
#execfile(os.path.join(testdir, "sph_rot_gmarker.py"))
#execfile(os.path.join(testdir, "trat2r1.py"))
#execfile(os.path.join(testdir, "t2r1.py"))
#execfile(os.path.join(testdir, "freet2r1.py"))
#execfile(os.path.join(testdir, "freejnt.py"))
