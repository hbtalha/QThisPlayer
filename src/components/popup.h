#ifndef POPUP_H
#define POPUP_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

class PopUp : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)

public:
    explicit PopUp();

    enum ShowOptions {GENERAL, ERROR_};

    void setPopupText(const QString& text);
    void setPopupTextColor(const QString&);
    void setPopupTextDuration(const float);
    void informBackgroundColor(QColor color);
    void displayMessage(const QString& text);
    void setParentWidget(QWidget* parent);

private:
    void paintEvent(QPaintEvent *event);

    QLabel label;
    QGridLayout layout;
    float popupOpacity;
    QTimer *timer;

    QString textColor;
    int popupTextDuration;
    QWidget* parentWidget;

    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;
};

#endif // POPUP_H
