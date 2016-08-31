/*!
 * \file RequirementFileAbstractZipped.h
 * \brief Definition of the abstract class RequirementFileAbstractZipped
 * \date 2016-08-26
 * \author f.souliers
 */

#ifndef REQUIREMENTFILEABSTRACTZIPPED_H_
#define REQUIREMENTFILEABSTRACTZIPPED_H_

#include <QDomElement>

#include "RequirementFileAbstract.h"

/*!
 * \class RequirementFileAbstractZipped
 * \brief Implements unzipping actions required for zipped format such as odt and docx
 */
class RequirementFileAbstractZipped : public RequirementFileAbstract
{
	Q_OBJECT

public:

	/*!
	 * \brief Constructor of the class, does nothing special
	 * \param[in] p_cnfFile Map of the attributes configured for this file (ID, path of the file ...)
	 */
	RequirementFileAbstractZipped(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile);

	/*!
	 * \brief Destructor of the class, does nothing
	 */
	virtual ~RequirementFileAbstractZipped();


protected:

	/*!
	 * \brief unzip an odt or docx file to read its content
	 *
	 * An odt or docx file is basically a zip and only one XML file in this zip is interesting for requirement
	 * management. This method extracts the file defined by its path and read the _documentFileName
	 * \param[out] p_textData contains the content of _documentFileName
	 */
	void _readTextDataFromZippedFormat(QString* p_textData);

	/*!
	 * \brief Path of the file located into the zipped that must be parsed
	 */
	QString _documentFileName ;

	/*!
	 * \brief buffer used to read the data in a thread-safe manner
	 */
	char* _zippedDataBuffer ;
};

#endif /* REQUIREMENTFILEABSTRACTZIPPED_H_ */
