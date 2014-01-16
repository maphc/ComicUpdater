from distutils.core import setup,Extension
MOD="MyModule"
setup(name=MOD,ext_modules=[Extension(MOD,sources=['PythonAndCpp.cpp'])])

