/*
 * ModelConfigurationErrors.h
 *
 *  Created on: 1 août 2016
 *      Author: f.souliers
 */

#ifndef MODELCONFIGURATIONERRORS_H_
#define MODELCONFIGURATIONERRORS_H_

#include <QAbstractItemModel>

class ModelConfigurationErrors: public QAbstractItemModel
{
public:
	ModelConfigurationErrors(QObject *parent = Q_NULLPTR);
	virtual ~ModelConfigurationErrors();

	/*!
	 * Severity of an error during configuration reading
	 */
	typedef enum e_severity {WARNING, ERROR, CRITICAL} severity_t ;

	/*!
	 * Category of an error during configuration reading
	 */
	typedef enum e_category {PARSING, FILESYSTEM, CONTENT} category_t ;

	/*!
	 * Data structure of an error
	 */
	typedef struct s_error
	{
		severity_t severity;
		category_t category;
		QString description;
	} error_t;

	int columnCount(const QModelIndex &parent = QModelIndex ()) const;
	int rowCount(const QModelIndex &parent = QModelIndex ()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex ()) const;

	QModelIndex parent(const QModelIndex &index) const;

	void clear() {__errors.clear() ;}

	void refresh() ;

	void addError(const error_t& e) {__errors.append(e) ; }
	void addErrors(const QVector<error_t>& ve){__errors.append(ve) ; }

	/*!
	 * - true if at least one error with severity ERROR or CRITICAL. It is used to
	 * prevent user from launching analysis if there are some configuration errors
	 */
	bool hasSignificantError() ;

private :
	/*!
	 * Vector of errors found in the configuration file
	 */
	QVector<error_t> __errors ;

	QString __severityToString(const severity_t& s) const;
	QString __categoryToString(const category_t& c) const;
};

typedef ModelConfigurationErrors* ModelConfigurationErrorsPtr ;

typedef ModelConfigurationErrors& ModelConfigurationErrorsRef ;

#endif /* MODELCONFIGURATIONERRORS_H_ */
