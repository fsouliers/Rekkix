/*!
 * \file Requirement.h
 * \brief Definition of the class Requirement
 * \date 2016-07-21
 * \author f.souliers
 */

#ifndef REQUIREMENT_H_
#define REQUIREMENT_H_

#include <QString>
#include <QVector>

class RequirementFileAbstract;

/*!
 * \class Requirement
 * \brief Class representing one single requirement
 */
class Requirement
{

public:
	/*!
	 * \brief invalid value for coverage, used for initialization (set to minimum double value
	 */
	static const double COVERAGE_INVALID_VALUE;

	/*!
	 * \typedef CreationState
	 * \brief State of a requirement
	 *
	 * The possible states are :
	 * - Defined : the requirement has been created after a regex match for requirement identification
	 * - Expected : the requirement has only been created because it is quoted as reference in another requirement
	 */
	typedef enum e_creationState
	{
		Defined, Expected
	} CreationState;

	/*!
	 * \brief Constructor
	 * \param[in] id unique identifier of the requirement, as regex matched in a requirement document
	 * \param[in] s  creation state of the requirement (has it been created after a requirement regex match or has it been
	 *               created because it is quoted as a reference in another requirement)
	 * \param[in] expected_by file id and requirement in witch the Requirement has been detected as expected
	 */
	Requirement(QString id = "", CreationState s = Expected, const QString& expected_by = "");

	/*!
	 * \brief Destructor, does nothing
	 */
	virtual ~Requirement();

	/*!
	 * \brief Getter for the ID of the requirement
	 * \return The ID of the requirement
	 */
	QString getId() const
	{
		return (__id);
	}

	/*!
	 * \brief Getter for the state of the requirement
	 * \return The state of the requirement
	 */
	CreationState getState() const
	{
		return (__reqState);
	}

	/*!
	 * \brief Setter for the state of the requirement
	 * \param[in] s  new state value
	 */
	void setState(CreationState s)
	{
		__reqState = s;
	}

	/*!
	 * \brief Getter for the location id, that is the file id of the file containing this requirement
	 * \return The file id of the file containing this requirement or empty string if the requirement is
	 *         still not defined
	 */
	QString getLocationId() const;

	/*!
	 * \brief Getter for the location, that is a pointer to the file containing this requirement
	 * \return The pointer to the file containing this requirement or NULL if the requirement is
	 *         still not defined
	 */
	RequirementFileAbstract* getLocation()
	{
		return (__location);
	}

	/*!
	 * \brief Setter for the location
	 * \param[in] p pointer to a requirement file that has to be stored as the location of the requirement
	 */
	void setLocation(RequirementFileAbstract* p)
	{
		__location = p;
	}

	/*!
	 * \brief if the requirement is part of a file that must have downstream, then this requirement must be covered
	 * \return
	 * The value of the attribute
	 */
	bool mustBeCovered() const
	{
		return (__mustBeCovered);
	}

	/*!
	 * \brief Setter for the attribute __mustBeCovered
	 * \param[in] m  value to use to force the attribute value
	 */
	void setMustBeCovered(bool m)
	{
		__mustBeCovered = m;
	}

	/*!
	 * \brief Getter to know if this requirement has a downstream one
	 * \return
	 * - true if the downstream requirement pointer is not NULL; else false
	 */
	bool isCovered()
	{
		return (!(__downstreamRequirement == NULL));
	}

	/*!
	 * \brief Getter for the downstream requirement
	 * \return
	 * A pointer to the downstream requirement. If there is no defined downstream requirement,
	 * then returns NULL
	 */
	Requirement* getDownstreamRequirement()
	{
		return (__downstreamRequirement);
	}

	/*!
	 * \brief Set the unique downstream requirement
	 * \param[in] p  pointer to the downstream requirement that must be stored
	 */
	void setDownstreamRequirement(Requirement* p)
	{
		__downstreamRequirement = p;
	}

	/*!
	 * \brief Getter for the coverage value
	 * \return the coverage value
	 */
	double getCoverage();

	/*!
	 * \brief Add one requirement to the list of requirements composing the current one
	 * \param[in] p  Pointer to the requirement that must be stored as a composing requirement
	 */
	void addComposingRequirement(Requirement* p)
	{
		__composingReqs.append(p);
	}

	/*!
	 * \brief Add one requirement to the list of upstream requirements for the current one
	 * \param[in] p  Pointer to the requirement that must be stored as an upstream requirement
	 */
	void addUpstreamRequirement(Requirement* p)
	{
		__upstreamRequirements.append(p);
	}

	/*!
	 * \brief Getter for the upstream requirements of the current one
	 * \return
	 * The vector of pointers to requirements stored as upstream
	 */
	QVector<Requirement*> getUpstreamRequirements()
	{
		return (__upstreamRequirements);
	}

	/*!
	 * \brief Getter for the composing requirements for the current one
	 * \return
	 * The vector of pointers to requirements stored as composing the current one
	 */
	QVector<Requirement*> getComposingRequirements()
	{
		return (__composingReqs);
	}

	/*!
	 * \brief Getter for the parent requirement
	 *
	 * If the requirement is composing another requirement then this last one is the parent of the current one
	 * \return
	 * A pointer to the parent object
	 */
	Requirement* getParent()
	{
		return (__parent);
	}

