// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#include "screen.h"

#include <QSGRectangleNode>
#include <QSGFlatColorMaterial>

#include <QColor>
#include <QFontDatabase>
#include <QRawFont>

// Hello implemtation details. Aren't you just great?
#include <QtQuick/private/qsgadaptationlayer_p.h>
#include <QtQuick/private/qquickitem_p.h>

static QRawFont mono_font;

Screen::Screen(QQuickItem* parent)
    : QQuickItem(parent)
    , letter(new Text{this})
{
    auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    //font.setPointSize(24);
    mono_font = QRawFont::fromFont(font);
}

Screen::~Screen()
{
    delete letter;
}

Text::Text(Screen* parent)
    : QQuickItem(parent)
{
    setParentItem(parent);
    setFlag(ItemHasContents, true);
}

Text::~Text()  = default;

static QString message = R"(ABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyz
1234567890
!@#$%^&*()
`-=[]\;',./
~_+{}|:"<>?
A quick brown fox jumps over the lazy dog
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
)";

QSGNode* Text::updatePaintNode(QSGNode* old_node, UpdatePaintNodeData*)
{
    QSGGlyphNode* node = dynamic_cast<QSGGlyphNode*>(old_node);

    if (!node) {
        auto sgr = QQuickItemPrivate::get(this)->sceneGraphRenderContext();
        node = sgr->sceneGraphContext()->createGlyphNode(sgr, false);
    }

    QVector<QPointF> positions;
    double x_pos = 0;
    double max_char_width = mono_font.averageCharWidth();
    double ascent = mono_font.ascent();
    if (positions.size())
        x_pos = positions.last().x() + max_char_width;
    int to_add = message.size() - positions.size();
    for (int i = 0; i < to_add; i++) {
        if (message[i] == '\n')
        {
            positions << QPointF(-1000, -1000);
            x_pos = 0;
            ascent += mono_font.ascent() + mono_font.descent();
        }
        else
        {
            positions << QPointF(x_pos, ascent);
            x_pos += max_char_width;
        }
    }

    node->setOwnerElement(this);

    node->geometry()->setIndexDataPattern(QSGGeometry::StaticPattern);
    node->geometry()->setVertexDataPattern(QSGGeometry::StaticPattern);

    QGlyphRun glyphrun;
    glyphrun.setRawFont(mono_font);
    glyphrun.setGlyphIndexes(mono_font.glyphIndexesForString(message));
    glyphrun.setPositions(positions);

    node->setGlyphs(QPointF(0, mono_font.ascent()), glyphrun);
    node->setStyle(QQuickText::Normal);
    node->setColor(QColor(0, 0, 0));


    node->update();

    return node;
}