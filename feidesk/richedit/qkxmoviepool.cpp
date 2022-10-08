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

#include "qkxmoviepool.h"
#include "qkxtextcursor.h"
#include "qkxtextbrowser.h"

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPointer>
#include <QMovie>
#include <QScrollBar>
#include <QTextBlock>

#define MAX_KEEP_RESOURCE_COUNT		(10)
#define MOVIE_UPDATE_INTERVAL		(30)

class QKxMoviePoolPrivate
{
    Q_DECLARE_PUBLIC(QKxMoviePool)
public:
    QKxMoviePool *q_ptr;
	QPointer<QTextDocument> m_pDocument;
	QBasicTimer m_timer;

	struct MovieData 
	{
		MovieData()
		{
			countNoUsed = 0;
			nextUpdateTime = 0;
		}
		QPointer<QMovie> ani;
		int countNoUsed;
		int nextUpdateTime;
	};
	QMap<QString, MovieData> m_movies;
	int m_idxupdate;
    QPointer<QKxTextBrowser> m_pTextBrowser;
	int m_vbarpos;
	QSizeF m_docSize;
	int m_movieUpdateInterval;
	int m_docLastVerticalOffset;
public:
    QKxMoviePoolPrivate(QTextDocument *doc)
		: m_pDocument(doc)
	{
		QAbstractTextDocumentLayout *docLayout = m_pDocument->documentLayout();
		const QMetaObject *mo = docLayout->metaObject();
		m_idxupdate = mo->indexOfMethod("update(QRectF)");
		QVariant vtext = m_pDocument->property("edit");
        m_pTextBrowser = qobject_cast<QKxTextBrowser*>(vtext.value<QObject*>());
		m_movieUpdateInterval = MOVIE_UPDATE_INTERVAL;
		m_docLastVerticalOffset = 0;
	}

    ~QKxMoviePoolPrivate()
	{
		for(QMap<QString, MovieData>::iterator iter = m_movies.begin(); iter != m_movies.end(); iter++)
		{
			MovieData &md = *iter;
			delete md.ani;
		}
	}

	bool insertMovie(const QString &imageKey, const QString &filePath, bool bForceReplace=false)
	{
        Q_Q(QKxMoviePool);

		if(m_idxupdate < 0)
		{
			return false;
		}

		if(m_movies.contains(imageKey))
		{
			if(bForceReplace)
			{
				MovieData& md = m_movies[imageKey];
				delete md.ani;
				md.ani = new QMovie(filePath);
				md.countNoUsed = 0;
				if(!m_timer.isActive())
				{
					m_timer.start(MOVIE_UPDATE_INTERVAL, q);
				}
			}
			return true;
		}

		QMovie *movie = new QMovie(filePath);
		if(filePath.endsWith(".mng", Qt::CaseInsensitive) || movie->frameCount() > 1)
		{
			MovieData md;
			md.ani = movie;
			md.countNoUsed = 0;
			m_movies.insert(imageKey, md);
			if(!m_timer.isActive())
			{
				m_timer.start(MOVIE_UPDATE_INTERVAL, q);
			}
			return true;
		}
		delete movie;
		return false;
	}

	bool contain(const QString &imageKey)
	{
		return m_movies.contains(imageKey);
	}

	QImage currentImage(const QString &imageKey) const
	{
		MovieData md = m_movies.value(imageKey);
		if(md.ani)
		{
			return md.ani->currentImage();
		}
		return QImage();
	}

