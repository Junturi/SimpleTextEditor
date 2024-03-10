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
    bool promptSave();
    void setFileName(const QString &fileName);
    bool saveFile(const QString &fileName);

    QString mFileName;

    QPlainTextEdit *textEdit;
    QHBoxLayout* mHLayout;
    QWidget* widget;
};
#endif // MAINWINDOW_H
