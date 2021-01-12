/** \file feature_selection_dialog.cpp  */

// Copyright (C) 2018-2021 Sanford Rockowitz <rockowitz@minsoft.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <string.h>     // for memcmp()
#include <assert.h>
#include <stdio.h>
#include <iostream>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QRadioButton>
// #include <QtDebug>

#include <ddcutil_types.h>

#include "base/core.h"
#include "base/feature_list.h"
#include "base/global_state.h"
#include "help/help_dialog.h"
#include "help/help_browser.h"

#include "nongui/msgbox_queue.h"

#include "ui_feature_selection_dialog.h"    // generated by uic from feature_selection_dialog.ui
#include "feature_selection_dialog.h"

using namespace std;


// to move to more generic location
void postErrorMessages(QString qstitle, QMessageBox::Icon icon, char** error_msgs) {
   bool debugFunc = false;
   MsgBoxQueue * msgboxQueue = GlobalState::instance()._mainWindow->_msgBoxQueue;
   if (error_msgs) {
      if (debugFunc) {
         int ct = 0;
         while(error_msgs[ct]) ct++;
          printf("(postErrorMessages) %d errors in custom features\n", ct);
      }
      int ndx = 0;
      while (error_msgs[ndx]) {
         // TRACECF(true,"Invalid custom feature:   %s", error_msgs[ndx]);
         QString qsexpl = QString::asprintf("%s", error_msgs[ndx]);
         MsgBoxQueueEntry * qe = new MsgBoxQueueEntry(qstitle,qsexpl,icon);
         if (debugFunc)
            printf( "(%s) Calling _msgboxQueue.put() for qe: %s\n", __func__, QS2S(qe->repr()) );
         msgboxQueue->put(qe);
         free(error_msgs[ndx]);
         ndx++;
      }
   }
}


DDCA_Feature_List FeatureSelectionDialog::validateCustomFeatureList(char * newval) {
   bool debugFunc = false;
   DDCA_Feature_List customFlist = DDCA_EMPTY_FEATURE_LIST;
   TRACECF(debugFunc, "Starting. text: |%s|", newval);
   // qDebug()  << "(validateCustomerFeatureList) newval = |" << newval << "|" <<  endl;

   char ** error_msgs = NULL;
   customFlist = parse_custom_feature_list(newval, &error_msgs);
   if (ddca_feature_list_count(customFlist) == 0) {
      QString qstitle = "Feature Code Error";
      QMessageBox::Icon icon = QMessageBox::Critical;
      // postErrorMessages(qstitle, icon, error_msgs);
      MsgBoxQueue * msgboxQueue = GlobalState::instance()._mainWindow->_msgBoxQueue;
      if (error_msgs) {
         TRACECF(debugFunc, "Calling _postErrorMessages()");
         postErrorMessages(qstitle, icon, error_msgs);
      }
      else {
         QString qsexpl("No custom features specified");
         MsgBoxQueueEntry * qe = new MsgBoxQueueEntry(qstitle,qsexpl,icon);
         TRACECF(debugFunc, "Calling _msgboxQueue.put() for qe: %s", QS2S(qe->repr()));
         msgboxQueue->put(qe);
      }
   }   // feature_list_count == 0

   const char * s = ddca_feature_list_string(customFlist, "x", ",");
   TRACECF(debugFunc, "Done. Returning custom feature list: %s", s);
   return customFlist;
}



