/*!
 * \file FactoryRequirementFile.h
 * \brief Definition of the class FactoryRequirementFile
 * \date 2016-07-21
 * \author f.souliers
 */

#ifndef FACTORYREQUIREMENTFILE_H_
#define FACTORYREQUIREMENTFILE_H_

#include "RequirementFileAbstract.h"

/*!
 * \class FactoryRequirementFile
 * \brief Simple and small factory used to instantiate the adequate implementation of RequirementFileAbstract
 *
 * Once Rekkix has parsed the configuration file, it is able to instantiate the adequate file parsers
 * for each configured file according to its file extension.
 *
 * Available parser are :
 * - docx : parse the file as a MS Word file
 * - odt : parse the file as a LibreOffice text file
 * - txt : parse the file as a plain text file
 * - pdf : parse the file as a pdf file
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
	 *
	 * If the parser isn't specified in the configuration, then the factory will try to detect the parser to
	 * use from the filename given.
	 * \warning The factory does not delete this new object at any time: the caller shall be responsible
	 *          for that.
	 * \param[in] p_cnfFile  list of the attributes and their value for the file. In particular,
	 *                       ModelConfiguration::REQFILE_ATTR_PATH and
	 *                       ModelConfiguration::REQFILE_ATTR_PARSER shall be used to detect which
	 *                       RequirementFile_xxx must be used.
	 * \return
	 * A pointer to the freshly built requirement file object or null if no parser seems acceptable
	 */
	static IRequirementFilePtr getRequirementFile(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile);
};

#endif /* FACTORYREQUIREMENTFILE_H_ */
