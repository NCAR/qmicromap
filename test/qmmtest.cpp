/*
 * qmmtest.cpp
 *
 *  Created on: May 2, 2011
 *      Author: martinc
 */

#include <iostream>
#include <string>
#include <vector>
#include "QMicroMapTest.h"

int main(int argc, char** argv) {
	if (argc != 2 && argc != 6) {
		std::cerr <<"usage: " << argv[0] << " db_path [xmin ymin xmax ymax]" << std::endl;
		exit(1);
	}

	// Get the geographic database
	std::string dbpath = std::string(argv[1]);
	SpatiaLiteDB db(dbpath);

	double xmin = -180.0;
	double ymin =  -90.0;
	double xmax = 180.0;
	double ymax =  90.0;

	if (argc == 6) {
		xmin = atof(argv[2]);
		ymin = atof(argv[3]);
		xmax = atof(argv[4]);
		ymax = atof(argv[5]);
	}

	// Create a Qt application
	QApplication app(argc, argv);

	QMicroMapTest map(db, xmin, ymin, xmax, ymax, "lightblue");
	map.resize(1000,800);

	map.setWindowTitle(dbpath.c_str());

	map.show();

	return app.exec();

}
