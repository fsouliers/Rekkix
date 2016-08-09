/*
 * Requirement.h
 *
 *  Created on: 21 juil. 2016
 *      Author: f.souliers
 */

#ifndef REQUIREMENT_H_
#define REQUIREMENT_H_

#include <QString>
#include <QVector>

class IRequirementFile ;

class Requirement
{
	public:
		static const double COVERAGE_INVALID_VALUE ; // -1.0

		/*!
		 * State of a requirement :
		 * - Defined : the requirement has been created following a regex match for requirement identification
		 * - Expected : the requirement has only been created because it is quoted as reference in another requirement
		 */
		typedef enum e_creationState {Defined, Expected} CreationState ;

		/*!
		 * - id : unique identifier of the requirement, as regex matched in a requirement document
		 * - s : creation state of the requirement (has it been created following a requirement regex match or has it been
		 *       created because it is quoted as a reference in another requirement
		 * - expected_by : file id in witch the Requirement has been detected as expected
		 */
		Requirement(QString id = "", CreationState s = Expected, const QString& expected_by = "");
		virtual ~Requirement();


		QString getId() const {return(__id) ;}
		void setId(const QString& i) {__id = i ;}

		CreationState getState() const {return(__reqState) ;}
		void setState(CreationState s) {__reqState = s ;}

		QString getLocationId() const ;
		IRequirementFile* getLocation() {return(__location) ; }
		void setLocation(IRequirementFile* p) { __location = p ;}

		bool mustBeCovered() const {return(__mustBeCovered) ;}
		void setMustBeCovered(bool m) {__mustBeCovered = m ; }


		bool isCovered() { return( ! (__downstreamRequirement == NULL) ) ;}
		Requirement* getDownstreamRequirement() { return(__downstreamRequirement) ; }
		void setDownstreamRequirement(Requirement* p) { __downstreamRequirement = p ;}

		void forceCoverage(double v) { __coverage = v ;}
		double getCoverage() ;

		void addComposingRequirement(Requirement* p) {__composingReqs.append(p) ;}

		void addUpstreamRequirement(Requirement* p) {__upstreamRequirements.append(p) ;}

		QVector<Requirement*> getUpstreamRequirements() {return(__upstreamRequirements) ;}

		QVector<Requirement*> getComposingRequirements() {return(__composingReqs) ;}
		Requirement* getParent() {return(__parent) ; }
		void setParent(Requirement* p) {__parent = p ; }

		QString getExpectedBy() const {return(__expectedBy) ; }

		QString toString() const ;

	private:
		/*!
		 * Creation state of the requirement, see CreationState
		 */
		CreationState __reqState ;

		/*!
		 * Unique identifier of a requirement. If a requirement is defined several times,
		 * only the first definition is used (and an error is reported in the analysis)
		 */
		QString __id ;

		/*!
		 * Is this requirement supposed to be covered ? (it means is the document containing it
		 * supposed to have downstream documents ?)
		 */
		bool __mustBeCovered ;

		/*!
		 * Location of a requirement : a requirement without location is a requirement
		 * mentioned as reference (covering, composing or covered) wich has never been found parsing
		 * the configured files
		 */
		IRequirementFile* __location ;

		/*!
		 * When the requirement is only Expected, this attributes keeps the value of the file id expecting it
		 */
		QString __expectedBy ;

		/*!
		 * Coverage of the requirement :
		 * - If the requirement has got a __coveringReq, then its coverage is 1.0
		 * - If the requirement hasn't got a __coveringReq but has one or more __composingReqs, then its
		 *   coverage is the average value of the coverages of its composites
		 * - Else, its coverage is 0.0
		 */
		double __coverage ;

		/*!
		 * A requirement must be covered by one, and only one, other requirement. If a requirement is
		 * covered several times, only the first coverage is used (and an error is reported in the analysis)
		 */
		Requirement* __downstreamRequirement ;

		/*!
		 * If a requirement is composing another one, it can have one (and only one) parent
		 * requirement
		 */
		Requirement* __parent ;

		/*!
		 * If the requirement is a composite of one or more other requirements, this
		 * attribute contains the links to the other requirements composing the
		 * current one
		 */
		QVector<Requirement*> __composingReqs ;

		/*!
		 * If the requirement is covering one or more other requirements, this attribute
		 * contains the links to the covered requirements
		 */
		QVector<Requirement*> __upstreamRequirements ;
};

typedef Requirement& RequirementRef ;

typedef Requirement* RequirementPtr ;

#endif /* REQUIREMENT_H_ */
