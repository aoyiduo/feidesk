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

#include "qkxtextcursor.h"
#include "qkxtextbrowser.h"
#include "qkxmoviepool.h"

#include <QPointer>

Q_DECLARE_METATYPE(QTextCharFormat)

class QKxTextCursorPrivate
{
public:
    QPointer<QKxMoviePool> m_movies;

public:
    explicit QKxTextCursorPrivate()
	{

	}
};

QKxTextCursor::QKxTextCursor()
:d_ptr(new QKxTextCursorPrivate())
{

}

QKxTextCursor::QKxTextCursor( const QTextCursor &tc )
: QTextCursor( tc )
, d_ptr( new QKxTextCursorPrivate() )
{
	
}

QKxTextCursor::QKxTextCursor( const QKxTextCursor &tc )
 : QTextCursor( tc )
 , d_ptr( new QKxTextCursorPrivate(*(tc.d_ptr)) )
{

}

QKxTextCursor::~QKxTextCursor(void)
{
    delete d_ptr;
}

void QKxTextCursor::insertImage( const QString & name )
{
	QTextImageFormat format;
	format.setName(name);
	format.setProperty(KImageKey, name);

	insertImage(name, format);
}

void QKxTextCursor::insertImage( const QString &name, const QString &key )
{
	QTextImageFormat imageFormat;
	imageFormat.setName(name);
	imageFormat.setProperty(KImageKey, key);

	insertImage(name, imageFormat);
}

void QKxTextCursor::insertImage( const QString &name, const QImage &image, const QTextImageFormat &format )
{
	QTextImageFormat inserFormat = format;
	inserFormat.setName(name);
	if(!inserFormat.hasProperty(KImageKey))
	{
		inserFormat.setProperty(KImageKey, name);
	}

	document()->addResource(QTextDocument::ImageResource, QUrl(name), image);
    QTextCursor::insertImage(inserFormat);
}


void QKxTextCursor::insertImage( const QString &name, const QString &key, const QString &linkValue, const QString &linkKey )
{
	QTextImageFormat imageFormat;

	imageFormat.setAnchor(true);
	imageFormat.setAnchorHref(linkValue);
	imageFormat.setName(name);
	imageFormat.setProperty(KImageKey, key);

	if(!linkKey.isEmpty())
	{
		imageFormat.setProperty( KLinkKey, linkKey );
	}

	insertImage(name, imageFormat);
}

void QKxTextCursor::insertImage( const QString &name, const QTextImageFormat &format )
{
	QTextImageFormat imageForamt = format;

	imageForamt.setName(name);
	if(!imageForamt.hasProperty(KImageKey))
	{
		imageForamt.setProperty(KImageKey, name);
	}

	if(moviePool()->insertMovie(name, name))
	{
        Q_D(QKxTextCursor);

		imageForamt.setProperty(KAnimationImage,true);
		document()->addResource(QTextDocument::ImageResource, QUrl(name), moviePool()->currentImage(name));
	}
	else
	{
		QImage image(name);
		if(!image.isNull())
		{
			document()->addResource(QTextDocument::ImageResource,QUrl(name), image);
		}
	}

    QTextCursor::insertImage(imageForamt);
}

void QKxTextCursor::insertText( const QString &displayName, const QColor &color, const QString &linkValue, const QString &linkKey, bool bUnderLine )
{
    Q_D(QKxTextCursor);

	QTextCharFormat linkFormat;

	linkFormat.setFontUnderline(bUnderLine);
	linkFormat.setForeground(color);

	insertText(displayName, linkFormat, linkValue, linkKey);
}

void QKxTextCursor::insertText( const QString &displayName, const QTextCharFormat &format, const QString &linkValue, const QString &linkKey )
{
	QTextCharFormat linkFormat = format;

	linkFormat.setAnchor(true);
	linkFormat.setAnchorName(displayName);
	linkFormat.setAnchorHref(linkValue);
	if(!linkKey.isEmpty())
	{
		linkFormat.setProperty( KLinkKey, linkKey );
	}

	insertText( displayName, linkFormat );
}

void QKxTextCursor::insertText( const QString& text )
{
    QTextCursor::insertText(text);
}

void QKxTextCursor::insertText( const QString &text, const QTextCharFormat &format )
{
	QTextCharFormat fmt = format;

    QTextCursor::insertText(text, fmt);
}

void QKxTextCursor::insertText( const QString &text, const QColor& clr, bool bUnderLine )
{
	QTextCharFormat format;
	format.setForeground(clr);
	format.setFontUnderline(bUnderLine);

    QTextCursor::insertText(text, format);
}

void QKxTextCursor::insertText( const QString &text, const QColor& clr, const QFont& font )
{
	QTextCharFormat format;
	format.setFont(font);
	format.setForeground(clr);
    QTextCursor::insertText(text, format);
}

