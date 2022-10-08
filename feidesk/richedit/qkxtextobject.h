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

#ifndef QKXTEXTOBJECT_H
#define QKXTEXTOBJECT_H

#include <QTextObjectInterface>
#include <QTextFormat>


class QKxTextObjectPrivate;
class QTextEdit;

#define defTextObjectIndex	QTextFormat::ObjectIndex + 1000

class QKxTextObject: public QObject, public QTextObjectInterface
{
public:
	Q_OBJECT
	Q_INTERFACES(QTextObjectInterface)
    Q_DISABLE_COPY(QKxTextObject)
    Q_DECLARE_PRIVATE_D(d_ptr, QKxTextObject)

public:
	enum { objectType = QTextFormat::UserObject + 1 };

public:
    explicit QKxTextObject( QTextEdit* editor );

protected:
    explicit QKxTextObject( QKxTextObjectPrivate &dd, QTextEdit* editor );
    virtual ~QKxTextObject();

	virtual QSizeF intrinsicSize( QTextDocument *doc, int posInDocument, const QTextFormat &format);
	virtual void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);
	virtual void commonEvent(QMouseEvent* event, quint32 textObjectIndex);

    QTextEdit *textEdit() const;

protected:
    QKxTextObjectPrivate * d_ptr;

private:
    friend class QKxTextBrowser;
	void _init();
};

#endif
