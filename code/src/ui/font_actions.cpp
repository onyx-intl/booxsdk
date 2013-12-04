#include "onyx/ui/font_actions.h"
#include "onyx/ui/ui_utils.h"

namespace ui
{

static const int BOLD = 1;
static const int ITALIC = 2;

FontActions::FontActions(void)
{
    category()->setIcon(QIcon(QPixmap(":/images/font_size.png")));

    size_group_.reset(new QActionGroup(0));
    size_group_->setExclusive(true);

    style_group_.reset(new QActionGroup(0));
    style_group_->setExclusive(false);
}

FontActions::~FontActions(void)
{
    actions_.clear();
}

/// Generate all font actions according to the specified parameters.
void FontActions::generateActions(QFont  font,
                                      vector<int> & size,
                                      const int selected_size)
{
    category()->setFont(actionFont());
    category()->setText(QCoreApplication::tr("Font"));
    font_ = font;

    actions_.clear();

    if (size.empty())
    {
        generateDefaultFontSize(size);
    }

    const int MAX_DISPLAY_SIZE = 30;
    vector<int>::const_iterator begin = size.begin();
    vector<int>::const_iterator end   = size.end();
    for(vector<int>::const_iterator iter = begin; iter != end; ++iter)
    {
        // The text.
        QString text = QString("%1").arg(*iter);

        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(text, size_group_.get()));

        // Change font and make it as checkable.
        QFont qFont(font);

        if ((*iter > MAX_DISPLAY_SIZE))
        {
            qFont.setPointSize(MAX_DISPLAY_SIZE);
        }
        else
        {
            qFont.setPointSize(*iter);
        }
        act->setFont(qFont);
        act->setCheckable(true);
        act->setData(*iter);
        act->setIcon(QIcon(QPixmap(":/images/font_size_item.png")));

        if (*iter == selected_size)
        {
            act->setChecked(true);
        }

        actions_.push_back(act);
    }

    shared_ptr<QAction> separator(new QAction(size_group_.get()));
    separator->setSeparator(true);
    actions_.push_back(separator);

    // Bold and italic.
    QString text(QCoreApplication::tr("Bold"));
    shared_ptr<QAction> bold(new QAction(text, style_group_.get()));
    QFont fb = bold->font();
    fb.setPointSize(13);
    fb.setBold(true);
    bold->setFont(fb);
    bold->setCheckable(true);
    bold->setChecked(font_.bold());
    bold->setData(BOLD);
    bold->setIcon(QIcon(QPixmap(":/images/font_bold.png")));
    actions_.push_back(bold);

    QString title(QCoreApplication::tr("Italic"));
    shared_ptr<QAction> italic(new QAction(title, style_group_.get()));
    QFont fi = italic->font();
    fi.setPointSize(13);
    fi.setItalic(true);
    italic->setFont(fi);
    italic->setCheckable(true);
    italic->setChecked(font_.italic());
    italic->setData(ITALIC);
    italic->setIcon(QIcon(QPixmap(":/images/font_italic.png")));
    actions_.push_back(italic);
}

QFont FontActions::selectedFont()
{
   // Search for the changed actions.
    QAction * act = size_group_->checkedAction();
    if (act)
    {
        font_.setPointSize(act->data().toInt());
    }

    QList<QAction *> all = style_group_->actions();
    foreach(QAction * action, all)
    {
        if (action->data().toInt() == BOLD)
        {
            font_.setBold(action->isChecked());
        }
        if (action->data().toInt() == ITALIC)
        {
            font_.setItalic(action->isChecked());
        }
    }
    return font_;
}

void FontActions::generateDefaultFontSize(vector<int> & size)
{
    size.clear();
    if (!ui::isHD())
    {
        size.push_back(12);
        size.push_back(14);
        size.push_back(16);
        size.push_back(18);
    }

    size.push_back(20);
    size.push_back(22);
    size.push_back(24);
    size.push_back(26);
    size.push_back(28);
    size.push_back(30);
    size.push_back(32);
    size.push_back(34);
    size.push_back(36);
    size.push_back(38);
    size.push_back(40);

    if (ui::isHD())
    {
        size.push_back(44);
        size.push_back(50);
        size.push_back(60);
        size.push_back(80);
    }
}

