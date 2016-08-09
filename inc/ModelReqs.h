/*
 * ModelReqs.h
 *
 *  Created on: 1 août 2016
 *      Author: f.souliers
 */

#ifndef MODELREQS_H_
#define MODELREQS_H_

#include <QAbstractItemModel>

#include "IRequirementFile.h"

class ModelReqs: public QAbstractItemModel
{
public:
	ModelReqs(QObject *parent = Q_NULLPTR);
	virtual ~ModelReqs();

	int columnCount(const QModelIndex &parent = QModelIndex()) const ;
	int rowCount(const QModelIndex &parent = QModelIndex()) const ;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const ;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const ;

	QModelIndex parent(const QModelIndex &index) const ;


	/*!
	 * add the requirements of a file to the model (the model will only take the covered requirements
	 */
	void addRequirementsOfAFile(const QVector<Requirement*>& p_reqs) ;

	/*!
	 * empty __requirements
	 */
	void clear() {__requirements.clear() ;}

	/*!
	 * Ask for a refresh of the model so the UI is updated
	 */
	void refresh() ;

private :
	QVector<Requirement*> __requirements ;

};

#endif /* MODELREQS_H_ */
