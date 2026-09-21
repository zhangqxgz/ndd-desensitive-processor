#include <QApplication>
#include <QTextStream>
#include <qsciscintilla.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextStream out(stdout);

    QsciScintilla editor;
    editor.resize(400, 200);
    editor.show();
    app.processEvents();

    editor.setText(QStringLiteral("abc xx abc"));
    app.processEvents();

    out << "before=" << editor.text() << Qt::endl;
    out << "isReadOnly=" << editor.isReadOnly() << Qt::endl;

    editor.selectAll();
    out << "hasSelectedText=" << editor.hasSelectedText() << Qt::endl;
    out << "selectedText=" << editor.selectedText() << Qt::endl;
    editor.replaceSelectedText(QStringLiteral("XYZ xx XYZ"));
    app.processEvents();
    out << "after-replaceSelectedText=" << editor.text() << Qt::endl;

    editor.setText(QStringLiteral("abc xx abc"));
    editor.beginUndoAction();
    editor.selectAll();
    editor.replaceSelectedText(QStringLiteral("2 xx 2"));
    editor.endUndoAction();
    app.processEvents();
    out << "after-undo-group=" << editor.text() << Qt::endl;

    editor.setText(QStringLiteral("abc xx abc"));
    editor.SendScintilla(static_cast<unsigned int>(QsciScintillaBase::SCI_SELECTALL));
    QByteArray payload = QStringLiteral("3 xx 3").toUtf8();
    editor.SendScintilla(static_cast<unsigned int>(QsciScintillaBase::SCI_REPLACESEL),
                         payload.constData());
    app.processEvents();
    out << "after-raw-sci=" << editor.text() << Qt::endl;

    return 0;
}
