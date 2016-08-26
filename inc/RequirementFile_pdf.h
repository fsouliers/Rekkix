/*!
 * \file RequirementFile_pdf.h
 * \brief Definition of the class RequirementFile_pdf
 * \date 2016-08-23
 * \author f.souliers
 */

#ifndef REQUIREMENTFILEPDF_H_
#define REQUIREMENTFILEPDF_H_

#include "RequirementFileAbstract.h"

/*!
 * \class RequirementFile_pdf
 * \brief Implements the RequirementFileAbstract interface for pdf files
 */
class RequirementFile_pdf : public RequirementFileAbstract
{
public:

	/*!
	 * \brief Constructor of the class, does nothing special
	 * \param[in] p_cnfFile Map of the attributes configured for this file (ID, path of the file ...)
	 */
	RequirementFile_pdf(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile);

	/*!
	 * \brief Destructor of the class, does nothing
	 */
	virtual ~RequirementFile_pdf();

	/*!
	 * \brief Parse the pdf file and call the methods of ModelSngReqMatrix to store the requirements found
	 */
	void parseFile();


};

#endif /* REQUIREMENTFILEPDF_H_ */
