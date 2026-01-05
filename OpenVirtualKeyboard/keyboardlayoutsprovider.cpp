/**
 *  MIT License
 *  Copyright (c) Pavel Hromada
 *  See accompanying LICENSE file
 */

#include <QDir>
#include <QJsonDocument>
#include <QFile>
#include <QLocale>

#include "keyboardlayoutsprovider.h"
#include "keyboardlayoutmodel.h"
#include "loggingcategory.h"
#include "utils.h"

const QString ALPHABET_FILENAME = QStringLiteral( "alphabet.json" );
const QString SYMBOLS_FILENAME  = QStringLiteral( "symbols.json" );
const QString DIGITS_FILENAME   = QStringLiteral( "digits.json" );
const QString NUMBERS_FILENAME  = QStringLiteral( "numbers.json" );
const QString DIAL_FILENAME     = QStringLiteral( "dial.json" );

KeyboardLayoutsProvider::KeyboardLayoutsProvider()
{
    loadEmbeddedLayouts();
    applySystemLocaleLayout();
    emit layoutsCountChanged();
}

KeyboardLayoutModel* KeyboardLayoutsProvider::alphabetModel() const
{
    return const_cast<KeyboardLayoutModel*>( &_alphabetModel );
}

KeyboardLayoutModel* KeyboardLayoutsProvider::symbolsModel() const
{
    return const_cast<KeyboardLayoutModel*>( &_symbolsModel );
}

KeyboardLayoutModel* KeyboardLayoutsProvider::dialModel() const
{
    return const_cast<KeyboardLayoutModel*>( &_dialModel );
}

KeyboardLayoutModel* KeyboardLayoutsProvider::numbersModel() const
{
    return const_cast<KeyboardLayoutModel*>( &_numbersModel );
}

KeyboardLayoutModel* KeyboardLayoutsProvider::digitsModel() const
{
    return const_cast<KeyboardLayoutModel*>( &_digitsModel );
}

void KeyboardLayoutsProvider::incrementPageForLayoutType( KeyboardLayoutType::Type layoutType )
{
    switch (layoutType) {
        case KeyboardLayoutType::Alphabet:
            _alphabetModel.setCurrentPage( _alphabetModel.currentPage() + 1 );
            break;
        case KeyboardLayoutType::Symbols:
            _symbolsModel.setCurrentPage( _symbolsModel.currentPage() + 1 );
            break;
        case KeyboardLayoutType::Dial:
            _dialModel.setCurrentPage( _dialModel.currentPage() + 1 );
            break;
        case KeyboardLayoutType::Numbers:
            _numbersModel.setCurrentPage( _numbersModel.currentPage() + 1 );
            break;
        case KeyboardLayoutType::Digits:
            _digitsModel.setCurrentPage( _digitsModel.currentPage() + 1 );
            break;
    }
}

int KeyboardLayoutsProvider::selectedLayoutIndex() const
{
    return _selectedLayoutIndex;
}

int KeyboardLayoutsProvider::layoutsCount() const
{
    return _layoutData.size();
}

void KeyboardLayoutsProvider::setSelectedLayoutIndex( int index )
{
    if (_selectedLayoutIndex == index)
        return;

    const auto layouts = layoutsList();

    if (index >= layouts.size() || index < 0)
        return;

    const auto& layout = _layoutData[ layouts.at( index )];

    _alphabetModel.setPages( layout.alphabet );
    _symbolsModel.setPages( layout.symbols );
    _dialModel.setPages( layout.dial );
    _numbersModel.setPages( layout.numbers );
    _digitsModel.setPages( layout.digits );

    _selectedLayoutIndex = index;
    emit selectedLayoutIndexChanged();
    emit selectedLayoutChanged();
}

QString KeyboardLayoutsProvider::selectedLayout() const
{
    return layoutsList()[ _selectedLayoutIndex ];
}

void KeyboardLayoutsProvider::loadDefaultLayout()
{
    const QString base = ":/ovk/qml/layouts/en_US/";

    auto& data = _layoutData["en_US"];
    data.alphabet = loadLayoutData(base + ALPHABET_FILENAME);
    data.symbols  = loadLayoutData(base + SYMBOLS_FILENAME);
    data.dial     = loadLayoutData(base + DIAL_FILENAME);
    data.numbers  = loadLayoutData(base + NUMBERS_FILENAME);
    data.digits   = loadLayoutData(base + DIGITS_FILENAME);
}

void KeyboardLayoutsProvider::loadEmbeddedLayouts()
{
    const QString basePath = ":/ovk/qml/layouts/";
    const QDir baseDir(basePath);

    if (!baseDir.exists()) {
        qCDebug(logOvk) << "No embedded layouts found";
        return;
    }

    const QStringList localeDirs =
        baseDir.entryList(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);

    if (localeDirs.isEmpty()) {
        qCDebug(logOvk) << "Layouts directory is empty";
        return;
    }

    qCDebug(logOvk).noquote()
        << "Loading embedded layouts:" << localeDirs.join(", ");

    for (const QString& locale : localeDirs) {
        const QString dir = basePath + locale + "/";

        auto& data = _layoutData[locale];

        data.alphabet = loadLayoutData(dir + ALPHABET_FILENAME);
        data.symbols  = loadLayoutData(dir + SYMBOLS_FILENAME);
        data.dial     = loadLayoutData(dir + DIAL_FILENAME);
        data.numbers  = loadLayoutData(dir + NUMBERS_FILENAME);
        data.digits   = loadLayoutData(dir + DIGITS_FILENAME);
    }
}

void KeyboardLayoutsProvider::applySystemLocaleLayout()
{
    const QString im = qEnvironmentVariable( "QT_IM_MODULE" );
    QString lang = im.split( "lang=" )[1];

    if ( !_layoutData.contains( lang ) ) {
        if ( _layoutData.contains( lang + "_FR" ) )
            lang = lang + "_FR";
        else {
            lang = "en_US";
        }
    }

    const QStringList layouts = layoutsList();

    qCDebug( logOvk ).noquote() << "Applying keyboard layout:" << lang;

    setSelectedLayoutIndex( layouts.indexOf( lang ) );
}

QJsonArray KeyboardLayoutsProvider::loadLayoutData( const QString& layoutFilename )
{
    QFile layoutFile( layoutFilename );

    if (!layoutFile.open( QIODevice::ReadOnly | QIODevice::Text ))
        return QJsonArray();

    return QJsonDocument::fromJson( layoutFile.readAll() ).array();
}

QStringList KeyboardLayoutsProvider::layoutsList() const
{
    return _layoutData.keys();
}
