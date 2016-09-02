/*!
 * \file UiSettings.h
 * \brief Definition of the class UiSettings
 * \date 2016-09-02
 * \author f.souliers
 */

#ifndef UISETTINGS_H_
#define UISETTINGS_H_

#include <QDialog>

// Auto-generated ui headers
#include "ui_Settings.h"

class UiSettings : public QDialog, public Ui::SettingsDlg
{
	Q_OBJECT

public:
	UiSettings(QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);
	~UiSettings();

public slots:
	void slt_OK() ;
};

#endif /* UISETTINGS_H_ */