static const qreal DELTA = 0.01;
TextSizeActions::TextSizeActions(void)
    : BaseActions()
    , multiplier_(1.0)
{
    category()->setIcon(QIcon(QPixmap(":/images/font_size.png")));
}

TextSizeActions::~TextSizeActions(void)
{
}

void TextSizeActions::generateActions(vector<qreal> & size,
                                      const qreal selected_multiplier)
{
    category()->setFont(actionFont());
    category()->setText(QCoreApplication::tr("Font"));
    actions_.clear();

    if (size.empty())
    {
        generateDefaultActions(size);
    }

    vector<qreal>::const_iterator begin = size.begin();
    vector<qreal>::const_iterator end   = size.end();
    for(vector<qreal>::const_iterator iter = begin; iter != end; ++iter)
    {
        // The text.
        QString text = QString("%1%").arg(*iter * 100);
        if(CUSTOM_TEXT_SIZE == *iter)
        {
            text = QCoreApplication::tr("Input Custom");
        }

        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(text, exclusiveGroup()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setFont(actionFont());
        act->setData(*iter);
        act->setIcon(QIcon(QPixmap(":/images/font_size_item.png")));

        if (qAbs(*iter - selected_multiplier) <= DELTA)
        {
            act->setChecked(true);
        }

        actions_.push_back(act);
    }

    multiplier_ = selected_multiplier;
}

qreal TextSizeActions::selectedMultiplier()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    multiplier_ = 1.0;
    if (act)
    {
        bool ok = false;
        multiplier_ = act->data().toDouble(&ok);
        if (ok)
        {
            return multiplier_;
        }
    }
    return multiplier_;
}

void TextSizeActions::generateDefaultActions(vector<qreal> & size)
{
    size.clear();
    size.push_back(0.25);
    size.push_back(0.5);
    size.push_back(0.75);
    size.push_back(1.0);
    size.push_back(1.25);
    size.push_back(1.50);
    size.push_back(1.75);
    size.push_back(2.0);
    size.push_back(2.25);
    size.push_back(2.50);
    size.push_back(2.75);
    size.push_back(3.0);
    size.push_back(3.25);
    size.push_back(3.50);
    size.push_back(3.75);
    size.push_back(4.0);
}

FontIndexActions::FontIndexActions(void)
    : BaseActions()
    , index_(0)
{
    category()->setIcon(QIcon(QPixmap(":/images/font_size.png")));
}

FontIndexActions::~FontIndexActions(void)
{
}

void FontIndexActions::generateActions( vector<int> & indexes,
                                        const int current )
{
    category()->setFont(actionFont());
    category()->setText(QCoreApplication::tr("Font"));
    actions_.clear();

    if (indexes.empty())
    {
        generateDefaultActions( indexes );
    }

    vector<int>::const_iterator begin = indexes.begin();
    vector<int>::const_iterator end   = indexes.end();
    for(vector<int>::const_iterator iter = begin; iter != end; ++iter)
    {
        // The text
        QString text;
        switch ( *iter )
        {
        case 0:
            text = QCoreApplication::tr("Small");
            break;
        case 1:
            text = QCoreApplication::tr("Medium");
            break;
        case 2:
            text = QCoreApplication::tr("Large");
            break;
        case 3:
            text = QCoreApplication::tr("Larger");
            break;
        case 4:
            text = QCoreApplication::tr("Largest");
            break;
        default:
            text = QCoreApplication::tr("Huge");
            break;
        }

        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(text, exclusiveGroup()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);
        act->setIcon(QIcon(QPixmap(":/images/font_size_item.png")));

        if ( *iter == current )
        {
            act->setChecked(true);
        }

        actions_.push_back(act);
    }

    index_ = current;
}

void FontIndexActions::setCurrentIndex( const int current )
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ( (*iter)->data().toInt() == current )
        {
            (*iter)->setChecked( true );
            return;
        }
    }
}

int FontIndexActions::selectedIndex()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        bool ok = false;
        index_ = act->data().toInt(&ok);
    }
    return index_;
}

void FontIndexActions::generateDefaultActions(vector<int> & indexes)
{
    indexes.clear();
    indexes.push_back(0);
    indexes.push_back(1);
    indexes.push_back(2);
    indexes.push_back(3);
    indexes.push_back(4);
}

}   // namespace ui
