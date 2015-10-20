#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

#include <QDirIterator>
#include <QDir>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QSettings>
#include <QInputDialog>
#include <QStringList>

#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->groupFileTableView, &GroupFileTableView::dropFinished, this, &MainWindow::onDropFinished);


    QSettings setting("aoyou", "QresDepot");
    QString path = setting.value("resPath").toString();
    if (!path.isEmpty()) {
        initHashFile(path);
    }

    //init group list
    this->groupListModel = new QStringListModel(this);
    ui->groupListView->setModel(this->groupListModel);

    //init group files
    this->groupFilesModel = new QStandardItemModel(0, 2, this);
    this->groupFilesModel->setHeaderData(0, Qt::Horizontal, QObject::tr("key"));
    this->groupFilesModel->setHeaderData(1, Qt::Horizontal, QObject::tr("path"));

    this->groupFilesProxyModel = new QSortFilterProxyModel(this);
    this->groupFilesProxyModel->setSourceModel(this->groupFilesModel);
    ui->groupFileTableView->setModel(this->groupFilesProxyModel);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initModelWithHashFile() {
    this->resFilesModel = new QStandardItemModel(this->hashFile.size(), 2, this);
    this->resFilesModel->setHeaderData(0, Qt::Horizontal, QObject::tr("key"));
    this->resFilesModel->setHeaderData(1, Qt::Horizontal, QObject::tr("path"));

    this->resFilesProxyModel = new QSortFilterProxyModel(this);
    this->resFilesProxyModel->setSourceModel(this->resFilesModel);


    //this->resFilesModel->setRowCount(this->hashFile.size());
    HashFile::Iterator it = this->hashFile.begin();
    int row = 0;
    while(it != this->hashFile.end()) {
        this->resFilesModel->setData(this->resFilesModel->index(row, 0), it.key());
        this->resFilesModel->setData(this->resFilesModel->index(row,1), it.value());
        row++;
        it++;
    }

    ui->resFileTableView->setModel(this->resFilesProxyModel);

}

void MainWindow::initHashFile(QString path)
{
    QDir dir(path);
    dir.setFilter(QDir::Files);
    QDirIterator it(dir, QDirIterator::Subdirectories);

    QString baseName;
    QString filePath;
    while(it.hasNext()) {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        filePath = fileInfo.canonicalFilePath();
        filePath = filePath.right(filePath.length() - path.length() - 1);

        baseName = fileInfo.fileName();
        int index = baseName.lastIndexOf(".");
        if (index != -1) {
            baseName = baseName.replace(index, 1, "_");
        }
        hashFile[baseName.toUpper()] = filePath;
    }

    initModelWithHashFile();
}

void MainWindow::openFile() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    qDebug() << path;
    if (path.isEmpty()) {
        path = ".";
    }

    initHashFile(path);

    QSettings setting("aoyou", "QresDepot");
    setting.setValue("resPath", path);
}


void MainWindow::on_pushButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("In put a group name"),
                                         tr("Name"),
                                         QLineEdit::Normal,
                                         "test",
                                         &ok
                                         );
    if (!ok || text.isEmpty()) {
        return;
    }

    QStringList list = this->groupListModel->stringList();
    if (list.indexOf(text) != -1) {
        return;
    }

    this->groupListModel->insertRows(0,1);
    this->groupListModel->setData(this->groupListModel->index(0), text);
    groupHashFile[text] = QStringList();
}

void MainWindow::on_groupListView_clicked(const QModelIndex &index)
{
    //this->groupListModel->
    QString groupName = index.data().toString();
    QStringList& groupList = groupHashFile[groupName];
    this->groupFilesModel->removeRows(0, this->groupFilesModel->rowCount());

    //HashFile groupHashFile;
    foreach (QString key, groupList) {
        QString path = this->hashFile.value(key, "---");
        this->groupFilesModel->insertRow(0);
        this->groupFilesModel->setData(this->groupFilesModel->index(0, 0), key);
        this->groupFilesModel->setData(this->groupFilesModel->index(0, 1), path);
    }

    ui->groupFileTableView->setGroupName(groupName);
}

void MainWindow::onDropFinished()
{
    QString groupName = ui->groupListView->currentIndex().data().toString();

    GroupHashFile::Iterator it = groupHashFile.find(groupName);
    if (it == groupHashFile.end()) {
        return;
    }

    QStringList& groupList = it.value();

    for(int i = 0; i < this->groupFilesModel->rowCount(); i++) {
        QModelIndex index = this->groupFilesModel->index(i, 0);
        QString key = this->groupFilesModel->data(index).toString();
        groupList << key;
    }

    qDebug() << groupList;
}
