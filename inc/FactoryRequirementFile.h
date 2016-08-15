/*!
 * \file FactoryRequirementFile.h
 * \brief Definition of the class FactoryRequirementFile
 * \date 2016-07-21
 * \author f.souliers
 */

#ifndef FACTORYREQUIREMENTFILE_H_
#define FACTORYREQUIREMENTFILE_H_

#include "IRequirementFile.h"

/*!
 * \class FactoryRequirementFile
 * \brief Simple and small factory used to instantiate the adequate implementation of IRequirementFile
 *
 * Once Rekkix has parsed the configuration file, it is able to instantiate the adequate file parsers
 * for each configured file according to its file extension.
 *
 * TODO Using QSetting, Rekkix should know which extension must trigger the use of which parser
 *      (for example, *.txt or *.tex or *.cpp files will use the same "plain text" parser)
 */
class FactoryRequirementFile
{
public:
	/*!
	 * \brief Constructor of the class, does nothing special
	 */
	FactoryRequirementFile();

	/*!
	 * \brief Destructor of the class, does nothing special
	 */
	virtual ~FactoryRequirementFile();

	/*!
	 * \brief Build a new RequirementFile and returns a pointer on it
	 * \warning The factory does not delete this new object at any time: the caller shall be responsible
	 *          for that.
	 * \param[in] p_cnfFile  list of the XML attributes and their value for the file. In particular,
	 *                       ModelConfiguration::REQFILE_ATTR_PATH shall be used to detect which
	 *                       RequirementFile_xxx must be used.
	 * \return
	 * A pointer to the freshly built requirement file object.
	 */
	static IRequirementFilePtr getRequirementFile(ModelConfiguration::XmlConfiguredFileAttributesMap_t& p_cnfFile);
};

#endif /* FACTORYREQUIREMENTFILE_H_ */