void FeatureSelectionDialog::useSelectorData(FeatureSelector * fsel)
{
    bool debugFunc = false;
    debugFunc = debugFunc || debugFeatureSelection;
    if (debugFunc) {
        TRACEC("Setting dialog box widgets from FeatureSelector:");
        fsel->dbgrpt();
    }

    QRadioButton * curButton;
    // switch is exhaustive, so curButton is always assigned, however -Wmaybe-uninitialized
    // flags the curButton->setChecked() line  Assign a dummy value to quiet the warning
    curButton = _ui->known_radioButton;   // dummy value
    DDCA_Feature_Subset_Id local_fsid = fsel->_featureSubsetId;
    switch(local_fsid) {
    case DDCA_SUBSET_KNOWN:
        curButton = _ui->known_radioButton;
        _ui->custom_lineEdit->setEnabled(false);
        _ui->allCapabilities_checkbox->setEnabled(true);
        _ui->onlyCapabilities_checkbox->setEnabled(true);
        break;
    case DDCA_SUBSET_COLOR:
        curButton = _ui->color_radioButton;
        _ui->custom_lineEdit->setEnabled(false);
        _ui->allCapabilities_checkbox->setEnabled(false);
        _ui->onlyCapabilities_checkbox->setEnabled(true);
        break;
    case DDCA_SUBSET_PROFILE: // removed from dialog, case left to satisfy that all cases addressed
        // curButton = _ui->profile_radioButton;
        assert(false);
        break;
    case DDCA_SUBSET_MFG:
        curButton = _ui->mfg_radioButton;
        _ui->custom_lineEdit->setEnabled(false);
        _ui->allCapabilities_checkbox->setEnabled(false);
        _ui->onlyCapabilities_checkbox->setEnabled(true);
        break;
    case DDCA_SUBSET_CAPABILITIES:
        curButton = _ui->capabilities_radioButton;
        _ui->custom_lineEdit->setEnabled(false);
        _ui->allCapabilities_checkbox->setEnabled(false);
        _ui->onlyCapabilities_checkbox->setEnabled(false);
        break;
    case DDCA_SUBSET_SCAN:
        curButton = _ui->scan_radioButton;
        _ui->custom_lineEdit->setEnabled(false);
        _ui->allCapabilities_checkbox->setEnabled(false);
        _ui->onlyCapabilities_checkbox->setEnabled(false);
        break;
    case DDCA_SUBSET_CUSTOM:
    {
       curButton = _ui->custom_radioButton;
       const char * s =  ddca_feature_list_string(fsel->_customFeatureList, "x", " ");
       _ui->custom_lineEdit->setText( s);
       _ui->custom_lineEdit->setEnabled(true);
       _ui->allCapabilities_checkbox->setEnabled(false);
       _ui->onlyCapabilities_checkbox->setEnabled(true);
    }
       break;
    case DDCA_SUBSET_UNSET:
        assert(false);
        break;
    }
    _ui->showUnsupported_checkbox->setEnabled(true);   // true for all feature sets

    curButton->setChecked(true);
    // to do: enable/disable other buttons as appropriate

    if (debugFunc) {
       TRACEC("fsel before setting flag checkboxes:");
       fsel->dbgrpt();
    }
    _ui->includeTable_checkbox->setChecked(    fsel->_includeTableFeatures);
    _ui->showUnsupported_checkbox->setChecked( fsel->_showUnsupportedFeatures);
    _ui->onlyCapabilities_checkbox->setChecked(fsel->_includeOnlyCapabilities);
    _ui->allCapabilities_checkbox->setChecked( fsel->_includeAllCapabilities);

    TRACECF(debugFunc, "Done. _ui->custom_lineEdit->isEnabled() = %s", SBOOL(_ui->custom_lineEdit->isEnabled()) );
}


void FeatureSelectionDialog::useSelectorData() {
   useSelectorData(_featureSelector);
}


FeatureSelectionDialog::FeatureSelectionDialog(
      QWidget *         parent,
      FeatureSelector * featureSelector
     ) :  QDialog(parent)
       ,  _cls(strdup(metaObject()->className()))
       ,  _ui(new Ui::FeatureSelectionDialog)
       ,  _featureSelector(featureSelector)
{
    // TRACE("_ui=%p, _featureSelector = %p\n", _ui, _featureSelector);
    _ui->setupUi(this);

    QObject::connect(_ui->known_radioButton, &QAbstractButton::clicked,
                     this, &FeatureSelectionDialog::on_known_radioButton_clicked);
    QObject::connect(_ui->capabilities_radioButton, &QAbstractButton::clicked,
                     this, &FeatureSelectionDialog::on_capabilities_radioButton_clicked);
    QObject::connect(_ui->mfg_radioButton, &QAbstractButton::clicked,
                     this, &FeatureSelectionDialog::on_mfg_radioButton_clicked);
    QObject::connect(_ui->color_radioButton, &QAbstractButton::clicked,
                     this, &FeatureSelectionDialog::on_color_radioButton_clicked);
    QObject::connect(_ui->scan_radioButton, &QAbstractButton::clicked,
                     this, &FeatureSelectionDialog::on_scan_radioButton_clicked);
    QObject::connect(_ui->custom_radioButton, &QAbstractButton::clicked,
                     this, &FeatureSelectionDialog::on_custom_radioButton_clicked);

    // QObject::connect(_ui->custom_lineEdit, &QLineEdit::returnPressed,
    //                  this, &FeatureSelectionDialog::on_custom_lineEdit_returnPressed );

    // QObject::connect(_ui->custom_lineEdit, &QLineEdit::editingFinished,
    //                  this, &FeatureSelectionDialog::on_custom_lineEdit_editingFinished );

    // QObject::connect(_ui->custom_lineEdit, &QLineEdit::textEdited,
    //                  this, &FeatureSelectionDialog::on_custom_lineEdit_textEdited );


    useSelectorData(_featureSelector);
}


