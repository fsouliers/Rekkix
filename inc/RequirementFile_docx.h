/*!
 * \file RequirementFile_docx.h
 * \brief Definition of the class RequirementFile_docx
 * \date 2016-07-21
 * \author f.souliers
 */

#ifndef REQUIREMENTFILEDOCX_H_
#define REQUIREMENTFILEDOCX_H_

#include "RequirementFileAbstract.h"

/*!
 * \class RequirementFile_docx
 * \brief Implements the RequirementFileAbstract interface for MS Word docx format
 */
class RequirementFile_docx : public RequirementFileAbstract
{
public:

	/*!
	 * \brief Constructor of the class, does nothing special
	 * \param[in] p_cnfFile Map of the attributes configured for this file (ID, path of the file ...)
	 */
	RequirementFile_docx(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile);

	/*!
	 * \brief Destructor of the class, does nothing
	 */
	virtual ~RequirementFile_docx();

	/*!
	 * \brief Parse the docx file and call the methods of ModelSngReqMatrix to store the requirements found
	 */
	void parseFile();

private:

	/*!
	 * \brief unzip a docx file to read its content
	 *
	 * A docx file is basically a zip and only one XML file in this zip is interesting for requirement
	 * management. This method extracts the file defined by its path and read the "word/document.xml"
	 * \param[out] p_textData contains the content of "word/document.xml"
	 */
	void __readTextDataFromZippedFormat(QString* p_textData);
};

#endif /* REQUIREMENTFILEDOCX_H_ */