QKxMoviePool * QKxTextCursor::moviePool()
{
    Q_D(QKxTextCursor);
	if(d->m_movies)
		return d->m_movies;
	QTextDocument *doc = document();
	QVariant vmovie = doc->property("movies");
	if(vmovie.isValid())
	{
        QKxMoviePool *pool = qobject_cast<QKxMoviePool*>(vmovie.value<QObject*>());
		d->m_movies = pool;
		return pool;
	}

    QKxMoviePool *pool = new QKxMoviePool(doc);
	doc->setProperty("movies", QVariant::fromValue<QObject*>(pool));
	d->m_movies = pool;
	return pool;
}

QKxTextCursor & QKxTextCursor::operator=( const QTextCursor &other )
{
    QTextCursor::operator=(other);
	if(document() != other.document())
	{
		d_ptr->m_movies = NULL;
	}
	return *this;
}

QKxTextCursor & QKxTextCursor::operator=( const QKxTextCursor &other )
{
    QTextCursor::operator=(other);
	*d_ptr = *other.d_ptr;
	return *this;
}

void QKxTextCursor::insertHtml( const QString &text, const QMap<QString, QString>& imgs )
{
	if(!imgs.isEmpty())
	{
		for(QMap<QString,QString>::const_iterator iter = imgs.begin(); iter != imgs.end(); iter++)
		{
			QString key = iter.key();
			QString file = iter.value();
			if(moviePool()->insertMovie(file, file))
			{
				//�޸�����Ϊ������
				document()->addResource(QTextDocument::ImageResource, QUrl(file), moviePool()->currentImage(file));
			}
			else
			{
				QImage image(file);
				if(!image.isNull())
				{
					document()->addResource(QTextDocument::ImageResource, QUrl(file), image);
				}
			}
		}
	}

	int istart = position();
	insertHtml(text);
	int inow = position();

	if(!imgs.isEmpty() && inow > istart)
	{
		setPosition(istart);
		movePosition(NextCharacter, KeepAnchor, inow-istart);
		QString txt = selectedText();
		int index = txt.indexOf(QChar::ObjectReplacementCharacter, 0);
		while(index >= 0)
        {
			setPosition(istart+index);
			movePosition(NextCharacter, KeepAnchor, 1);
			QTextCharFormat fmt = charFormat();			
			QTextImageFormat imgFmt = fmt.toImageFormat();
			QString key = imgFmt.name();
			if(imgs.contains(key))
			{
				imgFmt.setProperty(KImageKey, key);
				imgFmt.setName(imgs.value(key));
				imgFmt.setProperty(KAnimationImage, true);
				setCharFormat(imgFmt);
			}
			int idx = index+1;
			index = txt.indexOf(QChar::ObjectReplacementCharacter, idx);
		}
	}

	setPosition(inow);
}

void QKxTextCursor::insertHtml( const QString &text, const QMap<QString, QImage>& imgs )
{
	if(!imgs.isEmpty())
	{
		for(QMap<QString,QImage>::const_iterator iter = imgs.begin(); iter != imgs.end(); iter++)
		{
			QString key = iter.key();
			QImage image = iter.value();
			if(!image.isNull())
			{
				document()->addResource(QTextDocument::ImageResource, QUrl(key), image);
			}
		}
	}
	
    QTextCursor::insertHtml(text);
}

void QKxTextCursor::insertHtml( const QString& text )
{
    QTextCursor::insertHtml(text);
}

void QKxTextCursor::insertFragment( const QByteArray& frag )
{
//	QList<QPair<QString,QTextCharFormat>> frags = KUtility::fragmentsFromByteArray(frag);
//	insertFragment(frags);
}

void QKxTextCursor::insertFragment( QList<QPair<QString,QTextCharFormat>>& frags )
{
	for(QList<QPair<QString,QTextCharFormat>>::iterator iter = frags.begin(); iter != frags.end(); iter++)
	{
		QPair<QString,QTextCharFormat>& item = *iter;
		QString text = item.first;
		QTextCharFormat fmt = item.second;
		if(text.at(0) == QChar::ObjectReplacementCharacter)
		{
			//ͼƬ
			QTextImageFormat imgFmt = fmt.toImageFormat();
			QString file = imgFmt.name();
			if(!file.isEmpty())
			{
				if(moviePool()->insertMovie(file, file))
				{
					fmt.setProperty(KAnimationImage, true);
					document()->addResource(QTextDocument::ImageResource, QUrl(file), moviePool()->currentImage(file));
				}
				else
				{
					QImage image(file);
					if(!image.isNull())
					{
						document()->addResource(QTextDocument::ImageResource, QUrl(file), image);
					}
				}
			}
		}
		insertText(text, fmt);
	}
}
