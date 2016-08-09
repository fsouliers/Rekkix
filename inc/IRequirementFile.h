/*
 * IRequirementFile.h
 *
 *  Created on: 15 juil. 2016
 *      Author: f.souliers
 *
 *  D'une façon ou une autre, un fichier de requirement a une structure suivante :
 *  PATTERN_EXIGENCE 1
 *
 *  bla bla bla bla bla
 *  pattern_couverture PATTERN_EXIGENCE_COUVERTE, PATTERN_EXIGENCE_COUVERTE
 *
 *  bla bla bla bla bla
 *  pattern_couverture PATTERN_EXIGENCE_COUVERTE
 *
 *  PATTERN_EXIGENCE 2
 *  ...
 */

#ifndef IREQUIREMENTFILE_H_
#define IREQUIREMENTFILE_H_

#include <QVector>
#include <QRegularExpression>
#include <QRegularExpressionMatch>


#include "ModelConfiguration.h"
#include "Requirement.h"

/*!
 * Abstract class used to define the common behaviours and attributes of all the documents containing
 * requirements.
 *
 * REQ_ID1
 * Composed of : REQ_ID2, REQ_ID3
 * Covers : REQ_ID4, REQ_ID5
 */
class IRequirementFile
{
public:
	IRequirementFile(ModelConfiguration::XmlAttributesMap_t& p_cnfFile);
	virtual ~IRequirementFile();

	/**
	 * TODO make some explanations ...
	 */
	virtual void parseFile() = 0 ;

	/*!
	 * true if a CMP regex has been defined, else false
	 */
	bool hasCmpRegex() { return(! _cnfFile[ModelConfiguration::REQFILE_ATTR_CMPREGEX].isEmpty()) ; }

	/*!
	 * true if a COV regex has been defined, else false
	 */
	bool hasCovRegex() { return(! _cnfFile[ModelConfiguration::REQFILE_ATTR_COVREGEX].isEmpty()) ; }

	/*!
	 * true if a stopafter regex has been defined, else false
	 */
	bool hasStopAfterRegex() { return(! _cnfFile[ModelConfiguration::REQFILE_ATTR_STOPAFTERREGEX].isEmpty()) ; }

	/*!
	 * returns the id of this file as configured in the XML configuration file
	 */
	QString getFileId() const { return(_cnfFile[ModelConfiguration::REQFILE_ATTR_ID]) ; }

	/*!
	 * returns the file absolute path of this file as configured in the XML configuration file
	 * (see ModelConfiguration::setFile)
	 */
	QString getFilePath() const { return(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH]) ; }

	/*!
	 * returns wether the document must have downstream documents or not (if not, it means that not
	 * any requirement of the document has to be covered)
	 */
	bool mustHaveDownstreamDocuments() const { return(_cnfFile[ModelConfiguration::REQFILE_ATTR_HASDWN] == ModelConfiguration::REQFILE_ATTR_VALUE_YES) ; }

	bool mustHaveUpstreamDocuments() const { return(_cnfFile[ModelConfiguration::REQFILE_ATTR_HASUP] == ModelConfiguration::REQFILE_ATTR_VALUE_YES) ; }


	/*!
	 * During coverage processing, add a requirement to the number of requirements of the file
	 * and update the average coverage of the file.
	 * p_coverage : coverage of the requirement to add, so it is possible for the method to
	 * calculate the new value of _avgCoverage
	 */
	void addOneMoreRequirementCoverage(double p_coverage) ;


	void addUpstreamDocument (const QString& p_fileId, IRequirementFile* p_file) ;
	QMap<QString, IRequirementFile*> getUpstreamDocuments() const { return(_upstreamDocs) ; }

	void addDownstreamDocument (const QString& p_fileId, IRequirementFile* p_file) ;
	QMap<QString, IRequirementFile*> getDownstreamDocuments() const { return(_downstreamDocs) ; }


	/*!
	 * Get the current value of the coverage for the file
	 */
	double getCoverage() const {return(_avgCoverage) ;}

	/*!
	 * This method is called by the generic layer of the software once the file has been parsed,
	 * see ReqMatrix::addRequirementFile
	 */
	void setRequirements(const QVector<Requirement*>& p) {_requirements = p ;}

	const QVector<Requirement*>& getRequirements() const {return(_requirements) ;}


protected:
	/*!
	 * Number of requirements identified in the file. This value is not intended to be set during
	 * parsing of the file --> to avoid misprogramming of parsers, they only take care of parsing
	 * files. This value is computed during coverage computation when all the requirements of all
	 * the files are known and usable by the common abstract part of the software
	 * (see ReqMatrix::computeCoverage)
	 */
	int _nbReqs ;

	/*!
	 * Sum of requirements coverage. This attribute is calculated during
	 * coverage computing.
	 */
	double _sumOfCov ;

	/*!
	 * Average coverage ... _sumOfCov / _nbReqs ... at the end of coverage
	 * computing
	 */
	double _avgCoverage ;

	/*!
	 * Requirement File as seen in the configuration file : id, path, patterns ...
	 */
	ModelConfiguration::XmlAttributesMap_t _cnfFile ;

	/*!
	 * Regular expression used to match the requirements in the document,
	 * see ModelConfiguration::REQFILE_ATTR_REQREGEX
	 */
	QRegularExpression _regexReq ;

	/*!
	 * Regular expression used to match the beginning of a list of requirements composing the current one
	 * see ModelConfiguration::REQFILE_ATTR_CMPREGEX
	 */
	QRegularExpression _regexCmp ;

	/*!
	 * Regular expression used to match a the beginning of a list of covered requirements by the current one
	 * see ModelConfiguration::REQFILE_ATTR_COVREGEX
	 */
	QRegularExpression _regexCov ;

	/*!
	 * Regular expression used to match the regex indicating the end of file parsing
	 * see ModelConfiguration::REQFILE_ATTR_STOPAFTERREGEX
	 */
	QRegularExpression _regexStopAfter ;

	/*!
	 * Map of upstream files. It means vector of files for which the current one is covering at least
	 * one requirement.
	 * - key : file id as defined in the XML configuration
	 * - value : pointer to the requirement file
	 */
	QMap<QString, IRequirementFile*> _upstreamDocs ;

	/*!
	 * Map of downstream files. It means vector of files for which at least a requirement is covering a requirement
	 * of the current file.
	 * - key : file id as defined in the XML configuration
	 * - value : pointer to the requirement file
	 */
	QMap<QString, IRequirementFile*> _downstreamDocs ;

	/*!
	 * requirements read after parsing (automatically set by the abstract layer)
	 */
	QVector<Requirement*> _requirements ;

};

typedef IRequirementFile* IRequirementFilePtr ;

#endif /* IREQUIREMENTFILE_H_ */
