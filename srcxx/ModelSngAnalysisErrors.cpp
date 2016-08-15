/*!
 * \file ModelSngAnalysisErrors.cpp
 * \brief Implementation of the class ModelSngAnalysisErrors
 * \date 2016-08-01
 * \author f.souliers
 */

#include <QDebug>
#include <QFile>

#include "ModelSngAnalysisErrors.h"
#include "FactoryReportBaseString.h"

static const int AERROR_COL_SEVERITY = 0;
static const int AERROR_COL_CATEGORY = 1;
static const int AERROR_COL_LOCATION = 2;
static const int AERROR_COL_DESCRIPTION = 3;

/*!
 * \brief Text value for column headers.
 * \warning The index of each item must be consistent with the value of the associated column
 */
static const QVector<QString> COL_HEADERS { QObject::trUtf8("Sévérité"),
                                            QObject::trUtf8("Catégorie"),
                                            QObject::trUtf8("Localisation"),
                                            QObject::trUtf8("Description") };

ModelSngAnalysisErrors::ModelSngAnalysisErrors(QObject *parent)
		: QAbstractItemModel(parent)
{
}

ModelSngAnalysisErrors::~ModelSngAnalysisErrors()
{
}

int ModelSngAnalysisErrors::columnCount(const QModelIndex & /*parent*/) const
{
	return (COL_HEADERS.count());
}

int ModelSngAnalysisErrors::rowCount(const QModelIndex & /*parent*/) const
{
	return (__errors.count());
}

QVariant ModelSngAnalysisErrors::data(const QModelIndex &index, int role) const
{
	QVariant retVal;

	if (role == Qt::DisplayRole)
	{
		AnalysisError e = __errors.at(index.row());

		switch (index.column())
		{
			case AERROR_COL_SEVERITY:
				retVal = e.getSeverityString();
				break;

			case AERROR_COL_CATEGORY:
				retVal = e.getCategoryString();
				break;

			case AERROR_COL_LOCATION:
				retVal = e.getLocation();
				break;

			case AERROR_COL_DESCRIPTION:
				retVal = e.getDescription();
				break;

			default:
				// invalid value
				qDebug() << "ModelSngAnalysisErrors::data --> INVALID VALUE";
		}
	}

	return (retVal);
}

QVariant ModelSngAnalysisErrors::headerData(int section, Qt::Orientation orientation, int role) const
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

QModelIndex ModelSngAnalysisErrors::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return (createIndex(row, column));
}

QModelIndex ModelSngAnalysisErrors::parent(const QModelIndex & /*index*/) const
{
	return (QModelIndex());
}

void ModelSngAnalysisErrors::addError(const AnalysisError& e)
{
	__errors.append(e);

	QString loc = e.getLocation();
	if (__nbErrorsByFileId.contains(loc))
	{
		__nbErrorsByFileId[loc] += 1;
	}
	else
	{
		__nbErrorsByFileId[loc] = 1;
	}
}

void ModelSngAnalysisErrors::addErrors(const QVector<AnalysisError>& ve)
{
	__errors.append(ve);

	foreach(AnalysisError e, ve)
	{
		QString loc = e.getLocation();
		if ( __nbErrorsByFileId.contains(loc) )
		{
			__nbErrorsByFileId[loc] += 1;
		}
		else
		{
			__nbErrorsByFileId[loc] = 1;
		}
	}
}

void ModelSngAnalysisErrors::clear()
{
	__errors.clear();
	__nbErrorsByFileId.clear();
}

int ModelSngAnalysisErrors::getNberrorsInAFile(const QString& p_fileId)
{
	if (!__nbErrorsByFileId.contains(p_fileId))
	{
		return (0);
	}
	else
	{
		return (__nbErrorsByFileId[p_fileId]);
	}
}

void ModelSngAnalysisErrors::refresh()
{
	// Data has been updated , the view can be freshened
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();
}

const QString ModelSngAnalysisErrors::getReportErrorSummaryTable(const QString& p_writer,
                                                                 const QString& p_delimiter) const
{
	FactoryReportBaseString fact;
	QString s_base(fact.getBaseString("errors_line", p_writer, p_delimiter));

	QString result = "";

	foreach(AnalysisError a, __errors)
	{
		QString newline = s_base;
		newline.replace("REKKIX_ERRORS_SEVERITY", a.getSeverityString());

		if (a.getSeverity() == AnalysisError::ERROR)
		{
			newline.replace("REKKIX_ERRORS_ROW_COLOR", " class=\"invalid\" ");
		}
		else
		{
			newline.replace("REKKIX_ERRORS_ROW_COLOR", "");
		}

		newline.replace("REKKIX_ERRORS_CATEGORY", a.getCategoryString());
		newline.replace("REKKIX_ERRORS_LOCATION_ID", a.getLocation());
		newline.replace("REKKIX_ERRORS_DESCRIPTION", a.getDescription());

		result += newline + "\n";
	}

	return (result);
}

