/*******************************************************************************************
*
* Copyright (C) 2022 Guangzhou AoYiDuo Network Technology Co.,Ltd. All Rights Reserved.
*
* Contact: http://www.aoyiduo.com
*
*   this file is used under the terms of the GPLv3[GNU GENERAL PUBLIC LICENSE v3]
* more information follow the website: https://www.gnu.org/licenses/gpl-3.0.en.html
*
*******************************************************************************************/

#ifndef QWOLOADINGWIDGET_H
#define QWOLOADINGWIDGET_H

#include <QWidget>
#include <QTimer>

class QKxLoadingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QKxLoadingWidget(const QColor& clr, bool isBar = false, QWidget *parent = 0);
    explicit QKxLoadingWidget(const QString& imagePath, QWidget *parent = 0);
    ~QKxLoadingWidget();

    void setSpeed(int fps=30);
    void setRadius(int n);
protected:
    void paintEvent(QPaintEvent *ev);
    void showEvent(QShowEvent *ev);
    void hideEvent(QHideEvent *ev);
private slots:
    void onRotateRequest();
private:
    bool m_isBar;
    QColor m_color;
    QPixmap m_pixmap;
    QTimer m_timer;
    qreal m_angle;
    qreal m_step;
    int m_radius;
};

#endif // QWOLOADINGWIDGET_H
