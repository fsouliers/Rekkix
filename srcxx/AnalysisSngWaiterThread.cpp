/*!
 * \file AnalysisSngWaiterThread.cpp
 * \brief Implementation of the class AnalysisSngWaiterThread
 * \date 2016-08-31
 * \author f.souliers
 */

#include "AnalysisSngWaiterThread.h"
#include "ModelSngReqMatrix.h"

AnalysisSngWaiterThread::AnalysisSngWaiterThread()
	: QThread()
{

}

AnalysisSngWaiterThread::~AnalysisSngWaiterThread()
{

}

void AnalysisSngWaiterThread::run()
{
	// 2nd Step of analysis : wait until all threads have terminated their work
	bool allFinished ;
	QVector<RequirementFileAbstractPtr>::Iterator it_rf ;
	do
	{
		allFinished = true ; // by default, suppose all threads are terminated

		for (it_rf = __registeredFiles.begin() ; it_rf != __registeredFiles.end() ; ++it_rf)
		{
			// Only wait for threads that have been actually launched (otherwise waits return true in any case),
			// and only wait for 5ms for termination
			if ((*it_rf)->isThreadActuallyLaunched() && (*it_rf)->wait(5))
			{
				ModelSngReqMatrix::instance().updateRequirementFileWithAcceptedRequirements(*it_rf) ;
				__registeredFiles.remove(__registeredFiles.indexOf(*it_rf)) ;
			}
			else
			{
				// at least one isn't finished
				allFinished = false ;
			}
		}

		// Don't overload the CPU, if the threads hasn't been launched yet, we can wait a little bit
		QThread::msleep(50) ;
	} while (!allFinished) ;

	// now all the files have been parsed, let's emit the information
	emit allRegisteredTerminated() ;
}

