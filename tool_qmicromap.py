import os
import sys

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
    #frameworkpath='/usr/local/lib'
    #env.AppendUnique(FRAMEWORKPATH=[frameworkpath,])
    #env.AppendUnique(FRAMEWORKS=qt4Modules)
    pass

libsources = Split("""
  QMicroMap.cpp
  QStationModelGraphicsItem.cpp
""")

headers = Split("""
  QMicroMap.h
  QStationModelGraphicsItem.h
  MicroMapOverview.h
""")

if env['PLATFORM'] == 'darwin':
    mac_qt_setup(env)

if env['PLATFORM'] == 'win32':
    win_qt_setup(env)

libqmicromap = env.Library('qmicromap', libsources)
env.Default(libqmicromap)

html = env.Apidocs(libsources + headers,  DOXYFILE_DICT={'PROJECT_NAME':'QMicroMap', 'PROJECT_NUMBER':'1.0'})

thisdir = env.Dir('.').srcnode().abspath

def qmicromap(env):
    env.AppendLibrary('qmicromap')
    env.Require(tools)
    env.EnableQt4Modules(qt4Modules)
    env.AppendUnique(CPPPATH   =[thisdir,])
    env.AppendDoxref('QMicroMap')    
    if env['PLATFORM'] == 'darwin':
		mac_qt_setup(env)
    if env['PLATFORM'] == 'win32':
        win_qt_setup(env)

Export('qmicromap')
