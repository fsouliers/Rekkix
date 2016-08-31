/*!
 * \file AnalysisSngWaiterThread.h
 * \brief Definition of the class AnalysisSngWaiterThread
 * \date 2016-08-31
 * \author f.souliers
 */

#ifndef ANALYSISSNGWAITERTHREAD_H_
#define ANALYSISSNGWAITERTHREAD_H_

#include <QVector>
#include <QThread>

#include "RequirementFileAbstract.h"

/*!
 * \class AnalysisSngWaiterThread
 * \brief Class used to «listen» to the end of parsing file threads and update the GUI accordingly, outside the Qt main loop
 */
class AnalysisSngWaiterThread : public QThread
{
	Q_OBJECT

public:
	/*!
	 * \brief Instance getter for the singleton object
	 * \return The reference of the instance
	 */
	static AnalysisSngWaiterThread& instance()
	{
		static AnalysisSngWaiterThread r;
		return (r);
	}

	/*!
	 * \brif Destructor, does nothing special
	 */
	virtual ~AnalysisSngWaiterThread();

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

public slots:

	/*!
	 * \brief When a file starts its parsing, it must inform the waiter ... to wait for him ...
	 */
	void slt_parsingStarted()
	{
		static_cast<RequirementFileAbstractPtr>(QObject::sender())->setThreadActuallyLaunched() ;
	}

signals:

	/*!
	 * \brief this signal is emitted when all the registered files have been parsed
	 */
	void allRegisteredTerminated() ;

private:

	/*!
	 * \brief Constructor, does nothing special
	 */
	AnalysisSngWaiterThread();

	/*!
	 * \brief list of the files for which an end of parsing is monitored
	 */
	QVector<RequirementFileAbstractPtr> __registeredFiles ;
};

#endif /* ANALYSISSNGWAITERTHREAD_H_ */
