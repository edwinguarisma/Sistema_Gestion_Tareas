#include "ui/MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QLocale>
#include <QTranslator>
#include <QIcon>

int main(int argc, char *argv[]) {
    // Crear aplicación
    QApplication app(argc, argv);
    
    // Configurar información de la aplicación
    app.setApplicationName("Sistema de Gestión de Tareas");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Universidad");
    app.setOrganizationDomain("universidad.edu");
    
    // Configurar estilo moderno
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Paleta de colores moderna
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    // Comentar la siguiente línea si prefieres tema claro
    // app.setPalette(darkPalette);
    
    // Configurar locale español
    QLocale::setDefault(QLocale(QLocale::Spanish, QLocale::Spain));
    
    // Crear y mostrar ventana principal
    MainWindow window;
    window.show();
    
    return app.exec();
}

