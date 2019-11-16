// feature_selection_dialog.cpp

// Copyright (C) 2018 Sanford Rockowitz <rockowitz@minsoft.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <assert.h>
#include <stdio.h>
#include <iostream>

#include <QtWidgets/QRadioButton>

#include "base/ddcui_globals.h"
#include "help/help_dialog.h"

// generated by uic from feature_selection_dialog.ui:
#include "ui_feature_selection_dialog.h"

#include "feature_selection_dialog.h"


using namespace std;


void FeatureSelectionDialog::useSelectorData(FeatureSelector * fsel)
{
    if (debugFeatureSelection) {
        TRACE("_feature_selector:");
        fsel->dbgrpt();
    }

    QRadioButton * curButton;
    // switch is exhaustive, so curButton is always assigned, however -Wmaybe-uninitialized
    // flags the curButton->setChecked() line  Assign a dummy value to quiet the warning
    curButton = _ui->known_radioButton;   // dummy value
    DDCA_Feature_Subset_Id local_fsid = fsel->_featureListId;
    switch(local_fsid) {
    case DDCA_SUBSET_KNOWN:
        curButton = _ui->known_radioButton;
        break;
    case DDCA_SUBSET_COLOR:
        curButton = _ui->color_radioButton;
        break;
    case DDCA_SUBSET_PROFILE:
        curButton = _ui->profile_radioButton;
        break;
    case DDCA_SUBSET_MFG:
        curButton = _ui->mfg_radioButton;
        break;
    case DDCA_SUBSET_CAPABILITIES:
        curButton = _ui->capabilities_radioButton;
        break;
    case DDCA_SUBSET_SCAN:
        curButton = _ui->scan_radioButton;
        break;
    case DDCA_SUBSET_UNSET:
        assert(false);
        break;
    }
    curButton->setChecked(true);
    // to do: enable/disable other buttons as appropriate

    _ui->includeTable_checkbox->setChecked(    fsel->_includeTableFeatures);
    _ui->showUnsupported_checkbox->setChecked( fsel->_showUnsupportedFeatures);
    _ui->onlyCapabilities_checkbox->setChecked(fsel->_includeOnlyCapabilities);
    _ui->allCapabilities_checkbox->setChecked( fsel->_includeAllCapabilities);
}


void FeatureSelectionDialog::useSelectorData() {
   useSelectorData(_featureSelector);
}


FeatureSelectionDialog::FeatureSelectionDialog(
      QWidget *         parent,
      FeatureSelector * featureSelector
     )
   :  QDialog(parent),
      _cls(strdup(metaObject()->className())),
      _ui(new Ui::FeatureSelectionDialog),
      _featureSelector(featureSelector)
{
    // TRACE("_ui=%p, _featureSelector = %p\n", _ui, _featureSelector);
    _ui->setupUi(this);

    QObject::connect(_ui->known_radioButton, &QAbstractButton::clicked,
                      this, &FeatureSelectionDialog::on_known_radioButton_clicked);
    QObject::connect(_ui->capabilities_radioButton, &QAbstractButton::clicked,
                      this, &FeatureSelectionDialog::on_capabilities_radioButton_clicked);
    QObject::connect(_ui->mfg_radioButton, &QAbstractButton::clicked,
                      this, &FeatureSelectionDialog::on_mfg_radioButton_clicked);
    QObject::connect(_ui->profile_radioButton, &QAbstractButton::clicked,
                      this, &FeatureSelectionDialog::on_profile_radioButton_clicked);
    QObject::connect(_ui->color_radioButton, &QAbstractButton::clicked,
                      this, &FeatureSelectionDialog::on_color_radioButton_clicked);
    QObject::connect(_ui->scan_radioButton, &QAbstractButton::clicked,
                        this, &FeatureSelectionDialog::on_scan_radioButton_clicked);

    useSelectorData(_featureSelector);
}


FeatureSelectionDialog::~FeatureSelectionDialog()
{
    delete _ui;
}


#ifdef UNUSED
void FeatureSelectionDialog::setFeatureSet(int fsid) {

    cout << "(setFeatureSet) fsid=" << fsid << endl;
}

