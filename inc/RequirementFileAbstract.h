/*!
 * \file RequirementFileAbstract.h
 * \brief Definition of the abstract class RequirementFileAbstract
 * \date 2016-08-15
 * \author f.souliers
 */

#ifndef REQUIREMENTFILEABSTRACT_H_
#define REQUIREMENTFILEABSTRACT_H_

#include <QThread>
#include <QVector>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "ModelConfiguration.h"
#include "Requirement.h"

/*!
 * \class RequirementFileAbstract
 * \brief Abstract class used to define the common behaviors and attributes of every document containing
 *        requirements.
 *
 * This class cannot be used directly: it must be subclassed to implement the parsing of a specifing file
 * format.
 *
 * From a general point of view, a requirement file is composed of many things almost totally uninteresting ;)
 * except text parts such as :
 * > REQ_ID1<br/>
 * > Composed of : REQ_ID2, REQ_ID3<br/>
 * > Covers : REQ_ID4, REQ_ID5<br/>
 * > Blah blah blah blah blah
 *
 * This must be understood as <i>"In this file, there is a requirement named REQ_ID1 which is composed of
 * REQ_ID2 and REQ_ID3. Furthermore, this requirement REQ_ID1 covers the upstream requirements REQ_ID4 and
 * REQ_ID5."</i> <br/>
 * <i>Blah blah blah</i>  is the real description of the requirement. By now, Rekkix does not interpret at all this
 * part of the file and will start interpreting a new requirement when the requirement regex
 * (see ModelConfiguration::REQFILE_ATTR_REQREGEX) matches again.
 */
class RequirementFileAbstract : public QThread
{
	Q_OBJECT

public:
	/*!
	 * \brief Constructor for the non pure abstract part of the class
	 * \param[in]  p_cnfFile   Map of the configuration attributes for this particular file.
	 *
	 * This constructor also initialize the patterns for all the regular expressions needed in file parsing
	 * step.
	 */
	RequirementFileAbstract(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile);

	/*!
	 * \brief Destructor of the class, does nothing special.
	 */
	virtual ~RequirementFileAbstract();

	/*!
	 * \brief method called by Rekkix internals to parse a file containing requirements
	 *
	 * This methods must call several methods of ModelSngReqMatrix to load the requirements
	 * that have been identified during parsing.
	 *
	 * \see RequirementFile_docx for examples of implementation of this method
	 */
	virtual void parseFile() = 0;

	/*!
	 * \brief method called to run the parsing into a dedicated thread.
	 *
	 * It only calls parseFile that must be defined by each subclass and send a signal to inform that the
	 * parsing of the file is terminated
	 */
	void run() Q_DECL_OVERRIDE
	{
		parseFile() ;
		emit parsingFinished() ;
	}

	/*!
	 * \brief Test if a CMP regex has been defined
	 * \return true if a CMP regex has been defined, else false
	 * \see ModelConfiguration::REQFILE_ATTR_CMPREGEX
	 */
	bool hasCmpRegex() const
	{
		return (!_cnfFile[ModelConfiguration::REQFILE_ATTR_CMPREGEX].isEmpty());
	}

	/*!
	 * \brief Test if a COV regex has been defined
	 * \return true if a COV regex has been defined, else false
	 * \see ModelConfiguration::REQFILE_ATTR_COVREGEX
	 */
	bool hasCovRegex() const
	{
		return (!_cnfFile[ModelConfiguration::REQFILE_ATTR_COVREGEX].isEmpty());
	}

	/*!
	 * \brief Test if a stopafter regex has been defined
	 * \return true if a stopafter regex has been defined, else false
	 * \see ModelConfiguration::REQFILE_ATTR_STOPAFTERREGEX
	 */
	bool hasStopAfterRegex() const
	{
		return (!_cnfFile[ModelConfiguration::REQFILE_ATTR_STOPAFTERREGEX].isEmpty());
	}

	/*!
	 * \brief getter for the attribute ModelConfiguration::REQFILE_ATTR_ID
	 * \return the id of this file as configured in the configuration file
	 */
	QString getFileId() const
	{
		return (_cnfFile[ModelConfiguration::REQFILE_ATTR_ID]);
	}

