#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <unistd.h>

#include "SpatiaLiteDB.h"

////////////////////////////////////////////////////////////////
void usage(char* argv0) {
	std::cout << "usage: " << argv0 << "\n -d <spatialite database path> [-p]" << std::endl;
}

////////////////////////////////////////////////////////////////
void getOptions(int argc, char** argv, std::string &dbPath, bool &doPrint) {

	extern char *optarg;
	int opt;

	doPrint = false;

	while ((opt = getopt(argc, argv, "d:p")) != -1) {
		switch (opt) {
		case 'd':
			dbPath = optarg;
			break;
		case 'p':
			doPrint = true;
			break;
		default:
			usage(argv[0]);
			exit(1);
		}
	}

	if (dbPath == "") {
		usage(argv[0]);
		exit(1);
	}
}

////////////////////////////////////////////////////////////////
void printPoints (
		SpatiaLiteDB::PointList& points,
		SpatiaLiteDB::LinestringList& linestrings,
		SpatiaLiteDB::PolygonList& polygons ) {

	// print the individual points, if requested
	if (points.size()) {
		for (int i = 0; i < points.size(); i++) {
			std::cout << points[i] << " ";
		}
		std::cout << std::endl;
	}

		if (linestrings.size()) {
		for (int i = 0; i < linestrings.size(); i++) {
			std::cout << linestrings[i] << " ";
		}
		std::cout << std::endl;
	}

	if (polygons.size()) {
		for (int i = 0; i < polygons.size(); i++) {
			std::cout << polygons[i] << " ";
		}
		std::cout << std::endl;
	}

}

////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {

	// The path to the spatialite database
	std::string dbPath;

	// if we will print the point values
	bool doPrint;

	// get the options. All must be present
	getOptions(argc, argv, dbPath, doPrint);

	try {

		// open the database
		SpatiaLiteDB db(dbPath);
		std::cout << db.version() << std::endl;

		// find the tables which contain a geometry column
		std::vector<std::string> geometry_tables = db.geometryTables();

		double deltaX = 360;
		for (double x = -180; x < 180; x += deltaX) {

			std::map<std::string, int> pointlistTotal;
			std::map<std::string, int> linestringListTotal;
			std::map<std::string, int> polygonListTotal;

			// query each table of the database, counting the number of

			// points, linestrings and polygons for each
			for (std::vector<std::string>::iterator table = geometry_tables.begin();
				table != geometry_tables.end(); table++) {

				db.queryGeometry(*table, "Geometry", x, 35.0, x+deltaX, 45.0);

				SpatiaLiteDB::PointList points = db.points();
				SpatiaLiteDB::LinestringList linestrings = db.linestrings();
				SpatiaLiteDB::PolygonList polygons = db.polygons();

				pointlistTotal[*table]      += points.size();
				linestringListTotal[*table] += linestrings.size();
				polygonListTotal[*table]    += polygons.size();

				if (doPrint) {
					printPoints(points, linestrings, polygons);
				}
			}

			// print summary results
			int totalObjects = 0;
			for (std::vector<std::string>::iterator table = geometry_tables.begin();
					table != geometry_tables.end(); table++) {
				std::cout
				<< *table << "  "
				<< pointlistTotal[*table] << "  "
				<< linestringListTotal[*table] << "  "
				<< polygonListTotal[*table] << "  "
				<< std::endl;
				totalObjects += pointlistTotal[*table] + linestringListTotal[*table] + polygonListTotal[*table];
			}
			std::cout << "*** total objects: " << totalObjects << std::endl;
		}
	} catch (std::string error) {
		std::cout << error << std::endl;
	}

	return 0;
}
