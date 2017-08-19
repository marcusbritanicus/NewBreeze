/***************************************************************************
 *   Copyright (C) 2012 by Glad Deschrijver                                *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include <QDebug>
#include "pageitem.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
#else
	#include <poppler-qt4.h>
#endif
#include <poppler-form.h>

bool PageItem::s_isLinkHovered;
Link PageItem::s_hoveredLink;

PageItem::PageItem(Poppler::Page *popplerPage, QObject *parent)
	: QObject(parent)
	, m_popplerPage(popplerPage)
{
	s_isLinkHovered = false;
}

PageItem::~PageItem()
{
	for (int i = 0; i < m_formFields.size(); ++i)
	{
		delete m_formFields.at(i).field;
	}
	qDeleteAll(m_formWidgets);
}

/*******************************************************************/
// Wrappers around Poppler::Page functions

QSizeF PageItem::pageSizeF() const
{
	return m_popplerPage->pageSizeF();
}

QString PageItem::label() const
{
	return m_popplerPage->label();
}

QImage PageItem::renderToImage(double xres, double yres, int x, int y, int w, int h, Poppler::Page::Rotation rotate) const
{
	return m_popplerPage->renderToImage(xres, yres, x, y, w, h, rotate);
}

QString PageItem::text(const QRectF &rect, Poppler::Page::TextLayout textLayout) const
{
	return m_popplerPage->text(rect, textLayout);
}

QString PageItem::text(const QRectF &rect) const
{
	return m_popplerPage->text(rect);
}

QList<Poppler::TextBox*> PageItem::textList(Poppler::Page::Rotation rotate) const
{
	return m_popplerPage->textList(rotate);
}

bool PageItem::search(const QString &text, double &rectLeft, double &rectTop, double &rectRight, double &rectBottom, Poppler::Page::SearchDirection direction, Poppler::Page::SearchMode caseSensitive, Poppler::Page::Rotation rotate) const
{
	return m_popplerPage->search(text, rectLeft, rectTop, rectRight, rectBottom, direction, caseSensitive, rotate);
}

/*******************************************************************/

/*
static QList<Link> getAnnotations(Poppler::Page *popplerPage)
{
	QList<Annotation> annotations;

	QList<Poppler::Annotation*> popplerAnnotations = popplerPage->annotations();
	annotations.reserve(popplerAnnotations.size());

	while (!popplerAnnotations.isEmpty())
	{
		Poppler::Annotation* popplerAnnotation = popplerAnnotations.takeFirst();
		Annotation annotation;
		switch (popplerAnnotation->subType())
		{
//			case Poppler::Annotation::AFileAttachment:
//			{
//				PopplerFileAttachmentAnnotation *attachAnnotation = static_cast<Poppler::FileAttachmentAnnotation*>(popplerAnnotation);
//				annotation.fileIconName = attachAnnotation->fileIconName();
//				annotation.embeddedFile = attachAnnotation->embeddedFile();
//			}
		}
		delete popplerAnnotation;
	}

	return annotations;
}

void Label::generateAnnotations(Poppler::Page *popplerPage)
{
	m_annotations.clear();
	m_annotations = getAnnotations(popplerPage);
}
*/

/*******************************************************************/
// Forms

static QList<FormField> getFormFields(Poppler::Page *popplerPage)
{
	QList<FormField> formFields;

//QTime t = QTime::currentTime();
	QList<Poppler::FormField*> popplerFormFields = popplerPage->formFields();
//qCritical() << t.msecsTo(QTime::currentTime());
	QSizeF popplerPageSizeF = popplerPage->pageSizeF();
	for (int i = 0; i < popplerFormFields.size(); ++i)
	{
		FormField formField;
		Poppler::FormField* popplerFormField = popplerFormFields.at(i);
		const QRectF popplerFormFieldRect = popplerFormField->rect();
		QRectF formFieldRect;
		formFieldRect.setX(popplerFormFieldRect.x() * popplerPageSizeF.width());
		formFieldRect.setY(popplerFormFieldRect.y() * popplerPageSizeF.height());
		formFieldRect.setWidth((popplerFormFieldRect.right() - popplerFormFieldRect.left()) * popplerPageSizeF.width());
		formFieldRect.setHeight((popplerFormFieldRect.bottom() - popplerFormFieldRect.top()) * popplerPageSizeF.height());
		formField.rect = formFieldRect;
//		if (popplerFormField->type() == Poppler::FormField::FormText)
//			formField.text = static_cast<Poppler::FormFieldText*>(popplerFormField)->text();
		formField.field = popplerFormField;
		formFields << formField;
	}

	return formFields;
}

