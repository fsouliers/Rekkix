/*!
 * \file SngSettings.h
 * \brief Definition of the class SngSettings
 * \date 2016-09-02
 * \author f.souliers
 */

#ifndef SNGSETTINGS_H_
#define SNGSETTINGS_H_

#include <QSettings>

/*!
 * \class SngSettings
 * \brief Class used to access parameters of the application (in user space)
 */
class SngSettings
{
public:
	/*!
	 * \brief Instance getter for the singleton object
	 * \return The reference of the instance
	 */
	static SngSettings& instance()
	{
		static SngSettings r;
		return (r);
	}

	virtual ~SngSettings();

	/*!
	 * \brief Name of the «General» section of parameters
	 */
	static const QString section_general ;

	/*!
	 * \brief Name of the key for the current application version used to store the settings
	 */
	static const QString key_version ;

	/*!
	 * \brief Name of the key for the number of threads used for parsing files. 10 by default
	 */
	static const QString key_parsingThreadsNum ;

	/*!
	 * \brief Value of the current application version
	 */
	static const QString value_version ;

	/*!
	 * \brief Default number of parsing threads
	 */
	static const int value_nbParsingThreadsDefault ;

	/*!
	 * \brief getter for the number of parsing threads to be used
	 */
	int getNbParsingThreads()
	{
		__settings.beginGroup(section_general) ;
		int val = __settings.value(key_parsingThreadsNum, value_nbParsingThreadsDefault).toInt() ;
		__settings.endGroup() ;

		return(val) ;
	}

	/*!
	 * \brief setter for the number of parsing threads to be used
	 *
	 * The control of the possible values is ensured through spinbox limits in the GUI
	 */
	void setNbParsingThreads(int p_nb)
	{
		__settings.beginGroup(section_general) ;
		__settings.setValue(key_parsingThreadsNum, p_nb) ;
		__settings.endGroup() ;
	}

private:
	SngSettings();

	QSettings __settings ;
};

#endif /* SNGSETTINGS_H_ */
