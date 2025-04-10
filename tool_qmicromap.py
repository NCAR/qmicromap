
tools = ['spatialdb', 'doxygen', 'prefixoptions']
env = Environment(tools=['default'] + tools)

# qt modules
qttools = env.Split('qtcore qtgui qtsvg')
env.Require(qttools)

libsources = Split("""
  QMicroMap.cpp
  QStationModelGraphicsItem.cpp
""")

headers = Split("""
  QMicroMap.h
  QStationModelGraphicsItem.h
  MicroMapOverview.h
""")

libqmicromap = env.Library('qmicromap', libsources)
env.Default(libqmicromap)

html = env.Apidocs(libsources + headers,  DOXYFILE_DICT={'PROJECT_NAME':'QMicroMap', 'PROJECT_NUMBER':'1.0'})

thisdir = env.Dir('.').srcnode().abspath

def qmicromap(env):
    env.AppendLibrary('qmicromap')
    env.Require(tools)
    env.Require(qttools)
    env.AppendUnique(CPPPATH=[thisdir])
    env.AppendDoxref('QMicroMap')

Export('qmicromap')
