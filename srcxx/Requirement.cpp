/*
 * Requirement.cpp
 *
 *  Created on: 21 juil. 2016
 *      Author: f.souliers
 */

#include "Requirement.h"
#include "IRequirementFile.h"

const double Requirement::COVERAGE_INVALID_VALUE = -1.0 ;

Requirement::Requirement(QString id, CreationState s, const QString& expected_by)
	: __reqState(s)
	, __id(id)
	, __mustBeCovered(false)
	, __location(NULL)
	, __expectedBy(expected_by)
	, __coverage(COVERAGE_INVALID_VALUE)
	, __downstreamRequirement(NULL)
	, __parent(NULL)

{
}

Requirement::~Requirement()
{
}


QString
Requirement::getLocationId() const
{
	QString ret = "" ;
	if (__location) ret = __location->getFileId() ;
	return(ret) ;
}


double
Requirement::getCoverage()
{
	// If the coverage has already been computed (by recursive call),
	// there is no need to compute it again
	if (__coverage == COVERAGE_INVALID_VALUE)
	{
		if (__downstreamRequirement)
		{
			__coverage = 1.0 ;
		}
		else if (__composingReqs.count() == 0)
		{
			__coverage = 0.0 ;
		}
		else
		{
			double sum = 0.0 ;
			foreach(Requirement* r, __composingReqs)
			{
				sum += r->getCoverage() ;
			}

			__coverage = sum / __composingReqs.count() ;
		}
	}


	return(__coverage) ;
}

QString
Requirement::toString() const
{
	QString out("") ;
	out += "My state is : " + (__reqState == Defined ? QString("Defined") : QString("Expected")) ;
	if (__location)
	{
		out += "\nI am located at : " + __location->getFileId() ;
		out += (__mustBeCovered ? QString("\nI must be covered") : QString("\nI should not be covered")) ;
	}
	else
	{
		out += "\nI am located somewhere in a parallel dimension" ;
	}

	out += QString("\nMy coverage is : %1").arg(__coverage) ;
	if (__downstreamRequirement)
	{
		out+= QString("\nI have a direct covering requirement which is : %1").arg(__downstreamRequirement->getId()) ;
	}
	else
	{
		out += "\nI don't have any direct covering requirement" ;
	}

	int cnt ;
	cnt = __composingReqs.count() ;
	if (cnt > 0)
	{
		out += QString("\nI am composed of %1 subrequirements :").arg(cnt) ;
		foreach(Requirement* r, __composingReqs)
		{
			out += "\n    - " + r->getId() ;
		}
	}
	else
	{
		out += "\nI don't have any subrequirement" ;
	}

	cnt = __upstreamRequirements.count() ;
	if (cnt > 0)
	{
		out += QString("\nI am covering %1 requirements :").arg(cnt) ;
		foreach(Requirement* r, __upstreamRequirements)
		{
			out += "\n    - " + r->getId() ;
		}
	}
	else
	{
		out += "\nI am not covering any requirement" ;
	}

	return(out) ;
}
