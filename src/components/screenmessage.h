/****************************************************************************
* QThisPlayer - media player
* Copyright (C) 2021 Helder Batalha <helderbatalha3@gmail.com>
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef POPUP_H
#define POPUP_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

class ScreenMessage : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)

public:
    explicit ScreenMessage();

    enum ShowOption {GENERAL, ERROR_};

    void setPopupText(const QString& text);
    void setPopupTextColor(const QString&);
    void setPopupTextDuration(const float);
    void informBackgroundColor(QColor color);
    void displayMessage(const QString& text, ShowOption showOpt);
    void setViewWidget(QWidget* parent);
    void adjustPositionToParent();

signals:
    void mouseWheelRolledUp();
    void mouseWheelRolledDown();

private:
    void paintEvent(QPaintEvent *event) override;

    QLabel label;
    QGridLayout layout;
    float popupOpacity;
    QTimer *timer;

    QString textColor;
    int popupTextDuration;
    QWidget* viewWidget;
    ShowOption showOption;

    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;

    void wheelEvent(QWheelEvent *event) override;
};

#endif // POPUP_H
