/* features_scrollarea_view.cpp */

// Copyright (C) 2018 Sanford Rockowitz <rockowitz@minsoft.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "feature_scrollarea/features_scrollarea_view.h"

#include <QtCore/QString>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <typeinfo>

#include "base/ddcui_globals.h"
#include "base/debug_utils.h"
#include "base/global_state.h"
#include "base/monitor.h"
#include "base/other_options_state.h"
// #include "base/vertical_scroll_area.h"

#include "nongui/ddc_error.h"
#include "nongui/msgbox_queue.h"

#include "feature_scrollarea/feature_widget.h"
#include "feature_scrollarea/feature_widget_header.h"
#include "feature_scrollarea/features_scrollarea_contents.h"
#include "feature_scrollarea/features_scrollarea.h"


static bool showDimensionReport = false;


FeaturesScrollAreaView::FeaturesScrollAreaView(
        Monitor *          monitor,
        FeatureBaseModel * model,
        QStackedWidget *   centralStackedWidget,
        MsgBoxQueue *      msgboxQueue,
        QObject *          parent)
    : QObject(parent)
    , _cls(metaObject()->className())
    , _monitor(monitor)
    , _baseModel(model)
    , _centralStackedWidget(centralStackedWidget)
    , _msgboxQueue(msgboxQueue)
    , _curNcValuesSource(OtherOptionsState::DefaultNcValuesSource)  // , _curNcValuesSource(NcValuesSourceUnset)
    , _controlKeyRequired(false)
{
   // TRACE("Executing. _msgboxQueue=%p", _msgboxQueue);
}


void FeaturesScrollAreaView::freeContents(void) {
   // TODO
}


void FeaturesScrollAreaView::onEndInitialLoad(void) {
    bool debug = true;
    TRACEF(debug, "Starting, Monitor=%s", _monitor->_displayInfo->model_name);

    // TODO:
    // free existing QScrollArea, QScrollAreaContents

    // scrollWrap contains a FeatureWidgetHeader and a scrollable
    // collection of FeatureWidget rows

    QFrame * scrollWrap = new QFrame();
    QVBoxLayout * wrapLayout = new QVBoxLayout;
    wrapLayout->setSpacing(0);
    wrapLayout->addWidget(new FeatureWidgetHeader());

    FeaturesScrollArea * scrollArea = new FeaturesScrollArea();
    scrollArea->setWidgetResizable(true);

    FeaturesScrollAreaContents * scrollAreaContents = new FeaturesScrollAreaContents();
    scrollAreaContents->setObjectName("scrollAreaContents local to onEndInitialLoad");

    // no effect, only applies if no layout is set
    // scrollAreaContents->setSizePolicy(pageSizePolicy());

    // screws things up, value block forced right and truncated
    // scrollAreaContents->setMinimumSize(QSize(900,0));

    QVBoxLayout * vLayout  = new QVBoxLayout();
    vLayout->setObjectName("vLayout in onEndInitLoad");
    vLayout->setContentsMargins(0,0,0,0);
    // vLayout->setMargin(0);
    vLayout->setSpacing(0);    // <=== CONTROLS SPACING BETWEEN ROWS

    // doesnt solve the non-expanding widgets
    QSizePolicy adjSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    scrollAreaContents->setSizePolicy(adjSizePolicy);

    if (debugLayout) {
       scrollAreaContents->setStyleSheet("background-color:chartreuse;");
       _centralStackedWidget->setStyleSheet("background-color:chocolate:");
       // so this is where the gap is
       scrollArea->setStyleSheet("background-color:purple");
    }

    // vLayout->addWidget(new FeatureWidgetHeader());

    int ct = 0;
    for (int feature_code = 0; feature_code < 256; feature_code++) {
         FeatureValue * fv =  _baseModel->modelVcpValueFilteredFind(feature_code);
         if (fv) {
             // FeatureWidget * w = new FeatureWidget();
             FeatureWidget * w = new FeatureWidget(*fv);
             if (debugLayout) {
                // gets applied to the widgets inside w, not to w itself
                w->setStyleSheet("background-color:brown;");
             }
             // w->setFeatureValue(*fv);

             QObject::connect(w ,   &FeatureWidget::valueChanged,
                              this, &FeaturesScrollAreaView::onUIValueChanged);
             vLayout->addWidget(w);
             _widgets[feature_code] = w;
             ct++;
         }
    }

    scrollAreaContents->setLayout(vLayout);
    scrollArea->setWidget(scrollAreaContents);

    wrapLayout->addWidget(scrollArea);
    scrollWrap->setLayout(wrapLayout);

    QObject::connect(_baseModel, &FeatureBaseModel::signalFeatureUpdated3,
                     this,       &FeaturesScrollAreaView::onModelValueChanged);


    /* int pageno = */ _centralStackedWidget->addWidget(scrollWrap);   // was scrollArea
    // _centralStackedWidget->hide();    // no effect
    _centralStackedWidget->setCurrentWidget(scrollWrap);    // was scrollArea

    if (debugLayout) {

       static bool dimensionReportShown = false;
       if ( (showDimensionReport && !dimensionReportShown) ) {
           TRACE("---------------------> scrollAreaContents in QScrollArea");
           reportWidgetDimensions(scrollAreaContents,    _cls, __func__, "scrollAreaContents in QScrollArea");
           TRACE("---------------------> QScrollArea in _centralStackedWidget");
           reportWidgetDimensions(scrollArea,            _cls, __func__, "QScrollArea in _centralStackedWidget");
           TRACE("---------------------> centralStackedWidget" );
           reportWidgetDimensions(_centralStackedWidget, _cls, __func__, "centralStackedWidget");

           dimensionReportShown = true;
       }
    }

    GlobalState& globalState = GlobalState::instance();
    _curNcValuesSource = globalState._otherOptionsState->ncValuesSource;
    _scrollAreaContents = scrollAreaContents;
    _centralStackedWidget->show();

    TRACEF(debug, "Done.  feature count: %d", ct);
}


