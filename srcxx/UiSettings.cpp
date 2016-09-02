/*!
 * \file UiSettings.cpp
 * \brief Implementation of the class UiSettings
 * \date 2016-09-02
 * \author f.souliers
 */

#include <QMessageBox>
#include "UiSettings.h"

#include "SngSettings.h"

// Only for Eclipse colorisation --> APP_VERSION is defined in rekkix.pro
#ifndef APP_VERSION
#define APP_VERSION "UNEXPECTED APP VERSION"
#endif

UiSettings::UiSettings(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	// Build GUI
	Ui::SettingsDlg::setupUi(this);

	QString version_str = this->lbl_version->text() ;
	version_str.replace("APP_VERSION", APP_VERSION) ;
	this->lbl_version->setText(version_str) ;

	this->sb_maxParsingThreads->setValue(SngSettings::instance().getNbParsingThreads()) ;
}

UiSettings::~UiSettings()
{

}

void UiSettings::slt_OK()
{
	SngSettings::instance().setNbParsingThreads(sb_maxParsingThreads->value()) ;
	this->accept() ;
}

