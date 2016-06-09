#ifndef TOPBAR_H
#define TOPBAR_H

#include <QMainWindow>

namespace Ui {
class TopBar;
}

class QActionGroup;
class QAction;
class QMenu;
class QSystemTrayIcon;

class LayoutViewer;
class AboutDialog;
class AboutFile;
class SettingsDialog;

class TopBar : public QMainWindow
{
    Q_OBJECT

public:
    explicit TopBar(QWidget *parent = 0);
    ~TopBar();

protected:
    void closeEvent(QCloseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void layoutMenuLayouts_clicked();
    void layoutMenuInstall_clicked();
    void settingsMenuFixedLayoutAutoVForm_clicked();
    void settingsMenuFixedLayoutAutoChandra_clicked();
    void settingsMenuFixedLayoutOldReph_clicked();
    void settingsMenuFixedLayoutTraditionalKar_clicked();
    void settingsMenuFixedLayoutNumberPad_clicked();
    void settingsMenuShowDialog_clicked();
    void aboutMenuLayout_clicked();
    void aboutMenuAbout_clicked();
    void quitMenuOnTray_clicked();
    void quitMenuQuit_clicked();

    void trayMenuRestore_clicked();

    void on_buttonAbout_clicked();

    void on_buttonSetLayout_clicked();

    void on_buttonShutdown_clicked();

    void on_buttonViewLayout_clicked();

    void on_buttonSettings_clicked();

private:
    Ui::TopBar *ui;
    bool canMoveTopbar;
    int pressedMouseX, pressedMouseY;
    QSystemTrayIcon *tray;

    /* Dialogs */
    AboutDialog *aboutDialog;
    LayoutViewer *layoutViewer;
    SettingsDialog *settingsDialog;

    /* Layout Popup Menu */
    QMenu *layoutMenu;
    enum { MaxLayoutFiles = 10 };
    QAction *layoutMenuLayouts[MaxLayoutFiles];
    QActionGroup *layoutMenuLayoutsGroup;
    QAction *layoutMenuInstall;
    /* Settings Popup Menu */
    QMenu *settingsMenu;
    QAction *settingsMenuShowDialog;
    QMenu *settingsMenuFixedLayout;
    QAction *settingsMenuFixedLayoutAutoVForm;
    QAction *settingsMenuFixedLayoutAutoChandra;
    QAction *settingsMenuFixedLayoutOldReph;
    QAction *settingsMenuFixedLayoutTraditionalKar;
    QAction *settingsMenuFixedLayoutNumberPad;
    /* About Popup Menu */
    QMenu *aboutMenu;
    QAction *aboutMenuLayout;
    QAction *aboutMenuAbout;
    /* Quit Popup Menu */
    QMenu *quitMenu;
    QAction *quitMenuOnTray;
    QAction *quitMenuQuit;

    /* Tray Popup Menu */
    QMenu *trayMenu;
    QAction *trayMenuRestore;

    void SetupTopBar();
    void SetupPopupMenus();
    void SetupTrayIcon();

    void RefreshLayouts();
};

#endif // TOPBAR_H
