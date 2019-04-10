
import os, sys
py3=0
if sys.hexversion == 50660592: 
    py3=1
    def execfile(f):
        exec(open(f).read())
home=os.getenv("HOMEPATH")
def gh():
    os.chdir(home)
obdir=os.path.join(home, 'OneDrive', 'Desktop', 'obdyn')
odpy=os.path.join(obdir, 'obdyn', 'src', 'python')
if py3==1:
    libdir=os.path.join(obdir, 'od_solver3', 'odsystem', 'x64', "debug" )
else:    
    libdir=os.path.join(obdir, 'od_solver', 'odsystem', 'x64', "debug" )
sys.path.append(libdir)
testdir=os.path.join(obdir, 'obdyn', 'test')
sys.path.append(testdir)
def gh():
    os.chdir(home)
    cwd=os.getcwd()
    print cwd
def gt():
    os.chdir(testdir)
    cwd=os.getcwd()
    print cwd
def cwd():
    cwd=os.getcwd()
    print cwd

def toPL(p, m):
    mat=App.Matrix(m.get(0), m.get(1), m.get(2), 0.0, m.get(3), m.get(4), m.get(5), 0.0, m.get(6), m.get(7), m.get(8),0.0)
    pl=App.Placement(App.Vector(p.get(0), p.get(1), p.get(2)), App.Rotation(mat))
    return pl
def priPL(p1):
    ptemp=p1.Shape.PrincipalProperties
    v1=ptemp["FirstAxisOfInertia"]
    v2=ptemp["SecondAxisOfInertia"]
    v3=ptemp["ThirdAxisOfInertia"]
    m=ptemp["Moments"]
    In=[m[0], 0, 0, 0, m[1], 0, 0, 0, m[2]]
    #mat=App.Rotation((v1[0], v2[0], v3[0], 0.0, v1[1], v2[1], v3[1], 0.0, v1[2], v2[2], v3[2], 0.0))
    mat=App.Rotation(v1, v2, v3)
    cmpos=p1.Shape.CenterOfMass
    plcm=App.Placement(cmpos, mat)
    return plcm, In

def m2m(pl):
    m=pl.toMatrix()
    return [m.A11, m.A12, m.A13, m.A14,m.A21, m.A22, m.A23, m.A24, m.A31, m.A32, m.A33, m.A34, m.A41, m.A42, m.A43, m.A44] 
