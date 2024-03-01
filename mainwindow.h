#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QHBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();

private:
    void createActions();
    void createMenus();
    bool promptSave();
    void setFileName(const QString &fileName);
    bool saveFile(const QString &fileName);

    QMenu *fileMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

    QString mFileName;

    QPlainTextEdit *textEdit;
    QHBoxLayout* mHLayout;
    QWidget* widget;
};
#endif // MAINWINDOW_H
