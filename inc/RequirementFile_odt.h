/*!
 * \file RequirementFile_odt.h
 * \brief Definition of the class RequirementFile_odt
 * \date 2016-08-17
 * \author f.souliers
 */

#ifndef REQUIREMENTFILEODT_H_
#define REQUIREMENTFILEODT_H_

#include <QDomElement>

#include "RequirementFileAbstract.h"

/*!
 * \class RequirementFile_odt
 * \brief Implements the RequirementFileAbstract interface for OpenDocument format (typically LibreOffice)
 */
class RequirementFile_odt : public RequirementFileAbstract
{

public:

	/*!
	 * \brief Constructor of the class, does nothing special
	 * \param[in] p_cnfFile Map of the attributes configured for this file (ID, path of the file ...)
	 */
	RequirementFile_odt(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile);

	/*!
	 * \brief Destructor of the class, does nothing
	 */
	virtual ~RequirementFile_odt();

	/*!
	 * \brief Parse the odt file and call the methods of ModelSngReqMatrix to store the requirements found
	 */
	void parseFile();

private:

	/*!
	 * \brief unzip an odt file to read its content
	 *
	 * An odt file is basically a zip and only one XML file in this zip is interesting for requirement
	 * management. This method extracts the file defined by its path and read the "content.xml"
	 * \param[out] p_textData contains the content of "content.xml"
	 */
	void __readTextDataFromZippedFormat(QString& p_textData);

	/*!
	 * \brief look for the node «text» under which the whole document is contained
	 *
	 * In the odt format, the whole document is in a tag document-content which contains a tag body which contains
	 * a tag text which contains paragrahs and text and so on. The objective of this method is to retrieve this
	 * «text» tag or log errors.
	 * \param[in] p_firstNode  First node of the document. It is document-content even if we don't name it.
	 * \param[out] p_textNode  The QDomElement corresponding to the tag «text» below p_firstNode
	 */
	void __lookForFirstTextNode(const QDomElement& p_firstNode, QDomElement& p_textNode) ;
};

#endif /* REQUIREMENTFILEODT_H_ */
