/// @mainpage MicroMap Overview
/// @section MicroMapIntroduction MicroMap Introduction
///
/// MicroMap provides an embedded spatial database suitable for
/// distribution with stand alone applications.
///
/// Most applications that need to render geophysical representations
/// make use of network based APIs such as Google Maps,
/// Open Street Maps or Bing to provide the desired information.
/// Request are sent to the server for a defined region, and the result
/// is returned, rendered as image tiles.
///
/// MicroMap provides an alternative: a lightweight, embedded
/// and non-networked component which can navigate and serve
/// geographic data to the parent program. This makes it suitable
/// for inclusion in stand alone programs which do not always
/// have a high bandwidth Internet connection available.
///
/// The geographic data is reasonably compact. For example, the
/// Natural Earth cultural and physical data set, at a scale of
/// 1 to 50 million, requires about 21 MB for coverage of the the entire
/// earth. A more detailed resolution data set, at a scale of 1 to 10 million,
/// uses about 144 MB of storage. These are reasonable sizes for
/// distribution with stand alone applications. The object code footprint
/// is less than 1 MB.
///
/// The basic function of MicroMap is to respond to queries for geographical
/// information, contained within a specified bounding box. Thus, two
/// basic requirements must be met: there must be a repository which
/// contains the geographic data, and there must be a facility for finding the
/// information within the specified region. As a third requirement,
/// there must be a facility for creating a graphical rendering of the
/// data.
///
/// Management of the geographic data is provided by
/// sqlite, "the world's most widely used database". Sqlite is a very
/// lean embedded database implementation, with a C API. It manages the database as
/// as a single file. Thus MicroMap distributes the geographic data as
/// an sqlite database file. Different scales of data, or data that has
/// been obtained from different sources, can be swapped into a MicroMap
/// application simply by distributing different MicroMap sqlite database
/// files.
///
/// Navigation of the geographical database is provided by the spatialite
/// extension to sqlite. SpatialLite implements geospatial database capabilities,
/// supporting most of the functionality that is found in PostGIS. You
/// could say that SpatiaLite is to sqlite as PostGIS is PostgreSQL. Note that
/// the standard for the SQL geographical functionality is established by the
/// Open Geospatial Consortium OGC.
///
/// Data access and navigation are purposely separated from the graphical presentation.
/// A third component of MicroMap is QGeoMap, which is a Qt toolbox that
/// provides graphical rendering using the Qt framework.
///
/// @section MicroMapDataSources Data Sources
///
/// There are several high quality public domain geographic data sets available. Natural Earth and
/// Oepn StreetMaps are two such sources. These data are generally
/// made available in the form of "shape files". One of the more vexing tasks when
/// working with geographic databases is the ingest from shape files to the database.
/// The database schema ultimately depends on the geographic database component that
/// will be accessing the database, and perhaps the final application software.
/// Various custom utilities and work flows, of varying robustness, can be found
/// to perform the task of importing the geographic data into the geographic database.
/// Fortunately, the spatialite_tool application provided with SpatiaLite will competently
/// import Natural Earth shape files into a SpatiaLite database.
///
/// @subsection MicroMapNaturalEarth Natural Earth
///
/// Natural Earth (http://www.naturalearthdata.com/) provides a very high quality public domain
/// map dataset, available in various scales. Cultural, physical and raster products can be
/// down loaded from the site. MicroMap currently utilizes the cultural and physical products.
///
/// Each type, theme and resolution are distributed in separate shape files. Themes are named
/// for the feature type, such as "Admin 0", "Countries", or "Rivers + Lake centerlines".
/// The desired shape files are selected from the Natural Earth datasets, and used to build
/// the desired SpatiaLite database.
///
/// @subsection MicroMapOSM Open Street Maps
///
/// Open Street Maps (OSM) is another high quality public domain geographic database. OSM
/// is a highly collaborative activity, with a focus on individual users
/// digitizing and submitting geographic data to the master online OSM database, which changes
/// on the order of every few minutes.
///
/// OSM is a potential source of data for MicroMap. However, the main thrust of
/// OSM seems to be oriented towards a combination of PostgreSQL, PostGIS, Mapnik and
/// Apache, which provide for rendered tile serving to network applications. To make
/// use of OSM, MicroMap needs a method of extracting shape files from the database.
/// It was not clear exactly how to do this in a way that produces shape files that
/// are compatible with SpatiaLite. There appear to be at least two tools for creating
/// shape files from OSM, but unlike Natural Earth these did not seem to import
/// correctly. Some more investigation and code modification could most likely
/// smooth these wrinkles out.
///
/// @section MicroMapNotes Introductory Notes About SQLite and SpatiaLite
///
/// As described above, the two core components used in MicroMap are sqlite and
/// SpatiaLite. Both of these packages are distributed with libraries and header
/// files for building custom applications.
///
/// @subsection Utilities
/// Each package also includes a number of
/// utility applications for testing and demonstrating the package functionality:
///
/// SQLite Utilities
/// <ul>
/// <li> sqlite3: command line interface for manipulating sqlite databases, analogous to psql
/// <li> sqlite3_analyzer: exhaustively analyze your SQLite database.
/// </ul>
/// SpatiaLite Utilities
/// <ul>
/// <li> spatialite: command line interface for manipulating spatialite databases, analogous to psql
/// <li> spatialite_tool: an application for loading shapefiles into a spatialite database.
/// <li> spatialite_gui: GUI application for examining and manipulating a spatialite database, using dql queries.
/// <li> spatialite_gis: full feature GIS application
/// <li> spatialite_osm_*: applications for creating spatialite databases from OpenStreetMap.
/// <li> spatialite_gml: application supporting direct acquisition of base-line GML/XML datasets.
/// <li> shp_doctor: an analysis/sanitizing tool for shapefiles.
/// <li> exif_loader: for importing EXIF pictures into a SpatiaLite database.
/// </ul>
///
/// @subsection Distributions
///
/// Both sqlite and SpatiaLite are available in source and binary distributions. It would be
/// great to simply use the binary distributions, which are provided as zip and tar files, and often
/// are available through MacPorts and yum. However, there were some difficulties when attempting
/// to do this, and so a combined approach was followed. It was found that applications built against the
/// binary distributions would work properly on one platform (e.g. OS X), and not on another (e.g. XP).
/// It wasn't clear what the exact problem was, but trial and error determined that cross-platform functionality
/// is achieved if the SpatiaLite amalgamation library (libspatialite) was built and linked to on each platform.
/// There are other benefits to using the amalgmation, and it is discussed in the next section.
///
/// In general the prebuilt binary distributions for the utility programs are functional on each platform.
/// Building each of these on all platforms would be a substantial effort. Thus the following approach is
/// followed: libspatialite is built from the source code distribution, while the pre-built binaries are used
/// for the sqlite and spatialite utility programs.
///
/// It is crucial to understand that the spatialite amalgamation contains a complete copy of the source code for
/// sqlite, and all sqlite calls in a spatialite application must link to this code, rather than any
/// native sqlite installation on the system. You may build an application which creates and accesses both
/// sqlite and spatialite databases. This application should be sure to link only to the sqlite library
/// provided in libspatialite.
///
/// @section MicroMapAmalgamation SQLite and the SpatiaLite Amalgamation
///
/// The most current versions of spatialite rely on the latest features available
/// in sqlite. The website points out that often the sqlite installed on a user system
/// may be significantly out of date. For this reason, spatialite offers a version of the
/// source code which also contains a source copy of a current sqlite release. If users
/// do not have a current enough version of sqlite installed, they are encouraged to
/// download and build the spatialite amalgamation distribution. It is also mentioned
/// that this method is required if the user wishes to create a statically linked
/// spatialite application.
///
/// It seems that this is not necessary, if your sqlite installation is recent and
/// you are content with dynamic linking. Thus during the initial explorations with spatialite,
/// the binary distributions were used. Demo prgrams and a test application were successfully
/// built and run on the OS X platform. Unfortunately, although the test program would build
/// under windows, it would not execute correctly on that platform. The problem seemed to
/// be that sqlite couldn't find the spatialite extension.
///
/// In order to get around this problem, the amalgamation version of libspatialite was
/// built and installed. This solved the problem, and working applications were tested on both OS X
/// and Windows. The amalgamation version of libspatialite is used on both platforms
/// in order to minimize differences.
///
/// Note that static linking was attempted (for libspatialite) on OS X, and locale_charset()
/// could not be located. Rather than try to sort this out, we will stay with dynamic linking.
/// This is not an issue since there are so many other dependencies which are distributed as
/// dynamic libraries.
///
/// @section MicroMapSQLite SQLite and SQLiteDB
///
/// The current version of the sqlite package is sqlite3. At the most basic level, sqlite
/// provides a C interface through which SQL commands are submitted to the database.
/// No doubt there are many other access libraries available for sqlite, but they
/// are not utilized by MicroMap.
///
/// MicroMap has wrapped the SQLite C API in a C++ wrapper class; see SQLiteDB.
///
/// SQLite is available in pre-compiled distributions for some
/// platforms:
/// <ul>
/// <li> Mac (MacPorts): port install sqlite3
/// <li> Linux: usually available in the Linux distribution
/// </ul>
///
/// This provides the sqlite libraries and headers. The sqlite3 command
/// line utility application is included as well. It is similar to the Postgres
/// psql program, and is used for command line based examination and manipulation of
/// a sqlite database.
///
/// Test (on all platforms) by examining an existing database using sqlite3:
/// @code
/// sqlite3 ne1to10m.sqlite
/// sqlite3> .tables
///  ...
/// select name from sqlite_master where type='table'
/// ...
/// .quit
/// @endcode
///
/// @subsection SQLiteWindows Installing SQLite on Windows
/// <ul>
/// <li> Fetch the autoconf source distribution from http://www.sqlite.org/download.html.
/// <li> From an MSYS shell window, use tar to extract the distribution, in a location of
/// your choice.
/// <li> ./configure
/// <li> make
/// <li> make install
/// </ul>
/// sqlite3.exe and libsqlite3-0.dll will be installed in /usr/local/bin. The linking libraries
/// will be installed to /usr/local/lib, and the headers will be placed in /usr/local/include.
///
/// @section MicroMapSpatiaLite SpatiaLite and SpatialDB
///
/// SpatiaLite is hosted at http://www.gaia-gis.it/spatialite.
///
/// MicroMap has worked hard
/// to insure that the same source code and binary installation methods are followed for
/// all platforms, in order to avoid the confusion which would arise is each platform
/// called for a different mix of build and installation procedures.
///
/// MicroMap has wrapped the spatialite C API in a C++ wrapper class; see SpatiaLiteDB.
///
/// SpatiaLite depends on several other GIS components. There are various methods,
/// depending on platform type, for installing them. The dependencies are:
/// <ul>
/// <li> geos: Geometry Engine, Open Source
/// <li> proj: Cartographic Projections Library
/// <li> iconv: Character Set Conversion
/// <li> zlib
/// <li> libpng
/// <li> libjpeg
/// <li> libtiff
/// <li> libgeotiff
/// <li> expat
/// </ul>
///
/// The libspatialite amalgamation source distribution is found at: http://www.gaia-gis.it/spatialite-2.4.0-4/libspatialite-amalgamation-2.4.0.zip.
/// Download and unpack this distribution.
///
/// @subsection x0 To install libspatialite on Linux (Centos):
/// Install the dependencies. For some reason, iconv is not avaiable and doesn't seem to be needed.
/// The geos package that is available via yum is way out of date, and
/// so one has to use other sources. Rpmbone had geos packages for Centos which worked.
/// The versions are likely to be different than those shown below, and so the most current versions
/// should be substituted.
/// <ul>
/// <li> yum install proj-devel
/// <li> yum install geos-devel
/// </ul>
/// Build the amalgamation:
/// <ul>
/// <li> ./configure
/// <li> make
/// <li> sudo make install
/// </ul>
/// If root access is not available, the --prefix <dir> option can be given to ./configure
/// in order to install in a non-standard , user-writable location. In this case, add
/// "SPATIALITEDIR='directory'" to config.py, and tool_spatialdb.py will be able to
/// find the include and lib directories for building and linking against the amalgamation.
///
/// @subsection x1 To install libspatialite on OS X:
/// Install the dependencies:
/// <ul>
/// <li> port install proj
/// <li> port install geos
/// </ul
/// Note about iconv and ginstall: iconv is used by a bunch of the Mac Ports tools, and
/// will probably get installed when you install them. However, the Mac Ports version is 1.13 (or higher). On
/// Lion, Apple has provided /usr/lib/libiconv.dylib, which is v1.11. Some of the Mac libraries are counting on this
/// version being avaiable. If we distribute the Mac Ports vrsion of iconv, Aspen won't run because for instance, the
/// cups library can't find a symbol in our version, that is available in the Mac version.
///
/// The workaround is to deactivate iconv (via Mac Ports or Porticus) when building Aspen. Then the liking will specify
/// the OS X version in /usr/lib. The same is true when building Spatialite. Make sure that the Mac Ports version of iconv
/// is deactivated when building and installing Spatialite. You can reactivate iconv when you get complaints from the
/// other Mac Ports packages.
///
/// Build the amalgamation:
/// <ul>
/// <li> CFLAGS=-I/opt/local/include LDFLAGS=-L/opt/local/lib ./configure --prefix=/opt/local
/// <li> make
/// <li> sudo make install
/// </ul>
///
/// @subsection x2 To install libspatialite on Windows (MinGW)
/// The following instructions are from from http://www.gaia-gis.it/spatialite-2.4.0/mingw_how_to.html#libspatialite).
///
/// Install the dependencies:
/// <ul>
/// <li> Download and unpack http://www.gaia-gis.it/spatialite-2.4.0-4/dependencies-win-x86.zip
/// <li> Copy the DLLs to /usr/local/bin
/// </ul>
/// Build the amalgamation (in an MSYS shell):
/// <ul>
/// <li> CFLAGS=-I/usr/local/include LDFLAGS=-L/usr/local/lib ./configure --target=mingw32
/// <li> make
/// <li> make install
/// </ul>
///
/// @todo Add details about installing spatialite tools (on Windows; do they automatically come along on the other platforms?)
///
/// @section MicroMapQGeoMap QGeoMap
///
/// @section MicroMapSupporting Supporting Items
///
/// @subsection MicroMapDoxygen Doxygen
///
/// A precompiled version of doxygen is available for Windows. Download and run the installer.
/// Have the installer place it anywhere you want; it will create a doxygen directory with
/// bin/ and other directories below. Copy bin/doxygen.exe, bin/doxywizrd.exe and bin/doxytags.exe
/// to the MSYS local binary directory; e.g. c:/msys/1.0/bin.
///
/// Then with the scons config.py
/// file containing OPT_PREFIX="c:/msys/1.0/local", the EOL doxygen.py tool will be able to find
/// doxygen. Note that you have to use the Windows file path convention. The MSYS path
/// such as /c/msys/1.0/local, or /usr/local/, will not be appreciated by scons.
///