static QList<QWidget*> getFormWidgets(QList<FormField> formFields, const QObject *receiver)
{
	QList<QWidget*> formWidgets;

	for (int i = 0; i < formFields.size(); ++i)
	{
		Poppler::FormField *formField = formFields.at(i).field;
		if (formField->type() == Poppler::FormField::FormText)
		{
			Poppler::FormFieldText *formFieldText = static_cast<Poppler::FormFieldText*>(formField);
			switch (formFieldText->textType())
			{
				case Poppler::FormFieldText::FileSelect:
					{
						// TODO replace this by a file selection widget
						QLineEdit *lineEdit = new QLineEdit;
						lineEdit->setText(formFieldText->text());
						lineEdit->setObjectName(QLatin1String("PageItem::formField") + QString::number(i));
						QObject::connect(lineEdit, SIGNAL(textEdited(QString)), receiver, SLOT(slotSetFormData(QString)));
						formWidgets << lineEdit;
					}
					break;
				case Poppler::FormFieldText::Multiline:
					{
						QTextEdit *textEdit = new QTextEdit;
						textEdit->setText(formFieldText->text());
						textEdit->setObjectName(QLatin1String("PageItem::formField") + QString::number(i));
						QObject::connect(textEdit, SIGNAL(textChanged()), receiver, SLOT(slotSetFormData()));
						formWidgets << textEdit;
					}
					break;
				case Poppler::FormFieldText::Normal:
				default:
					{
						QLineEdit *lineEdit = new QLineEdit;
						lineEdit->setText(formFieldText->text());
						lineEdit->setObjectName(QLatin1String("PageItem::formField") + QString::number(i));
						QObject::connect(lineEdit, SIGNAL(textEdited(QString)), receiver, SLOT(slotSetFormData(QString)));
						formWidgets << lineEdit;
					}
					break;
			}
		}
		else if (formField->type() == Poppler::FormField::FormButton)
		{
			Poppler::FormFieldButton *formFieldButton = static_cast<Poppler::FormFieldButton*>(formField);
			switch (formFieldButton->buttonType())
			{
				case Poppler::FormFieldButton::CheckBox:
					{
						QCheckBox *checkBox = new QCheckBox;
//						checkBox->setText(formFieldButton->caption());
						checkBox->setChecked(formFieldButton->state());
						checkBox->setObjectName(QLatin1String("PageItem::formField") + QString::number(i));
						QObject::connect(checkBox, SIGNAL(toggled(bool)), receiver, SLOT(slotSetFormData(bool)));
						formWidgets << checkBox;
					}
					break;
				case Poppler::FormFieldButton::Radio:
					{
						QRadioButton *radioButton = new QRadioButton;
						radioButton->setText(formFieldButton->caption());
						radioButton->setChecked(formFieldButton->state());
						radioButton->setObjectName(QLatin1String("PageItem::formField") + QString::number(i));
						QObject::connect(radioButton, SIGNAL(toggled(bool)), receiver, SLOT(slotSetFormData(bool)));
						formWidgets << radioButton;
					}
					break;
				case Poppler::FormFieldButton::Push:
				default:
					{
						QPushButton *pushButton = new QPushButton;
						pushButton->setText(formFieldButton->caption());
						pushButton->setChecked(formFieldButton->state());
						pushButton->setObjectName(QLatin1String("PageItem::formField") + QString::number(i));
						QObject::connect(pushButton, SIGNAL(toggled(bool)), receiver, SLOT(slotSetFormData(bool)));
						formWidgets << pushButton;
					}
					break;
			}
		}
		else if (formField->type() == Poppler::FormField::FormChoice)
		{
			Poppler::FormFieldChoice *formFieldChoice = static_cast<Poppler::FormFieldChoice*>(formField);
			switch (formFieldChoice->choiceType())
			{
				case Poppler::FormFieldChoice::ComboBox:
					{
						QComboBox *comboBox = new QComboBox;
						comboBox->addItems(formFieldChoice->choices());
						comboBox->setEditable(formFieldChoice->isEditable());
						comboBox->setCurrentIndex(formFieldChoice->currentChoices().at(0));
						comboBox->setObjectName(QLatin1String("PageItem::formField") + QString::number(i));
						QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), receiver, SLOT(slotSetFormData(int)));
						formWidgets << comboBox;
					}
					break;
				case Poppler::FormFieldChoice::ListBox:
				default:
					{
						QListWidget *listWidget = new QListWidget;
						listWidget->addItems(formFieldChoice->choices());
						if (formFieldChoice->isEditable())
						{
							for (int j = 0; j < listWidget->count(); ++j)
								listWidget->item(j)->setFlags(listWidget->item(j)->flags() | Qt::ItemIsEditable);
						}
						else
						{
							for (int j = 0; j < listWidget->count(); ++j)
								listWidget->item(j)->setFlags(listWidget->item(j)->flags() & ~Qt::ItemIsEditable);
						}
						QList<int> currentChoices = formFieldChoice->currentChoices();
						for (int j = 0; j < currentChoices.size(); ++j)
							listWidget->item(currentChoices.at(j))->setSelected(true);
						listWidget->setObjectName(QLatin1String("PageItem::formField") + QString::number(i));
						QObject::connect(listWidget, SIGNAL(itemSelectionChanged()), receiver, SLOT(slotSetFormData()));
						formWidgets << listWidget;
					}
					break;
			}
		}
		else if (formField->type() == Poppler::FormField::FormSignature)
		{
			// TODO
			formWidgets << new QWidget;
		}
		else // make sure that m_formWidgets.at(i) corresponds to m_formFields.at(i)
			formWidgets << new QWidget;
	}

	return formWidgets;
}

