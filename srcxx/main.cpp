/*!
 * \file main.cpp
 * \brief start of the program
 * \date 2016-07-15
 * \author f.souliers
 */

#include <QApplication>
#include <QString>

#include "Rekkix.h"

#ifndef APP_VERSION
#define APP_VERSION "UNEXPECTED APP VERSION"
#endif

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(rekkix);  // Corresponding to rekkix.qrc

	QApplication app(argc, argv);
	app.setApplicationVersion(APP_VERSION);

	RekkixPtr oRekkix;
	if (argc == 2)
	{
		// The argument given is the file that has to be opened
		oRekkix = new Rekkix(&app, argv[1]);

		//TODO Implement a real batch mode --> if no error in the configuration, automatically run analysis & generate report
	}
	else
	{
		// There is no file to open automatically
		oRekkix = new Rekkix(&app);
	}

	oRekkix->show();

	return (app.exec());
}