	/*!
	 * \brief getter for the attribute ModelConfiguration::REQFILE_ATTR_PATH
	 * \return the file absolute path of this file as configured in the configuration file
	 * (see ModelConfiguration::setFile)
	 */
	QString getFilePath() const
	{
		return (_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH]);
	}

	/*!
	 * \brief getter for the expected presence of downstream documents
	 * \return
	 * - true if, according to the configuration file, this document must have downstream documents
	 * - false else. In this case, it means that not any requirement of this document has to be covered
	 */
	bool mustHaveDownstreamDocuments() const
	{
		return (_cnfFile[ModelConfiguration::REQFILE_ATTR_HASDWN] == ModelConfiguration::REQFILE_ATTR_VALUE_YES);
	}

	/*!
	 * \brief getter for the expected presence of upstream documents
	 * \return
	 * - true if, according to the configuration file, this document must have upstream documents
	 * - false else. In this case, it means that not any requirement of this document is covering another one
	 */
	bool mustHaveUpstreamDocuments() const
	{
		return (_cnfFile[ModelConfiguration::REQFILE_ATTR_HASUP] == ModelConfiguration::REQFILE_ATTR_VALUE_YES);
	}

	/*!
	 * \brief compute the average coverage for the file
	 */
	void computeCoverage();

	/*!
	 * \brief Stores a pointer to a requirement file, considering the pointed file is an upstream document
	 *        for the current one.
	 * \param[in] p_fileId  Unique ID of the file
	 * \param[in] p_file    Pointer to the corresponding requirement file object
	 */
	void addUpstreamDocument(const QString& p_fileId, RequirementFileAbstract* p_file);

	/*!
	 * \brief Getter for the upstream documents of this file
	 * \return
	 * The map of all the upstream documents of this file:
	 * - the key of the map is the unique ID of the file
	 * - the value is the pointer to the corresponding file object
	 */
	QMap<QString, RequirementFileAbstract*> getUpstreamDocuments() const
	{
		return (_upstreamDocs);
	}

	/*!
	 * \brief Stores a pointer to a requirement file, considering the pointed file is a downstream document
	 *        for the current one.
	 * \param[in] p_fileId  Unique ID of the file
	 * \param[in] p_file    Pointer to the corresponding requirement file object
	 */
	void addDownstreamDocument(const QString& p_fileId, RequirementFileAbstract* p_file);

	/*!
	 * \brief Getter for the downstream documents of this file
	 * \return
	 * The map of all the downstream documents of this file:
	 * - the key of the map is the unique ID of the file
	 * - the value is the pointer to the corresponding file object
	 */
	QMap<QString, RequirementFileAbstract*> getDownstreamDocuments() const
	{
		return (_downstreamDocs);
	}

	/*!
	 * \brief Getter of the current value of the coverage for this file
	 * \return The last known value for the average coverage calculated for this file
	 */
	double getCoverage() const
	{
		return (_avgCoverage);
	}

	/*!
	 * \brief Setter for the requirements defined in this file
	 * \param[in] p Vector of requirements that must be considered as all the requirements defined in this file
	 *
	 * This method is called by the generic layer of the software once the file has been parsed,
	 * see ModelSngReqMatrix::addRequirementFile. The main goal of processing requirements in this way is
	 * to avoid any multi-definition of requirements: the vector p only contains pointers to requirements defined
	 * only once.
	 */
	void setRequirements(const QVector<Requirement*>& p)
	{
		_requirements = p;
	}

	/*!
	 * \brief Getter for the requirements defined in this file
	 * \return
	 * The vector of pointers to the requirement objects defined in this file
	 */
	const QVector<Requirement*>& getRequirements() const
	{
		return (_requirements);
	}

	/*!
	 * \brief setter for the _threadLaunched flag
	 */
	void setThreadActuallyLaunched(bool p_v = true)
	{
		_threadLaunched = p_v ;
	}

	/*!
	 * \brief Getter for the _threadLaunched flag
	 * \return
	 * - true if the thread has been actually started (so wait method is meaningfull)
	 * - false if the thread has never been started
	 */
	bool isThreadActuallyLaunched()
	{
		return(_threadLaunched) ;
	}

signals:
	/*!
	 * \brief Signal emitted when the thread has finished to parse its associated file.
	 */
	void parsingFinished() ;