	void timerEvent(QTimerEvent *e)
	{
		/**/
		if(m_movies.isEmpty())
		{
			m_timer.stop();
			return;
		}

		QList<QString> imgKeys;
		QList<QRectF> updateRts;
		QList<QString> imgDecodeKeys;

		QAbstractTextDocumentLayout *docLayout = m_pDocument->documentLayout();
		QSizeF docSize = docLayout->documentSize();
		QRectF docExposeRt = QRectF(0, 0, docSize.width(), docSize.height());
		if(m_pTextBrowser)
		{
			 QScrollBar *vbar = m_pTextBrowser->verticalScrollBar();
			 if(vbar)
			 {
				 int vpos = vbar->value();
				 docExposeRt = m_pTextBrowser->rect();
				 docExposeRt.translate(0, vpos);

				 if(vpos == m_docLastVerticalOffset)
				 {
					 m_movieUpdateInterval = MOVIE_UPDATE_INTERVAL;
				 }
				 else
				 {
					 m_movieUpdateInterval = MOVIE_UPDATE_INTERVAL / 10;
				 }

				 m_docLastVerticalOffset = vpos;
			 }
		}

		QTextBlock blockStart = m_pDocument->firstBlock();
		QTextBlock blockEnd = m_pDocument->lastBlock();
		int iblockstart = blockStart.blockNumber();
		int iblockend = blockEnd.blockNumber();
		QTextBlock block = blockStart;
		while(block.isValid() && block.blockNumber() <= iblockend)
		{
			for(QTextBlock::iterator iter = block.begin(); !(iter.atEnd()); iter++)
			{
				QTextFragment frag = iter.fragment();
				int ipos = frag.position();
				int length = frag.length();
				QString text = frag.text();
				if(text.at(0) == QChar::ObjectReplacementCharacter)
				{
					QTextCharFormat fmt = frag.charFormat();
					QVariant vbani = fmt.property(KAnimationImage);
					if(!vbani.isValid())
						continue;
					QTextImageFormat imgfmt = fmt.toImageFormat();
					QString key = imgfmt.name();
					if(!imgKeys.contains(key))
					{
						imgKeys.push_back(key);
					}

					QMap<QString, MovieData>::iterator iter = m_movies.find(key);					
					if(iter != m_movies.end())
					{
						MovieData &md = iter.value();
						if(md.nextUpdateTime <= 0)
						{
							QRectF brt = docLayout->blockBoundingRect( block );
							if(!updateRts.contains(brt) && docExposeRt.intersects(brt))
							{
								updateRts.push_back(brt);
								imgDecodeKeys.push_back(key);
							}
						}
					}

				}
			}
			block = block.next();
		}

		for(QList<QRectF>::iterator iter = updateRts.begin(); iter != updateRts.end(); iter++)
		{
			QRectF brt = *iter;
			void *param[] = {0, &brt};
			QMetaObject::metacall(docLayout, QMetaObject::InvokeMetaMethod, m_idxupdate, (void**)&param);
			//QMetaObject::invokeMethod(docLayout, "update", Q_ARG(QRectF, brt));
			//QRectF tmpbrt[] = {brt};
			//QMetaObject::metacall(docLayout, QMetaObject::InvokeMetaMethod, 0, &tmpbrt);
			//docLayout->update(brt);
		}

		for(QMap<QString,MovieData>::iterator iter = m_movies.begin(); iter != m_movies.end();)
		{
			MovieData &md = iter.value();
			QString key = iter.key();
			if(imgKeys.contains(key))
			{
				if(md.nextUpdateTime <= 0)
				{
					if(imgDecodeKeys.contains(key))
					{
						md.ani->jumpToNextFrame();
						QImage img = md.ani->currentImage();
						m_pDocument->addResource(QTextDocument::ImageResource, key, img);
					}
					md.nextUpdateTime = md.ani->nextFrameDelay();
				}
				else
				{
					md.nextUpdateTime -= m_movieUpdateInterval;
				}
				md.countNoUsed = 0;
				iter++;
			}
			else
			{
				md.countNoUsed++;
				md.nextUpdateTime = 0;
				if(md.countNoUsed > MAX_KEEP_RESOURCE_COUNT)
				{
					delete md.ani;
					iter = m_movies.erase(iter);
				}
				else
				{
					iter++;
				}
			}
		}
	}
};

QKxMoviePool::QKxMoviePool( QTextDocument *doc )
: QObject(doc)
, d_ptr(new QKxMoviePoolPrivate(doc))
{
	d_ptr->q_ptr = this;
}

QKxMoviePool::~QKxMoviePool()
{
	delete d_ptr;
}

bool QKxMoviePool::insertMovie( const QString &imageKey, const QString &filePath, bool bForceReplace)
{
    Q_D(QKxMoviePool);
	return d->insertMovie(imageKey, filePath, bForceReplace);
}

bool QKxMoviePool::contain( const QString &imageKey )
{
    Q_D(QKxMoviePool);
	return d->contain(imageKey);
}

void QKxMoviePool::timerEvent( QTimerEvent *e )
{
    Q_D(QKxMoviePool);
	d->timerEvent(e);
}

QImage QKxMoviePool::currentImage( const QString &imageKey ) const
{
    Q_D(const QKxMoviePool);
	return d->currentImage(imageKey);
}
