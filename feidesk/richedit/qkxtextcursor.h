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

#ifndef QKXTEXTCURSOR_H
#define QKXTEXTCURSOR_H

#include <QtGlobal>
#include <QTextCursor>

const int KImageKey	= QTextFormat::UserProperty + 1;
const int KLinkKey	= QTextFormat::UserProperty + 2;

const int KAnimationImage = QTextFormat::UserProperty + 3;

class QKxTextCursor;

class QKxTextCursorPrivate;
class QKxMoviePool;

class QKxTextCursor : public QTextCursor
{
    Q_DECLARE_PRIVATE(QKxTextCursor)
public:
    QKxTextCursor();
    QKxTextCursor(const QTextCursor &tc);
    QKxTextCursor(const QKxTextCursor &tc);
    ~QKxTextCursor();
    QKxTextCursor &operator=(const QTextCursor &other);
    QKxTextCursor &operator=(const QKxTextCursor &other);
public:
	void insertImage(const QString &name);
	void insertImage(const QString &name, const QString &key);
	void insertImage(const QString &name, const QTextImageFormat &format);
	void insertImage(const QString &name, const QImage &image, const QTextImageFormat &format = QTextImageFormat());
	void insertImage(const QString &name, const QString &key, const QString &linkValue, const QString &linkKey);

	void insertText(const QString &text);
	void insertText(const QString &text, const QTextCharFormat &format);
	void insertText(const QString &text, const QColor& clr, bool bUnderLine=false);
	void insertText(const QString &text, const QColor& clr, const QFont& font);
	void insertText(const QString &text, const QTextCharFormat &format, const QString &linkValue, const QString &linkKey);
	void insertText(const QString &text, const QColor &color, const QString &linkValue, const QString &linkKey, bool bUnderLine = false);

    void insertHtml(const QString &text);
	void insertHtml(const QString &text, const QMap<QString, QString>& imgs);
	void insertHtml(const QString &text, const QMap<QString, QImage>& imgs);

	void insertFragment(const QByteArray& frag);
	void insertFragment(QList<QPair<QString,QTextCharFormat>>& frags);

    QKxMoviePool *moviePool();
private:
    QKxTextCursorPrivate *d_ptr;
};

#endif
