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
#include "qkxtextobject.h"

#include <QBuffer>
#include <QMimeData>
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QFileInfo>
#include <QImageReader>
#include <QScrollBar>
#include <QApplication>
#include <QMenu>
#include <QDebug>

//////////////////////////////////////////////////////////////////////////
// delay delete the gif from the gif pool. while delete the gif in edit.
const int KDeleteGifDelayTime = 1000;
const int KUpdateDelayTime = 100;
#define defTextObjectIndex	QTextFormat::ObjectIndex + 1000

QKxTextBrowser::QKxTextBrowser( QWidget *parent )
: QTextBrowser(parent)
, m_makeCtrlEnterAsEnter( true )
, m_maxLength( -1 )
, m_isAcceptEnter(true)
, m_keyEditFinish(Qt::Key_unknown)
, m_moidfierEditFinish(Qt::NoModifier)
, m_pasteFlags(All)
{
	init();

	connect( this, SIGNAL( anchorClicked(const QUrl& )), SLOT(on_link_clicked(const QUrl&)));

	setTabChangesFocus(false);
}

QKxTextBrowser::~QKxTextBrowser(void)
{
}

bool QKxTextBrowser::canInsertFromMimeData( const QMimeData *source ) const
{
	if(m_pasteFlags == NotSupport)
		return false;

	if(m_pasteFlags.testFlag(Custom))
	{
		QByteArray bytes = source->data(KTextEditMime);
		if(bytes.size() > 0)
			return true;
	}
	else if(m_pasteFlags.testFlag(Html))
	{
		if(source->hasHtml())
			return true;
	}
	else if(m_pasteFlags.testFlag(Image))
	{
		if(source->hasImage())
			return true;
	}
	else if(m_pasteFlags.testFlag(Url))
	{
		if(source->hasUrls())
			return true;
	}
	else if(m_pasteFlags.testFlag(Text))
	{
		if(source->hasText())
			return true;
	}
	return false;
}

QMimeData* QKxTextBrowser::createMimeDataFromSelection() const
{
	QMimeData *data = new QMimeData();

	QTextCursor txtCursor = textCursor();
	QTextDocumentFragment htmlFrag = txtCursor.selection();

	data->setHtml(htmlFrag.toHtml());
	data->setText(htmlFrag.toPlainText());

	QString html = htmlFrag.toHtml();

	QList<QPair<QString,QTextCharFormat>> frags;
	getSelectTextFragments(frags);
	QList<QByteArray> lstFrags;
	for(int i = 0; i < frags.count(); i++)
	{
		const QPair<QString,QTextCharFormat> &frag = frags.at(i);
		QString txt = frag.first;
		QTextCharFormat txtfmt = frag.second;
		QByteArray buf;
		QDataStream ds(&buf, QIODevice::WriteOnly);
		ds << txt << txtfmt;
		lstFrags.push_back(buf);
	}
	QByteArray fragsBuffer;
	QDataStream dataStream(&fragsBuffer, QIODevice::WriteOnly);
	dataStream << lstFrags;
	data->setData(KTextEditMime, fragsBuffer);
 	return data;
}

