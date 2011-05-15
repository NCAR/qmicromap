/*
 * qmmtest.cpp
 *
 *  Created on: May 2, 2011
 *      Author: martinc
 */

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include "QMicroMapTest.h"

void options(int argc, char**argv,
		std::string& dbpath,
		double& xmin,
		double& xmax,
		double& ymin,
		double& ymax) {

	extern char *optarg;
	int opt;
	bool err = false;

	while ((opt = getopt(argc, argv, "b:d:")) != -1) {
		switch (opt) {
		case 'b': {
			std::string arg(optarg);
			std::vector<std::string> tokens;
			std::string::size_type lastPos = arg.find_first_not_of(",", 0);
			std::string::size_type pos     = arg.find_first_of(",", lastPos);

			while (std::string::npos != pos || std::string::npos != lastPos)
			{
				tokens.push_back(arg.substr(lastPos, pos - lastPos));
				lastPos = arg.find_first_not_of(",", pos);
				pos     = arg.find_first_of(",", lastPos);
			}
			if (tokens.size() != 4) {
				err = true;
				break;
			}
			xmin = atof(tokens[0].c_str());
			ymin = atof(tokens[1].c_str());
			xmax = atof(tokens[2].c_str());
			ymax = atof(tokens[3].c_str());
			break;
		}
		case 'd':
			dbpath = std::string(optarg);
			break;
		default:
			break;
		}
	}

	if (dbpath.size() == 0) {
		err = true;
	}

	if (err) {
		std::cerr <<"usage: " << argv[0] << " -d db_path [-b xmin,ymin,xmax,ymax] [qt args]" << std::endl;
		exit(1);
	}
}

int main(int argc, char** argv) {

	std::string dbpath;
	double xmin = -180.0;
	double ymin =  -90.0;
	double xmax = 180.0;
	double ymax =  90.0;

	options(argc, argv, dbpath, xmin, ymin, xmax, ymax);

	// get the database
	SpatiaLiteDB db(dbpath);


	// Create a Qt application
	QApplication app(argc, argv);

	QMicroMapTest map(db, xmin, ymin, xmax, ymax, "lightblue");
	map.resize(1000,800);

	map.setWindowTitle(dbpath.c_str());

	map.show();

	return app.exec();

}
