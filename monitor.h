/* monitor.h */

#ifndef MONITOR_H
#define MONITOR_H

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtWidgets/QPlainTextEdit>

#include <ddcutil_c_api.h>

#include "nongui/vcprequest.h"
#include "nongui/vcpthread.h"
#include "nongui/feature_base_model.h"

#include "feature_selection/feature_selector.h"
#include "table_model_view/feature_table_model.h"
#include "feature_scrollarea/features_scroll_area_contents.h"
#include "list_model_view/feature_item_model.h"
#include "list_widget/feature_list_widget.h"

class FeaturesScrollAreaView;

// Represents a single display

class Monitor : public QObject
{
    Q_OBJECT

public:

    enum FeaturesView {
       FEATURES_VIEW_SCROLLAREA_VIEW,
       FEATURES_VIEW_SCROLLAREA_MOCK,
       FEATURES_VIEW_TABLEVIEW,
       FEATURES_VIEW_TABLEWIDGET,
       FEATURES_VIEW_LISTVIEW,
       FEATURES_VIEW_LISTWIDGET,
       FEATURES_VIEW_UNSET
    };

    Monitor(DDCA_Display_Info * display_info, int monitorNumber);
    ~Monitor();

    DDCA_Feature_List getFeatureList(DDCA_Feature_Subset_Id);
    void setFeatureItemModel(FeatureItemModel * listModel);      // ALT
    void setFeatureTableModel(FeatureTableModel * tableModel);

    const int _monitorNumber = -1;    // 1 based

    FeatureItemModel *   _listModel;
    FeatureTableModel *  _tableModel;
    FeatureBaseModel *   _baseModel;

    VcpRequestQueue*     _requestQueue;
    DDCA_Display_Info *  _displayInfo;
    DDCA_Display_Handle  _dh;
    // VcpThread        _vcpThread;

    QWidget *           _page_moninfo;
    int                 _pageno_moninfo;
    QPlainTextEdit *    _moninfoPlainText;

    QWidget *           _page_capabilities;
    int                 _pageno_capabilities;
    QPlainTextEdit *    _capabilitiesPlainText;


    FeaturesView       _curFeaturesView = FEATURES_VIEW_UNSET;

#ifdef UNUSED
    QWidget *           page_vcp;
    QListWidget *       vcpListWidget;
    int                 _pageno_vcp;
#endif

    // FEATURES_VIEW_LISTWIDGET, initListWidget()
    QWidget *           _page_listWidget   = NULL;
    FeatureListWidget * _featureListWidget = NULL;
    int                 _pageno_listWidget = -1;


#ifdef UNUSED
    QWidget *page_list_widget;
    // QListWidget *feature_listWidget;
    FeatureListWidget * feature_listWidget;
    int _pageno_list_widget;
#endif


    // FEATURES_VIEW_LISTVIEW
    QWidget *           page_list_view   = NULL;
    QListView *         vcp_listView     = NULL;
    int                _pageno_list_view = -1;

    // FEATURES_VIEW_TABLEWIDGET
    // init function defined, but no action
    QWidget *      page_table_item     = NULL;
    QTableWidget * tableWidget         = NULL;
    int            _pageno_table_item  = -1;

    // FEATURES_VIEW_TABLEVIEW
    QWidget *      _page_table_view   = NULL;
    QTableView *   _vcp_tableView     = NULL;
    int            _pageno_table_view = -1;

    // QScrollArea *                 _page_features_scrollarea;
    // FeaturesScrollAreaContents *  _featuresScrollAreaContents;
    // int                           _pageno_features_scrollarea;


    // FEATURES_VIEW_SCROLLAREAVIEW
    // When using FeaturesScrollAreaView, do not allocate a permanent
    // QScrollArea and contents.  These must be created dynamically
    // each time features are loaded.
    FeaturesScrollAreaView *     _featuresScrollAreaView = NULL;

    const char * _cls;    // className

public slots:
    void putVcpRequest(VcpRequest * rqst);

private:
    QHash<DDCA_Feature_Subset_Id, DDCA_Feature_List> _features;
};

#endif // MONITOR_H