void QKxTextBrowser::insertFromMimeData( const QMimeData *source )
{
	Q_ASSERT(source);

	if(m_pasteFlags == NotSupport)
		return;

	QByteArray bytes = source->data(KTextEditMime);

    QKxTextCursor tc = textCursor();

	if(!bytes.isEmpty() && m_pasteFlags.testFlag(Custom))
	{
		QDataStream ds(bytes);
		QList<QByteArray> lstFrags;
		ds >> lstFrags;
		for(int i = 0; i < lstFrags.count(); i++)
		{
			const QByteArray frags = lstFrags.at(i);
			QDataStream ds(frags);
			QString text;
			QTextCharFormat format;
			ds >> text >> format;
            bool bHandled = false;
			emit insertMimiData(tc, text, format, bHandled);
            if(bHandled){
                continue;
            }
			if(text.at(0) == QChar::ObjectReplacementCharacter)
			{
				for(int i = 0; i < text.length(); i++)
				{
					QTextImageFormat fmt = format.toImageFormat();
					if(!fmt.isValid())
                    {
						continue;
					}
					QVariant vimgkey = fmt.property(KImageKey);
					QString imgkey = vimgkey.toString();
                    if(vimgkey.isValid()) {
                        tc.insertImage(imgkey, fmt);
                    }else{
                        tc.insertText(text, format);
                    }
				}
			}
			else
            {
				tc.insertText(text, format);
			}
		}
		return;
	}

	QString html = source->html();
	if(!html.isEmpty() && m_pasteFlags.testFlag(Html))
	{
        qDebug() << "html:" << html;
		tc.insertHtml(html);
		return;
	}

	QString txt = source->text();
	if(!txt.isEmpty() && m_pasteFlags.testFlag(Text))
	{
		tc.insertText(txt);
		return;
	}

	if(source->hasImage() && m_pasteFlags.testFlag(Image))
	{
		static int i = 1;
		QString key = QString("dropped_image_%1").arg(i++);
		tc.insertImage(key, qvariant_cast<QImage>(source->imageData()));
		return;
	}
	
	if(source->hasUrls() && m_pasteFlags.testFlag(Url))
	{
		foreach (QUrl url, source->urls())
		{
			QFileInfo info(url.toLocalFile());
			if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
			{
				tc.insertImage(url.toString(), info.filePath());
			}
			else
			{
				QFile file(url.toLocalFile());
				if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					tc.insertText(file.readAll());
				}
			}
		}
		return;
	}
	
	QTextEdit::insertFromMimeData(source);
}

void QKxTextBrowser::contextMenuEvent( QContextMenuEvent * event )
{
	Q_UNUSED(event);
	event->accept();
	emit contextMenu( event );
}

void QKxTextBrowser::scrollToEnd()
{
	QScrollBar *vBar = verticalScrollBar();
	if ( vBar != NULL )
	{
		vBar->setSliderPosition( vBar->maximum() );
	}
	else
    {
		scroll(0, 0x7fffffff);
	}
}

void QKxTextBrowser::scrollToTop()
{
	QScrollBar *vBar = verticalScrollBar();
	if ( vBar != NULL )
	{
		vBar->setSliderPosition( vBar->minimum() );
	}
	else
	{
		scroll(0, 0);
	}
}

void QKxTextBrowser::init()
{
	setWordWrapMode(QTextOption::WrapAnywhere);

	setOpenLinks(false);
	setOpenExternalLinks(true);

	setReadOnly(false);
	setTextInteractionFlags( Qt::TextBrowserInteraction | Qt::TextEditorInteraction );
}

void QKxTextBrowser::makeCtrlEnterAsEnter( bool makeCtrlEnterAsEnter )
{
	m_makeCtrlEnterAsEnter = makeCtrlEnterAsEnter;
}

void QKxTextBrowser::keyPressEvent( QKeyEvent *event )
{
	bool bHandled = false;
	emit keyEvent(event, &bHandled);
	if(bHandled)
		return;

	if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)		
	{
		if (isReadOnly())
		{
			event->ignore();
			return;
		}

		if (!m_isAcceptEnter)
		{
			event->ignore();
			return;
		}

		Qt::KeyboardModifiers modifier = event->modifiers();

		// set shift enter as enter default.
		if((modifier & Qt::ShiftModifier) == Qt::ShiftModifier)
		{
			event->setModifiers(Qt::NoModifier);
		}
		else if (( modifier & Qt::ControlModifier) == Qt::ControlModifier)
		{
			if (m_makeCtrlEnterAsEnter)
			{
				event->accept();
				emit returnPressed();
				return;
			}
			else
			{
				event->setModifiers(Qt::NoModifier);
			}
		}
		else
		{
			if ( !m_makeCtrlEnterAsEnter )
			{
				event->accept();
				emit returnPressed();

				return;
			}
			else
			{
				event->setModifiers(Qt::NoModifier);
			}
		}
	}
	QTextBrowser::keyPressEvent(event);
}

void QKxTextBrowser::on_link_clicked( const QUrl& url)
{
	QTextCursor tc = cursorFromPoint(m_linkPoint);
	QTextCharFormat format = tc.charFormat();
	
	if ( !format.isValid() )
	{
		return ;
	}

	QString linkValue = url.toString();

	if ( format.isAnchor() )
	{
		if (format.hasProperty( KLinkKey ) )
		{
			QString linkKey = format.property( KLinkKey ).toString();
			
			emit clickedLink(linkKey, linkValue);
		}
		else
		{
			emit htmlLinkClicked( linkValue );
		}
	}
}