FeatureSelectionDialog::~FeatureSelectionDialog()
{
    delete _ui;
}


//
// Radio Buttons
//

void FeatureSelectionDialog::on_known_radioButton_clicked(bool checked) {
   _ui->allCapabilities_checkbox->setEnabled(true);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
   _ui->custom_lineEdit->setEnabled(false);
}

void FeatureSelectionDialog::on_capabilities_radioButton_clicked(bool checked) {
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(false);
   _ui->allCapabilities_checkbox->setChecked(false);
   _ui->onlyCapabilities_checkbox->setChecked(false);
   _ui->custom_lineEdit->setEnabled(false);
}

void FeatureSelectionDialog::on_mfg_radioButton_clicked(bool checked) {
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
   _ui->allCapabilities_checkbox->setChecked(false);
   _ui->custom_lineEdit->setEnabled(false);
}

void FeatureSelectionDialog::on_color_radioButton_clicked(bool checked) {
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
   _ui->allCapabilities_checkbox->setChecked(false);
   _ui->custom_lineEdit->setEnabled(false);
}

#ifdef NO_PROFILE_BUTTON
void FeatureSelectionDialog::on_profile_radioButton_clicked(bool checked) {
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
   _ui->allCapabilities_checkbox->setChecked(false);
}
#endif

void FeatureSelectionDialog::on_scan_radioButton_clicked(bool checked) {
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->onlyCapabilities_checkbox->setEnabled(false);
   _ui->allCapabilities_checkbox->setChecked(false);
   _ui->onlyCapabilities_checkbox->setChecked(false);
   _ui->custom_lineEdit->setEnabled(false);
}

void FeatureSelectionDialog::on_custom_radioButton_clicked(bool checked) {
   _ui->allCapabilities_checkbox->setEnabled(false);
   _ui->allCapabilities_checkbox->setChecked(false);
   _ui->onlyCapabilities_checkbox->setEnabled(true);
   _ui->onlyCapabilities_checkbox->setChecked(false);

   _ui->custom_lineEdit->setEnabled(true);
   // move cursor to custom_lineEdit
   _ui->custom_lineEdit->setFocus(Qt::OtherFocusReason);
}


//
// Checkboxes
//

void FeatureSelectionDialog::on_onlyCapabilities_checkbox_stateChanged(int arg1)
{
   // cout << "(on_onlyCapabilities_checkBox_stateChanged) arg1 = " << arg1 << endl;
   if (_ui->onlyCapabilities_checkbox->isChecked() ) {     // or != 0
      _ui->allCapabilities_checkbox->setCheckState(Qt::Unchecked);
   }
}

void FeatureSelectionDialog::on_allCapabilities_checkbox_stateChanged(int arg1)
{
   // cout << "(on_allCapabilities_checkBox_stateChanged) arg1 = " << arg1 << endl;
   if (arg1 == 2) {
      _ui->onlyCapabilities_checkbox->setCheckState(Qt::Unchecked);
   }
}


//
// Button Box buttons
//

