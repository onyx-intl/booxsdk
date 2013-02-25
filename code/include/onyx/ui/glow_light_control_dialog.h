#ifndef GLOW_LIGHT_CONTROL_DIALOG_H_
#define GLOW_LIGHT_CONTROL_DIALOG_H_

#include "onyx/ui/onyx_dialog.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/catalog_view.h"

namespace ui
{

class MoonLightProgressBar:public QWidget
{
    Q_OBJECT
public:
    MoonLightProgressBar(QWidget *parent=0);
    ~MoonLightProgressBar();

    void setRange(int min, int max);
    void setValue(int value);
    int  maximum(){return max_value_;}
    int  minimum(){return min_value_;}

public Q_SLOTS:
    void addValue();
    void subValue();

Q_SIGNALS:
    void valueChanged(int value);

protected:
    void changePoint(QPoint &pos);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    int max_value_;
    int min_value_;
    int value_;
    int step_value_;
};

/// Clock dialog.
class GlowLightControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GlowLightControlDialog(QWidget *parent = 0);
    ~GlowLightControlDialog(void);

    int exec();

public Q_SLOTS:
    void onValueChanged(int v);

protected:
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void keyReleaseEvent(QKeyEvent * event);

private Q_SLOTS:
    void onSwitchClicked(CatalogView *catalog, ContentView *item, int user_data);
    void onOkClicked();

private:
    void createLayout();
    void updateText();
    void createSwitchView();
    void createOKView();
    void createSubLightView();
    void createAddLightView();

private:
    QVBoxLayout layout_;
    QHBoxLayout slider_h_layout_;
    QHBoxLayout h_layout_;
    QHBoxLayout ok_h_layout_;
    OnyxLabel title_;
    MoonLightProgressBar slider_;

    CatalogView switch_view_;
    CatalogView ok_view_;
    CatalogView add_light_view_;
    CatalogView sub_light_view_;
};


}  // namespace ui


#endif  // GLOW_LIGHT_CONTROL_DIALOG_H_
