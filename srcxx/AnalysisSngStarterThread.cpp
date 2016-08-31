/*!
 * \file AnalysisSngStarterThread.cpp
 * \brief Implementation of the class AnalysisSngStarterThread
 * \date 2016-08-31
 * \author f.souliers
 */
#include <QDebug>

#include "AnalysisSngStarterThread.h"
#include "ModelSngReqMatrix.h"

AnalysisSngStarterThread::AnalysisSngStarterThread()
	: QThread(), QSemaphore()
{

}

AnalysisSngStarterThread::~AnalysisSngStarterThread()
{

}

void AnalysisSngStarterThread::run()
{
	QVector<RequirementFileAbstractPtr>::Iterator it_rf ;
	for (it_rf = __registeredFiles.begin() ; it_rf != __registeredFiles.end() ; ++it_rf)
	{
		qDebug() << "AnalysisSngStarterThread::run --> remaining available : " << available() ;

		acquire(1) ; // Semaphore control : keep stuck on this call until enough threads have terminated their job
		             // (ressource is released in Rekkix::slt_reqFileParsingOneMoreFileFinished)

		(*it_rf)->start() ;
	}
}

