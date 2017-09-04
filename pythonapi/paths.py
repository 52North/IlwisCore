import os
path = os.path.dirname(__file__)
if (len(path) > 0):
    path += os.sep
f = open(path + 'ilwislocation.config', 'r')
f.readline()
path = f.readline().strip()
path = path.split('=')[1]
f.close()
os.environ["PATH"] = path + os.pathsep + os.environ["PATH"]