void QKxTextBrowser::mousePressEvent( QMouseEvent *e )
{
    QTextBrowser::mousePressEvent(e);
	
	selectImageOnMousePress(e->pos());

	m_linkPoint = e->pos();

	bool bHandled = false;
	emit mouseEvent(e, &bHandled);
	if(bHandled)
		return;

	textObjectEvent(e);
}

void QKxTextBrowser::mouseMoveEvent( QMouseEvent *e )
{
    QTextBrowser::mouseMoveEvent(e);

	bool bHandled = false;
	emit mouseEvent(e, &bHandled);
	if(bHandled)
		return;

	textObjectEvent(e);
}

void QKxTextBrowser::mouseReleaseEvent( QMouseEvent *e )
{
    QTextBrowser::mouseReleaseEvent(e);
	
	bool bHandled = false;
	emit mouseEvent(e, &bHandled);
	if(bHandled)
		return;

	textObjectEvent(e);
}

void QKxTextBrowser::mouseDoubleClickEvent( QMouseEvent *e )
{
    QTextBrowser::mouseDoubleClickEvent(e);

	bool bHandled = false;
	emit mouseEvent(e, &bHandled);
	if(bHandled)
		return;

	textObjectEvent(e);	
}

void QKxTextBrowser::removeLinkInfo( const QString &linkKey, const QString &linkValue )
{
	QTextDocument *doc = document();
	QTextCursor tc(doc);
	
	QTextCharFormat format = tc.charFormat();

	while ( !( tc = doc->find(linkValue, tc.position()) ).isNull() )
	{
		QTextCharFormat format = tc.charFormat();
		
		if ( format.isAnchor() )
		{
			QString lkey = format.property(KLinkKey).toString();
			if(lkey == linkKey)
			{
				tc.setPosition(tc.position() + linkValue.size(), QTextCursor::KeepAnchor);
				tc.setCharFormat(format);				
				break;
			}
		}
	}
}

void QKxTextBrowser::setMaxLength( int nMaxLen )
{
	m_maxLength = nMaxLen;
}


void QKxTextBrowser::setReadOnly( bool isReadOnly )
{
	QTextEdit::setReadOnly( isReadOnly );

	if ( isReadOnly )
	{
		setTextInteractionFlags( Qt::TextBrowserInteraction );
	}
	else
	{
		setTextInteractionFlags( Qt::TextEditorInteraction | Qt::LinksAccessibleByMouse );
	}
}

void QKxTextBrowser::setAcceptEnter( bool accept )
{
	m_isAcceptEnter = accept;
}

bool QKxTextBrowser::on_edit_aboutToInsertText( QString& addText, const QTextCharFormat &format )
{
	Q_UNUSED(format);

	if ( m_maxLength == -1 )
	{
		return false;
	}

	int size = toPlainText().size();
	int remainSize = m_maxLength - size;

	if(remainSize >= addText.length())
	{
		return false;
	}

	if ( remainSize > 0 )
	{
		 if ( remainSize < addText.size() )
		 {
			 addText.remove( remainSize, addText.size() - remainSize );
		 }
	}
	else
	{
		addText.clear();
	}

	return true;
}

void QKxTextBrowser::textObjectEvent( QMouseEvent* event )
{
	QTextCursor tc = cursorFromPoint(event->pos());
	int textObjectIndex = tc.charFormat().property( defTextObjectIndex ).toUInt();	
	int textObjectType = tc.charFormat().objectType();

	if(textObjectIndex == 0 || textObjectType < QTextFormat::UserObject)
	{
		return;
	}

    QKxTextObject* item = static_cast<QKxTextObject*> ( document()->documentLayout()->handlerForObject( textObjectType ) );
	if ( item == NULL )
	{
		return;
	}

	item->commonEvent( event, textObjectIndex );
}

void QKxTextBrowser::selectImageOnMousePress( const QPoint &point)
{
	QTextCursor tc = textCursor();
	if ( tc.hasSelection() )
	{
		return;
	}

	int pos = positionFromPoint(point);
	if(pos == -1)
		return;

	QTextCursor cursor(document());
	cursor.setPosition(pos+1);
	
	QTextCharFormat format = cursor.charFormat();	
	if(!format.isImageFormat())
	{
		return;	
	}

	if(format.isAnchor())
	{
		return;
	}


	cursor.setPosition(pos);
	cursor.setPosition(pos+1, QTextCursor::KeepAnchor);
	setTextCursor(cursor);
}

