/*!
 * \file RequirementFile_txt.h
 * \brief Definition of the class RequirementFile_txt
 * \date 2016-08-15
 * \author f.souliers
 */

#ifndef REQUIREMENTFILETXT_H_
#define REQUIREMENTFILETXT_H_

#include "IRequirementFile.h"

/*!
 * \class RequirementFile_txt
 * \brief Implements the IRequirementFile interface for plain text files
 */
class RequirementFile_txt : public IRequirementFile
{
public:

	/*!
	 * \brief Constructor of the class, does nothing special
	 * \param[in] p_cnfFile Map of the XML attributes configured for this file (ID, path of the file ...)
	 */
	RequirementFile_txt(const ModelConfiguration::XmlConfiguredFileAttributesMap_t& p_cnfFile);

	/*!
	 * \brief Destructor of the class, does nothing
	 */
	virtual ~RequirementFile_txt();

	/*!
	 * \brief Parse the docx file and call the methods of ModelSngReqMatrix to store the requirements found
	 */
	void parseFile();


};

#endif /* REQUIREMENTFILETXT_H_ */
