from OdSystem import *
from math import *

expr = "sin(pi/2)"
pexpr = sin(pi/2)
Exp = Expression(expr)
d = Exp.eval()
print expr+ " = "
print pexpr
print d
del Exp

expr = "cos(pi/4)+2*sin(pi/2)"
pexpr = cos(pi/4)+2*sin(pi/2)
Exp = Expression(expr)
d = Exp.eval()
print expr+ " = "
print pexpr
print d
del Exp

expr = "(2+3)^2.9"
Exp = Expression(expr)
d = Exp.eval()
print expr+ " = "
print d
del Exp

expr = "exp(1.0)"
pexpr = exp(1.0)
Exp = Expression(expr)
print expr+ " = "
d = Exp.eval()
print pexpr
print d
del Exp


expr = "log(exp(1.0))"
Exp = Expression(expr)
print expr+ " = "
d = Exp.eval()
print d
del Exp

expr = "sqrt(log(exp(100.0)))"
Exp = Expression(expr)
print expr+ " = "
d = Exp.eval()
print d
del Exp