QTextCharFormat QKxTextBrowser::exactCharFormat(int pos)
{
	QTextCursor cursor(document());
	cursor.setPosition(pos+1);

	return cursor.charFormat();	
}

QTextCharFormat QKxTextBrowser::exactCharFormat(const QPoint& point)
{
	int pos = positionFromPoint(point);
	return exactCharFormat(pos);
}


QTextCursor QKxTextBrowser::cursorFromPoint( const QPoint &point )
{
	int pos = positionFromPoint(point);

	QTextCursor tcExact = textCursor();
	QTextDocument *doc = document();

	tcExact.setPosition(pos+1);
	return tcExact;
}

bool QKxTextBrowser::isLinkObject(const QPoint &point)
{
	int pos = positionFromPoint(point);

	if(pos == -1)
		return false;

	QTextCursor tcExact = textCursor();
	QTextDocument *doc = document();

	tcExact.setPosition( pos+1 );
	QTextCharFormat charFormat = tcExact.charFormat();
	return charFormat.isAnchor();
}

int QKxTextBrowser::positionFromPoint( const QPoint &point )
{
	QPoint pt = mapToContents(point);
	QAbstractTextDocumentLayout *layout = document()->documentLayout();
	int index = layout->hitTest(pt, Qt::ExactHit);
	return index;
}

int QKxTextBrowser::exactPositionFromPoint( const QPoint& point )
{
	return positionFromPoint(point);
}

void QKxTextBrowser::getTextFragments( int ipos, int count, QList<QPair<QString,QTextCharFormat>>& frags ) const
{
	int istart = ipos;
	int iend = istart + count;
	QTextBlock blockStart = document()->findBlock(istart);
	QTextBlock blockEnd = document()->findBlock(iend);
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

			if(ipos + length <= istart)
				continue;
			if(ipos >= iend)
				return;

			if(ipos < istart && ipos + length > iend)
			{
				text = text.mid(istart-ipos, count);
			}
			else if(ipos < istart)
			{
				text = text.mid(istart-ipos);
			}
			else if(ipos + length > iend)
			{
				text = text.mid(0, iend-ipos);
			}
			frags.push_back(QPair<QString,QTextCharFormat>(text, frag.charFormat()));
		}
        block = block.next();
	}
}

void QKxTextBrowser::getTextFragments( QList<QPair<QString,QTextCharFormat>>& frags ) const
{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::Document);
	int istart = tc.selectionStart();
	int iend = tc.selectionEnd();
	
	getTextFragments(istart, iend-istart, frags);
}

void QKxTextBrowser::getSelectTextFragments( QList<QPair<QString,QTextCharFormat>>& frags ) const
{
	QTextCursor txtCursor = textCursor();
	int istart = txtCursor.selectionStart();
	int iend = txtCursor.selectionEnd();

	getTextFragments(istart, iend-istart, frags);
}

QPoint QKxTextBrowser::mapToContents( const QPoint &point ) const
{
	return QPoint(point.x() + horizontalOffset(), point.y() + verticalOffset());
}

QRect QKxTextBrowser::documentExposeRect() const
{
	return QRect(horizontalOffset(), verticalOffset(), width(), height());
}

int QKxTextBrowser::horizontalOffset() const
{
	QScrollBar* hbar = horizontalScrollBar();
	if ( hbar == NULL )
	{
		return 0;
	}

	return isRightToLeft() ? (hbar->maximum() - hbar->value()) : hbar->value(); 
}

int QKxTextBrowser::verticalOffset() const
{
	QScrollBar* vbar = verticalScrollBar();
	if ( vbar == NULL )
	{
		return 0;
	}
	return vbar->value(); 
}

void QKxTextBrowser::setEditingFinishKey( Qt::Key key/*=Qt::Key_Return*/, QFlags<Qt::KeyboardModifier> modifier/*=Qt::NoModifier*/ )
{
	m_keyEditFinish = key;
	m_moidfierEditFinish = modifier;
}

