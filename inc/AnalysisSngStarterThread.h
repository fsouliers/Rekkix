/*!
 * \file AnalysisSngStarterThread.h
 * \brief Definition of the class AnalysisSngStarterThread
 * \date 2016-08-31
 * \author f.souliers
 */

#ifndef ANALYSISSNGSTARTERTHREAD_H_
#define ANALYSISSNGSTARTERTHREAD_H_

#include <QVector>
#include <QThread>
#include <QSemaphore>

#include "RequirementFileAbstract.h"

/*!
 * \class AnalysisSngStarterThread
 * \brief Thread used to start all the file parsing thread out of the Qt main loop
 */
class AnalysisSngStarterThread : public QThread, public QSemaphore
{
	Q_OBJECT

public:
	/*!
	 * \brief Instance getter for the singleton object
	 * \return The reference of the instance
	 */
	static AnalysisSngStarterThread& instance()
	{
		static AnalysisSngStarterThread r;
		return (r);
	}

	/*!
	 * \brif Destructor, does nothing special
	 */
	virtual ~AnalysisSngStarterThread();

	/*!
	 * \brief Setter for the __registeredFiles attribute
	 * \param[in] p_files   Vector of files for which a thread ending is expected
	 */
	void setRegisteredFiles(const QVector<RequirementFileAbstractPtr>& p_files)
	{
		__registeredFiles = p_files ;
	}

	/*!
	 * \brief «main» method of the thread : walk through the list of files and send allRegisteredTerminated
	 * signal when all the files have been parsed
	 */
	void run() Q_DECL_OVERRIDE ;

private:

	/*!
	 * \brief Constructor, does nothing special
	 */
	AnalysisSngStarterThread();

	/*!
	 * \brief list of the files for which an end of parsing is monitored
	 */
	QVector<RequirementFileAbstractPtr> __registeredFiles ;

};

#endif /* ANALYSISSNGSTARTERTHREAD_H_ */
