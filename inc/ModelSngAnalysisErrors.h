/*
 * ModelSngAnalysisErrors.h
 *
 *  Created on: 1 août 2016
 *      Author: f.souliers
 */

#ifndef MODELSNGANALYSISERRORS_H_
#define MODELSNGANALYSISERRORS_H_

#include <QAbstractItemModel>

#include "AnalysisError.h"

class ModelSngAnalysisErrors : public QAbstractItemModel
{
	Q_OBJECT

	public:
		static ModelSngAnalysisErrors& instance()
		{
			static ModelSngAnalysisErrors r ;
			return(r) ;
		}


		int columnCount(const QModelIndex &parent = QModelIndex()) const ;
		int rowCount(const QModelIndex &parent = QModelIndex()) const ;

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const ;

		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const ;

		QModelIndex parent(const QModelIndex &index) const ;

		void addError(const AnalysisError& e) ;
		void addErrors(const QVector<AnalysisError>& ve) ;

		int getNberrorsInAFile(const QString& p_fileId) ;

		void clear() ;

		void refresh() ;

		const QString getReportErrorSummaryTable(const QString& p_writer, const QString& p_delimiter) const ;

	private:
		ModelSngAnalysisErrors(QObject *parent = Q_NULLPTR);
		virtual ~ModelSngAnalysisErrors();

		QVector<AnalysisError> __errors ;

		QMap<QString, int> __nbErrorsByFileId ;
};

#endif /* MODELSNGANALYSISERRORS_H_ */
