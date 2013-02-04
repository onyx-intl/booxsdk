#include "onyx/ui/reading_tools_actions.h"

namespace ui
{

ReadingToolsActions::ReadingToolsActions(void)
: BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/reading_tools.png")));
}

ReadingToolsActions::~ReadingToolsActions(void)
{
}

void ReadingToolsActions::generateActions(const vector<ReadingToolsType> & values, bool append)
{
    category()->setFont(actionFont());
    category()->setText(QCoreApplication::tr("Tools"));
    if ( !append )
    {
        actions_.clear();
    }
    else
    {
        // add separator
        shared_ptr<QAction> separator(new QAction(exclusiveGroup()));
        separator->setSeparator(true);
        actions_.push_back(separator);
    }

    vector<ReadingToolsType>::const_iterator begin = values.begin();
    vector<ReadingToolsType>::const_iterator end   = values.end();
    for(vector<ReadingToolsType>::const_iterator iter = begin;
        iter != end; ++iter)
    {
        //Check to disable TTS if need
        if (((*iter == TEXT_TO_SPEECH) && (qgetenv("DISABLE_TTS").toInt() > 0)) ||
            ((*iter == DICTIONARY_TOOL) && (qgetenv("DISABLE_DICT").toInt() > 0))
           )
        {
            continue;
        }

        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(exclusiveGroup()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);
        act->setFont(actionFont());
        switch (*iter)
        {
        case SEARCH_TOOL:
            {
                act->setText(QCoreApplication::tr("Search"));
                act->setIcon(QIcon(QPixmap(":/images/free_text_search.png")));
            }
            break;
        case RETRIEVE_WORD:
            {
                act->setText(QCoreApplication::tr("Mark"));
                act->setIcon(QIcon(QPixmap(":/images/retrieve_word.png")));
            }
            break;
        case TOC_VIEW_TOOL:
            {
                act->setText(QCoreApplication::tr("Table of Content"));
                act->setIcon(QIcon(QPixmap(":/images/table_of_content.png")));
            }
            break;
        case ANNOTATION_VIEW_TOOL:
            {
                act->setText(QCoreApplication::tr("Annotations View"));
                act->setIcon(QIcon(QPixmap(":/images/show_all_bookmarks.png")));
            }
            break;
        case NOTES_BACKGROUND_SELECTION:
            {
                act->setText(QCoreApplication::tr("Select Background"));
                act->setIcon(QIcon(QPixmap(":/images/background_selection.png")));
            }
            break;
        case EXPORT_SKETCH_DATA:
            {
                act->setText(QCoreApplication::tr("Export"));
                act->setIcon(QIcon(QPixmap(":/images/export.png")));
            }
            break;
        case INSERT_NOTE:
            {
                act->setText(QCoreApplication::tr("Insert Page"));
                act->setIcon(QIcon(QPixmap(":/images/insert_note.png")));
            }
            break;
        case REMOVE_NOTE:
            {
                act->setText(QCoreApplication::tr("Remove Page"));
                act->setIcon(QIcon(QPixmap(":/images/remove_note.png")));
            }
            break;
        case SLIDE_SHOW:
            {
                act->setText(QCoreApplication::tr("Slide Show"));
                act->setIcon(QIcon(QPixmap(":/images/slide_show.png")));
            }
            break;
        case DISPLAY_HYPERLINKS:
            {
                act->setText(QCoreApplication::tr("Show Hyperlinks"));
                act->setIcon(QIcon(QPixmap(":/images/show_hyperlinks.png")));
            }
            break;
        case DICTIONARY_TOOL:
            {
                act->setText(QCoreApplication::tr("Dictionary Lookup"));
                act->setIcon(QIcon(QPixmap(":/images/dictionary.png")));
            }
            break;
        case GOTO_PAGE:
            {
                act->setText(QCoreApplication::tr("Go To Page"));
                act->setIcon(QIcon(QPixmap(":/images/goto_page.png")));
            }
            break;
        case TEXT_TO_SPEECH:
            {
                act->setText(QCoreApplication::tr("Text To Speech"));
                act->setIcon(QIcon(QPixmap(":/images/tts.png")));
            }
            break;
        case SCROLL_PAGE:
            {
                act->setText(QCoreApplication::tr("Hand Tool"));
                act->setIcon(QIcon(QPixmap(":/images/scroll_page.png")));
            }
            break;
        case PREVIOUS_VIEW:
            {
                act->setText(QCoreApplication::tr("Previous View"));
                act->setIcon(QIcon(QPixmap(":/images/backward.png")));
            }
            break;
        case NEXT_VIEW:
            {
                act->setText(QCoreApplication::tr("Next View"));
                act->setIcon(QIcon(QPixmap(":/images/forward.png")));
            }
            break;
        case ADD_BOOKMARK:
            {
                act->setText(QCoreApplication::tr("Add Bookmark"));
                act->setIcon(QIcon(QPixmap(":/images/add_bookmark.png")));
            }
            break;
        case DELETE_BOOKMARK:
            {
                act->setText(QCoreApplication::tr("Delete Bookmark"));
                act->setIcon(QIcon(QPixmap(":/images/delete_bookmark.png")));
            }
            break;
        case SHOW_ALL_BOOKMARKS:
            {
                act->setText(QCoreApplication::tr("Show Bookmarks"));
                act->setIcon(QIcon(QPixmap(":/images/show_all_bookmarks.png")));
            }
            break;
        case SET_TOBE_BOOTSPLASH:
            {
                act->setText(QCoreApplication::tr("Set Boot Splash"));
                act->setIcon(QIcon(QPixmap(":/images/set_boot_splash.png")));
            }
            break;
        case SELECT_ALL:
            {
                act->setText(QCoreApplication::tr("Select All"));
                act->setIcon(QIcon(QPixmap(":/images/select_all.png")));
            }
            break;
        case SAVE_DOCUMENT:
            {
                act->setText(QCoreApplication::tr("Save Document"));
                act->setIcon(QIcon(QPixmap(":/images/save_document.png")));
            }
            break;
        case UNDO_TOOL:
            {
                act->setText(QCoreApplication::tr("Undo"));
                act->setIcon(QIcon(QPixmap(":/images/undo.png")));
            }
            break;
        case REDO_TOOL:
            {
                act->setText(QCoreApplication::tr("Redo"));
                act->setIcon(QIcon(QPixmap(":/images/redo.png")));
            }
            break;
        case SAVE_ACCOUNT:
            {
                act->setText(QCoreApplication::tr("Save Account"));
                act->setIcon(QIcon(QPixmap(":/images/save_account.png")));
            }
            break;
        case DISPLAY_ACCOUNT:
            {
                act->setText(QCoreApplication::tr("Display Account"));
                act->setIcon(QIcon(QPixmap(":/images/display_bookmark.png")));
            }
            break;
        case DELETE_ACCOUNT:
            {
                act->setText(QCoreApplication::tr("Delete Password"));
                act->setIcon(QIcon(QPixmap(":/images/delete_account.png")));
            }
            break;
        case CLEAR_COOKIES:
            {
                act->setText(QCoreApplication::tr("Clear Cookies"));
                act->setIcon(QIcon(QPixmap(":/images/clear_cookies.png")));
            }
            break;
        case AUTO_LOAD_IMAGE:
            {
                act->setText(QCoreApplication::tr("Auto-load Image"));
                act->setIcon(QIcon(QPixmap(":/images/auto_load_image.png")));
            }
            break;
        case COPY_CONTENT:
            {
                act->setText(QCoreApplication::tr("Copy"));
                act->setIcon(QIcon(QPixmap(":/images/copy.png")));
            }
            break;
        case PASTE_CONTENT:
            {
                act->setText(QCoreApplication::tr("Paste"));
                act->setIcon(QIcon(QPixmap(":/images/paste.png")));
            }
            break;
        case CLOCK_TOOL:
            {
                act->setText(QCoreApplication::tr("Clock"));
                act->setIcon(QIcon(QPixmap(":/images/clock_tool.png")));
            }
            break;
        case COMIC_MODE:
            {
                act->setText(QCoreApplication::tr("Comic Mode"));
                act->setIcon(QIcon(QPixmap(":/images/comic_mode.png")));
            }
            break;
        case IMAGE_EMBOLDEN_TOOL:
            {
                act->setText(QCoreApplication::tr("Glyph Embolden"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_shape_5.png")));
            }
            break;
        case READING_OPTIONS:
            {
                act->setText(QCoreApplication::tr("Reading Options"));
                act->setIcon(QIcon(QPixmap(":/images/reading_options.png")));
            }
            break;
        case MARGIN_SETTING:
            {
                act->setText(QCoreApplication::tr("Page Margin"));
                act->setIcon(QIcon(QPixmap(":/images/zoom_hide_margin.png")));
            }
            break;
        case KEY_MAPPING:
            {
                act->setText(QCoreApplication::tr("Key Mapping"));
                act->setIcon(QIcon(QPixmap(":/images/edit.png")));
            }
            break;
        case DISPLAY_HYPHENATION:
            {
                act->setText(QCoreApplication::tr("Show Hyphenation"));
                act->setIcon(QIcon(QPixmap(":/images/hyphenation.png")));
            }
            break;
        default:
            break;
        }
        actions_.push_back(act);
    }
}

void ReadingToolsActions::setActionStatus(const ReadingToolsType type,
                                          bool selected)
{
    assert(type > INVALID_TOOL && type < UNDEFINED_TOOL);

    QAction *act = 0;
    for (int i = 0; i < static_cast<int>(actions_.size()); ++i)
    {
        if (type == actions_[i]->data().toInt())
        {
            act = actions_[i].get();
            break;
        }
    }

    if (act == 0)
    {
        return;
    }

    // The text.
    QString text;
    switch (type)
    {
    case RETRIEVE_WORD:
        {
            selected ? text = QCoreApplication::tr("Turn Off Retrieve Word") :
                text = QCoreApplication::tr("Retrieve Word");
        }
        break;
    case SLIDE_SHOW:
        {
            selected ? text = QCoreApplication::tr("Turn Off Slide Show") :
                text = QCoreApplication::tr("Slide Show");
        }
        break;
    case COMIC_MODE:
        {
            selected ? text = QCoreApplication::tr("Exit Comic Mode") :
                text = QCoreApplication::tr("Comic Mode");
        }
        break;
    case DISPLAY_HYPERLINKS:
        {
            selected ? text = QCoreApplication::tr("Hide Hyperlinks") :
                text = QCoreApplication::tr("Show Hyperlinks");
        }
        break;
    case TEXT_TO_SPEECH:
        {
            selected ? text = QCoreApplication::tr("Start TTS") :
                text = QCoreApplication::tr("Stop TTS");
        }
        break;
    case SCROLL_PAGE:
        {
            text = QCoreApplication::tr("Hand Tool");
        }
        break;
    case SAVE_ACCOUNT:
        {
            selected ? text = QCoreApplication::tr("Don't Save Account") :
                text = QCoreApplication::tr("Save Account");
            selected ? act->setIcon(QIcon(QPixmap(":/images/don't_save_account.png"))) :
                act->setIcon(QIcon(QPixmap(":/images/save_account.png")));
        }
        break;
    case DISPLAY_ACCOUNT:
        {
            selected ? text = QCoreApplication::tr("Hide Account") :
                text = QCoreApplication::tr("Display Account");
            selected ? act->setIcon(QIcon(QPixmap(":/images/hide_account.png"))) :
                act->setIcon(QIcon(QPixmap(":/images/display_account.png")));
        }
        break;
    case AUTO_LOAD_IMAGE:
        {
            selected ? text = QCoreApplication::tr("Don't Auto-load Image") :
                text = QCoreApplication::tr("Auto-load Image");
            selected ? act->setIcon(QIcon(QPixmap(":/images/don't_auto_load_image.png"))) :
                act->setIcon(QIcon(QPixmap(":/images/auto_load_image.png")));
        }
        break;
    case DISPLAY_HYPHENATION:
        {
            selected ? text = QCoreApplication::tr("Hide Hyphenation") :
                text = QCoreApplication::tr("Show Hyphenation");
        }
        break;
    default:
        break;
    }

    act->setText(text);
    act->setChecked(selected);
}

QAction * ReadingToolsActions::action(const ReadingToolsType type)
{
    for (int i = 0; i < static_cast<int>(actions_.size()); ++i)
    {
        if (type == actions_[i]->data().toInt())
        {
            return actions_[i].get();
        }
    }
    return 0;
}


ReadingToolsType ReadingToolsActions::selectedTool()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return static_cast<ReadingToolsType>(act->data().toInt());
    }
    return INVALID_TOOL;
}

}   // namespace ui
