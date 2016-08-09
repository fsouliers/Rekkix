/*
 * ModelCompositeReqs.cpp
 *
 *  Created on: 31 juil. 2016
 *      Author: f.souliers
 */

#include <QDebug>

#include "ModelCompositeReqs.h"

static const int REQSCOMP_COL_ID = 0 ;
static const int REQSCOMP_COL_FILE_ID = 1 ;
static const int REQSCOMP_COL_COMPOSING_REQ_ID = 2 ;
static const int REQSCOMP_COL_COMPOSING_REQ_FILE_ID = 3 ;

static const QVector<QString> COL_HEADERS { QObject::trUtf8 ("Exigence"),
                                            QObject::trUtf8 ("Définie dans"),
                                            QObject::trUtf8 ("Composée de"),
                                            QObject::trUtf8 ("Définie dans") };

ModelCompositeReqs::ModelCompositeReqs(QObject *parent)
: QAbstractItemModel(parent)
{

}

ModelCompositeReqs::~ModelCompositeReqs()
{
}

int
ModelCompositeReqs::columnCount(const QModelIndex & /*parent*/) const
{
	return(COL_HEADERS.count()) ;
}

int
ModelCompositeReqs::rowCount(const QModelIndex & /*parent*/) const
{
	return(__requirements.count()) ;
}

QVariant
ModelCompositeReqs::data(const QModelIndex &index, int role) const
{
	QVariant retVal ;

	// The framework is requiring the data to display
	if (role == Qt::DisplayRole)
	{
		Requirement* cmpreq = __requirements.values().at(index.row()) ;


		switch (index.column())
		{
			case REQSCOMP_COL_ID :
				retVal = cmpreq->getParent()->getId() ;
				break;

			case REQSCOMP_COL_FILE_ID :
				retVal = cmpreq->getParent()->getLocationId() ;
				break;

			case REQSCOMP_COL_COMPOSING_REQ_ID :
				retVal = cmpreq->getId() ;
				break;

			case REQSCOMP_COL_COMPOSING_REQ_FILE_ID :
				retVal = cmpreq->getLocationId() ;
				break;

			default :
				// invalid value
				qDebug() << "ModelCompositeReqs::data --> INVALID VALUE" ;
		}

	}

	return(retVal) ;
}

QVariant
ModelCompositeReqs::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			return(COL_HEADERS[section]);
		}
		else if (orientation == Qt::Vertical)
		{
			return(section+1);
		}
	}

	return(QAbstractItemModel::headerData(section, orientation, role)) ;
}

QModelIndex
ModelCompositeReqs::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return(createIndex(row, column)) ;
}


QModelIndex
ModelCompositeReqs::parent(const QModelIndex & /*index*/) const
{
	return(QModelIndex()) ;
}


void
ModelCompositeReqs::addRequirementsOfAFile(const QVector<Requirement*>& p_reqs)
{
	// Only requirement with composites are kept
	foreach(Requirement* r, p_reqs)
	{
		QVector<Requirement*> cmpreqs = r->getComposingRequirements() ;
		if (cmpreqs.count() > 0)
		{
			foreach(Requirement* cmpreq, cmpreqs)
			{
				__requirements.insertMulti(r->getId(), cmpreq) ;
			}
		}
	}
}

void
ModelCompositeReqs::refresh()
{
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();
}

