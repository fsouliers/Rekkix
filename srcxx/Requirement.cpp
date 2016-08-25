/*!
 * \file Requirement.cpp
 * \brief Implementation of the class Requirement
 * \date 2016-07-21
 * \author f.souliers
 */

#include <QDebug>
#include <limits>

#include "Requirement.h"
#include "IRequirementFile.h"
#include "AnalysisError.h"
#include "ModelSngAnalysisErrors.h"

/*!
 * \brief A coverage value must be >= 0, so lowest double can be used as invalid value
 */
constexpr double Requirement::COVERAGE_INVALID_VALUE = std::numeric_limits<double>::lowest() ;

/*!
 * \brief delimiter used in the construction of downstream / composing chain when looking for loops
 */
static const QString chainDelim = "->" ;

Requirement::Requirement(QString id, CreationState s, const QString& expected_by)
		: __reqState(s),
		  __id(id),
		  __mustBeCovered(false),
		  __location(NULL),
		  __expectedBy(expected_by),
		  __coverage(COVERAGE_INVALID_VALUE),
		  __downstreamRequirement(NULL),
		  __parent(NULL),
		  __isConsistent(false),
		  __isConsistencyAlreadyCalculated(false)

{
}

Requirement::~Requirement()
{
}

QString Requirement::getLocationId() const
{
	QString ret = "";
	if (__location) ret = __location->getFileId();
	return (ret);
}

double Requirement::getCoverage()
{
	// if the requirement is not consistent, do not calculate its coverage
	if (!__isConsistent) return(COVERAGE_INVALID_VALUE) ;

	// If the coverage has already been computed (by recursive call),
	// there is no need to compute it again
	if (__coverage == COVERAGE_INVALID_VALUE)
	{
		if (__downstreamRequirement)
		{
			__coverage = 1.0;
		}
		else if (__composingReqs.count() == 0)
		{
			__coverage = 0.0;
		}
		else
		{
			double sum = 0.0;
			foreach(Requirement* r, __composingReqs)
			{
				sum += r->getCoverage();
			}
		}
	}

	return (__coverage);
}

void Requirement::computeConsistency()
{
	bool errLoc = false ;
	bool errDwnLoop = false ;
	bool errCmpLoop = false ;
	bool errCmp = false ;

	if (!__isConsistencyAlreadyCalculated)
	{
		// 1. Check for requirement file
		if (!__location)
		{
			AnalysisError e(AnalysisError::WARNING,
			                AnalysisError::CONSISTENCY,
			                __expectedBy,
			                QObject::trUtf8("Exigence attendue mais définie dans aucun fichier : %1").arg(__id));

			ModelSngAnalysisErrors::instance().addError(e);
			errLoc = true ;
		}


		// 2. Check for infinite loop in downstream requirements
		// (A covers B, B covers C, C covers A)
		QString chain = "" ;
		errDwnLoop = hasLoopInDownstream(chain) ;
		if (errDwnLoop)
		{
			AnalysisError e(AnalysisError::ERROR,
			                AnalysisError::CONSISTENCY,
			                __expectedBy,
			                QObject::trUtf8("%1 : Rebouclage dans la *couverture* des exigences : %2").arg(__id).arg(chain));

			ModelSngAnalysisErrors::instance().addError(e);
		}

		// 3. Check for infinite loop in composing requirements
		// A composed of B, B composed of A
		chain = "" ;
		errCmpLoop = hasLoopInComposingReqs(chain) ;
		if (errCmpLoop)
		{
			AnalysisError e(AnalysisError::ERROR,
			                AnalysisError::CONSISTENCY,
			                __expectedBy,
			                QObject::trUtf8("%1 : Rebouclage dans la *composition* des exigences : %2").arg(__id).arg(chain));

			ModelSngAnalysisErrors::instance().addError(e);
		}
		else
		{
			// 4. Check for inconsistent requirements in composing requirements ; if there is a loop in composing
			// requirements --> nothing to test, we already know it is invalid
			errCmp = hasInvalidComposingReqs() ;
			if (errCmp)
			{
				AnalysisError e(AnalysisError::ERROR,
				                AnalysisError::CONSISTENCY,
				                __expectedBy,
				                QObject::trUtf8("%1 est composée d'au moins une exigence invalide").arg(__id));

				ModelSngAnalysisErrors::instance().addError(e);
			}
		}

		__isConsistent = !(errLoc || errDwnLoop || errCmpLoop || errCmp) ;
		__isConsistencyAlreadyCalculated = true ;
	}
}

QString Requirement::toString() const
{
	QString out("");
	out += "My state is : " + (__reqState == Defined ? QString("Defined") : QString("Expected"));
	if (__location)
	{
		out += "\nI am located at : " + __location->getFileId();
		out += (__mustBeCovered ? QString("\nI must be covered") : QString("\nI should not be covered"));
	}
	else
	{
		out += "\nI am located somewhere in a parallel dimension";
	}

	out += QString("\nMy coverage is : %1").arg(__coverage);
	if (__downstreamRequirement)
	{
		out += QString("\nI have a direct covering requirement which is : %1").arg(
		        __downstreamRequirement->getId());
	}
	else
	{
		out += "\nI don't have any direct covering requirement";
	}

	int cnt;
	cnt = __composingReqs.count();
	if (cnt > 0)
	{
		out += QString("\nI am composed of %1 subrequirements :").arg(cnt);
		foreach(Requirement* r, __composingReqs)
		{
			out += "\n    - " + r->getId();
		}
	}
	else
	{
		out += "\nI don't have any subrequirement";
	}

	cnt = __upstreamRequirements.count();
	if (cnt > 0)
	{
		out += QString("\nI am covering %1 requirements :").arg(cnt);
		foreach(Requirement* r, __upstreamRequirements)
		{
			out += "\n    - " + r->getId();
		}
	}
	else
	{
		out += "\nI am not covering any requirement";
	}

	return (out);
}

bool Requirement::hasLoopInDownstream(QString& dwnChain)
{
	bool retVal = false ;

	if (dwnChain.contains(__id))
	{
		dwnChain += chainDelim + __id ;
		retVal = true ;
	}
	else if (__downstreamRequirement)
	{
		dwnChain += chainDelim + __id ;
		retVal = __downstreamRequirement->hasLoopInDownstream(dwnChain) ;
	}

	return(retVal) ;
}

bool Requirement::hasLoopInComposingReqs(QString& cmpChain)
{
	bool retVal = false ;

	if (cmpChain.contains(__id))
	{
		cmpChain += chainDelim + __id ;
		retVal = true ;
	}
	else if (__composingReqs.count() > 0)
	{
		cmpChain += chainDelim + __id ;
		QString base = cmpChain ;

		foreach(Requirement* r, __composingReqs)
		{
			retVal = r->hasLoopInComposingReqs(cmpChain) ;
			if (retVal) break ;

			cmpChain = base ; // reset cmpChain as it has been modified by recursive call of hasLoopInComposingReqs
		}
	}

	return(retVal) ;
}

bool Requirement::hasInvalidComposingReqs()
{
	bool retVal = false ;

	foreach(Requirement* r, __composingReqs)
	{
		r->computeConsistency() ;
		retVal |= r->isConsistent() ;
	}

	return(retVal) ;
}