void FeatureSelectionDialog::on_known_radioButton_clicked(bool checked)
{
   cout << "(FeatureSelectionDialog::on_known_radioButton_clicked) checked=" << checked << endl;
}

void FeatureSelectionDialog::on_known_radioButton_clicked()
{
  setFeatureSet(DDCA_SUBSET_KNOWN);
}

void FeatureSelectionDialog::on_scan_radioButton_clicked()
{
  // setFeatureSet(DDCA_FEATURE_LIST_SCAN);
}

void FeatureSelectionDialog::on_mfg_RadioButton_clicked()
{
  setFeatureSet(DDCA_SUBSET_MFG);
}

void FeatureSelectionDialog::on_profile_RadioButton_clicked()
{
  setFeatureSet(DDCA_SUBSET_PROFILE);
}

void FeatureSelectionDialog::on_color_radioButton_clicked()
{
  setFeatureSet(DDCA_SUBSET_COLOR);
}
#endif


void FeatureSelectionDialog::on_known_radioButton_clicked(bool checked) {
   cout << "(on_known_radioButton_clicked) arg1 = " << checked << endl;
   _ui->allCapabilities_checkbox->setEnabled(true);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
}

void FeatureSelectionDialog::on_scan_radioButton_clicked(bool checked) {
   cout << "(on_scan_radioButton_clicked) arg1 = " << checked << endl;
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(false);
   _ui->allCapabilities_checkbox->setChecked(false);
   _ui->onlyCapabilities_checkbox->setChecked(false);
}


void FeatureSelectionDialog::on_capabilities_radioButton_clicked(bool checked) {
   cout << "(on_capabilities_radioButton_clicked) arg1 = " << checked << endl;
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(false);
   _ui->allCapabilities_checkbox->setChecked(false);
   _ui->onlyCapabilities_checkbox->setChecked(false);
}


void FeatureSelectionDialog::on_mfg_radioButton_clicked(bool checked) {
   cout << "(mfg_radioButton_clicked) arg1 = " << checked << endl;
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
   _ui->allCapabilities_checkbox->setChecked(false);
}

void FeatureSelectionDialog::on_profile_radioButton_clicked(bool checked) {
   cout << "(on_profile_radioButton_clicked) arg1 = " << checked << endl;
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
   _ui->allCapabilities_checkbox->setChecked(false);
}

void FeatureSelectionDialog::on_color_radioButton_clicked(bool checked) {
   cout << "(on_color_radioButton_clicked) arg1 = " << checked << endl;
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
   _ui->allCapabilities_checkbox->setChecked(false);
}




void FeatureSelectionDialog::on_onlyCapabilities_checkbox_stateChanged(int arg1)
{
   cout << "(on_onlyCapabilities_checkBox_stateChanged) arg1 = " << arg1 << endl;
   if (_ui->onlyCapabilities_checkbox->isChecked() ) {     // or != 0
      _ui->allCapabilities_checkbox->setCheckState(Qt::Unchecked);
   }
}

void FeatureSelectionDialog::on_allCapabilities_checkbox_stateChanged(int arg1)
{
   cout << "(on_allCapabilities_checkBox_stateChanged) arg1 = " << arg1 << endl;
   if (arg1 == 2) {
      _ui->onlyCapabilities_checkbox->setCheckState(Qt::Unchecked);
   }
}



void FeatureSelectionDialog::on_showUnsupported_checkbox_stateChanged(int arg1)
{
   cout << "(on_showUnsupported_checkBox_stateChanged) arg1 = " << arg1 << endl;
   // if (_ui->showUnsupported_checkbox->isChecked() ) {     // or != 0
   //    _ui->allCapabilities_checkbox->setCheckState(Qt::Unchecked);
   // }
}



#ifdef UNUSED
void FeatureSelectionDialog::on_showUsupported_checkbox_stateChanged(int arg1)
{
   cout << "(on_show_unpported_checkBox_stateChanged) arg1 = " << arg1 << endl;
}
void FeatureSelectionDialog::on_includeTable_checkbox_stateChanged(int arg1)
{
   cout << "(on_includeTable_checkbox_StateChanged) arg1 = " << arg1 << endl;
}
#endif