protected:
	/*!
	 * \brief Average coverage ... _sumOfCov / _nbReqs ... at the end of coverage computing
	 */
	double _avgCoverage;

	/*!
	 * \brief Requirement File as seen in the configuration file : id, path, patterns ...
	 */
	ModelConfiguration::CnfFileAttributesMap_t _cnfFile;

	/*!
	 * \brief Regular expression used to match the requirements in the document.
	 * \see ModelConfiguration::REQFILE_ATTR_REQREGEX
	 */
	QRegularExpression _regexReq;

	/*!
	 * \brief Regular expression used to match the beginning of a list of requirements composing the current one
	 * \see ModelConfiguration::REQFILE_ATTR_CMPREGEX
	 */
	QRegularExpression _regexCmp;

	/*!
	 * \brief Regular expression used to match a the beginning of a list of covered requirements by the current one
	 * \see ModelConfiguration::REQFILE_ATTR_COVREGEX
	 */
	QRegularExpression _regexCov;

	/*!
	 * \brief Regular expression used to match the regex indicating the end of file parsing
	 * \see ModelConfiguration::REQFILE_ATTR_STOPAFTERREGEX
	 */
	QRegularExpression _regexStopAfter;

	/*!
	 * \brief Map of upstream files.
	 *
	 * It means the map of pointers to files for which the current one is covering at least
	 * one requirement.
	 * - key : file id as defined in the configuration
	 * - value : pointer to the requirement file
	 */
	QMap<QString, RequirementFileAbstract*> _upstreamDocs;

	/*!
	 * \brief Map of downstream files.
	 *
	 * It means map of pointers to files for which at least a requirement is covering a requirement
	 * of the current file.
	 * - key : file id as defined in the configuration
	 * - value : pointer to the requirement file
	 */
	QMap<QString, RequirementFileAbstract*> _downstreamDocs;

	/*!
	 * \brief requirements read after parsing (automatically set by the abstract layer)
	 */
	QVector<Requirement*> _requirements;

	/*!
	 * \brief flag used to verify that the thread has been actually started
	 *
	 * When this flag is true, the file has been recognized as actually started.
	 */
	bool _threadLaunched ;

	/*!
	 * \brief Checks if the text matches the req_regex
	 * \param[in] p_text  Text to be checked
	 * \param[out] p_reqfound  If the regex matches, then it contains the requirement identified.
	 *                         If the regex doesn't match, it contains an empty string
	 * \param[out] p_reqAcceptable  true if the identified requirement has been accepted by the matrix,
	 *                              false if the global matrix rejected the requirement
	 * \return
	 * - true if the regex matched
	 * - false else
	 */
	bool _hasStoredAnyRequirementDefinition(const QString& p_text, QString& p_reqfound, bool& p_reqAcceptable);

	/*!
	 * \brief Checks if the text matches the cmp_regex
	 *
	 * If the «composed of several requirements» regex matches, then those must be stored in the global
	 * matrix (ModelSngReqMatrix) as expected requirements (if they are already defined, then
	 * ModelSngReqMatrix handles it)
	 * \param[in] p_text  Text to be checked
	 * \param[in] p_parentReqId  The requirement id of the parent of the composite requirements that are going
	 *                       to be stored
	 * \return
	 * - true if the regex matched
	 * - false else
	 */
	bool _hasStoredAnyExpectedCompositeRequirements(const QString& p_text, const QString& p_parentReqId);

	/*!
	 * \brief Checks if the text matches the cov_regex
	 *
	 * If the «covering several requirements» regex matches, then those must be stored in the global
	 * matrix (ModelSngReqMatrix) as expected requirements (if they are already defined, then
	 * ModelSngReqMatrix handles it)
	 * \param[in] p_text  Text to be checked
	 * \param[in] p_currentReqId  The current requirement id (it means the one covering the upstream requirements)
	 * \return
	 * - true if the regex matched
	 * - false else
	 */
	bool _hasStoredAnyExpectedUpstreamRequirements(const QString& p_text, const QString& p_currentReqId) ;

	/*!
	 * \brief checks whether the "Stop After" regex has been reached
	 * \param[in] p_text  Text to be checked
	 * \return
	 * - true if the regex matched
	 * - false else
	 */
	bool _mustStopParsing(const QString& p_text) ;
};

/*!
 * \typedef RequirementFileAbstractPtr
 * \brief Just to make the code easier to type when a pointer to RequirementFileAbstract is required
 */
typedef RequirementFileAbstract* RequirementFileAbstractPtr;

#endif /* REQUIREMENTFILEABSTRACT_H_ */
