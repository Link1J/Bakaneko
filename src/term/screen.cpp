// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "screen.h"

#include <QSGRectangleNode>
#include <QSGFlatColorMaterial>

// Hello implemtation details. Aren't you just great?
#include <QtQuick/private/qsgadaptationlayer_p.h>
#include <QtQuick/private/qquickitem_p.h>

#include "managers/settings.h"

#include "terms/dumb.h"

Screen::Screen(QQuickItem* parent)
    : QQuickItem(parent)
    , letter(new Dumb{this})
    , pty_thread(new QThread)
{
    pty_thread->start();

    connect(&Settings::Instance(), &Settings::changed_font        , this  , &Screen::on_font_change      );
    connect(this                 , &Screen  ::widthChanged        , letter, &Term  ::line_recount        );
    connect(this                 , &Screen  ::want_line_recount   , letter, &Term  ::line_recount        );
    connect(letter               , &Term    ::on_line_count_change, this  , &Screen::update_scroll_height);
    connect(letter               , &Term    ::on_line_count_change, this  , &Screen::update              );
    connect(letter               , &Term    ::new_text            , this  , &Screen::update              );

    setFlag(ItemHasContents, true);
    on_font_change();
}

Screen::~Screen()
{
    // Stops the thread.
    pty_thread->quit();
    while (pty_thread->isRunning());

    // Memory cleanup
    delete pty_thread;
    delete letter;
    delete pty; // Make Qt complain about killing timers
}

QFontMetricsF Screen::Metrics() const
{
    return QFontMetricsF{font.base};
}

QSGNode* Screen::updatePaintNode(QSGNode* old_node, UpdatePaintNodeData*)
{
    auto sgr = QQuickItemPrivate::get(this)->sceneGraphRenderContext();

    if (!old_node) {
        old_node = new QSGTransformNode();
    }

    old_node->removeAllChildNodes();

    QVector<QPointF> positions;

    auto columns = get_columns();
    auto metrics = Metrics();

    double x_pos = 0;
    double y_pos = parentItem()->property("topPadding").value<double>();
    int    count = 0;

    for (auto& line : letter->get_data())
    {
        for (auto& block : line)
        {
            if (block.text.size() == 0)
                continue;

            for (auto letter : block.text)
            {
                if (count > columns)
                {
                    y_pos += metrics.lineSpacing();
                    x_pos = 0;
                    count = 0;
                }
                positions << QPointF(x_pos, y_pos);
                x_pos += metrics.averageCharWidth();
                count++;
            }

            QRawFont font_to_use = font.normal;
            
            QGlyphRun glyphrun;
            glyphrun.setRawFont(font_to_use);
            glyphrun.setGlyphIndexes(font_to_use.glyphIndexesForString(QString::fromStdString(block.text)));
            glyphrun.setPositions(positions);

            auto node = sgr->sceneGraphContext()->createGlyphNode(sgr, false);

            node->setOwnerElement(this);

            node->geometry()->setIndexDataPattern(QSGGeometry::StaticPattern);
            node->geometry()->setVertexDataPattern(QSGGeometry::StaticPattern);
        
            node->setGlyphs(QPointF(0, font_to_use.ascent()), glyphrun);
            node->setStyle(QQuickText::Normal);
            node->setColor(QColor(0, 0, 0));

            node->update();

            old_node->appendChildNode(node);

            y_pos += metrics.lineSpacing();
            x_pos = 0;
            count = 0;
            positions.clear();
        }
    }

    return old_node;
}

void Screen::on_font_change()
{
    font.base = Settings::Instance().get_font();
    font.normal = QRawFont::fromFont(font.base);
    font.base.setBold(true);
    font.bold = QRawFont::fromFont(font.base);
    font.base.setItalic(true);
    font.bold_italic = QRawFont::fromFont(font.base);
    font.base.setBold(false);
    font.italic = QRawFont::fromFont(font.base);
    font.base.setItalic(false);
    Q_EMIT want_line_recount();
}

void Screen::set_server(Server* server)
{
    // This will leak memory if set_server is called more then once per a Screen.
    pty = new Pty(server, letter->term_type());
    pty->moveToThread(pty_thread);

    auto metrics = Metrics();
    connect(parentItem(), &QQuickItem::widthChanged, pty, [this, metrics](){
        auto cols = (int)floor(parentItem()->width () / metrics.averageCharWidth());
        auto rows = (int)floor(parentItem()->height() / metrics.lineSpacing     ());
        pty->set_size(QSize{cols, rows});
    });

    connect(pty, &Pty::receved_data, letter, &Term::add_text);
}

void Screen::update_scroll_height()
{
    setHeight(letter->line_count() * Metrics().lineSpacing());
    setImplicitHeight(letter->line_count() * Metrics().lineSpacing());
}

int Screen::get_columns() const
{
    return (int)floor(width() / Metrics().averageCharWidth());
}

void Screen::keyPressEvent(QKeyEvent* event)
{
    if (pty)
    {
        auto text = event->text();
        pty->send_data(text);
    }
    else
    {
        event->ignore();
    }
}