void PageItem::generateFormFields()
{
	m_formFields.clear();
	m_formFields = getFormFields(m_popplerPage);
	m_formWidgets = getFormWidgets(m_formFields, this);
}

QList<FormField> PageItem::formFields() const
{
	return m_formFields;
}

QList<QWidget*> PageItem::formWidgets() const
{
	return m_formWidgets;
}

void PageItem::slotSetFormData()
{
	QTextEdit *textEdit = qobject_cast<QTextEdit*>(sender());
	if (textEdit) // textEdit == 0 if the above cast fails
	{
		slotSetFormData(textEdit->toPlainText());
		return;
	}
	QListWidget *listWidget = qobject_cast<QListWidget*>(sender());
	if (listWidget)
	{
		QList<int> choices;
		for (int i = 0; i < listWidget->count(); ++i)
		{
			if (listWidget->item(i)->isSelected())
				choices << i;
		}
		QString objectName = sender()->objectName();
		if (!objectName.startsWith(QLatin1String("PageItem::formField")))
			return;
		int which = objectName.remove(QLatin1String("PageItem::formField")).toInt();
		Poppler::FormFieldChoice *formField = static_cast<Poppler::FormFieldChoice*>(m_formFields.at(which).field);
		formField->setCurrentChoices(choices);
		return;
	}
}

void PageItem::slotSetFormData(const QString &text)
{
	QString objectName = sender()->objectName();
	if (!objectName.startsWith(QLatin1String("PageItem::formField")))
		return;
	int which = objectName.remove(QLatin1String("PageItem::formField")).toInt();
	Poppler::FormFieldText *formField = static_cast<Poppler::FormFieldText*>(m_formFields.at(which).field);
	formField->setText(text);
}

void PageItem::slotSetFormData(bool checked)
{
	QString objectName = sender()->objectName();
	if (!objectName.startsWith(QLatin1String("PageItem::formField")))
		return;
	int which = objectName.remove(QLatin1String("PageItem::formField")).toInt();
	Poppler::FormFieldButton *formField = static_cast<Poppler::FormFieldButton*>(m_formFields.at(which).field);
	formField->setState(checked);
}

void PageItem::slotSetFormData(int index)
{
	QString objectName = sender()->objectName();
	if (!objectName.startsWith(QLatin1String("PageItem::formField")))
		return;
	int which = objectName.remove(QLatin1String("PageItem::formField")).toInt();
	Poppler::FormFieldChoice *formField = static_cast<Poppler::FormFieldChoice*>(m_formFields.at(which).field);
	formField->setCurrentChoices(QList<int>() << index);
}

/*******************************************************************/
// Links

