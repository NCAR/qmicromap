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
	if (argc != 6) {
		std::cerr <<"usage: " << argv[0] << " db_path xmin ymin xmax ymax" << std::endl;
		exit(1);
	}

	// Get the geographic database
	std::string dbpath = std::string(argv[1]);
	SpatiaLiteDB db(dbpath);

	double xmin = atof(argv[2]);
	double ymin = atof(argv[3]);
	double xmax = atof(argv[4]);
	double ymax = atof(argv[5]);

	// Create a Qt application
	QApplication app(argc, argv);

	QMicroMapTest map(db, xmin, ymin, xmax, ymax, "lightblue");
	map.resize(1000,800);

	map.setWindowTitle("qmmtest");

	map.show();

	return app.exec();

}
