#include "FileNameLineEdit.h" 
 
namespace QuarkMeta { 
 
FileNameLineEdit::FileNameLineEdit(QWidget* parent) : QLineEdit(parent) {} 
 
void FileNameLineEdit::setIsFolder(bool isFolder) { 
    m_isFolder = isFolder; 
} 
 
void FileNameLineEdit::focusInEvent(QFocusEvent* event) { 
    QLineEdit::focusInEvent(event); 
    if (m_isFolder) { 
        selectAll(); 
    } else { 
        int lastDot = text().lastIndexOf('.'); 
        if (lastDot > 0) { 
            setSelection(0, lastDot); 
        } else { 
            selectAll(); 
        } 
    } 
} 
 
void FileNameLineEdit::keyPressEvent(QKeyEvent* event) { 
    int key = event->key(); 
    if (key == Qt::Key_Up) {
        setCursorPosition(0);
        deselect();
        event->accept(); 
        return; // Consume Up to prevent item view row shifting and jump to start
    }
    if (key == Qt::Key_Down) {
        setCursorPosition(text().length());
        deselect();
        event->accept();
        return; // Consume Down to prevent item view row shifting and jump to end
    } 
    if (key == Qt::Key_Left || key == Qt::Key_Right) { 
        if (hasSelectedText()) { 
            if (key == Qt::Key_Left) { 
                setCursorPosition(0); 
            } else { 
                int lastDot = text().lastIndexOf('.'); 
                if (lastDot > 0) { 
                    setCursorPosition(lastDot); 
                } else { 
                    setCursorPosition(text().length()); 
                } 
            } 
            deselect(); 
            event->accept(); 
            return; 
        } 
    } 
    QLineEdit::keyPressEvent(event); 
} 
 
} // namespace QuarkMeta 
