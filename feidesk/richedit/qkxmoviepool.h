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

#ifndef QKXMOVIEPOOL_H
#define QKXMOVIEPOOL_H

#include <QObject>
#include <QBasicTimer>

class QTextDocument;
class QKxMoviePoolPrivate;

class QKxMoviePool : public QObject
{
	Q_OBJECT
    Q_DECLARE_PRIVATE(QKxMoviePool)
public:
    explicit QKxMoviePool(QTextDocument *doc);
    ~QKxMoviePool();
public:
	bool insertMovie(const QString &imageKey, const QString &filePath, bool bForceReplace=false);
	bool contain(const QString &imageKey);
	QImage currentImage(const QString &imageKey) const;
private:
	void timerEvent(QTimerEvent *e);
private:
    QKxMoviePoolPrivate *d_ptr;
};

#endif
