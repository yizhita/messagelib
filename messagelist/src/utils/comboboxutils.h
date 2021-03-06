/******************************************************************************
 *
 *  Copyright 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#ifndef MESSAGELIST_UTILS_COMBOBOXUTILS_H
#define MESSAGELIST_UTILS_COMBOBOXUTILS_H

#include <QList>
#include <QPair>

class QComboBox;

namespace MessageList {
namespace Utils {
/**
 * Namespace containing some helper functions for QComboBox widgets.
 */
namespace ComboBoxUtils {
/**
   * Fills the specified QComboBox with the options available in optionDescriptors.
   * Each option descriptor contains a description string and a distinct integer (possibly enum)
   * identifier value.
   */
void fillIntegerOptionCombo(QComboBox *combo, const QList< QPair< QString, int > > &optionDescriptors);

/**
   * Returns the identifier of the currently selected option in the specified combo.
   * If the combo has no current selection or something goes wrong then the defaultValue
   * is returned instead.
   */
int getIntegerOptionComboValue(QComboBox *combo, int defaultValue);

/**
   * Sets the currently selected option in the specified combo.
   */
void setIntegerOptionComboValue(QComboBox *combo, int value);
} // namespace ComboBoxUtils
} // namespace Utils
} // namespace MessageList

#endif //!__MESSAGELIST_UTILS_COMBOBOXUTILS_H