void FeaturesScrollAreaView::onUIValueChanged(
      uint8_t featureCode,
      bool    writeOnly,
      uint8_t sh,
      uint8_t sl)
{
   bool debug = debugSignals;
   // debug = true;
   TRACEF(debug,
             "Starting. feature_code = 0x%02x, writeOnly=%s, sh=0x%02x, sl=0x%02x",
             featureCode, sbool(writeOnly), sh, sl);

   FeatureValue * curFv = _baseModel->modelVcpValueFind(featureCode);
   if (curFv && curFv->val().sh == sh && curFv->val().sl == sl) {
      TRACE("New value matches model value, Suppressing.");
   }
   else {
      TRACEF(debug, "=> emitting signalVcpRequest() for VcpSetRequst, featureCode=0x%02x", featureCode);
      VcpRequest * rqst = new VcpSetRequest(featureCode, sh, sl, writeOnly);
      emit signalVcpRequest(rqst);  // used to call into monitor

      // If feature value change affects other features, reread possibly affected features
      switch(featureCode) {
      case 0x05:      // restore factory defaults brightness/contrast
         emit signalVcpRequest( new VcpGetRequest(0x10) );
         emit signalVcpRequest( new VcpGetRequest(0x12) );    // what if contrast is unsupported feature?
         break;
      case 0x04:      // restore factory defaults
      case 0x06:      // restore geometry defaults  - treat as restore factory defaults
      case 0x08:      // restore color defaults     - treat as restore factory defaults
      case 0x14:      // select color preset        - treat as restore factory defaults
         _monitor->_baseModel->reloadFeatures();
         break;
      default:
         break;
      }
   }

   TRACEF(debug, "Done");
}


void FeaturesScrollAreaView::onModelValueChanged(
      const char* caller,
      uint8_t     featureCode,
      uint8_t     sh,
      uint8_t     sl)
{
   TRACEF(debugSignals,
             "caller = %s, feature_code = 0x%02x, sh=0x%02x, sl=0x%02x",
             caller, featureCode, sh, sl);

   // find the FeatureWidget for the feature code
   FeatureWidget * curWidget = _widgets[featureCode];

   // set value in the widget
   uint16_t newval = sh << 8 | sl;
   curWidget->setCurrentValue(newval);
}


