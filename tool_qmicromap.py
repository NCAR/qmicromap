import os
import sys
import eol_scons

tools = ['qt4','spatialdb','doxygen','prefixoptions']
env = Environment(tools = ['default'] + tools)

# qt modules
qt4Modules = Split('QtCore QtGui QtSvg')
env.EnableQt4Modules(qt4Modules)

def win_qt_setup(env):
    # Windows needs an extra include path for Qt modules.
    qt4include = env['QT4DIR'] + '/include'
    env.AppendUnique(CPPPATH=[qt4include,]) 
    env.EnableQt4Modules(qt4Modules)

def mac_qt_setup(env):
	# Mac OS setup
	# Qt configuration:
	#    the following uses the frameworks scheme available for gcc on Mac OS
	#    to provide libraries and library paths
	frameworkpath='/Library/Frameworks'
	env.AppendUnique(FRAMEWORKPATH=[frameworkpath,])
	env.AppendUnique(FRAMEWORKS=qt4Modules)
	#    specify the include paths for the qt4 modules. It seems like the
	#    gcc frameworks scheme should do this, but it doesn't
	for f in qt4Modules:
	    p = frameworkpath+'/'+f+'.framework/Headers'
	    env.AppendUnique(CPPPATH=[p,])

libsources = Split("""
QMicroMap.cpp
QStationModelGraphicsItem.cpp
""")

headers = Split("""
QMicroMap.h
QStationModelGraphicsItem.h
""")

if env['PLATFORM'] == 'darwin':
    mac_qt_setup(env)

if env['PLATFORM'] == 'win32':
    win_qt_setup(env)

libqmicromap = env.Library('qmicromap', libsources)
env.Default(libqmicromap)

html = env.Apidocs(libsources + headers,  DOXYFILE_DICT={'PROJECT_NAME':'QMicroMap', 'PROJECT_NUMBER':'1.0'})
env.Default(html)

thisdir = env.Dir('.').srcnode().abspath

def qmicromap(env):
    env.Require(tools)
    env.EnableQt4Modules(qt4Modules)
    env.AppendUnique(CPPPATH   =[thisdir,])
    env.AppendLibrary('qmicromap')
    env.AppendDoxref('QMicroMap')    
    env.Replace(CCFLAGS=['-g','-O2'])
    if env['PLATFORM'] == 'darwin':
		mac_qt_setup(env)
    if env['PLATFORM'] == 'win32':
        win_qt_setup(env)

Export('qmicromap')
