#pragma once

#include <QWidget>
#include <QVector>
#include "desensitizer.h"

class QCheckBox;
class QLabel;
class QTableWidget;
class QTableWidgetItem;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addRow();
    void removeRows();
    void moveUp();
    void moveDown();
    void generateRandom();
    void pasteBatch();
    void importFile();
    void exportFile();
    void saveConfig();
    void applyForward();
    void applyReverse();
    void onItemChanged(QTableWidgetItem *item);
    void openConfigDir();

private:
    void applyText(bool reverse);
    QVector<Mapping> collectMappings() const;
    void setMappings(const QVector<Mapping> &mappings, bool append);
    void appendMappings(const QVector<Mapping> &mappings);
    void appendRow(const Mapping &mapping);
    void markAsChanged();
    void setStatus(const QString &text);

    QTableWidget *m_table = nullptr;
    QCheckBox *m_caseSensitive = nullptr;
    QCheckBox *m_selectedOnly = nullptr;
    QLabel *m_status = nullptr;
    bool m_loading = false;
};
