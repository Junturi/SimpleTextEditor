#include <QtWidgets>
#include "mainwindow.h"
#include "QLabel"

MainWindow::MainWindow()
    : textEdit(new QPlainTextEdit)
    , mHLayout(new QHBoxLayout)
    , widget(new QWidget)
{
    //mHLayout->addWidget(new QLabel("lollerds"));
    mHLayout->addWidget(textEdit);
    widget->setLayout(mHLayout);
    setCentralWidget(widget);

    createActions();

    setMinimumSize(160, 160);
    resize(1000, 800);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "Attempting to close the program";
    if (promptSave()){
        event->accept();
    }
    else {
        event->ignore();
    }

}

void MainWindow::newFile()
{
    qDebug() << "New File accessed";

    textEdit->clear();
    setFileName(QString());
    qDebug() << "File name " << mFileName;
}

void MainWindow::open()
{
    qDebug() << "Open file accessed";
    if (promptSave()){
        mFileName = QFileDialog::getOpenFileName(this);
        QFile file(mFileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "Unable to open the file";
            return;
        }
        QTextStream in(&file);
        textEdit->setPlainText(in.readAll());
        textEdit->document()->setModified(false);
        qDebug() << "File name " << mFileName << " opened";
    }
}

bool MainWindow::save()
{
    qDebug() << "Save file accessed";
    if (mFileName.isEmpty()){
        qDebug() << "Trying to save untitled file";
        saveAs();
    }
    else{
    saveFile(mFileName);
    }

    return true;
}

bool MainWindow::saveAs()
{
    qDebug() << "Save as accessed";
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;

    auto selectedFiles = dialog.selectedFiles();
    return saveFile(selectedFiles.first());
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);

    QAction *openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    QAction *saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);

    QAction *saveAsAct = new QAction(tr("Save As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save File As..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    QAction *exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit"));
    connect(exitAct, &QAction::triggered, this, &QCoreApplication::quit, Qt::QueuedConnection);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    QAction *cutAct = new QAction(tr("&Cut"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut"));
    connect(cutAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);

    QAction *copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy"));
    connect(copyAct, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);

    QAction *pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste"));
    connect(pasteAct, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD

}

bool MainWindow::promptSave()
{
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::setFileName(const QString &currentFile)
{
    qDebug() << "Setting the file name from " << mFileName << " to " << currentFile;
    mFileName = currentFile;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        textEdit->document()->setModified(false);
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                              .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

    setFileName(fileName);
    qDebug() << "Saved to file name " << fileName;
    return true;
}
