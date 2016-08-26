/*!
 * \file RequirementFile_docx.h
 * \brief Definition of the class RequirementFile_docx
 * \date 2016-07-21
 * \author f.souliers
 */

#ifndef REQUIREMENTFILEDOCX_H_
#define REQUIREMENTFILEDOCX_H_

#include "RequirementFileAbstractZipped.h"

/*!
 * \class RequirementFile_docx
 * \brief Implements the RequirementFileAbstract interface for MS Word docx format
 *
 * the unzipping phase of the file reading is taken into account by RequirementFileAbstractZipped.
 */
class RequirementFile_docx : public RequirementFileAbstractZipped
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

};

#endif /* REQUIREMENTFILEDOCX_H_ */