void QKxTextBrowser::setVerticalScrollBarStyleColor( const QColor& border, const QColor& normal, const QColor& hover, const QColor& pressed, int thinkness )
{
	QScrollBar *scroll = verticalScrollBar();
	QColor border1 = border.isValid() && border.alpha() > 0 ? border : normal;
	QColor border2 = border.isValid() && border.alpha() > 0 ? border : hover;
	QColor border3 = border.isValid() && border.alpha() > 0 ? border : pressed;
	QString barcss = QString("QScrollBar:vertical{border:1px;background:transparent;width:%1px}").arg(thinkness);
	QString handlecss = QString("QScrollBar::handle:vertical{background:%1;border:1px solid %2;border-radius:3px;min-height:20px;}").arg(normal.name()).arg(border1.name());
	QString hovercss = QString("QScrollBar::handle:vertical:hover{background:%1;border:1px solid %2;border-radius:3px;min-height:20px;}").arg(hover.name()).arg(border2.name());
	QString downcss = QString("QScrollBar::handle:vertical:pressed{background:%1;border:1px solid %2;border-radius:3px;min-height:20px;}").arg(pressed.name()).arg(border3.name());
	QString linecss = QString("QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{background: none;}");
	QString pagecss=QString("QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background: none;}");
	scroll->setStyleSheet(barcss+handlecss+hovercss+downcss+linecss+pagecss);
}

void QKxTextBrowser::setHorizontalScrollBarStyleColor( const QColor& border, const QColor& normal, const QColor& hover, const QColor& pressed, int thinkness )
{
	QScrollBar *scroll = horizontalScrollBar();
	QColor border1 = border.isValid() && border.alpha() > 0 ? border : normal;
	QColor border2 = border.isValid() && border.alpha() > 0 ? border : hover;
	QColor border3 = border.isValid() && border.alpha() > 0 ? border : pressed;
	QString barcss = QString("QScrollBar:horizontal{border:1px;background:transparent;height:%1px}").arg(thinkness);
	QString handlecss = QString("QScrollBar::handle:horizontal{background:%1;border:1px solid %2;border-radius:3px;min-width:20px;}").arg(normal.name()).arg(border1.name());
	QString hovercss = QString("QScrollBar::handle:horizontal:hover{background:%1;border:1px solid %2;border-radius:3px;min-width:20px;}").arg(hover.name()).arg(border2.name());
	QString downcss = QString("QScrollBar::handle:horizontal:pressed{background:%1;border:1px solid %2;border-radius:3px;min-width:20px;}").arg(pressed.name()).arg(border3.name());
	QString linecss = QString("QScrollBar::add-line:horizontal,QScrollBar::sub-line:horizontal{background: none;}");
	QString pagecss=QString("QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{background: none;}");
	scroll->setStyleSheet(barcss+handlecss+hovercss+downcss+linecss+pagecss);
}

QKxTextBrowser::EPasteFlags QKxTextBrowser::pasteFlags()
{
	return m_pasteFlags;
}

void QKxTextBrowser::setPasteFlags( EPasteFlags fg )
{
	m_pasteFlags = fg;
}

void QKxTextBrowser::inputMethodEvent( QInputMethodEvent *e )
{
	QList<QInputMethodEvent::Attribute> imeAttrs;
	QString commitString = e->commitString();
	QString preeditString = e->preeditString();
	int repStart = e->replacementStart();
	int repLength = e->replacementLength();

	for (int i = 0; i < e->attributes().size(); ++i) 
	{
		const QInputMethodEvent::Attribute &a = e->attributes().at(i);
		if (a.type == QInputMethodEvent::Cursor) 
		{
			//qDebug() << "astar:" << a.start  << "alength:" << a.length;			
		} 
		else
		{
			imeAttrs.push_back(a);
		}
	}
	QInputMethodEvent eNew(preeditString, imeAttrs);
	eNew.setCommitString(commitString, repStart, repLength);
	//qDebug() << "preedit:" << preeditString  << "commit:" << commitString << "start:" << repStart  << "length:" << repLength;
    QTextBrowser::inputMethodEvent(&eNew);
}

void QKxTextBrowser::focusInEvent( QFocusEvent *e )
{
    QTextBrowser::focusInEvent(e);
}

void QKxTextBrowser::focusOutEvent( QFocusEvent *e )
{
	QWidget *popup = QApplication::activePopupWidget();
	Qt::FocusReason reason = e->reason();
    if(reason == Qt::PopupFocusReason && qobject_cast<QMenu*>(popup)){
        return;
    }
    QTextBrowser::focusOutEvent(e);
}
