#ifdef BUILD_FOR_ARM
#include <QtGui/qscreen_qws.h>
#endif

#include "onyx/ui/system_actions.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys_conf.h"
#include "onyx/sys/platform.h"

namespace ui
{

SystemActions::SystemActions(void)
    : BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/system.png")));
}

SystemActions::~SystemActions(void)
{
}

void SystemActions::generateActions(const std::vector<int> & actions)
{
    category()->setFont(actionFont());
    category()->setText(QCoreApplication::tr("System"));
    actions_.clear();

    std::vector<int> all = actions;
    if (all.size() <= 0)
    {
        all.push_back(ROTATE_SCREEN);
        all.push_back(SCREEN_UPDATE_TYPE);
        all.push_back(MUSIC);

#ifdef BUILD_WITH_TFT
        all.push_back(BACKLIGHT_BRIGHTNESS);
#endif
        all.push_back(RETURN_TO_LIBRARY);
    }

    for(int i = 0; i < static_cast<int>(all.size()); ++i)
    {
        switch (all[i])
        {
        case ROTATE_SCREEN:
            {
                shared_ptr<QAction> rotate(new QAction(exclusiveGroup()));
                rotate->setCheckable(true);
                rotate->setFont(actionFont());
                rotate->setText(QCoreApplication::tr("Rotate Screen"));
                rotate->setIcon(QIcon(QPixmap(":/images/screen_rotation.png")));
                rotate->setData(ROTATE_SCREEN);
                actions_.push_back(rotate);
                break;
            }
        case SCREEN_UPDATE_TYPE:
            {
                // Screen update type.
                shared_ptr<QAction> screen(new QAction(exclusiveGroup()));
                screen->setCheckable(true);
                screen->setFont(actionFont());
                if (onyx::screen::instance().defaultWaveform() == onyx::screen::ScreenProxy::GC)
                {
                    screen->setText(QCoreApplication::tr("Full Refresh Off"));
                    screen->setIcon(QIcon(QPixmap(":/images/fast_update.png")));
                }
                else
                {
                    screen->setText(QCoreApplication::tr("Full Refresh On"));
                    screen->setIcon(QIcon(QPixmap(":/images/full_update.png")));
                }
                screen->setData(SCREEN_UPDATE_TYPE);
                actions_.push_back(screen);
                break;
            }
        case FULL_SCREEN:
            {
                shared_ptr<QAction> fullScreen(new QAction(exclusiveGroup()));
                fullScreen->setCheckable(true);
                fullScreen->setFont(actionFont());
                fullScreen->setText(QCoreApplication::tr("Full Screen"));
                fullScreen->setIcon(QIcon(QPixmap(":/images/full_screen.png")));
                fullScreen->setData(FULL_SCREEN);
                actions_.push_back(fullScreen);
                break;
            }
        case EXIT_FULL_SCREEN:
            {
                shared_ptr<QAction> exitFullScreen(new QAction(exclusiveGroup()));
                exitFullScreen->setCheckable(true);
                exitFullScreen->setFont(actionFont());
                exitFullScreen->setText(QCoreApplication::tr("Exit Full Screen"));
                exitFullScreen->setIcon(QIcon(QPixmap(
                        ":/images/exit_full_screen.png")));
                exitFullScreen->setData(EXIT_FULL_SCREEN);
                actions_.push_back(exitFullScreen);
                break;
            }
        case MUSIC:
            {
                if (sys::SystemConfig::isMusicPlayerAvailable())
                {
                    // Music.
                    shared_ptr<QAction> music(new QAction(exclusiveGroup()));
                    music->setCheckable(true);
                    music->setFont(actionFont());
                    music->setText(QCoreApplication::tr("Music"));
                    music->setIcon(QIcon(QPixmap(":/images/music.png")));
                    music->setData(MUSIC);
                    actions_.push_back(music);
                }
                break;
            }
        case SYSTEM_VOLUME:
            {
                // when music and tts both are disabled, do not show volume configure
                bool music_available = sys::SystemConfig::isMusicPlayerAvailable();
                bool disable_tts = qgetenv("DISABLE_TTS").toInt();
                if (!music_available && disable_tts)
                {
                    continue;
                }

                // system volume.
                shared_ptr<QAction> volume(new QAction(exclusiveGroup()));
                volume->setCheckable(true);
                volume->setFont(actionFont());
                volume->setText(QCoreApplication::tr("Volume"));
                volume->setIcon(QIcon(QPixmap(":/images/system_volume.png")));
                volume->setData(SYSTEM_VOLUME);
                actions_.push_back(volume);
                break;
            }
        case RETURN_TO_LIBRARY:
            {
                // Close document.
                shared_ptr<QAction> close(new QAction(exclusiveGroup()));
                close->setCheckable(true);
                close->setFont(actionFont());
                close->setText(QCoreApplication::tr("Close"));
                close->setIcon(QIcon(QPixmap(":/images/return_to_library.png")));
                close->setData(RETURN_TO_LIBRARY);
                actions_.push_back(close);
                break;
            }
        case BACKLIGHT_BRIGHTNESS:
            {
                shared_ptr<QAction> br(new QAction(exclusiveGroup()));
                br->setCheckable(true);
                br->setFont(actionFont());
                br->setText(QCoreApplication::tr("Brightness"));
                br->setIcon(QIcon(QPixmap(":/images/return_to_library.png")));
                br->setData(BACKLIGHT_BRIGHTNESS);
                actions_.push_back(br);
                break;
            }
        case GLOW_LIGHT_CONTROL:
            {
                if (!sys::hasGlowLight())
                {
                    continue;
                }

                shared_ptr<QAction> glow_light(new QAction(exclusiveGroup()));
                glow_light->setCheckable(true);
                glow_light->setFont(actionFont());
                glow_light->setText(QCoreApplication::tr("MOON Light Control"));
                glow_light->setIcon(QIcon(QPixmap(":/images/glow_light_switch.png")));
                glow_light->setData(GLOW_LIGHT_CONTROL);
                actions_.push_back(glow_light);
                break;
            }
        case SHOW_TASK_LIST:
            {
                shared_ptr<QAction> task(new QAction(exclusiveGroup()));
                task->setCheckable(true);
                task->setFont(actionFont());
                task->setText(QCoreApplication::tr("Task Management"));
                task->setIcon(QIcon(QPixmap(":/images/bookmark.png")));
                task->setData(SHOW_TASK_LIST);
                actions_.push_back(task);
                break;
            }
        }
    }
}

void SystemActions::addAboutAction()
{
    shared_ptr<QAction> about(new QAction(exclusiveGroup()));
    about->setCheckable(true);
    about->setFont(actionFont());
    about->setText(QCoreApplication::tr("About"));
    about->setIcon(QIcon(QPixmap(":/images/about.png")));
    about->setData(ABOUT_INFO);
    actions_.push_back(about);
}

SystemAction SystemActions::selected()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return static_cast<SystemAction>(act->data().toInt());
    }
    return INVALID_SYSTEM_ACTION;
}

}
