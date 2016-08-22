/*!
 * \file ModelConfigurationErrors.cpp
 * \brief Implementation of the class ModelConfigurationErrors
 * \date 2016-08-01
 * \author f.souliers
 */

#include <QDebug>

#include "ModelConfigurationErrors.h"

static const int CNFERR_COL_SEVERITY = 0;  //!< Column for the severity of the error
static const int CNFERR_COL_CATEGORY = 1;  //!< Column for the category of the error
static const int CNFERR_COL_DESCRIPTION = 2;  //!< Column for the description of the error

/*!
 * \brief Text value for column headers.
 * \warning The index of each item must be consistent with the value of the associated column
 */
static const QVector<QString> COL_HEADERS { QObject::trUtf8("Sévérité"),
                                            QObject::trUtf8("Catégorie"),
                                            QObject::trUtf8("Description") };


ModelConfigurationErrors::ModelConfigurationErrors(QObject *parent)
		: QAbstractItemModel(parent)
{
}

ModelConfigurationErrors::~ModelConfigurationErrors()
{
}

int ModelConfigurationErrors::columnCount(const QModelIndex & /*parent*/) const
{
	return (COL_HEADERS.count());
}

int ModelConfigurationErrors::rowCount(const QModelIndex & /*parent*/) const
{
	return (__errors.count());
}

QVariant ModelConfigurationErrors::data(const QModelIndex &index, int role) const
{
	QVariant retVal;

	// The framework is requiring the data to display
	if (role == Qt::DisplayRole)
	{
		error_t e = __errors.at(index.row());

		switch (index.column())
		{
			case CNFERR_COL_SEVERITY:
				retVal = __severityToString(e.severity);
				break;

			case CNFERR_COL_CATEGORY:
				retVal = __categoryToString(e.category);
				break;

			case CNFERR_COL_DESCRIPTION:
				retVal = e.description;
				break;

			default:
				// invalid value
				qDebug() << "ModelConfigurationErrors::data --> INVALID VALUE";
		}
	}

	return (retVal);
}

QVariant ModelConfigurationErrors::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			return (COL_HEADERS[section]);
		}
		else if (orientation == Qt::Vertical)
		{
			return (section + 1);
		}
	}

	return (QAbstractItemModel::headerData(section, orientation, role));
}

QModelIndex ModelConfigurationErrors::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return (createIndex(row, column));
}

QModelIndex ModelConfigurationErrors::parent(const QModelIndex & /*index*/) const
{
	return (QModelIndex());
}

void ModelConfigurationErrors::refresh()
{
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();
}

bool ModelConfigurationErrors::hasSignificantError()
{
	foreach(error_t e, __errors)
	{
		if (e.severity == ERROR || e.severity == CRITICAL) return(true);
	}

	return(false);
}

bool ModelConfigurationErrors::hasWarning()
{
	foreach(error_t e, __errors)
	{
		if (e.severity == WARNING) return(true);
	}

	return(false);
}

void ModelConfigurationErrors::appendErrorString(QString& str)
{
	foreach(error_t e, __errors)
	{
		if (e.severity == ERROR || e.severity == CRITICAL)
		{
			str += "\n" + QObject::trUtf8("Sévérité<%1> Catégorie<%2> Description<%3>").arg(__severityToString(e.severity))
			                                                                           .arg(__categoryToString(e.category))
			                                                                           .arg(e.description) ;
		}
	}
}

void ModelConfigurationErrors::appendWarningString(QString& str)
{
	foreach(error_t e, __errors)
	{
		if (e.severity == WARNING)
		{
			str += "\n" + QObject::trUtf8("Sévérité<%1> Catégorie<%2> Description<%3>").arg(__severityToString(e.severity))
			                                                                           .arg(__categoryToString(e.category))
			                                                                           .arg(e.description) ;
		}
	}
}

QString ModelConfigurationErrors::__severityToString(const severity_t& s) const
{
	switch (s)
	{
		case WARNING:
			return (QObject::trUtf8("Alerte"));
		case ERROR:
			return (QObject::trUtf8("Erreur"));
		case CRITICAL:
			return (QObject::trUtf8("Critique"));
	}

	return (QObject::trUtf8("INVALIDE"));
}

QString ModelConfigurationErrors::__categoryToString(const category_t& c) const
{
	switch (c)
	{
		case PARSING:
			return (QObject::trUtf8("Syntaxe"));
		case FILESYSTEM:
			return (QObject::trUtf8("Système de fichier"));
		case CONTENT:
			return (QObject::trUtf8("Contenu"));
	}

	return (QObject::trUtf8("INVALIDE"));
}