	/*!
	 * \brief Setter for the parent requirement
	 *
	 * If the requirement is composing another requirement then this last one is the parent of the current one
	 * \param[in] p pointer to the parent requirement
	 */
	void setParent(Requirement* p)
	{
		__parent = p;
	}

	/*!
	 * \brief Getter for the expected by location
	 * \return
	 * The file id of the file in which this requirement has been marked as expected
	 */
	QString getExpectedBy() const
	{
		return (__expectedBy);
	}

	/*!
	 * \brief Transform the requirement into a string
	 * \return A QString givin the states of all attributes (for debug purpose)
	 */
	QString toString() const;

	/*!
	 * \brief Computes whether the state of the requirement is acceptable or not
	 *
	 * For instance, if there is some errors in the documents, a requirement A can be composed of a
	 * requirement B that is itself composed of A ... in such a case, it is impossible to calculate
	 * a coverage.
	 *
	 * The consistency checks performed are :
	 * 1. Existence of the location file (the file in which the requirement is defined)
	 * 2. Verify that there is no loop in the downstream requirements (A covered by B covered by C covered by A)
	 * 3. Verify that there is no loop in the composing requirements (A composed of B and C, C composed of D and E, E composed of A and F)
	 * \return
	 * - true if there is no error and the requirement is consistent
	 * - false if there is any inconsistency. In such a case, an AnalysisError has been added to
	 *   ModelSngAnalysisErrors
	 */
	void computeConsistency() ;

	/*!
	 * \brief Getter for the consistency (see computeConsistency)
	 * \return
	 * - true if the requirement is consistent
	 * - false else
	 */
	bool isConsistent() const
	{
		return(__isConsistent) ;
	}


	/*!
	 * \brief Look for loop in the downstream chain of a requirement
	 * \param[inout]  dwnChain   Downstream chain A -> B -> C  (where -> stands for "covered by") in which
	 *                           a loop may be found
	 * \return
	 * - true if a loop has been detected
	 * - else false
	 */
	bool hasLoopInDownstream(QString& dwnChain) ;

	/*!
	 * \brief Look for loop in the composing chain of a requirement
	 * \param[inout]  cmpChain   Composing chain A -> B -> C  (where -> stands for "is composed of") in which
	 *                           a loop may be found
	 * \return
	 * - true if a loop has been detected
	 * - else false
	 */
	bool hasLoopInComposingReqs(QString& cmpChain) ;

	/*!
	 * \brief Look for invalid requirements in the composing chain of requirements
	 * \param[inout] p_invalidReqs   String representing the invalid requirements found in the current one
	 * \return
	 * - true if there is at least one invalid composing requirement
	 * - false else
	 */
	bool hasInvalidComposingReqs(QString& p_invalidReqs) ;

private:
	/*!
	 * Creation state of the requirement, see CreationState
	 */
	CreationState __reqState;

	/*!
	 * Unique identifier of a requirement. If a requirement is defined several times,
	 * only the first definition is used (and an error is reported in the analysis)
	 */
	QString __id;

	/*!
	 * Is this requirement supposed to be covered ? (it means is the document containing it
	 * supposed to have downstream documents ?)
	 */
	bool __mustBeCovered;

	/*!
	 * Location of a requirement : a requirement without location is a requirement
	 * mentioned as reference (covering, composing or covered) wich has never been found parsing
	 * the configured files
	 */
	RequirementFileAbstract* __location;

	/*!
	 * When the requirement is only Expected, this attributes keeps the value of the file id expecting it
	 */
	QString __expectedBy;

	/*!
	 * Coverage of the requirement :
	 * - If the requirement has got a __coveringReq, then its coverage is 1.0
	 * - If the requirement hasn't got a __coveringReq but has one or more __composingReqs, then its
	 *   coverage is the average value of the coverages of its composites
	 * - Else, its coverage is 0.0
	 */
	double __coverage;

	/*!
	 * A requirement must be covered by one, and only one, other requirement. If a requirement is
	 * covered several times, only the first coverage is used (and an error is reported in the analysis)
	 */
	Requirement* __downstreamRequirement;

	/*!
	 * If a requirement is composing another one, it can have one (and only one) parent
	 * requirement
	 */
	Requirement* __parent;

	/*!
	 * true if everything is properly defined for this requirement else false (location missing,
	 * loop detected in downstream chain or in composing chain). By default, a requirement is not consistent
	 * until it has been verified as consistent.
	 */
	bool __isConsistent ;

	/*!
	 * true if the consistency has already been calculated, else false
	 */
	bool __isConsistencyAlreadyCalculated ;

	/*!
	 * If the requirement is a composite of one or more other requirements, this
	 * attribute contains the links to the other requirements composing the
	 * current one
	 */
	QVector<Requirement*> __composingReqs;

	/*!
	 * If the requirement is covering one or more other requirements, this attribute
	 * contains the links to the covered requirements
	 */
	QVector<Requirement*> __upstreamRequirements;
};

/*!
 * \typedef RequirementRef
 * \brief Just to make the code easier to type when a reference of Requirement is required
 */
typedef Requirement& RequirementRef;

/*!
 * \typedef RequirementPtr
 * \brief Just to make the code easier to type when a pointer to Requirement is required
 */
typedef Requirement* RequirementPtr;

#endif /* REQUIREMENT_H_ */
