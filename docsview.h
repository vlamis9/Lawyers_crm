#ifndef DOCSVIEW_H
#define DOCSVIEW_H

#include "enums.h"
#include <QDialog>
#include <QMap>

class Docs;
class QSqlTableModel;
class QTableView;
class QComboBox;
class QMediaPlayer;
class QBuffer;

namespace Ui {
class docsView;
}

class docsView : public QDialog
{
    Q_OBJECT

public:
    explicit docsView(bool, QWidget* parent = nullptr, int = 0, int = 0);
    ~docsView();

private slots:
    void previewFoto(const QModelIndex&);
    void on_checkBoxUser_toggled(bool);
    void on_checkBoxClient_toggled(bool);
    void on_checkBoxCase_toggled(bool);
    void finishUpdateView(QSqlTableModel*, DocViews);
    void on_comboBoxClient_activated(int);
    void on_comboBoxCase_activated(int);
    void on_widgetDocs_tabBarClicked(int);
    void on_buttonAddFoto_clicked();
    void on_buttonDelFoto_clicked();
    void on_buttonShowIm_clicked();
    void on_buttonAddAudio_clicked();
    void on_buttonPlayAudio_clicked();
    void on_buttonSelectAllFoto_clicked();
    void getImageFromScan(QImage);
    void on_buttonScan_clicked();
    void on_volumeSlider_valueChanged(int value);
    void on_buttonSaveFoto_clicked();
    void on_buttonEdittextDoc_clicked();
    void on_buttonPrintFoto_clicked();
    void on_buttonExit_clicked();
    void on_buttonSaveAudio_clicked();
    void on_buttonNewText_clicked();
    void startMessageTextWarning();
    void endMessageTextWarning();
    void on_buttonRenameFoto_clicked();
    void on_buttonRenameAudio_clicked();
    void on_buttonRenameText_clicked();
    void on_buttonSaveText_clicked();
    void on_buttonDelAudio_clicked();
    void on_buttonDeleteText_clicked();
    void on_buttonSelectAllAudio_clicked();
    void on_buttonSelectAllText_clicked();
    void on_buttonAddText_clicked();

signals:
    void startUpdateView(DocViews, bool, int = 0, int = 0);
    void addDocs(DocViews, bool, int = 0, int = 0);
    void addScanImage(QByteArray, DocViews, bool, int = 0, int = 0);
    void audioPosTime(int);
    void openTextDocs(const QModelIndex&);

private:
    void setComboBoxes();
    void addAnyDocs();
    void saveAnyDocs(QTableView*);
    void renameAnyDocs(QTableView*);
    void deleteAnyDocs(QTableView*);

    Ui::docsView *ui;
    bool m_user;
    int m_idCase;
    int m_idClient;
    Docs* docsBE;
    QMap<int, int> indexesClientsCombo;
    QMap<int, int> indexesCasesCombo;
    DocViews m_dv;
    QList<QComboBox*> allCombos;
    int comboChanged = 0;
    using pFunc = void(docsView::*)(int);
    QMap<int, pFunc> arrFunc;
    QImage image;
    QByteArray arrAudio;
    QMediaPlayer* player;
    QBuffer* audioStream;
    bool isLoaded {false};
};

#endif // DOCSVIEW_H
