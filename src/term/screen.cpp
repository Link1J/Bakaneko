// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "screen.h"

#include <QBrush>
#include <QGlyphRun>
#include <QPainter>

#include "managers/settings.h"

#include "terms/dumb.h"

Screen::Screen(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , letter(Term::create_term(Settings::Instance().get_term_type(true), this))
    , pty_thread(new QThread)
{
    pty_thread->start();

    connect(&Settings::Instance(), &Settings::changed_font        , this  , &Screen    ::on_font_change      );
    connect(this                 , &Screen  ::widthChanged        , letter, &Term      ::line_recount        );
    connect(this                 , &Screen  ::want_line_recount   , letter, &Term      ::line_recount        );
    connect(letter               , &Term    ::on_line_count_change, this  , &Screen    ::update_scroll_height);
    connect(letter               , &Term    ::on_line_count_change, this  , &QQuickItem::update              );
    connect(letter               , &Term    ::new_text            , this  , &QQuickItem::update              );

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

void Screen::paint(QPainter* painter)
{
    painter->setBackgroundMode(Qt::OpaqueMode);

    QVector<QPointF> positions;

    auto columns = get_columns();
    auto metrics = Metrics();
    auto top_pad = parentItem()->property("topPadding").value<double>();

    double x_pos = 0;
    double y_pos = 0;
    int    count = 0;

    for (auto& line : letter->get_data())
    {
        for (auto& symbol : line)
        {
            font.base.setBold     (symbol.bold     );
            font.base.setItalic   (symbol.italic   );
            font.base.setUnderline(symbol.underline);

            painter->setFont      (font  .base      );
            painter->setPen       (symbol.foreground);
            painter->setBackground(symbol.background);

            if (count > columns)
            {
                y_pos += metrics.lineSpacing();
                x_pos = 0;
                count = 0;
            }

            painter->drawText(x_pos, y_pos + metrics.ascent(), QString(symbol.letter));

            x_pos += metrics.averageCharWidth();
            count++;
        }
        y_pos += metrics.lineSpacing();
        x_pos = 0;
        count = 0;
    }
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
    pty = new Pty(server, letter->term_report());
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