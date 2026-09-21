#pragma once

#include <QObject>
#include <QPointer>
#include <QWidget>

class MainWindow;

class InstanceObj : public QObject
{
public:
    explicit InstanceObj(QWidget *pNotepad);
    ~InstanceObj() override;

public slots:
    void doMainWork();

public:
    QWidget *m_pNotepad = nullptr;
    QPointer<MainWindow> m_pMainWindow;

private:
    InstanceObj(const InstanceObj &other) = delete;
    InstanceObj &operator=(const InstanceObj &other) = delete;
};
