#ifndef VALUE_NC_WIDGET_H
#define VALUE_NC_WIDGET_H

#include <QWidget>
#include <QtWidgets/QComboBox>

#include "value_base_widget.h"


class ValueNcWidget : public ValueBaseWidget
{

        Q_OBJECT

public:
    ValueNcWidget(QWidget *parent = nullptr);

    void setFeatureValue(const FeatureValue &fv);    // virtual
    void setCurrentValue(uint16_t newval); // virtual

    QComboBox *_cb;

    int findItem(uint8_t sl_value);

    uint16_t getCurrentValue() override;

};

#endif // VALUE_NC_WIDGET_H
