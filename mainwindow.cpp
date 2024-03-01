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
    createMenus();

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

    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    saveAsAct = new QAction(tr("Save As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save File As..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit"));
    connect(exitAct, &QAction::triggered, this, &QCoreApplication::quit, Qt::QueuedConnection);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}

bool MainWindow::promptSave()
{
    qDebug() << "Document has been modified, prompt to save";
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
