// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#include "screen.h"

#include <QSGRectangleNode>
#include <QSGFlatColorMaterial>

// Hello implemtation details. Aren't you just great?
#include <QtQuick/private/qsgadaptationlayer_p.h>
#include <QtQuick/private/qquickitem_p.h>

#include "managers/settings.h"

Screen::Screen(QQuickItem* parent)
    : QQuickItem(parent)
    , letter(new Text{this})
    , pty_thread(new QThread)
{
    pty_thread->start();

    connect(&Settings::Instance(), &Settings::changed_font, this, &Screen::on_font_change);
    connect(this, &Screen::widthChanged, letter, &Text::line_recount);
    connect(this, &Screen::want_line_recount, letter, &Text::line_recount);
    connect(letter, &Text::on_line_count_change, this, &Screen::update_scroll_height);
    connect(letter, &Text::on_line_count_change, this, &Screen::update);
    connect(letter, &Text::new_text, this, &Screen::update);

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
    if (!(this->widthValid() && this->heightValid()))
        return old_node;

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
    pty = new Pty(server);
    pty->moveToThread(pty_thread);

    auto metrics = Metrics();
    connect(parentItem(), &QQuickItem::widthChanged, pty, [this, metrics](){
        auto cols = (int)floor(parentItem()->width () / metrics.averageCharWidth());
        auto rows = (int)floor(parentItem()->height() / metrics.lineSpacing     ());
        pty->set_size(QSize{cols, rows});
    });

    connect(pty, &Pty::receved_data, letter, &Text::add_text);
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

Text::Text(Screen* parent)
    : QObject(parent)
{
    data.emplace_back().emplace_back();
}

Text::~Text()  = default;

int Text::line_count() const
{
    return line_wrap_count;
}

void Text::line_recount()
{
    Screen* screen = static_cast<Screen*>(parent());
    auto pre_line_count = line_count();
    auto col = screen->get_columns();

    line_wrap_count = data.size();

    for (auto& line : data)
    {
        int count = 0;
        for (auto& block : line)
        {
            for (auto& letter : block.text)
            {
                if (count > col)
                {
                    line_wrap_count++;
                    count = 0;
                }
                count++;
            }
        }
    }
    
    if (line_wrap_count != pre_line_count)
        Q_EMIT on_line_count_change();
}

void Text::add_text(QString text)
{
    std::string input = text.toUtf8().data();
    Screen* screen = static_cast<Screen*>(parent());
    bool lines_added = false;
    for (auto letter : input)
    {
        if (letter == '\n')
        {
            data.emplace_back().emplace_back();
            current_line++;
            current_block = 0;
            lines_added = true;
        }
        else if (letter == '\r')
        {
            current_block = 0;
            current_char  = 0;
        }
        else if (letter == '\b')
        {
            if (current_char > 0)
            {
                current_char--;
                if ((current_char % screen->get_columns()) == 0)
                    lines_added = true;
            }
        }
        else if (letter < ' ')
        {
        }
        else
        {
            auto& block = data[current_line][current_block];
            if (current_char == block.text.size())
                block.text += letter;
            else
                block.text[current_char] = letter;
            current_char++;
            if ((current_char % screen->get_columns()) == 0)
                lines_added = true;
        }
    }
    if (lines_added)
        line_recount();
    Q_EMIT new_text();
}

const std::vector<std::vector<Text::Block>>& Text::get_data() const
{
    return data;
}