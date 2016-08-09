#include <QApplication>
#include <QString>

#include "Rekkix.h"

#ifndef APP_VERSION
#define APP_VERSION "UNEXPECTED APP VERSION"
#endif

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(rekkix); // Corresponding to rekkix.qrc

	QApplication app(argc, argv);
	app.setApplicationVersion(APP_VERSION);

	RekkixPtr qp ;
	if ( argc == 2 )
	{
		// The argument given is the file that has to be opened
		qp = new Rekkix(&app, argv[1]) ;
	}
	else
	{
		// There is no file to open automaticaly
		qp = new Rekkix(&app) ;
	}

	qp->show() ;

	return(app.exec());
}
