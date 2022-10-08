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

#include "qkxtextobject.h"

#include <QTextEdit>

class QKxTextObjectPrivate
{
    Q_DECLARE_PUBLIC(QKxTextObject);
public:
    QKxTextObjectPrivate();
    virtual ~QKxTextObjectPrivate();

protected:
    QKxTextObject *q_ptr;
    QTextEdit *m_edit;
};

QKxTextObjectPrivate::QKxTextObjectPrivate()
: q_ptr( NULL )
, m_edit( NULL )
{

}

QKxTextObjectPrivate::~QKxTextObjectPrivate()
{

}


//////////////////////////////////////////////////////////////////////////

QKxTextObject::QKxTextObject( QTextEdit* editor )
: d_ptr( new QKxTextObjectPrivate )
{
	d_ptr->q_ptr = this;
	d_func()->m_edit = editor;
	_init();
}

QKxTextObject::QKxTextObject( QKxTextObjectPrivate &dd, QTextEdit* editor )
: d_ptr( &dd )
{
	d_ptr->q_ptr = this;
	d_func()->m_edit = editor;
	_init();
}

QKxTextObject::~QKxTextObject()
{
	Q_ASSERT_X(d_ptr, __FUNCTION__, "private data is null.");
	delete d_ptr;
	d_ptr = NULL;
}

QSizeF QKxTextObject::intrinsicSize( QTextDocument *doc, int posInDocument, const QTextFormat &format )
{
	return QSizeF( 80.0, 36.0 );
}

void QKxTextObject::drawObject( QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format )
{
    Q_D(QKxTextObject);

}

void QKxTextObject::_init()
{
    Q_D(QKxTextObject);
	if (d->m_edit && d->m_edit->document())
	{
		setParent(d->m_edit->document());
	}
}

void QKxTextObject::commonEvent( QMouseEvent* event, quint32 textObjectIndex )
{
    Q_D(QKxTextObject);

    //QPointF mousePos = d_func()->m_edit->mapToContents( event->pos() );

	switch ( event->type() )
	{
	case QEvent::MouseButtonPress:
		break;
	case  QEvent::MouseButtonRelease:
		break;
	case QEvent::MouseMove:
		break;
	}
}
