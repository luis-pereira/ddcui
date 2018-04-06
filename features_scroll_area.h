/* features_scroll_area.h
 *
 * <copyright>
 * Copyright (C) 2018 Sanford Rockowitz <rockowitz@minsoft.com>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * </endcopyright>
 */

#ifndef FEATURES_SCROLL_AREA_H_
#define FEATURES_SCROLL_AREA_H_

#include <QtWidgets/QScrollArea>

#include "nongui/feature_value.h"
#include "nongui/feature_base_model.h"

class FeaturesScrollArea : public QScrollArea {

   Q_OBJECT

public:
   FeaturesScrollArea(QWidget *parent = Q_NULLPTR);
   virtual ~FeaturesScrollArea();

   void setModel(FeatureBaseModel * baseModel);

   const char * _cls;    // className



public slots:

   void featureAdded(FeatureValue& fv);
   void featureUpdated(char feature_code);
   void startInitialLoad(void);
   void endInitialLoad();

private:
   FeatureBaseModel * _baseModel;

};

#endif /* FEATURES_SCROLL_AREA_H_ */
