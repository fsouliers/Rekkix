/*
 * FactoryRequirementFile.h
 *
 *  Created on: 21 juil. 2016
 *      Author: f.souliers
 */

#ifndef FACTORYREQUIREMENTFILE_H_
#define FACTORYREQUIREMENTFILE_H_

#include "IRequirementFile.h"

class FactoryRequirementFile
{
public:
	FactoryRequirementFile();
	virtual ~FactoryRequirementFile();

	static IRequirementFilePtr getRequirementFile(ModelConfiguration::XmlAttributesMap_t& p_cnfFile) ;
};

#endif /* FACTORYREQUIREMENTFILE_H_ */
