#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QHash>

typedef QHash<QString, QString> HashFile;
typedef QHash<QString, QStringList> GroupHashFile;

class QStandardItemModel;
class QSortFilterProxyModel;
class QStringListModel;

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initHashFile(QString path);
private:
    void initModelWithHashFile();

private slots:
    void openFile();
    void on_pushButton_clicked();

    void on_groupListView_clicked(const QModelIndex &index);

    void onDropFinished();
private:
    Ui::MainWindow *ui;
    HashFile hashFile;
    QStandardItemModel* resFilesModel;
    QSortFilterProxyModel* resFilesProxyModel;

    QStandardItemModel* groupFilesModel;
    QSortFilterProxyModel* groupFilesProxyModel;



    QStringListModel* groupListModel;

    GroupHashFile     groupHashFile;
};

#endif // MAINWINDOW_H