// Accept
void FeatureSelectionDialog::on_buttonBox_accepted()
{
    bool debugFunc = false;
    debugFunc = debugFunc || debugFeatureSelection;
    TRACECF(debugFunc, "=== Starting.");


    DDCA_Feature_List customFlist = DDCA_EMPTY_FEATURE_LIST;
    DDCA_Feature_Subset_Id fsid;

    // Which feature selection button is currently checked?
    if (_ui->color_radioButton->isChecked())
        fsid = DDCA_SUBSET_COLOR;
    else if (_ui->known_radioButton->isChecked())
        fsid = DDCA_SUBSET_KNOWN;
    else if (_ui->mfg_radioButton->isChecked())
        fsid = DDCA_SUBSET_MFG;
#ifdef NO_PROFILE_BUTTON
    else if (_ui->profile_radioButton->isChecked())
        fsid = DDCA_SUBSET_PROFILE;
#endif
    else if (_ui->capabilities_radioButton->isChecked())
        fsid = DDCA_SUBSET_CAPABILITIES;
    else if (_ui->scan_radioButton->isChecked())
        fsid = DDCA_SUBSET_SCAN;

    // Custom radio button
    else if (_ui->custom_radioButton->isChecked()) {
        fsid = DDCA_SUBSET_CUSTOM;
        QString text = _ui->custom_lineEdit->text().trimmed();
        TRACECF(debugFunc, "custom radioButton: text: |%s|", QS2S(text));
        customFlist = validateCustomFeatureList(QS2S(text));
        if (ddca_feature_list_count(customFlist) == 0) {
           TRACECF(debugFunc, "Custom feature error, returning");
           return;   // there's a custom feature error, don't exit dialog
        }
        TRACECF(debugFunc, "custom feature list: %s",
              ddca_feature_list_string(customFlist, "x", ", ") );
    }   // custom_radioButton

    else                 // no radio button checked
        assert(false);   // should never occur
    // end, custom radio button

    TRACECF(debugFunc, "Checking for any changes...fsid=%d, _featureSelector->featureSubsetId = %d",
               fsid, _featureSelector->_featureSubsetId);
    bool changed = false;
    if (fsid != _featureSelector->_featureSubsetId) {
       _featureSelector->_featureSubsetId = fsid;
       _featureSelector->_customFeatureList = customFlist;
       changed = true;
       TRACECF(debugFunc,"feature set changed");
    }
    else if (fsid == DDCA_SUBSET_CUSTOM) {
       DDCA_Feature_List old_flist = _featureSelector->_customFeatureList;
       if (!ddca_feature_list_eq(customFlist, old_flist)) {
          _featureSelector->_customFeatureList = customFlist;
          changed = true;
       }
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

    if (debugFunc) {
        TRACEC("_feature_selector:");
        _featureSelector->dbgrpt();
    }

    if (changed) {
       TRACECF(debugFunc, "Signaling featureSelectionChanged()");
       emit featureSelectionChanged();
       // emit featureSelectionAccepted(fsid);
    }
    else {
       TRACECF(debugFunc, "NOT Signaling featureSelectionChanged()");
    }
    accept();
}


// Reset
void FeatureSelectionDialog::on_buttonBox_clicked(QAbstractButton* button)
{
   if(button == (QAbstractButton*) _ui->buttonBox->button(QDialogButtonBox::Reset) ){
      // TRACE("Reset");
      FeatureSelector * defaultSelector = new FeatureSelector;
      useSelectorData(defaultSelector);
      delete defaultSelector;
   }
}


// Help
void FeatureSelectionDialog::on_buttonBox_helpRequested()
{
    // TRACE();
    // QString fn(":/docs/feature_selection.html");
    // QString fn("qrc:docs/help_feature_selection.html");
    // QFile f(fn);
    // f.open(QFile::ReadOnly | QFile::Text);
    // QTextStream in(&f);

    // QString htmlText = in.readAll();

    // qDebug() << htmlText;

    // HelpDialog2(htmlText, this);
    // HelpDialog2* hd = new HelpDialog2(this);
    // hd->setText(htmlText);
    // hd->setSource(fn);
    // hd->setWindowTitle("ddcui Help - Feature Selection");
    // hd->show();

    // HelpBrowser::showPage(fn, false);

    HelpDialog* hd = new HelpDialog(this);
    hd->setSource("qrc:docs/help_feature_selection.html");
    hd->exec();

}

