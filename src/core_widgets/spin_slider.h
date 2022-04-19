/** @file spin_slider.h
 *  Combines a slider with a spinbox
 */

// Copyright (C) 2020-2022 Sanford Rockowitz <rockowitz@minsoft.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SLIDER_SPINNER_H_
#define SLIDER_SPINNER_H_

#include <QtWidgets/QWidget>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLayout>

#include "nongui/feature_value.h"

// #include "core_widgets/enhanced_slider.h"
// #include "core_widgets/enhanced_spinbox.h"     // has bug in widget display

class SpinSlider : public QWidget {
   Q_OBJECT

// *** Constructors and Methods

public:
   static void setClassControlKeyRequired(bool onoff);

   // allocation and initialization
   SpinSlider(QWidget * parent = nullptr);
   ~SpinSlider();
   void     setRange(int minval, int maxval);
   void     setFeatureCode(uint8_t featureCode);

   // For containing class
   void     setShSl(uint16_t newval) ;
   uint16_t getShSl();
   void     enable(bool enabled);

public slots:
   void     setInstanceControlKeyRequired(bool onoff);
   void     setInstanceControlKeyPressed(bool onoff);

signals:
    // compiler warning: signals may not be declared virtual
    void    featureValueChanged(uint8_t feature_code, uint8_t sh, uint8_t sl);

private slots:
    // for signaling valueChanged():
    void    onSliderReleased();
    void    onSpinBoxValueChanged(int value);
    void    onSpinBoxTimedOut();

private:
   void     createWidgets();
   QLayout* layoutWidget();

// *** Variables ***
public:
   static bool classControlKeyRequired;

private:
   const char *     _cls;
   // EnhancedSlider * _slider;
   QSlider *        _slider;
   QSpinBox *       _spinBox;
   // EnhancedSpinBox * _spinBox;
   QTimer *         _spinBoxTimer;
   uint8_t          _featureCode;
   bool             _isFeatureCodeSet = true;  // for assert()
   uint16_t         _latestSpinBoxValue;
   // enum          _valueChangeInitiator { ChangedByModel, ChangedByGUI };
   bool             _instanceControlKeyRequired = false;
   bool             _instanceControlKeyPressed = false;
};

#endif /* SLIDER_SPINNER_H_ */