void FeatureSelectionDialog::on_buttonBox_accepted()
{
    bool debugFunc = debugFeatureSelection;
    debugFunc = true;
    TRACEF(debugFunc, "Executing");
    // which button is currently clicked?

    DDCA_Feature_Subset_Id feature_list;
    if (_ui->color_radioButton->isChecked())
        feature_list = DDCA_SUBSET_COLOR;
    else if (_ui->known_radioButton->isChecked())
        feature_list = DDCA_SUBSET_KNOWN;
    else if (_ui->mfg_radioButton->isChecked())
        feature_list = DDCA_SUBSET_MFG;
    else if (_ui->profile_radioButton->isChecked())
        feature_list = DDCA_SUBSET_PROFILE;
    else if (_ui->capabilities_radioButton->isChecked())
        feature_list = DDCA_SUBSET_CAPABILITIES;
    else if (_ui->scan_radioButton->isChecked())
        feature_list = DDCA_SUBSET_SCAN;
    else
        feature_list = DDCA_SUBSET_KNOWN;    // should never occur

    TRACEF(debugFunc, "Checking for any changes...");
    bool changed = false;
    if (feature_list != _featureSelector->_featureListId) {
       _featureSelector->_featureListId = feature_list;
       changed = true;
    }
    if (_ui->includeTable_checkbox->isChecked() !=  _featureSelector->_includeTableFeatures)
    {
       _featureSelector->_includeTableFeatures = _ui->includeTable_checkbox->isChecked();
       changed = true;
    }
    if (_featureSelector->_showUnsupportedFeatures !=_ui->showUnsupported_checkbox->isChecked())
    {
        _featureSelector->_showUnsupportedFeatures = _ui->showUnsupported_checkbox->isChecked();
       changed = true;
    }
    if (_featureSelector->_includeOnlyCapabilities != _ui->onlyCapabilities_checkbox->isChecked() )
    {
        _featureSelector->_includeOnlyCapabilities = _ui->onlyCapabilities_checkbox->isChecked();
       changed = true;
    }
    if (_featureSelector->_includeAllCapabilities != _ui->allCapabilities_checkbox->isChecked() )
     {
         _featureSelector->_includeAllCapabilities = _ui->allCapabilities_checkbox->isChecked();
        changed = true;
     }
    if (_featureSelector->_showUnsupportedFeatures != _ui->showUnsupported_checkbox->isChecked() )
     {
         _featureSelector->_showUnsupportedFeatures = _ui->showUnsupported_checkbox->isChecked();
        changed = true;
     }
    if (_featureSelector->_includeTableFeatures != _ui->includeTable_checkbox->isChecked() )
     {
         _featureSelector->_includeTableFeatures = _ui->includeTable_checkbox->isChecked();
        changed = true;
     }
    // TODO: showUnsupported and show-table check boxes

    if (debugFeatureSelection) {
        TRACE("_feature_selector:");
        _featureSelector->dbgrpt();
    }

    if (changed) {
       TRACEF(debugFunc, "Signaling featureSelectionChanged()");
       emit featureSelectionChanged();
       // emit featureSelectionAccepted(feature_list);
    }
    else {
       TRACEF(debugFunc, "NOT Signaling featureSelectionChanged()");
    }


}


void FeatureSelectionDialog::on_buttonBox_helpRequested()
{
    // TRACE();
    QString fn(":/docs/feature_selection.html");
    QFile f(fn);
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&f);

    QString htmlText = in.readAll();

    // qDebug() << htmlText;

    // HelpDialog2(htmlText, this);
    HelpDialog2* hd = new HelpDialog2(this);
    hd->setText(htmlText);
    // hd->_textBrowser->setSource(fn);
    hd->setWindowTitle("ddcui Help - Feature Selection");
    hd->show();
}


// Reset
void FeatureSelectionDialog::on_buttonBox_clicked(QAbstractButton* button)
{
   if(button== (QAbstractButton*) _ui->buttonBox->button(QDialogButtonBox::Reset) ){
      // TRACE("Reset");
      FeatureSelector * defaultSelector = new FeatureSelector;
      useSelectorData(defaultSelector);
      delete defaultSelector;
   }
}

