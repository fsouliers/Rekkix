/*!
 * \file main.cpp
 * \brief start of the program
 * \date 2016-07-15
 * \author f.souliers
 */

#include <QDebug>
#include <QApplication>
#include <QString>

#include "Rekkix.h"

#ifndef APP_VERSION
#define APP_VERSION "UNEXPECTED APP VERSION"
#endif

static const QString BATCH_OPTION = "--batch" ;

static void usage()
{
	fprintf(stderr, "%s\n", QObject::trUtf8("Mauvaise utilisation de la commande rekkix").toStdString().c_str()) ;
	fprintf(stderr, "%s\n", QObject::trUtf8("Utilisation #1, simple utilisation de l'interface graphique").toStdString().c_str()) ;
	fprintf(stderr, "rekkix\n") ;
	fprintf(stderr, "%s\n", QObject::trUtf8("Utilisation #2, utilisation de l'interface graphique avec pré-chargement d'un fichier de configuration").toStdString().c_str()) ;
	fprintf(stderr, "rekkix ./myConfigFile.ini\n") ;
	fprintf(stderr, "%s\n", QObject::trUtf8("Utilisation #3, lancement en mode batch (console) : les messages d'erreurs seront affichés sur stderr").toStdString().c_str()) ;
	fprintf(stderr, "rekkix --batch ./myConfigFile.ini\n") ;
}


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(rekkix);  // Corresponding to rekkix.qrc

	QApplication app(argc, argv);
	app.setApplicationVersion(APP_VERSION);

	RekkixPtr oRekkix;
	if (argc == 1)
	{
		// no doubt, we are running in GUI mode
		oRekkix = new Rekkix(&app);
		oRekkix->show();
		return (app.exec());
	}
	else if (argc == 2)
	{
		// The argument given is necessarily the file that has to be opened
		oRekkix = new Rekkix(&app);
		oRekkix->loadFileAndInitGui(argv[1]) ;
		oRekkix->show();
		return (app.exec());
	}
	else if (argc == 3)
	{
		if (app.arguments().at(1) == BATCH_OPTION)
		{
			// running in batch mode --> no gui
			oRekkix = new Rekkix(&app, false);
			return (oRekkix->loadFileAndRunBatch(argv[2]));
		}
		else
		{
			usage() ;
			return(EXIT_FAILURE) ;
		}
	}
	else
	{
		usage() ;
		return(EXIT_FAILURE) ;
	}
}
