/*
 * RequirementFile_docx.h
 *
 *  Created on: 21 juil. 2016
 *      Author: f.souliers
 */

#ifndef REQUIREMENTFILEDOCX_H_
#define REQUIREMENTFILEDOCX_H_

#include "IRequirementFile.h"

class RequirementFile_docx : public IRequirementFile
{
public:
	RequirementFile_docx(ModelConfiguration::XmlAttributesMap_t& p_cnfFile);
	virtual ~RequirementFile_docx();

	void parseFile() ;

private:
	void __readTextDataFromZippedFormat(QString* p_textData);
};

#endif /* REQUIREMENTFILEDOCX_H_ */