static QList<Link> getLinks(Poppler::Page* popplerPage, const QStringList &popplerPageLabels)
{
	QList<Link> links;

//QTime t = QTime::currentTime();
	QList<Poppler::Link*> popplerLinks = popplerPage->links(); // this is slow
//qCritical() << t.msecsTo(QTime::currentTime());
	links.reserve(popplerLinks.size());
	while (!popplerLinks.isEmpty())
	{
		Poppler::Link *popplerLink = popplerLinks.takeFirst();
		Link link;
//		link.linkArea = popplerLink->linkArea();
		const QRectF linkArea = popplerLink->linkArea();
		link.linkArea = QRectF(linkArea.left(), qMin(linkArea.top(), linkArea.bottom()), qAbs(linkArea.right() - linkArea.left()), qAbs(linkArea.bottom() - linkArea.top())); // poppler switches top and bottom of this box :(
		switch (popplerLink->linkType())
		{
			case Poppler::Link::Goto:
			{
				const Poppler::LinkGoto *popplerLinkGoto = static_cast<const Poppler::LinkGoto*>(popplerLink);
				const Poppler::LinkDestination popplerDest = popplerLinkGoto->destination();
				link.pageNumber = popplerDest.pageNumber() - 1 + popplerDest.top();
				link.pageLabel = popplerPageLabels.at(int(link.pageNumber));
			}
			break;
			case Poppler::Link::Browse:
			{
				const Poppler::LinkBrowse *popplerLinkBrowse = static_cast<const Poppler::LinkBrowse*>(popplerLink);
				link.url = popplerLinkBrowse->url();
			}
			break;
			case Poppler::Link::Action:
			{
				const Poppler::LinkAction *popplerLinkAction = static_cast<const Poppler::LinkAction*>(popplerLink);
				link.pageNumber = -1; // since Poppler::LinkAction::ActionType doesn't specify a "None" value, we use pageNumber to distinguish this type of action, we do not check whether popplerLinkAction->actionType() is > 0 because it is not documented that all valid action types have a value > 0
				link.actionType = popplerLinkAction->actionType();
			}
			break;
			case Poppler::Link::Execute: // TODO
			case Poppler::Link::Sound: // TODO
			case Poppler::Link::Movie: // not implemented in poppler 0.16
			case Poppler::Link::JavaScript: // TODO
			default: // do nothing
			break;
		}
		links << link;
		delete popplerLink;
	}

	return links;
}

void PageItem::generateLinks(const QStringList &popplerPageLabels)
{
	m_links.clear();
	m_links = getLinks(m_popplerPage, popplerPageLabels);
}

QList<Link> PageItem::links() const
{
	return m_links;
}

void PageItem::findLinkAtPosition(const QPointF &pagePos)
{
	// rescale pagePos to [0,1]
	QPointF pos;
	const QSizeF popplerPageSizeF = m_popplerPage->pageSizeF();
	pos.setX(pagePos.x() / popplerPageSizeF.width());
	pos.setY(pagePos.y() / popplerPageSizeF.height());

	// search link that is hovered
	s_isLinkHovered = false;
	for (int i = 0; i < m_links.size(); ++i)
	{
		if (m_links.at(i).linkArea.contains(pos))
		{
			s_hoveredLink = m_links.at(i);
			s_isLinkHovered = true;
			break;
		}
	}
}

bool PageItem::isLinkHovered()
{
	return s_isLinkHovered;
}

Link PageItem::hoveredLink()
{
	return s_hoveredLink;
}

QString PageItem::toolTipText()
{
	if (!s_isLinkHovered)
		return QString();

	QString toolTipText;
	const Link link = s_hoveredLink;

	if (!link.url.isEmpty()) // we have a Browse link
		return QApplication::translate("PageItem", "Go to %1", "Link action text").arg(link.url);
	else if (link.pageNumber >= 0) // we have a Goto link
		return QApplication::translate("PageItem", "Go to page %1", "Link action text").arg(link.pageLabel);

	switch (link.actionType)
	{
		case Poppler::LinkAction::PageFirst:
			toolTipText = QApplication::translate("PageItem", "Go to first page", "Link action text");
			break;
		case Poppler::LinkAction::PagePrev:
			toolTipText = QApplication::translate("PageItem", "Go to previous page", "Link action text");
			break;
		case Poppler::LinkAction::PageNext:
			toolTipText = QApplication::translate("PageItem", "Go to next page", "Link action text");
			break;
		case Poppler::LinkAction::PageLast:
			toolTipText = QApplication::translate("PageItem", "Go to last page", "Link action text");
			break;
		case Poppler::LinkAction::HistoryBack:
			toolTipText = QApplication::translate("PageItem", "Backward", "Link action text");
			break;
		case Poppler::LinkAction::HistoryForward:
			toolTipText = QApplication::translate("PageItem", "Forward", "Link action text");
			break;
		case Poppler::LinkAction::Quit:
			toolTipText = QApplication::translate("PageItem", "Quit application", "Link action text");
			break;
		case Poppler::LinkAction::Presentation:
			toolTipText = QApplication::translate("PageItem", "Enter presentation mode", "Link action text");
			break;
		case Poppler::LinkAction::EndPresentation:
			toolTipText = QApplication::translate("PageItem", "Exit presentation mode", "Link action text");
			break;
		case Poppler::LinkAction::Find:
			toolTipText = QApplication::translate("PageItem", "Find...", "Link action text");
			break;
		case Poppler::LinkAction::GoToPage:
			toolTipText = QApplication::translate("PageItem", "Go to page...", "Link action text");
			break;
		case Poppler::LinkAction::Close:
			toolTipText = QApplication::translate("PageItem", "Close document", "Link action text");
			break;
		case Poppler::LinkAction::Print:
			toolTipText = QApplication::translate("PageItem", "Print...", "Link action text");
			break;
	}
	return toolTipText;
}