void FeaturesScrollAreaView::onNcValuesSourceChanged(NcValuesSource newsrc) {
   bool debugFunc = debugSignals;
   // debugFunc = true;
   TRACEF(debugFunc,
             "newsrc=%d - %s, _curNcValuesSource=%d - %s",
             newsrc,             (char*) ncValuesSourceName(newsrc),
             _curNcValuesSource, (char*) ncValuesSourceName(_curNcValuesSource));

   if (newsrc != _curNcValuesSource) {
      // reportWidgetChildren(_scrollAreaContents, (const char *) "Children of FeatuersScrollAreaView");

      QObjectList  childs = _scrollAreaContents->children();
      for (int ndx = 0; ndx < childs.size(); ndx++) {
          QObject* curobj = childs.at(ndx);
          // QString name   = curobj->objectName();
          // const char *  clsName = curobj->metaObject()->className();
          // printf("   Child: %s, type:%s\n", name.toLatin1().data(), clsName); fflush(stdout);

          // Both .inherits() and dyamic_cast work
         //  if (curobj->inherits("FeatureWidget"))
         //      printf("(%s::%s) inherits()\n", _cls, __func__); fflush(stdout);

          FeatureWidget * curWidget = dynamic_cast<FeatureWidget*>(curobj);
          if (curWidget) {
             TRACEF(debugFunc, "dynamic_cast succeeded");
             if (curWidget->hasSlTable()) {
                TRACEF(debugFunc, "feature_code=0x%02x, has SL table", curWidget->_feature_code);
                curWidget->setNcValuesSource(newsrc);
             }
          }
      }
      _curNcValuesSource = newsrc;
   }
   TRACEF(debugFunc, "Done");
}


void FeaturesScrollAreaView::onModelDdcDetailedError(DdcDetailedError* perec) {
   bool debugFunc = debugSignals;
   // debugFunc = true;

    // TRACE("Starting");
    TRACEF(debugFunc, "perec=%p, perec->%s", perec, perec->srepr() );
    // std::cout << "typeid(perec):  " << typeid(perec).name()  << std::endl;
    // std::cout << "typeid(*perec): " << typeid(*perec).name() << std::endl;

    // DDCA_Display_Info * dinfo = _monitor->_displayInfo;

    QString qstitle = QString("ddcutil Error");
    QString qsexpl  = perec->expl();
    QMessageBox::Icon icon = QMessageBox::Warning;

    MsgBoxQueueEntry * qe = new MsgBoxQueueEntry(
                                       qstitle,
                                       qsexpl,
                                       icon);
   TRACEF(debugFunc, "Calling _msgboxQueue.put() for qe: %s", qs2s(qe->repr()));
   _msgboxQueue->put(qe);
}


void FeaturesScrollAreaView::onModelDdcFeatureError(DdcFeatureError* perec) {
    bool debugFunc = debugSignals;
    // debugFunc = true;
    TRACEF(debugFunc, "perec=%p, perec->%s", perec, qs2s(perec->repr()) );
    // std::cout << "typeid(perec):  " << typeid(perec).name()  << std::endl;
    // std::cout << "typeid(*perec): " << typeid(*perec).name() << std::endl;

    DDCA_Display_Info * dinfo = _monitor->_displayInfo;

    QString qstitle = QString("ddcutil API Error");
    QString qsexpl  = perec->expl();
    QMessageBox::Icon icon = QMessageBox::Warning;

    if ( QString::compare(perec->_ddcFunction, QString("ddca_get_capabilities_string")) == 0) {
        // TRACE("ddca_get_capabilities_string() branch");
        qsexpl = QString::asprintf(
                          "Error reading capabilities string for display %d - %s",
                          dinfo->dispno+1, dinfo->model_name
                       );
        qstitle = "DDC Error";
    }

    MsgBoxQueueEntry * qe = new MsgBoxQueueEntry(
                                   qstitle,
                                   qsexpl,
                                   icon);
     TRACEF(debugFunc, "Calling _msgboxQueue.put() for qe: %s", qs2s(qe->repr()));
    _msgboxQueue->put(qe);
}

